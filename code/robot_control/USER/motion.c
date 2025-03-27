#include "motion.h"

//扫码数据
char * qr_dara = NULL;
//机械臂保持角度
float _abs_robot_arm_angle = 0;

//物料盘坐标
float disk_item_position[3][3] = {disk_num_3_position, 
                                  disk_num_2_position, 
                                  disk_num_1_position};

//转盘坐标
float rotator_item_position[3][3] = {rotator_num_1_position, 
                                     rotator_num_2_position, 
                                     rotator_num_3_position};

float ground_item_position[6][3] = {ground_num_3_position, 
                                    ground_num_2_position, 
                                    ground_num_1_position,
                                    ground_num_3_second_floor_position,
                                    ground_num_2_second_floor_position,
                                    ground_num_1_second_floor_position};

/**
 * @brief 获取扫码数据
 * @param 无
 * @return 无
 */
void qr_code_task(void)
{
    static char out_data[20] = {0};

    stop_Omni_Wheel();

    while(qr_dara == NULL)
    {
        qr_dara = get_scanner_data();
    }

    sprintf(out_data, "@%s#", qr_dara);

    // 重复发送防止丢包
    for(char j = 0; j < 10; j++)
    {
        for(char i = 0; i < 9; i++)
        {
            while((USART1->SR&0X40)==0);
            USART1->DR = out_data[i]; 
        }
    }

    printf("scan:%s\r\n", qr_dara);
    _abs_robot_arm_angle = 45.0f;
}

/**
 * @brief 从转盘拿物料
 * @param rotator_num:从视觉接收的转盘位置号码
 * @return 无
 */
void grab_item_on_rotator_plus(uint8_t rotator_num)
{
    rotator_num = rotator_num - '1';

    open_claw();
    robot_arm_control_yz(rotator_item_position[rotator_num][0], 
                         rotator_item_position[rotator_num][1]);
    robot_arm_control_botton_position(rotator_item_position[rotator_num][2]);
    delay_ms(500);
    close_claw();
    delay_ms(300);

    robot_arm_control_yz(210.0f, 280.0f);   //!!!!!
    delay_ms(500);
        // robot_arm_set_middle_position();
    robot_arm_control_botton_position(0.0f);
}

/**
 * @brief 从物料盘拿物料
 * @param item_color:需要放置的颜色
 * @return 无
 */
void garb_item_on_disk_plus(uint8_t item_color)
{
    item_color = item_color - '1';

    open_claw_small();
    robot_arm_control_botton_position(disk_item_position[item_color][2]);
    robot_arm_control_yz(disk_item_position[item_color][0], disk_item_position[item_color][1] + disk_hight);
    delay_ms(800);
    robot_arm_control_yz(disk_item_position[item_color][0], disk_item_position[item_color][1]);
    delay_ms(500);
    close_claw();
    delay_ms(500);
    robot_arm_control_yz(disk_item_position[item_color][0], disk_item_position[item_color][1] + 150.0f);
}

/**
 * @brief 从地面抓取物料
 * @param item_color:需要抓取的颜色
 * @return 无
 */
void garb_item_on_guound_plus(uint8_t item_color)
{
    item_color = item_color - '1';

    open_claw_small();
    robot_arm_control_botton_position(ground_item_position[item_color][2]);
    robot_arm_control_yz(ground_item_position[item_color][0], ground_item_position[item_color][1]);
    delay_ms(1000);
    close_claw();
    delay_ms(300);
    robot_arm_control_yz(ground_item_position[item_color][0], ground_item_position[item_color][1] + 150.0f);
}

/**
 * @brief 把物料盘放到地上
 * @param item_color:需要放置的颜色
 * @param second_floor:是否放置到二楼 0：一楼 3：二楼
 * @return 无
 */
void place_item_on_ground(uint8_t item_color, char second_floor)
{
    item_color = item_color - '1' + second_floor;

    robot_arm_control_botton_position(ground_item_position[item_color][2]);
    robot_arm_control_yz(ground_item_position[item_color][0], ground_item_position[item_color][1] + 20.0f);
    delay_ms(1200);
    gimbal_motor_set_speed_acc(1, 1);
    robot_arm_control_yz(ground_item_position[item_color][0], ground_item_position[item_color][1]);
    gimbal_motor_set_speed_acc(500, 100);
    delay_ms(1000);
    open_claw_small();
    delay_ms(200);
    robot_arm_control_yz(ground_item_position[item_color][0] - 50.0f, ground_item_position[item_color][1] + 50.0f);
    delay_ms(500);
    robot_arm_set_middle_position();
}

/**
 * @brief 物料放入物料盘
 * @param item_color:夹取物料颜色
 * @return 无
 */
void place_item_on_disk(uint8_t item_color)
{
    item_color = item_color - '1';

    robot_arm_control_botton_position(disk_item_position[item_color][2]);
    delay_ms(200);
    robot_arm_control_yz(disk_item_position[item_color][0], disk_item_position[item_color][1] + disk_hight + 20.0f);
    delay_ms(800);
    robot_arm_control_yz(disk_item_position[item_color][0], disk_item_position[item_color][1]);
    delay_ms(500);
    open_claw_small();
    delay_ms(300);
    robot_arm_control_yz(disk_item_position[item_color][0], disk_item_position[item_color][1] + disk_hight + 40.0f);
    delay_ms(400);
    robot_arm_set_middle_position();
    delay_ms(800);
}

/**
 * @brief 圆盘区快速定位
 * @param 无
 * @return 无
 */
void rotator_location_task(void)
{
    int* location_data = NULL;
    int err1 = 0;
    int err2 = 0;

    //定位直角
    robot_arm_control_botton_position(135.0f);
    delay_ms(1000);
    robot_arm_control_yz(232.0f, 220.0f);
    delay_ms(1000);

    //清除接收标志位
    get_pi_location_data();

    //角度定位
    while(1)
    {
        location_data = NULL;
        while(location_data == NULL)
        {
            call_pi(location);
            
            location_data = get_pi_location_data();
        }

        err1 =  location_data[0] * 0.05f;

        if(err1 > 20) err1 = 20;
        else if(err1 < -20) err1  = -20;

        set_car_speed_Omni_Wheel(0, 0, err1);
        if(abs(location_data[0]) < degree_location_dead_area) break;
    }

    //清除接收标志位
    get_pi_location_data();

    //xy平面定位
    while(1)
    {
        location_data = NULL;
        
        while(location_data == NULL)
        {
            call_pi(location);
            location_data = get_pi_location_data();
        }

        err1 = -(rotator_targe_x - location_data[1]) * 0.2f;
        err2 = (rotator_targe_y - location_data[2]) * 0.2f;

        set_car_speed_Omni_Wheel_diagonal(err1, err2);

        if((abs(rotator_targe_x - location_data[1]) < first_location_dead_area) &&
            (abs(rotator_targe_y - location_data[2]) < first_location_dead_area))
        {
            stop_Omni_Wheel();
                break;
        }
    }
}

/**
 * @brief 加工区快速定位
 * @param enable_circle_location:是否启用圆环定位
 * @return 无
 */
void processing_location_task(char enable_circle_location)
{
    static float Ki_1 = 0, Ki_2 = 0;
    static float last_err1 = 0, last_err2 = 0;
    float Kp_1 = 0, Kp_2 = 0;
    int* location_data = NULL;
    float err1 = 0;
    float err2 = 0;

    //定位直角动作
    robot_arm_control_botton_position(135.0f);
    delay_ms(1000);
    robot_arm_control_yz(232.0f, 220.0f);
    delay_ms(2000);

    //角度定位
    while(1)
    {
        location_data = NULL;
        while(location_data == NULL)
        {
            call_pi(location);
            location_data = get_pi_location_data();
        }

        err1 =  location_data[0] * 0.08f;

        if(err1 > 20) err1 = 20;
        else if(err1 < -20) err1  = -20;

        set_car_speed_Omni_Wheel(0, 0, err1);
        if(abs(location_data[0]) < degree_location_dead_area) break;
    }

    //xy平面定位
    while(1)
    {
        location_data = NULL;
        
        while(location_data == NULL)
        {
            call_pi(location);
            location_data = get_pi_location_data();
        }

        if(enable_circle_location == ENABLE)
        {
            if((abs(process_targe_x_first - location_data[1]) < first_location_dead_area) &&
            (abs(process_targe_y_first - location_data[2]) < first_location_dead_area))
            {
                stop_Omni_Wheel();
                    break;
            }
            
            err1 = -(process_targe_x_first - location_data[1]) * 0.2f;
            err2 = (process_targe_y_first - location_data[2]) * 0.2f;
        }
        else
        {
            if((abs(process_targe_x_first_height - location_data[1]) < first_location_dead_area) &&
            (abs(process_targe_y_first_height - location_data[2]) < first_location_dead_area))
            {
                stop_Omni_Wheel();
                    break;
            }
            
            err1 = -(process_targe_x_first_height - location_data[1]) * 0.2f;
            err2 = (process_targe_y_first_height - location_data[2]) * 0.2f;
        }


        set_car_speed_Omni_Wheel_diagonal(err1, err2);
    }

    //角度定位
    while(1)
    {
        location_data = NULL;
        while(location_data == NULL)
        {
            call_pi(location);
            location_data = get_pi_location_data();
        }

        err1 =  location_data[0] * 0.08f;

        if(err1 > 20) err1 = 20;
        else if(err1 < -20) err1  = -20;

        set_car_speed_Omni_Wheel(0, 0, err1);
        if(abs(location_data[0]) < degree_location_dead_area) break;
    }

    //精定位
    if(enable_circle_location == DISABLE)
    {
        robot_arm_control_botton_position(135.0f);
        robot_arm_control_yz(232.0f, 100.0f);
        delay_ms(1000);

        //xy平面定位
        get_pi_location_data();
        while(1)
        {
            location_data = NULL;
            
            while(location_data == NULL)
            {
                call_pi(location);
                location_data = get_pi_location_data();
            }

            if((abs(process_targe_x_height - location_data[1]) < first_location_dead_area) &&
            (abs(process_targe_y_height - location_data[2]) < first_location_dead_area))
            {
                stop_Omni_Wheel();
                    break;
            }
            
            err1 = -(process_targe_x_height - location_data[1]) * 0.2f;
            err2 = (process_targe_y_height - location_data[2]) * 0.2f;

            set_car_speed_Omni_Wheel_diagonal(err1, err2);
        }

        robot_arm_set_middle_position();
        delay_ms(2000);

        return;
    }

    //圆环定位动作
    robot_arm_control_botton_position(0.0f);
    robot_arm_control_yz(193.4f, 80.0f);
    delay_ms(2000);

    //圆环定位
    circle_location:
    while(1)
    {
        location_data = NULL;

        while(location_data == NULL)
        {
            call_pi(circle);
            location_data = get_pi_location_data();
        }

        if((abs(process_targe_x_second - location_data[1]) < second_location_dead_area) &&
           (abs(process_targe_y_second - location_data[2]) < second_location_dead_area))
        {
            stop_Omni_Wheel();
            break;
        }

        err1 = (float)(process_targe_y_second - location_data[2]);
        err2 = -(float)(process_targe_x_second - location_data[1]);

        Kp_1 = err1 * 0.055f;
        Kp_2 = err2 * 0.055f;

        Ki_1 += err1 * 0.008f;
        Ki_2 += err2 * 0.008f;

        //比例限幅
        if(Kp_1 > circle_locatin_limit) Kp_1 = circle_locatin_limit;
        else if(Kp_1 < -circle_locatin_limit) Kp_1 = -circle_locatin_limit;  
        if(Kp_2 > circle_locatin_limit) Kp_2 = circle_locatin_limit;
        else if(Kp_2 < -circle_locatin_limit) Kp_2 = -circle_locatin_limit;  

        //积分限幅
        // if(Ki_1 > 1) Ki_1 = 1;
        // else if(Ki_1 < -1) Ki_1 = -1;
        // if(Ki_2 > 1) Ki_2 = 1;
        // else if(Ki_2 < -1) Ki_2 = -1;

        //积分清零
        if((err1 < 0) && (last_err1 > 0)) Ki_1 = 0;
        else if((err1 > 0) && (last_err1 < 0)) Ki_1 = 0;
        if((err2 < 0) && (last_err2 > 0)) Ki_2 = 0;
        else if((err2 > 0) && (last_err2 < 0)) Ki_2 = 0;

        set_car_speed_Omni_Wheel(Kp_1 + Ki_1, Kp_2 + Ki_2, 0);

        last_err1 = err1;
        last_err2 = err2;
    }
    
    //清除缓冲区
    get_pi_location_data();
    delay_ms(500);
    //校验
    location_data = NULL;
    while(location_data == NULL)
    {
        call_pi(circle);
        location_data = get_pi_location_data();
    }

    if((abs(process_targe_x_second - location_data[1]) > second_location_dead_area) ||
       (abs(process_targe_y_second - location_data[2]) > second_location_dead_area))
    {
        //重新圆环定位
        goto circle_location;
    }
}

/**
 * @brief 转盘区域任务更新
 * @param 无
 * @return 无
 */
void rotator_task_plus(void)
{
    static char count_time = 0;
    char p_qr_data_packge = 0;
    u8 *color_area_data;

    stop_Omni_Wheel();
    gimbal_motor_set_speed_acc(500, 200);
    rotator_location_task();

    //摆到物料识别位置
    robot_arm_control_botton_position(0.0f);
    robot_arm_control_yz(210.0f, 280.0f);
    delay_ms(2000);

    //物料识别
    while(1)
    {
        call_pi(color);
        delay_ms(50);
        color_area_data = get_pi_color_area_data();
        if(color_area_data != NULL) break;
    }

    delay_ms(500);

    //第二段二维码读数
    if(count_time == 1) p_qr_data_packge = 4;

    grab_item_on_rotator_plus(color_area_data[qr_data_packge[0 + p_qr_data_packge] - '1']);
    place_item_on_disk(qr_data_packge[0 + p_qr_data_packge]);
    grab_item_on_rotator_plus(color_area_data[qr_data_packge[1 + p_qr_data_packge] - '1']);
    place_item_on_disk(qr_data_packge[1 + p_qr_data_packge]);
    delay_ms(500);

    //清除缓冲
    get_pi_color_area_data();

    //位置检测
    while(1)
    {
        call_pi(position);
        delay_ms(50);
        color_area_data = get_pi_color_area_data();
        if(color_area_data != NULL) break;
    }

    grab_item_on_rotator_plus(color_area_data[qr_data_packge[2 + p_qr_data_packge] - '1']);
    place_item_on_disk(qr_data_packge[2 + p_qr_data_packge]);

    robot_arm_reset_yz();

    _abs_robot_arm_angle = 225.0f;
    count_time++;
}

/**
 * @brief 加工区域任务更新
 * @param 无
 * @return 无
 */
void processing_task_plus(void)
{
    static char count_time = 0;
    char p_qr_data_packge = 0;

    stop_Omni_Wheel();

    gimbal_motor_set_speed_acc(500, 100);

    if(count_time == 3)
        processing_location_task(DISABLE);
    else
        processing_location_task(ENABLE);

    robot_arm_set_middle_position();

    if(count_time > 1) p_qr_data_packge = 4;
    
    //放置三个物料
    for(char i = 0;i < 3;i++)
    {
        delay_ms(500);
        garb_item_on_disk_plus(qr_data_packge[i + p_qr_data_packge]);
        delay_ms(500);

        if(count_time == 3)
            place_item_on_ground(qr_data_packge[i + p_qr_data_packge], 3);
        else 
        place_item_on_ground(qr_data_packge[i + p_qr_data_packge], 0);
    }

    delay_ms(1000);

    //粗加工区域
    if(count_time == 0 || count_time == 2)
    {
        //夹取三个物料
        for(char i = 0;i < 3;i++)
        {
            garb_item_on_guound_plus(qr_data_packge[i + p_qr_data_packge]);
            delay_ms(500);
            place_item_on_disk(qr_data_packge[i + p_qr_data_packge]);
        }
    }

    robot_arm_reset_yz();

    if((count_time == 0) || (count_time == 2)) 
        _abs_robot_arm_angle = 135.0f;
    else if(count_time == 1)  
        _abs_robot_arm_angle = 45.0f;
    else if(count_time == 3)
        _abs_robot_arm_angle = 0.0f;

    count_time++;
}

