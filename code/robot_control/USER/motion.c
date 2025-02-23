#include "motion.h"

/**
 * @brief 从物料盘取物料,弃用
 * @param disk_num 物料盘编号
 * @return 无
 */
void grab_item_on_disk(char disk_num)
{
    float degree[3];
    open_claw_small();

    if(disk_num == 0)
    {
        robot_arm_control_botton_position(-42.0f);
    }
    else if(disk_num == 2)
    {
        robot_arm_control_botton_position(42.0f);
    }

    open_claw_small();

    robot_arm_calculate_inverse(95.0f, 175.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    delay_ms(400);
    close_claw();
    delay_ms(200);
    robot_arm_calculate_inverse(95.0f, 215.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    delay_ms(450);
    robot_arm_calculate_inverse(150.0f, 220.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    delay_ms(300);
    
    open_claw_small();
}

/**
 * @brief 从转盘拿取物料
 * @param rotator_num 转盘编号
 * @return 无
 */
void grab_item_on_rotator(char rotator_num)
{
    static char disk_flag = 0;
    float degree[3];
    open_claw();

    if(rotator_num == '1')
    {
        robot_arm_control_botton_position(-14.0f);
        delay_ms(300);
        robot_arm_calculate_inverse(340.0f, 180.0f , degree);
        robot_arm_control_arm_poosition(degree[0], degree[1]);
        delay_ms(1000);
    }
    if(rotator_num == '2')
    {
        robot_arm_control_botton_position(14.0f);
        delay_ms(300);
        robot_arm_calculate_inverse(340.0f, 180.0f , degree);
        robot_arm_control_arm_poosition(degree[0], degree[1]);
        delay_ms(1000);
    }
    else if(rotator_num == '3')
    {
        robot_arm_control_botton_position(0.0f);
        delay_ms(300);
        robot_arm_calculate_inverse(185.0f, 170.0f , degree);
        robot_arm_control_arm_poosition(degree[0], degree[1]);
        delay_ms(1000);
    }

    close_claw();

    delay_ms(300);

    robot_arm_calculate_inverse(150.0f, 310.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);

    if(disk_flag == 0)
    {
        robot_arm_control_botton_position(41.0f);
        disk_flag++;
    }
    else if(disk_flag == 1)
    {
        robot_arm_control_botton_position(0.0f);
        disk_flag++;
    }
    else if(disk_flag == 2)
    {
        robot_arm_control_botton_position(-41.0f);
        disk_flag = 0;
    }

    delay_ms(700);
    robot_arm_calculate_inverse(92.0f, 210.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);

    delay_ms(700);
    robot_arm_calculate_inverse(92.0f, 190.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);

    delay_ms(200);
    open_claw_small();

    delay_ms(200);
    robot_arm_calculate_inverse(140.0f, 310.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
}

/**
 * @brief 获取扫码数据
 * @param 无
 * @return 无
 */
void qr_code_task(void)
{
    char * qr_dara = NULL;
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
        delay_ms(10);
    }

    printf("scan:%s\r\n", qr_dara);
}

/**
 * @brief 转盘区域任务
 * @param 无
 * @return 无
 */
void rotator_task(void)
{
    static char count_task_time = 0;

    int x, y, w, _x, _y;
    int* location_data;
    float degree[2];
    u8 *color_area_data;

    stop_Omni_Wheel();
    robot_arm_calculate_inverse(220, 250, degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    robot_arm_control_botton_position(-135.0f);
    
    for(char i = 0; i < 60; i++)
    {
        call_pi(location);
        delay_ms(50);
    }

    //粗定位
    while(1)
    {
        call_pi(location);
        delay_ms(50);
        location_data = get_pi_location_data();

        if(location_data != NULL)
        {
            x = rotator_first_location_x_target - location_data[1];
            y = rotator_first_location_y_targe - location_data[2];

            //检测定位完成
            if((abs(x) < first_location_dead_area) && 
               (abs(y) < first_location_dead_area) && 
               (abs(location_data[0]) < first_location_dead_area))
            {
                stop_Omni_Wheel();
                delay_ms(1000);
                break;
            }

            if(location_data[0] > 0) w = 3;
            else if (location_data[0] < 0) w = -3;
            else w = 0;

            if(y > 0)
                _x = _y = -3;
            else if(y < 0)
                _x = _y = 3;

            if(x > 0)
            {
                _x += -3;
                _y += 3;
            }
            else if(x < 0)
            {
                _x += 3;
                _y += -3; 
            }

            set_car_speed_Omni_Wheel(_x, _y, w);
        }
        else stop_Omni_Wheel();
    }

    robot_arm_calculate_inverse(250, 150, degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);

    for(char i = 0; i < 60; i++)
    {
        call_pi(location);
        delay_ms(50);
    }

    //精定位
    while(1)
    {
        call_pi(location);
        delay_ms(50);
        location_data = get_pi_location_data();

        if(location_data != NULL)
        {
            x = rotator_second_location_x_target - location_data[1];
            y = rotator_second_location_y_target - location_data[2];

            //检测定位完成
            if((abs(x) < second_location_dead_area) && 
               (abs(y) < second_location_dead_area) && 
               (abs(location_data[0]) < second_location_dead_area))
            {
                printf("%d %d %d\r\n", x, y, w);
                stop_Omni_Wheel();
                robot_arm_calculate_inverse(second_arm_lenght, first_arm_lenght + base_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                robot_arm_control_botton_position(0.0f);
                break;
            }

            if(location_data[0] > 0) w = 1;
            else if (location_data[0] < 0) w = -1;
            else w = 0;

            if(y > 0)
                _x = _y = -1;
            else if(y < 0)
                _x = _y = 1;

            if(x > 0)
            {
                _x += -1;
                _y += 1;
            }
            else if(x < 0)
            {
                _x += 1;
                _y += -1; 
            }

            //限幅
            if(_x > 1) _x = 1;
            else if(_x < -1) _x = -1;

            if(_y > 1) _y = 1;
            else if(_y < -1) _y = -1;

            set_car_speed_Omni_Wheel(_x, _y, w);
        }
        else stop_Omni_Wheel();
    }

    robot_arm_calculate_inverse(170.0f, 320.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    robot_arm_control_botton_position(0.0f);
    delay_ms(1000);

    //抓取
    while(1)
    {
        call_pi(color);
        delay_ms(50);
        color_area_data = get_pi_color_area_data();
        if(color_area_data != NULL) break;
    }

    grab_item_on_rotator(color_area_data[qr_data_packge[count_task_time    ] - '1']);
    grab_item_on_rotator(color_area_data[qr_data_packge[count_task_time + 1] - '1']);

    robot_arm_calculate_inverse(170.0f, 320.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    robot_arm_control_botton_position(0.0f);
    delay_ms(1000);

    while(1)
    {
        call_pi(position);
        delay_ms(50);
        color_area_data = get_pi_color_area_data();
        if(color_area_data != NULL) break;
    }
    
    printf("%s\r\n", color_area_data);
    grab_item_on_rotator(color_area_data[qr_data_packge[count_task_time + 2] - '1']);

    count_task_time = count_task_time + 4;

    robot_arm_control_botton_position(41.0f);
    robot_arm_calculate_inverse(220, 250, degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    open_claw_small();
}

/**
 * @brief 加工区域任务
 * @param 无
 * @return 无
 */
void processing_area_task(void)
{
    static char tasks_times = 0;
    static char qr_times = 0;
    
    int x, y, w, _x, _y;
    int* location_data;
    float degree[2];
    float item_height = 0;

    if(tasks_times > 1) qr_times = 4;
    if(tasks_times == 3) item_height = 70;
    stop_Omni_Wheel();
    open_claw_small();

    //夹物料
    robot_arm_control_botton_position(41.0f);
    robot_arm_calculate_inverse(150.0f, 210.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    delay_ms(500);
    robot_arm_calculate_inverse(90.0f, 180.0f , degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    delay_ms(500);
    close_claw();
    delay_ms(500);
    robot_arm_calculate_inverse(90.0f, 210, degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    delay_ms(500);
    robot_arm_calculate_inverse(220, 250, degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    robot_arm_control_botton_position(-135.0f);

    for(char i = 0; i < 30; i++)
    {
        call_pi(location);
        delay_ms(50);
    }

    //粗定位
    while(1)
    {
        call_pi(location);
        delay_ms(50);
        location_data = get_pi_location_data();

        if(location_data != NULL)
        {
            x = processing_area_first_location_x_targe - location_data[1];
            y = processing_area_first_location_y_targe - location_data[2];

            //检测定位完成
            if((abs(x) < first_location_dead_area) && 
               (abs(y) < first_location_dead_area) && 
               (abs(location_data[0]) < first_location_dead_area))
            {
                printf("location finish!\r\n");
                stop_Omni_Wheel();
                delay_ms(1000);
                break;
            }

            if(location_data[0] > 0) w = 3;
            else if (location_data[0] < 0) w = -3;
            else w = 0;

            if(y > 0)
                _x = _y = -3;
            else if(y < 0)
                _x = _y = 3;

            if(x > 0)
            {
                _x += -3;
                _y += 3;
            }
            else if(x < 0)
            {
                _x += 3;
                _y += -3; 
            }

            set_car_speed_Omni_Wheel(_x, _y, w);
        }
        else stop_Omni_Wheel();
    }

    robot_arm_calculate_inverse(220, 100, degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);

    for(char i = 0; i < 30; i++)
    {
        call_pi(location);
        delay_ms(50);
    }

    //精定位
    while(1)
    {
        call_pi(location);
        delay_ms(50);
        location_data = get_pi_location_data();

        if(location_data != NULL)
        {
            x = processing_area_second_location_x_targe - location_data[1];
            y = processing_area_second_location_y_targe - location_data[2];

            //检测定位完成
            if((abs(x) < second_location_dead_area) && 
               (abs(y) < second_location_dead_area) && 
               (abs(location_data[0]) < second_location_dead_area))
            {
                printf("%d %d %d\r\n", x, y, w);
                stop_Omni_Wheel();
                robot_arm_control_botton_position(0.0f);
                robot_arm_calculate_inverse(second_arm_lenght, first_arm_lenght + base_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                break;
            }

            if(location_data[0] > 0) w = 1;
            else if (location_data[0] < 0) w = -1;
            else w = 0;

            if(y > 0)
                _x = _y = -1;
            else if(y < 0)
                _x = _y = 1;

            if(x > 0)
            {
                _x += -1;
                _y += 1;
            }
            else if(x < 0)
            {
                _x += 1;
                _y += -1; 
            }

            //限幅
            if(_x > 1) _x = 1;
            else if(_x < -1) _x = -1;

            if(_y > 1) _y = 1;
            else if(_y < -1) _y = -1;

            set_car_speed_Omni_Wheel(_x, _y, w);
        }
        else stop_Omni_Wheel();
    }

    //粗加工区域定位数据
    if((tasks_times == 0) || (tasks_times == 2))
    {
        for(char i = 0; i < 3; i++)
        {
            //夹起第二个物料
            if(i == 1)
            {
                robot_arm_control_botton_position(0.0f);
                robot_arm_calculate_inverse(150.0f, 210.0f , degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);

                robot_arm_calculate_inverse(90.0f, 180.0f , degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(300);
                close_claw();
                delay_ms(300);
                robot_arm_calculate_inverse(90.0f, 210, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                robot_arm_calculate_inverse(220, 250, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }
            //夹起第三个物料
            else if(i == 2)
            {
                robot_arm_control_botton_position(-41.0f);
                robot_arm_calculate_inverse(150.0f, 210.0f , degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);

                robot_arm_calculate_inverse(90.0f, 180.0f , degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(300);
                close_claw();
                delay_ms(300);
                robot_arm_calculate_inverse(90.0f, 210, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                robot_arm_calculate_inverse(220, 250, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }

            //放置
            if(qr_data_packge[i + qr_times] == '1')
            {
                robot_arm_control_botton_position(30.5f);
                delay_ms(1000);
                robot_arm_calculate_inverse(305, 100, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                robot_arm_calculate_inverse(310, 90, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                open_claw_small();
                delay_ms(500);
                robot_arm_calculate_inverse(250, 150, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }
            else if(qr_data_packge[i + qr_times] == '2')
            {
                robot_arm_control_botton_position(3.7f);
                delay_ms(1000);
                robot_arm_calculate_inverse(270, 100, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                robot_arm_calculate_inverse(270, 90, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                open_claw_small();
                delay_ms(500);
                robot_arm_calculate_inverse(250, 150, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }
            else if(qr_data_packge[i + qr_times] == '3')
            {
                robot_arm_control_botton_position(-24.3f);
                delay_ms(1000);
                robot_arm_calculate_inverse(293, 100, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                robot_arm_calculate_inverse(303, 90, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                open_claw_small();
                delay_ms(500);
                robot_arm_calculate_inverse(250, 150, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }
        }

        robot_arm_calculate_inverse(250, 250, degree);
        robot_arm_control_arm_poosition(degree[0], degree[1]);
        robot_arm_control_botton_position(0.0f);
        delay_ms(500);

        //夹起
        for(char i = 0;i < 3; i++)
        {
            if(qr_data_packge[i + qr_times] == '1')
            {
                robot_arm_control_botton_position(30.5f);
                delay_ms(700);
                robot_arm_calculate_inverse(310, 90, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                close_claw();
            }
            else if(qr_data_packge[i + qr_times] == '2')
            {
                robot_arm_control_botton_position(3.7f);
                delay_ms(700);
                robot_arm_calculate_inverse(270, 90, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                close_claw();
            }
            else if(qr_data_packge[i + qr_times] == '3')
            {
                robot_arm_control_botton_position(-24.3f);
                delay_ms(700);
                robot_arm_calculate_inverse(303, 90, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                close_claw();
            }

            delay_ms(500);
            robot_arm_calculate_inverse(170, 250, degree);
            robot_arm_control_arm_poosition(degree[0], degree[1]);

            //放盘子
            if(i == 0)
            {
                robot_arm_control_botton_position(41.0f);
            }
            else if(i == 1)
            {
                robot_arm_control_botton_position(0.0f);
            }
            else if(i == 2)
            {
                robot_arm_control_botton_position(-41.0f);
            }

            delay_ms(600);
            robot_arm_calculate_inverse(90.0f, 210.0f , degree);
            robot_arm_control_arm_poosition(degree[0], degree[1]);

            delay_ms(700);
            robot_arm_calculate_inverse(90.0f, 190.0f , degree);
            robot_arm_control_arm_poosition(degree[0], degree[1]);

            delay_ms(200);
            open_claw_small();

            delay_ms(200);
            robot_arm_calculate_inverse(140.0f, 260.0f , degree);
            robot_arm_control_arm_poosition(degree[0], degree[1]);
        }
    }
    //成品区域定位数据
    else    
    {
        if(tasks_times == 3) tasks_times = 80;

        for(char i = 0; i < 3; i++)
        {
            //夹起第二个物料
            if(i == 1)
            {
                robot_arm_control_botton_position(0.0f);
                robot_arm_calculate_inverse(150.0f, 210.0f , degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                robot_arm_calculate_inverse(90.0f, 180.0f , degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(300);
                close_claw();
                delay_ms(300);
                robot_arm_calculate_inverse(90.0f, 210, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                robot_arm_calculate_inverse(220, 250, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }
            //夹起第三个物料
            else if(i == 2)
            {
                robot_arm_control_botton_position(-41.0f);
                robot_arm_calculate_inverse(150.0f, 210.0f , degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);

                robot_arm_calculate_inverse(90.0f, 180.0f , degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(300);
                close_claw();
                delay_ms(300);
                robot_arm_calculate_inverse(90.0f, 210, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                robot_arm_calculate_inverse(220, 250, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }

            //放置
            if(qr_data_packge[i + qr_times] == '1')
            {
                robot_arm_control_botton_position(30.7f);
                delay_ms(1000);
                robot_arm_calculate_inverse(300, 100 + item_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                robot_arm_calculate_inverse(305, 90 + item_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                open_claw_small();
                delay_ms(500);
                robot_arm_calculate_inverse(250, 150 + item_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }
            else if(qr_data_packge[i + qr_times] == '2')
            {
                robot_arm_control_botton_position(2.8f);
                delay_ms(1000);
                robot_arm_calculate_inverse(265, 100 + item_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                robot_arm_calculate_inverse(265, 90 + item_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                open_claw_small();
                delay_ms(500);
                robot_arm_calculate_inverse(250, 150 + item_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }
            else if(qr_data_packge[i + qr_times] == '3')
            {
                robot_arm_control_botton_position(-25.0f);
                delay_ms(1000);
                robot_arm_calculate_inverse(295, 100 + item_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(700);
                robot_arm_calculate_inverse(300, 90 + item_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
                open_claw_small();
                delay_ms(500);
                robot_arm_calculate_inverse(250, 150 + item_height, degree);
                robot_arm_control_arm_poosition(degree[0], degree[1]);
                delay_ms(500);
            }
        }
    }

    robot_arm_control_botton_position(41.0f);
    robot_arm_calculate_inverse(220, 250, degree);
    robot_arm_control_arm_poosition(degree[0], degree[1]);
    open_claw_small();
    tasks_times++;
}
