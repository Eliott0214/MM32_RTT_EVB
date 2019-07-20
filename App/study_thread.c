////////////////////////////////////////////////////////////////////////////////
#define _STUDY_THREAD_
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "EVBoard.h"

#include <rtthread.h>
#include "study_thread.h"

/* �߳�1��� */
void study_entry(void* parameter)
{
	while (1)
    {
        LED_0_OFF();    //LD2�����
        LED_1_ON();     //LD1�����
        rt_thread_delay(RT_TICK_PER_SECOND / 1);
	}   /* ��ѭ�� */
}

/* �߳�2��� */
void thread2_entry(void* parameter)
{
    while (1)
    {
        LED_0_ON();     //LD2�����
        LED_1_OFF();    //LD1�����
        rt_thread_delay(RT_TICK_PER_SECOND / 10);
        // ����1�� 
        // rt_thread_delay(RT_TICK_PER_SECOND);
    }
}







