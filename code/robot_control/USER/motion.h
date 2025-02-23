#ifndef __MOTION_H
#define __MOTION_H

#include "protocols.h"

#include "chassis_control.h"
#include "robot_arm_control.h"
#include "hwt101_driver.h"

//圆盘区域粗定位目标
#define rotator_first_location_x_target    247
#define rotator_first_location_y_targe    163

//圆盘区域精定位目标
#define rotator_second_location_x_target    172
#define rotator_second_location_y_target    157

//物料区域粗定位目标
#define processing_area_first_location_x_targe    307
#define processing_area_first_location_y_targe    255

//物料区域精定位目标
#define processing_area_second_location_x_targe 215
#define processing_area_second_location_y_targe 215

//定位死区
#define first_location_dead_area  10
#define second_location_dead_area  5


void grab_item_on_disk(char disk_num);
void grab_item_on_rotator(char rotator_num);

void qr_code_task(void);
void rotator_task(void);
void processing_area_task(void);

#endif