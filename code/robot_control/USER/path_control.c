#include "path_control.h"

/*
    路径控制文件
*/

/**
*@brief 底盘插值速度控制
*@param li5_path            路径队列
*@param choose_out_type     选择输出数据类型
*@return 无
*/
float chassis_path_control(path_quene_t *li5_path, char choose_out_type)
{
    count_time((&li5_path->timer));

    //根据时间推算节点
    if(((li5_path->quene)[li5_path->p_node].time) <= (&li5_path->timer)->count_now_time)
    {
        //该节点有任务
        if((li5_path->quene)[li5_path->p_node+1].task != NULL) 
        {
            //路径任务完成
            if(*(char *)(li5_path->quene)[li5_path->p_node+1].task == end_quene) 
                return 0;

            //运行节点任务
            ((void (*)(void))((li5_path->quene)[li5_path->p_node+1].task))();
            start_count_time(&li5_path->timer);
            li5_path->p_node ++;
        }
        li5_path->p_node ++;
    }
 
    if(choose_out_type == out_type_speed)
        return get_speed_from_li5(&((li5_path->quene)[li5_path->p_node].solve), (&li5_path->timer)->count_now_time);
    else if(choose_out_type == out_type_displacement)
        return get_displacement_from_li5(&((li5_path->quene)[li5_path->p_node].solve), (&li5_path->timer)->count_now_time);
    else 
        return get_acceleration_from_li5(&((li5_path->quene)[li5_path->p_node].solve), (&li5_path->timer)->count_now_time);
}