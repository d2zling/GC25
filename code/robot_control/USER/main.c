#include "main.h"

/**
*@note 该工程启用了FPU加速浮点运算
*@note 使用DSP库加速部分运算
*/

/**
*@brief  延时任务，调试时使用，节点延时一定时间
*@param 无
*@return 无
*/
void task_delay(void)
{
    stop_Omni_Wheel();
    robot_arm_control_botton_speed(0);
    delay_ms(2000);
}

/**
*@brief  阻塞任务，调试时使用，阻塞整个程序
*@param 无
*@return 无
*/
void task_while()
{
    stop_Omni_Wheel();
    while(1);
}

/**
*@brief  空任务，规划路径时占位
*@param 无
*@return 无
*/
void void_task(void){};

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断优先级分组
    delay_init(168);
    system_clock_init();
    protocols_init();

    chassis_control_init();
	robot_arm_control_init();
    scanner_init();

    delay_ms(2000);

    robot_arm_reset_yz();

    gimbal_motor_set_speed_acc(500, 100);

#if robot_arm_demonstration

    get_motor_angle(motor[4]);
    get_motor_angle(motor[5]);
    get_motor_angle(motor[6]);

    delay_ms(1000);

    robot_arm_pow_control(motor[4],DISABLE);
    robot_arm_pow_control(motor[5],DISABLE);
    robot_arm_pow_control(motor[6],DISABLE);

    while(1)
    {
        robot_arm_get_position();
        delay_ms(500);
    }

#endif
/*

        ^
        |x
        |
        ---->y

*/
//---------------------------time------dis---speed-----acc--------------//
    //创建x轴速度队列
    static li5_t node_x_speed[]={{0.0f, 0.0f, 0.0f, 0.0f, NULL},        //出发到扫码区域
                                 {1500.0f, 450.0f, 0.5f, 0.0f, NULL},
                                 {3000.0f, 640.0f, 0.0f, 0.0f, NULL},
                                 {.task =  qr_code_task},                 //扫码区域到转盘区域
                                 {3000.0f, 900.0f, 0.0f, 0.0f, NULL},

                                 {.task =  rotator_task_plus},               //转盘区域到粗加工区域
                                 {1500.0f, -450.0f, 0.0f, 0.0f, NULL},
                                 {6000.0f, -350.0f, 0.0f, 0.0f, NULL},
                                 {.task =  processing_task_plus},       //粗加工区域到精加工区域
                                 {2500.0f, 850.0f, 0.1f, 0.0f, NULL},
                                 {4000.0f, 900.0f, 0.0f, 0.0f, NULL},

                                 {.task =  processing_task_plus},       //精加工区到转盘
                                 {2000.0f, -100.0f, -0.2f, 0.0f, NULL},
                                 {4000.0f, -500.0f, 0.0f, 0.0f, NULL},

                                 //二程
                                 {.task =  rotator_task_plus},               //转盘区域到粗加工区
                                 {1500.0f, -450.0f, 0.0f, 0.0f, NULL},
                                 {6000.0f, -350.0f, 0.0f, 0.0f, NULL},
                                 {.task =  processing_task_plus},       //粗加工区域到精加工区域
                                 {2500.0f, 850.0f, 0.1f, 0.0f, NULL},
                                 {4000.0f, 900.0f, 0.0f, 0.0f, NULL},

                                 //回家
                                 {.task =  processing_task_plus},
                                 {6000.0f, -2000.0f, 0.0f, 0.0f, NULL},
                                 {9000.0f, -2200.0f, 0.0f, 0.0f, NULL},
                                 {.task =  &end_quene}};

    // //创建y轴速度队列
    static li5_t node_y_speed[]={{0.0f, 0.0f, 0.0f, 0.0f, NULL},        //出发到扫码区域
                                 {1500.0f, -200.0f, -0.1f, 0.0f, NULL},
                                 {3000.0f, -150.0f, 0.0f, 0.0f, NULL},
                                 {.task =  void_task},                  //扫码区域到转盘区域
                                 {3000.0f, 0.0f, 0.0f, 0.0f, NULL},

                                 {.task =  void_task},                  //转盘区域到粗加工区域
                                 {1500.0f, -200.0f, 0.0f, 0.0f, NULL},
                                 {6000.0f, -1900.0f, 0.0f, 0.0f, NULL},
                                 {.task =  void_task},                  //粗加工区域到精加工区域
                                 {2000.0f, 100.0f, 0.2f, 0.0f, NULL},
                                 {4000.0f, 950.0f, 0.0f, 0.0f, NULL},

                                 {.task =  void_task},                  //精加工区到转盘
                                 {2500.0f, 950.0f, 0.1f, 0.0f, NULL},
                                 {4000.0f, 1050.0f, 0.0f, 0.0f, NULL},

                                 //二程
                                 {.task =  void_task},                  //转盘区域到粗加工区
                                 {1500.0f, -200.0f, 0.0f, 0.0f, NULL},
                                 {6000.0f, -1900.0f, 0.0f, 0.0f, NULL},
                                 {.task =  void_task},                  //粗加工区域到精加工区域
                                 {2000.0f, 100.0f, 0.2f, 0.0f, NULL},
                                 {4000.0f, 950.0f, 0.0f, 0.0f, NULL},

                                 //回家
                                 {.task =  void_task},                  
                                 {6000.0f, 0.0f, 0.0f, 0.0f, NULL},        
                                 {9000.0f, 1300.0f, 0.0f, 0.0f, NULL},
                                 {.task =  &end_quene}};

    // //创建w轴速度队列
    static li5_t node_w_dis[] ={{0.0f, 0.0f, 0.0f, 0.0f, NULL},         //出发到扫码区域
                                {2500.0f, 0.0f, 0.0f, 0.0f, NULL},
                                {3000.0f, 0.0f, 0.0f, 0.0f, NULL},
                                {.dis= 0.0f, .task =  void_task},       //扫码区域到转盘区域
                                {3000.0f, -90.0f, 0.0f, 0.0f, NULL},
                                {.dis= -90.0f, .task =  void_task},     //转盘区域到粗加工区域
                                {1500.0f, -90.0f, 0.0f, 0.0f, NULL},
                                {6000.0f, 90.0f, 0.0f, 0.0f, NULL},
                                {.dis= 90.0f,.task =  void_task},       //粗加工区域到精加工区域
                                {2000.0f, 45.0f, 0.0f, 0.0f, NULL},
                                {4000.0f, 0.0f, 0.0f, 0.0f, NULL},
                                {.dis= 0.0f, .task =  void_task},       //精加工区到转盘
                                {2000.0f, -45.0f, 0.0f, 0.0f, NULL},
                                {4000.0f, -90.0f, 0.0f, 0.0f, NULL},
                                //二程
                                {.dis= -90.0f, .task =  void_task},     //转盘区域到粗加工区
                                {1500.0f, -90.0f, 0.0f, 0.0f, NULL},
                                {6000.0f, 90.0f, 0.0f, 0.0f, NULL},
                                {.dis= 90.0f,.task =  void_task},       //粗加工区域到精加工区域
                                {2000.0f, 45.0f, 0.0f, 0.0f, NULL},
                                {4000.0f, 0.0f, 0.0f, 0.0f, NULL},
                                //回家
                                {.task =  void_task},                    
                                {6000.0f, -90.0f, 0.0f, 0.0f, NULL},
                                {9000.0f, -90.0f, 0.0f, 0.0f, NULL},
                                {.dis= -90.0f,.task =  task_while},       
                                {.task =  &end_quene}};

//-------------------------------------end------------------------------//

    static path_quene_t x_speed = {.quene = node_x_speed,
                                   .timer.count_end_time = 0xFFFFFFFF};

    static path_quene_t y_speed = {.quene = node_y_speed,
                                   .timer.count_end_time = 0xFFFFFFFF};

    static path_quene_t w_dis = {.quene = node_w_dis,
                                 .timer.count_end_time = 0xFFFFFFFF};

    solve_li5th_node(x_speed.quene);
    solve_li5th_node(y_speed.quene);
    solve_li5th_node(w_dis.quene);

    start_count_time(&x_speed.timer);
    start_count_time(&y_speed.timer);
    start_count_time(&w_dis.timer);

    static float _x_speed = 0.0f, _y_speed = 0.0f, _w_dis = 0.0f;

    while(1)
    {
        if(timer_10ms_flag)
        {
            timer_10ms_flag = 0;

            _x_speed = chassis_path_control(&x_speed, out_type_speed);
            _y_speed = chassis_path_control(&y_speed, out_type_speed);
            _w_dis = chassis_path_control(&w_dis, out_type_displacement);

            chassis_control_Omni_Wheel(_x_speed, _y_speed, _w_dis);
            robot_arm_control_botton_speed_CL(_abs_robot_arm_angle);
        }
    }
}
