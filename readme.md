<div STYLE="page-break-after: always;"><div>

# 智能物流电控技术总结

本次物流从前期验证到最终实现所有功能花了很多时间，吸取了之前的经验，也有新技术的应用，本文章用于对智能物流的电控方向所用技术总结起来，方便以后随时阅读和查看。

---

### 着重关注问题

* **堆叠式PCB**设计
* 整机**电线规划**
* **易于调试**的代码架构
* **底盘**运动控制技术
* **机械臂**运动控制算法

围绕以上问题，结合结构和视觉的需求，电控开始进行从硬件电路到软件驱动、算法层面的全面设计。


<div style="text-align: center;"><img src="文档\image\readme\物流机器人整体视图2.jpg" alt="堆叠设计"></div>


<div style="text-align: center;"><img src="文档\image\readme\物流机器人整体视图.jpg" alt="堆叠设计"></div>

---

### 硬件设计

#### 概览

本次PCB最重要的设计思路就是堆叠式PCB设计，堆叠式PCB可以实现更快的**电路维修**，替换，实现电路的**模块化**，充分利用**垂直空间**。例如功率板和主控板的堆叠设计，充分利用了物流车的垂直空间，调试时主控板损坏，又或者主控板需要替换升级，我们只需要替换掉主控板就可以了，不需要维护功率板。截止到校赛，一共设计了四块PCB，分别是**功率板**、**主控板**、**分线板**和**显示板**。未来还将继续添加灯光板、泰山派拓展版等。功率板连接电池，负责整车电源控制，主控板使用**STM32F4**主控，引出多种通讯接口，满足各种外设控制需求。分线板通过40pin的FC排线连接主控板，然后引出各种接口，接电机、树莓派（泰山派）等设备。显示板使用74hc595串行芯片实现控制数码管显示任务码。具体文件可以参考[PCB\25工创PCB.eprj](./PCB/25工创PCB.eprj)文件，PCB文件使用嘉立创绘制。

<div STYLE="page-break-after: always;"><div>

* 下图展示各块PCB的连接方式

<div style="text-align: center;"><img src="文档\流程图\电路板连接示意图.drawio.png" alt="堆叠设计"></div>

#### 功率板

功率板具有升压和降压功能为整个物流车提供电源，搭载两路升压电源和两路降压电源，升压使用传统**boost**拓扑电路从电源电压**12v升压到24v**，供电机或者其他需要高功率用电器使用，使用**XL6019**，有最大5A的开关电流，因为电机功率比较大，这里选择两路同样的升压芯片，每路XL6019驱动四个步进电机，总共驱动八个。降压使用**buck**拓扑电路，分为两种降压电压，分别是降压到6v和5v，降压到**6v**给舵机使用，降压到**5v**给芯片、树莓派（泰山派）等使用，使用的是**TPS5430ADDR**，可以输出3A的连续电流，还是**宽输入**的可以输入5.5V到36V，对于驱动一些芯片或者舵机来说绰绰有余。更加具体的芯片信息可以从他的**数据手册**上找到，包括输出电压调整，典型应用等，这里不再赘述。

在设计功率板时，除了**电路完整性**之外，还要注意每个部分使用起来是否**便利**，比如功率板在设计时，我将三路（24v、6v、5v）电源输出都做了**单独开关**，调试物流车时可以单独开关三路电源，更加方便调车。另外要从升降压芯片**输入端**做开关控制，防止功率板关断不完全，如果从芯片输出端做开关控制，虽然电源开关是关断了，但是实际芯片还在从输入端供电，持续损耗电源的电，也就是没有完全断电。功率板输入接口选择**XT30**，这个接头是航模接头，很稳固，大小合适而且也好看。电源输入端还连着一个DC接头，有了这个DC接头，不但可以多一个电源输入接头，能兼容更多电池接口，平时还可以实现不断开电池的XT30接头，直接用这个DC头充电，更加的便利。大家设计PCB时可以参考功率板的设计，都是经过测试的方案，基本不会出现问题。另外在设计电路接口的时候，还可以多多上**淘宝**看看各种连接器，比如一些加高排母等连接器，可以让你的PCB调车的时候使用起来更加便携。

* 功率板原理图

<div style="text-align: center;"><img src="文档\image\readme\功率板原理图.png" alt="堆叠设计"></div>

功率板的**布局布线**要特别注意，因为是控制电源的，如果电源不稳，会出现很多莫名其妙的问题，重打一块PCB的时间成本一般在6~10天时间成本是很高的，所以我们设计PCB要争取一版做好。功率走线一定要粗，这里的功率走线使用了铺铜+开窗的方法增大过流能力，开窗之后可以在铜箔上铺一层锡，增大过流面积，下图使用黄色线勾勒的铜箔就是使用铺铜+开窗增大过流能力的例子。电源除了铜箔走线要粗，反馈线也要布局好，**反馈线很大程度上决定了电源稳不稳定**，下图标了数字1234的四条导线都是反馈线，无论是buck还是boost电路，都有**反馈（FB）引脚**要从输出电容附近取电，因为经过输出电容的电压是最稳的，最适合来做电压反馈的。设计电源的时候要分清哪些是**功率走线**，哪些是**信号走线**，功率走线尽量短和直，还有一定要够粗，信号线要走干扰少的地方，高频回路面积要小。另外，在可能发热的地方还可以多打散热孔，像是功率板的XL6019底下和TPS5430ADDR底下都有几个过孔增加散热。

* 功率板底层layout

<div style="text-align: center;"><img src="文档\image\readme\功率板底层layout.png" alt="堆叠设计" width="454" height="358"></div>

由于功率板涉及到**堆叠pcb**设计 ，在设计的时候我们并不确定某些元器件会不会和另外一块电路板有**干涉**，所以我们可以充分利用嘉立创的导出3D模型的功能，导出step文件，利用**solidworks**将两块电路板组合成一个装配体，可以大概的查看元器件有没有干涉，如下图所示。

* 3d模型图

<div style="text-align: center;"><img src="文档\image\readme\功率板+主控板3d模型.png" alt="堆叠设计" width="436" height="221"></div>

#### 主控板

主控板是整个物流车的核心，主控使用的是**STM32F407VET6**，选择这颗芯片的原因是第一个熟悉，第二个价格合适，外设刚好够用，性能绰绰有余，主控板集成了一个**CH340N**串口转USB芯片和一个**AMS1117**降压芯片，给芯片提供3.3V工作电压。还有两个**单独**开关，分别控制双层typec接口的电源和芯片的电源，设计上可以单独打开芯片或者typec电源，更加便于调试。还留了一个**无线DAP烧录器**的调试接口，支持远程串口，可以远程下载程序。主控板的布局布线比较简单，只要能连通基本就能工作，但是要注意，芯片的**去耦电容**要摆放在芯片的供电引脚旁边，保证电源的质量，去耦电容的**大小和数量**在芯片手册可以找到。主控板通过一个40pin的FC排线输出信号和电源，有12路24V电源，4路6V，两路5V，剩下的都是信号线，大部分时串口线，还预留了4根引脚，可以扩展功能。

* 主控板顶层layout

<div style="text-align: center;"><img src="文档\image\readme\主控板顶层layout.png" alt="堆叠设计" width="504" height="418"></div>

主控芯片的原理图可以直接使用，STM32F407的**PD10**引脚单独引出的原因是为了**测量程序运行的时间**，在程序需要计时的地方加一行反转PD10引脚的电平然后配合示波器就可以测量出程序运行的时间，可以看整个系统的时间响应情况。主控板原理图里比较有意思的是CH340N的**防倒灌电路**由一个二极管和一个三极管组成，在CH340手册里面有描述它们的作用，这里的主要目的是为了防止视觉模块通过CH340N的**TX和RX引脚**反向供电给STM32导致STM32无法关机，之前就遇到过**关闭不完全**的情况，导致芯片看上去断电实则还在运行，调试的时候就没那么方便。板载还有一颗LDO用的是常用的AMS1117，是用于5V降压到3.3V。目前只有一个指示3.3V的LED，主控板之后还可以改进给每一路电源做一个LED显示是否开启，不然在调试的时候经常分不清哪些开关时打开的，哪些时关闭的。

<div STYLE="page-break-after: always;"><div>

* 主控板原理图

<div style="text-align: center;"><img src="文档\image\readme\主控板原理图.png" alt="堆叠设计" ></div>

* 功率板+主控板堆叠设计实物图

<div style="text-align: center;"><img src="文档\image\readme\主控板和功率板堆叠设计.png" alt="堆叠设计" width="504" height="378"></div>

可以注意到的是，功率板和主控板设计了很多**开关**，加起来一共有五个， 下面三个是控制24V、6V、5V的上面两个是在5V的基础上再加两个开关，控制5V上的主控芯片和外接视觉模块的供电，这样的好处是我们可以**单独调试**某些功能，而让其他功能不工作。举个例子，如调试舵机的时候，我不希望电机上电，那么我们只要接通6V和5V和主控芯片的电源就可以调试了，电机是完全断电的状态，可以加快工作效率。

#### 分线板

设计分线板的目的是为了**更简洁**的接线，还有把**陀螺仪**放在物流车的**机械中心**。分线板大部分是接口元件，还有一个AMS1117从5V降到3.3V供陀螺仪使用。由于分线板是放在机器内部，比较难拆出来，所以设计的时候就想着要电路要简洁可靠，基本没有什么元件，这样才可靠性高，可靠性高意味着不用经常拆出来维护，最终设计出来的效果也确实如此，上面只有两个元器件，一个是降压芯片还有一个陀螺仪，其他都是接口，连接电机、舵机等各种设备。平时容易出问题的PCB都放在机器外面比较好拆卸的地方，通过一个40pin的FC排线连接分线板，拆卸的时候只要拆出排线和几颗螺丝就能拆除主控板和功率板，这种设计更能提高维护效率。在设计分线板的原理图的时候要注意**FC排线线序**的问题，不然线序错了接上就烧了。

* 分线板原理图

<div style="text-align: center;"><img src="文档\image\readme\分线板原理图.png" alt="堆叠设计"></div>

PCB的外形可以让结构同学规划，然后把DXF文件导出，利用嘉立创的DXF文件导入功能就可以导入外形文件，这样就不用担心外形会冲突，分线板的外形就是结构同学规划的。关于分线板的layout基本不用注意什么东西，只要功率走线够粗就行，最主要的是设计的要便利，分线板出线位置要和电机等设备的位置一一对应。关于连接件的选取可以多逛逛淘宝，很多方案都是在淘宝看到才了解到的，比如是主控板的双层typec接口。

* 分线板顶层layout

<div style="text-align: center;"><img src="文档\image\readme\分线板顶层layout.png" alt="堆叠设计" width=304" height="308"></div>

#### 显示板

* 显示板原理图

<div style="text-align: center;"><img src="文档\image\readme\显示板原理图.png" alt="堆叠设计"></div>

显示板还没有做测试，校赛使用的是串口屏显示任务码，目前用的方案是74HC595芯片驱动数码管，使用5V供电，通过芯片的这个芯片可以级联，然后仅用三根导线就可以控制所有数码管，由于还没有测试，这里不作详细说明，这个芯片的使用方法在网上有一堆教程，可以找到很详尽的资料。

<div style="text-align: center;"><img src="文档\image\readme\显示板顶层layout.png" alt="堆叠设计"></div>

#### * **注意**

1. 在功率板上并没有集成3.3V电压的降压芯片，因为3.3V电压太低，容易受干扰，最好在需要3.3V的场合单独接一个**AMS1117**或者其他替代LDO芯片，来给STM32F407和其他需要3.3V的使用。舵机单独6V原因是因为舵机的电流大，与其他用电器共一路会引起电压波动，严重的会造成树莓派等重启。
2. 目前电路板还存在电机上电时**电压不稳**的情况，推测是电池的电流输出能力不够或者升降压芯片的动态响应不够好的原因，会在电机上电时造成其他路有个瞬间低电压，造成其他路设备重启，可以尝试**增大输出电容**来解决这个问题。
3. 主控板的双层typec接口目前会概率出现无法识别的情况，估计是识别电阻的问题，可以通过换用更熟悉的单层typec接口来解决或者是尝试更换识别电阻。
4. 为了显示板的布线考虑，原理图74HC595连接数码管的方式是乱序的，编写驱动程序的时候要注意。

---

<div STYLE="page-break-after: always;"><div>

### 运动算法设计

#### 概览

工程使用的算法大概可以分为三种，分别是底**盘控制**算法，**机械臂控制**算法，5阶**插值**算法。

#### 底盘控制算法

关于底盘的控制算法，不同的驱动方式有不同的算法，经过考虑，整车选择了全向轮作为驱动机构，四个全向轮组成**O**形，实现全向移动。算法原理很简单，只需要用到速度矢量的分解，三个速度矢量分别是方向x速度、y方向速度和角速度w，其中x和y垂直且与组成的平面平行于地面，分解为四个全向轮的速度。

具体推导可以参考以下文章：`https://blog.csdn.net/weixin_42722140/article/details/134621461`

需要注意的是文章通过一个**旋转矩阵**实现了从物流车坐标系到世界坐标系的转换。

<div style="text-align: center;"><img src="文档\image\readme\物流车顶视图.png" width="300" height="300" alt="堆叠设计"></div>

#### 机械臂控制算法

机械臂运动学解算分为机械臂运动学正解算和逆解算，正解算是由各个关节的角度推算机械臂末端坐标，逆解算是由机械臂末端坐标推算各个机械臂关节的角度。逆解算用于**控制**，正解算用于机械臂**示教**（校赛暂未实现）。

* 值得注意的是，机械臂算法并没有刻意的去解算计算xy的平面（机械臂底部电机）的角度，仅解算yz平面的角度（大臂角和小臂角度）。xy平面的角度将分离为一个单独调整的变量来控制，这样做的**目的**是更方便的调试动作。

机械臂运动学逆解算算法可以参考以下视频：

```
https://www.bilibili.com/video/BV1mY411476h/?spm_id_from=333.337.search-card.all.click&vd_source=0b74b2b69095521c3b4b6fbfdcd7eae6
```

机械臂正解算的思路就像点在平面坐标系移动，已知机械臂的各个臂长和角度，通过三角函数可以推算出每一段的点的x和y所走的距离，叠加各段x和各段的y，最终得到末端坐标。

#### 5阶插值

关于五阶插值的原理可以参考以下文章：`https://www.zhihu.com/tardis/zm/art/269230598?source_id=1005`

简单来说，插值在本工程运动算法的作用是让一段运动变得丝滑，特别是底盘控制，若合理的设置节点的时刻、位移、速度、加速度，5阶线性插值算法能自动解算出一段路径的每个时刻的速度，让物流车运行更加的平稳，**防止车轮打滑**。

前文提到，本次物流车底盘控制算法在底层就是控制x、y、w方向上的速度，在此基础上，分别对这三个方向进行插值，即可实现对一段平滑的轨迹进行控制。下图展示的使用 `matlab`仿真设定x和y方向的每个节点位移、速度、加速度，自动解算出中间过程数值的结果。可以看出，在速度、加速度都是连续的，意味着运动的丝滑。

<div style="text-align: center;"><img src="文档\image\readme\线性插值数值仿真.jpg" alt="堆叠设计"></div>

把x和y上的位移作为坐标系的横轴和纵轴，我们可以得出**模拟轨迹**图如下。

<div style="text-align: center;"><img src="文档\image\readme\线性插值路径仿真.jpg" alt="堆叠设计"></div>

---

### 软件设计

#### 概览

在软件设计上，设计具有层级的代码结构，各层之间**耦合度底**，易于维护升级，可以单独维护升级每一层。在**用户层**，创新性的使用节点的编程思想，将物流车的运动抽象成一个个节点，并通过控制两个节点的状态，自动解算出两个节点间每个时刻的状态，实现稳定快速的物流车控制。调车时，我们只需要修改节点及其节点对应的任务，**其他什么都不需要动**，就可以实现对物流车运动状态进行修改。

#### 文件架构

* 代码文件结构概览（细节的文件暂不展示，后文会详细叙述）

```c
GC_25/
│
└── code/
    ├── .vscode/
    │   └── launch.json		//调试配置文件
    └── remote_control/
    │   └── test.py		//串口通讯测试文件
    │   └── UART.py		//手柄+串口透传遥控
    └── robot_control/
	└── CORE		//cotex-m4内核文件
	└── FWLIB		//STM32F4外设库
	└── DSP			//ARM算法加速库
	└── SYSTEM		//底层驱动配置
	└── HARDWARE 		//硬件驱动层
	└── KINEMATICS		//算法层
	└── USER		//用户层
```

代码的code-workspace文件在code\robot_control\USER\GC_25.code-workspace，通过这个文件可以打开工程。

工程共使用了两个库，一个是**stm32标准库**，另外一个是ARM提供的**DSP**库，可以加速一些数学运算，例如COS、SIN函数。

cotex-m4具备**FPU**，可以加速**单精度浮点数**的运算，本工程开启了FPU，开启的方式很简单，添加以下预处理宏定义即可

```c
__CC_ARM
__TARGET_FPU_VFP
__FPU_PRESENT
```

添加之后可以检查以下**宏定义** `__FPU_USED`是否为1，若为1，则表示启用了FPU，本工程启用了FPU。

工程所有代码都存放在**code**文件夹里面

* **.vscode**是cotex-debug插件自动生成的lauch.json文件，当我们需要调试时，需要在里面配置调试参数，如目标芯片型号，调试器型号等。
* ~~remote_control~~文件夹存放了早期调试底盘时写的使用PS5手柄操控物流车的驱动程序，现已不使用。具体原理是使用电脑读取手柄数据，然后通过调试器自带的远程串口传输给物流车，实现对物流车的控制，代码使用python编写。
* **robot_control**是本工程核心文件，该文件夹存放了所有STM32代码，其中CORE和FWLIB和DSP不做过多介绍，都是官方库文件。**SYSTEM**文件夹存放了芯片外设驱动文件，包括使用到的定时器、串口和DMA。**HARDWARE**文件夹存放了硬件驱动文件，包括步进电机驱动、陀螺仪驱动、通讯协议驱动、扫码模块驱动、舵机驱动和系统时钟驱动。**KINEMATICS**文件夹存放底盘控制算法、**5阶线性插值算法**和机械臂控制算法文件。最后是**USER**文件夹，存放了平时常用的文件，包括主程序main文件、物流车动作文件、路径控制文件、~~远程遥控文件~~和一些官方的中断服务函数文件等。

在robot_control中，除官方库外，SYSTEM文件是工程**最底层**，描述了片内外设如何初始化，HARDWARE内文件调用SYSTEM层和官方库文件，封装了一层**硬件驱动层**，负责硬件控制。KINEMTICS层负责**整车运动算法**，仅调用HARDWARE层和官方库文件。USER层调用HARDWARE最终实现**运动控制**。

整个工程的**层级结构**为：SYSTEM->HARDWARE->KINEMTICS->USER（SYSTEM为最底层）

另外需要注意的是，部分文件存在**跨层**或者**同层**调用的情况，分别是USER层的 `motion.c`调用了HARDWARE层的 `protocols.h`，和HARDWARE层的 `protocols.h`调用了同层的 `scanner_driver.h`。

#### 开发环境

电控的开发环境是vscode的插件EIDE,具体使用可以参考下面链接：

```
https://www.bilibili.com/video/BV1nr4y1R7Jb/?spm_id_from=333.337.search-card.all.click&vd_source=0b74b2b69095521c3b4b6fbfdcd7eae6
```

里面有详细的EIDE配置方法，但是想要使用调试功能，视频里的方法已经不适用，可以参考下面链接：

```
https://blog.csdn.net/qq_39854159/article/details/124511433
```

建议使用vscode的**插件**EIDE和cotex-debug加上openocd。EIDE写代码，编译烧录，cotex-debug负责调试。想要使用调试功能，要注意整个**工程不能有中文路径**，cotex-debug不支持中文路径。配置cotex-debug时需要注意的是**lauch.json**文件，该文件位于code\.vscode\launch.json。需要配置的路径在文件已经标明，目前配置了dap-link和stlink。

jlink也可以使用，支持实时查看变量对于电机或者运动控制等一些需要快速查看变量的场合很方便，使用串口输出调试信息会让整个程序运行效率变慢，对于一些高速任务如需要20KHz控制频率的FOC控制技术，加了一个串口输出函数会极大影响运行效果，这时如果我们还想要监控数据，可以考虑使用jlink。

* 以下为cotex-debug调试的配置文件 `launch.json`的示例（仅需修改注释提示的地方）

```json
"configurations": [
    {
        "cwd": "${workspaceRoot}",
        //需要配置以下elf文件路径
        "executable": "F:\\project_no_cn\\GC_25\\code\\robot_control\\USER\\build\\Template\\GC_25.elf",
	//dap为调试名称，可以更改
        "name": "dap",
        "request": "launch",
        "type": "cortex-debug",
        "servertype": "openocd",
        //需要配置以下openocd文件路径，选择调试设备和目标
        "configFiles": [
            "F:\\vscode_debug\\openocd-0.12.0\\openocd-0.12.0\\tcl\\interface\\cmsis-dap.cfg",
            "F:\\vscode_debug\\openocd-0.12.0\\openocd-0.12.0\\tcl\\target\\stm32f4x.cfg"
        ],
        "searchDir": [],
        "runToEntryPoint": "main",
        "showDevDebugOutput": "none"
    },
```

#### SYSTEM文件

SYSTEM文件主要是负责底层芯片外设驱动。

* delay

`delay.c/.h`文件直接使用的正点原子的delay文件，这里不做过多叙述。

* usart+dma

`usart.c/.h`文件是配置串口1~6，串口2**重定向**到printf()函数。`dma.c/.h`文件是配置串口DMA，包括发送和接收。这两个文件共同组成了硬件通讯最底层。

下表为串口与硬件对应关系（rx_dma表示开启了串口的**dma接收**+串口**空闲中断**）

|  usart1  |  usart2  |   usart3   |  uart4  |  uart5  | usart6 |
| :------: | :------: | :--------: | :------: | :------: | :----: |
|  rx_dma  |  rx_dma  |    NULL    |   NULL   |   NULL   | rx_dma |
| 视觉模块 | 调试信息 | 机械臂电机 | 底盘电机 | 扫码模块 | 陀螺仪 |

* timer

`timer.c/.h`文件是配置系统定时器文件，主要存放定时器初始化函数。

下表为定时器和功能对应关系

|    tim2    |    tim4    |    tim5    |
| :---------: | :---------: | :--------: |
| 电机PWM控制 | 舵机PWM控制 | 系统时间戳 |

#### HARDWARE文件

HARDWARE文件主要负责硬件驱动和系统时间控制。

* hwt101_driver

`hwt101_driver.c/.h`文件是陀螺仪**HWT101**的驱动，对数据的解包均由中断函数 `USART6_IRQHandler`完成，最后存放于两个全局变量，加速度为 `hwt101_acceleration_data`变量，世界坐标角度为 `abs_degree_data`。另外还有常见的陀螺仪功能，如陀螺仪复位、校准和设置输出速率。

* motor_driver

`motor_driver.c/.h`文件时整车电机驱动，使用以下结构体描述电机：

```c
//电机控制结构体
typedef struct 
{
	char ID;		//电机ID
	char direction;	    	//电机运动方向
	char synchronous;	//同步设定
	int speed;		//速度
	int acc_speed;	    	//加速度
	int pulse_num;	    	//总脉冲数

} steeping_motor;
```

工程创建了7个全局变量**steeping_motor**结构体，用于描述整车7个电机。

```c
extern steeping_motor motor[7];
```

其中 `motor[0]~motor[3]`为**底盘**电机，`motor[4]`为**机械臂小臂**电机，`motor[5]`为**云台底座**电机，`motor[6]`为**机械臂大臂**电机。

另外，除基础的电机速度控制，位置控制外，motor_driver还具有设置电机**零点**，电机回零功能。选择对应的**电机结构体**，设定好结构体参数，**作为参数**传入相应的控制函数即可控制

* protocols

`protocols.c/.h`文件为通讯协议，主要功能处理与视觉模块通讯协议，扫码模块通讯协议~~和远程遥控协议~~。

```c
/**
*@brief 初始化软件数据包
*@param 无
*@return 无
*/
void soft_packet_init(void)
{
	head.next_usart_pack = &tail;
	tail.next_usart_pack = &head;

	pi_location_data.package_header = 'L';
	pi_location_data.package_tail = 'E';
	pi_location_data.packet = pi_location;
	pi_location_data.package_len = 20;
	pi_location_data.package_flag = rec_unfinish;
	create_usart_package(&pi_location_data);
}

```

以上代码创建了一个数据包，该数据包由'L'开头，‘E’结尾，最长20byte个长度。当串口空闲中断触发时，`packet_scan()`内自动索检包头包尾，当包头包尾有创建时，如接收到’L‘开头‘E’结尾数据包，会自动将该数据包的 `pi_location_data.package_flag = rec_finish`用户可以判断该标志位判断是否接收完成，当读取完之后，要手动将该数据包重新置为 `rec_unfinish`，实现以上功能主要使用到**链表**操作。

* scanner_driver

`scanner_driver.c/.h`为扫码模块驱动文件，扫码模块使用串口通讯，由于扫码内容是固定的类似与123+321的数据格式所以我们的驱动也很简单。

* servo_driver

`servo_driver.c/.h`为舵机驱动文件，比较简单，这里也不详细叙述。

* system_clock

`system_clock.c/.h`实现了系统时间戳功能，使用tim5定时中断，系统计时以10ms为步进，每次中断使变量 `sys_time_ms+=10`，然后把 `timer_10ms_flag=1`。若要以10ms运行一个任务，可以参考以下代码：

```c
while(1)
{
    if(timer_10ms_flag)
    {
        timer_10ms_flag = 0;
	task();
    }
}
```

`sys_time_ms`与 `timer_10ms_flag`均为全局变量，`sys_time_ms`是系统运行时间戳。

另外，该文件还实现了计时功能，计时器抽象为如下结构体：

```c
typedef struct _sys_count_time_t 
{
    char count_time_state;
    long long int start_time;
    long long int count_now_time;
    long long int count_end_time;

}sys_count_time_t;
```

若要使用计时功能，需要创建结构体，创建结构体时只需要设定 `count_end_time`的值，开始计时要调用库函数内的 `start_count_time`函数。**注意**，要定期调用 `count_time`函数与系统时间戳同步，调用越频繁计时越准确，最小精确到10ms。

system_clock还集成了时间戳格式化输出功能，可以格式化输出分钟和秒数。

#### KINEMTICS文件

KINEMTICS文件主要负责**运动学算法**的实现。

* chassis_control

`chassis_control.c/.h`负责底盘运动学解算，兼容麦克纳姆轮和全向轮。`set_car_speed_Omni_Wheel`是物流车坐标系下的物流车速度控制，`chassis_control_Omni_Wheel`是世界坐标系下的物流车速度控制，内有角度环控制、世界坐标系转换、全向轮转速转换。

全向轮物流车坐标系下速度解算如下所示

```c
motor[0].speed = - x + y - w;
motor[1].speed = - x + y + w;
motor[2].speed = x + y - w;
motor[3].speed = x + y + w;
```

麦轮不赘述。

* robot_arm_control

`robot_arm_control.c/.h`负责机械臂运动学解算，包含机械臂运动学正解算 `robot_arm_calculate_forward`，逆解算 `robot_arm_calculate_inverse`，机械臂底座控制robot_arm_control_botton_position，机械臂臂控制 `robot_arm_control_arm_poosition`。

另外宏定义还定义了以下机械臂规格，使用的时候可以修改

```
//机械臂关节减速比
#define reduction_ratio     4.0f

//机械臂规格，单位为mm
#define base_height         92.3f  //底座高度
#define first_arm_lenght    180.0f //第一个关节长度
#define second_arm_lenght   180.0f //第二个关节长度
#define gripper_lenght      0.0f   //机械爪长度
#define gripper_height      0.0f   //机械爪高度
```

* linear_interpolation

`linear_interpolation.c/.h`负责5阶插值算法的实现，文件以节点的形式描述。每个阶段有对应的输入参数、5阶方程解、节点队列分别用以下结构体表示：

```c
typedef struct _input_li5_t
{
    float t0; //开始时间
    float s0; //开始位移
    float v0; //开始速度
    float a0; //开始加速度

    float t1; //结束时间
    float s1; //结束加速度
    float v1; //结束速度
    float a1; //结束加速度

}input_li5_t;

typedef struct _output_li5_t
{
    //解算出五阶方程的7个参数
    float t0;
    float a0;
    float a1;
    float a2;
    float a3;
    float a4;
    float a5;

}output_li5_t;

typedef struct _li5_t
{
    float time;                     //节点时间
    float dis;                      //节点位移
    float vel;                      //节点速度
    float acc;                      //节点加速度

    void * task;                    //节点任务

    output_li5_t solve;             //5阶计算解算结果
}li5_t;w
```

文件中 `li5th_solve`根据推导出的公式求解每一个节点的函数的解，`get_displacement_from_li5`、`get_speed_from_li5`、`get_acceleration_from_li5`分别从已经求解的节点获得该时刻的位移，速度，加速度。

文件中 `solve_li5th_node`自动求解一个5阶插值队列（**li5_t**）的所有函数的解。

#### USER文件

最终调车所需要的文件都存在这里。

* path_control

`path_control.c/.h`是路径控制文件，该文件的存在是为了更快速的编辑物流车任务，达到增大调车效率的效果。`path_control`以队列的形式描述物流车任务。每个队列有单独的路径队列、有单独的路径计时器（获取5阶段插值的结果需要以时间为参数）、有单独的变量指示当前队列运行到什么位置。具体形式以结构体表示，如下所示：

```
typedef struct _path_quene_t
{
    li5_t* quene;           //路径队列
    sys_count_time_t timer; //当前路径节点计时器
    char p_node;            //当前路径节点

}path_quene_t;
```

基于以上结构体，我们可以创建出一个队列对象，控制x方向上的速度，如下所示：

```c
//---------------------------time------dis---speed-----acc--------------//
//创建x轴速度队列
static li5_t node_x_speed[]={{0.0f, 0.0f, 0.0f, 0.0f, NULL},        //出发到扫码区域
                             {1500.0f, 500.0f, 0.5f, 0.0f, NULL},
                             {3000.0f, 740.0f, 0.0f, 0.0f, NULL},
                             {.task =  qr_code_task},               //扫码区域到转盘区域
                             {3000.0f, 1000.0f, 0.0f, 0.0f, NULL},
                             {.task =  rotator_task},               //转盘区域到粗加工区域
                             {1500.0f, -550.0f, 0.0f, 0.0f, NULL},
                             {6000.0f, -450.0f, 0.0f, 0.0f, NULL},
                             {.task =  processing_area_task},       //粗加工区域到精加工区域
                             {2500.0f, 950.0f, 0.1f, 0.0f, NULL},
                             {4000.0f, 1000.0f, 0.0f, 0.0f, NULL},
                             {.task =  processing_area_task},       //精加工区到转盘
                             {2000.0f, -100.0f, -0.2f, 0.0f, NULL},
                             {4000.0f, -600.0f, 0.0f, 0.0f, NULL},
                             //二程
                             {.task =  rotator_task},               //转盘区域到粗加工区
                             {1500.0f, -550.0f, 0.0f, 0.0f, NULL},
                             {6000.0f, -450.0f, 0.0f, 0.0f, NULL},
                             {.task =  processing_area_task},       //粗加工区域到精加工区域
                             {2500.0f, 950.0f, 0.1f, 0.0f, NULL},
                             {4000.0f, 1000.0f, 0.0f, 0.0f, NULL},
                             //回家
                             {.task =  processing_area_task},
                             {6000.0f, -2000.0f, 0.0f, 0.0f, NULL},
                             {9000.0f, -2100.0f, 0.0f, 0.0f, NULL},
                             {.task =  &end_quene}};

static path_quene_t x_speed = {.quene = node_x_speed, .timer.count_end_time = 0xFFFFFFFF};
```

可以看到，以上创建了一个名为x_speed的结构体，包含了一个计时时间被设置为0XFFFFFFFF的定时器，还有一个node_x_speed的队列。需要注意的是，创建队列时，不能连续两次创建节点任务。

文件中 `chassis_path_control`是推演函数，推演每个时刻5阶插值的速度输出，函数具体内容较为复杂，感兴趣的可以看一下，函数具体用法如下所示（此段代码在main.c）：

```c
    while(1)
    {
        if(timer_10ms_flag)
        {
            timer_10ms_flag = 0;

            _x_speed = chassis_path_control(&x_speed, out_type_speed);
            _y_speed = chassis_path_control(&y_speed, out_type_speed);
            _w_dis = chassis_path_control(&w_dis, out_type_displacement);

            chassis_control_Omni_Wheel(_x_speed, _y_speed, _w_dis);
        }
    }
```

* motion

`motion.c/.h`文件是除底盘外动作文件，因为校赛时间原因，该文件写的比较乱，参考一下即可，是待优化的代码。

* main

`main.c/.h`文件有着程序的**入口**，程序开始先进行各种设备的初始化，接着创建三个方向的插值速度队列，接着求解三个方向每个节点的解，最后每**10ms推演一次**路径。每推演一次路径解算一次该时刻三个方向的队列的速度，若推演到该节点有任务，则优先完成该节点的任务。整个程序以底盘运动三轴速度为主线，其中添加各种任务节点。

在使用的时候，比如需要调整x速度队列某个节点的速度，我们只需要往node_x_speed结构体数组**添加、修改、删除节点**即可。添加任务时，节点可单独使用.task成员指向对应函数，即可在这个节点添加一个自定义任务。

* ~~remote_car_control~~

已弃用，不做介绍。

---

### TODO

config.h文件编写

~~灯光板（完成）~~

电滑环

电滑环板

分线板v2

~~泰山派拓展版（完成）~~

定位算法速度优化
