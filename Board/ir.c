////////////////////////////////////////////////////////////////////////////////
#define _IR_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "HAL_device.h"
#include "sys.h"


#include "main.h" 
#include "ir_rx.h" 

////////////////////////////////////////////////////////////////////////////////
void initGPIO_IR()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	
	GPIO_SetBits(GPIOC,GPIO_Pin_9);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void NVIC_IR()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  							//TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  					//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  						//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 							//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  											//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	
	
	//	TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_CC4, ENABLE);						//��������ж� ,����CC2IE�����ж�	
	TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);						//��������ж� ,����CC2IE�����ж�	
}

////////////////////////////////////////////////////////////////////////////////
void initIR()
{  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	//	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 								//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   
	//	TIM_TimeBaseStructure.TIM_Prescaler =(720-1); 								//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   
	TIM_TimeBaseStructure.TIM_Prescaler =(1440-1); 								//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  				//TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = 65535; 										//�趨�������Զ���װֵ ���10ms���  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 					//����ʱ�ӷָ�:TDTS = Tck_tim
	//	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;							// LiuQ	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 							//����ָ���Ĳ�����ʼ��TIMx
	
	
	
	/////////////////	
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  							//ѡ������� IC2ӳ�䵽TI5��
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;					//�����ز���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 					//���������Ƶ,����Ƶ 
	TIM_ICInitStructure.TIM_ICFilter = 0x03;									//IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
	TIM_ICInit(TIM3, &TIM_ICInitStructure);										//��ʼ����ʱ�����벶��ͨ��
	
	TIM_Cmd(TIM3,ENABLE ); 														//ʹ�ܶ�ʱ��5
	
	NVIC_IR();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void getMarkSpace(u8 *p, u8 *mark, u8 *space, u8 f)
{
	u32 sumMark = 0;
	u32 sumSpace = 0;
	u16  markCnt = 0;
	u16 spaceCnt = 0;
	
	u8 first;
	if (!f) {
		first = *p++;
		while(*p < 255) {
			if ((*p >= (first - first / 3)) && (*p <= (first + first / 3))) {
				sumMark += *p; 
				markCnt++;
			}
			else {
				sumSpace += *p;
				spaceCnt++;
			}
			p++;
		}
		*mark = sumMark / markCnt;
		*space = sumSpace / spaceCnt;
	}
	else {
		first = *p++;
		while(*p < 255) {
			if ((first >= (*p  - *p / 5)) && ((first <= (*p + *p / 5)))) { 
				sumMark += *p; 
				markCnt++;
			}
			else {
				sumSpace += *p;
				spaceCnt++;
			}
			p++;
		}
		*mark = sumMark / markCnt;
		*space = sumSpace / spaceCnt;
		
	}
}

////////////////////////////////////////////////////////////////////////////////
void putMarkSpace(u8 *p, u8 mark, u8 space, u8 f)
{
	while(*p < 255) {
		((*p > (mark - mark / 5)) && (*p < (mark + mark / 5))) ?  (*p++ = mark) :  (*p++ = space); 
	}
}

////////////////////////////////////////////////////////////////////////////////
void irConvert(u8 *p, u8 f)
{
	u8 i = 0; 
	u8 dat = 0;
	u8 ref;
	if (!f) {
		ref = *p++;
		while(*p < 255) {
			for (u8 j = 0; j < 8; j++) {
				dat >>= 1;
				(*p++ == ref) ?  (dat &= ~0x80) : (dat |=  0x80); 
				if (*p == 255)  break;
				p++;
			}
			irDat[i++] = dat;
			dat = 0;
		}
	}
	else {
		ref = *p;
		while(*p < 255) {
			for (u8 j = 0; j < 8; j++) {
				dat >>= 1;
				(*++p == ref) ?  (dat &= ~0x80) : (dat |=  0x80); 
				if (*p == 255) {
					dat >>= (7 - j);
					break;
				}
				p++;
				if (*p == 255) {
					dat >>= (7 - j);
					break;
				}
			}
			irDat[i++] = dat;
			dat = 0;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
void irNormalization(u8* p, u8 f)
{
	u8 mark = 0;	u8 space = 0;
	getMarkSpace(p, &mark, &space, f);
	putMarkSpace(p,  mark,  space, f);
}

////////////////////////////////////////////////////////////////////////////////
void irNull()
{
	if (!irOver && (irOverCnt++ >= 9)) {
		irOver = TRUE;	
		TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Falling);
		irBuf[irp] = 255;
		irp = 0;
		
		if (irSuncF && !irBlock) {
			irBlock = TRUE;
			rfOverCnt.ir = 0;
			if (irSunc0 && irSunc1) {
				irNormalization(&irBuf[0], 0);
				irConvert(&irBuf[0], 0);
			}
			else if (irSunc0 && !irSunc1) {
				irNormalization(&irBuf[0], 1);
				irConvert(&irBuf[0], 1);
			}
			error.ir = FALSE;
			rf.ir = TRUE;
		}
	}
}


u8 count = 0;


u32 AAA,BBB,CCC,DDD;
////////////////////////////////////////////////////////////////////////////////
void TIM3_IRQHandler()
{ 		  
	static u32 _cnt = 0;
	
	if(TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET) {
		u32 cnt = TIM_GetCapture4(TIM3);
		TIM_ClearFlag(TIM3, TIM_IT_CC4);
		(IR) ? TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Falling) 			//		if(RDATA) {
			: TIM_OC4PolarityConfig(TIM3, TIM_ICPolarity_Rising);
			
			irOverCnt = 0;
			
			if (irBlock) {
				irOver = TRUE;
				rfOverCnt.ir = 0;
			}
			else if (irOver){
				_cnt = cnt;
				irOver = FALSE;
				irSunc0 = 0;
				irSunc1 = 0;
				irSuncF = FALSE;
				memset(&irBuf, 0, sizeof(irBuf));
				memset(&irDat, 0, sizeof(irDat));
			}
			else {
				if (!irSuncF) {
					if ((irSunc0 == 0) && ((cnt - _cnt) > 100)) {
						count++;
						irSunc0 = (cnt - _cnt);
						irSunc1 = 0;
						irSuncF = FALSE;
						//					memset(&irBuf, 0, sizeof(irBuf));
						//					memset(&irDat, 0, sizeof(irDat));
					}
					else if (irSunc0 && ((cnt - _cnt) > 75) && ((cnt - _cnt) < 120)) {
						irSunc1 = (cnt - _cnt);
						irSuncF = TRUE;
					}
					else if (irSunc0) {
						irBuf[irp++] = cnt - _cnt;
						irSunc1 = 0;
						irSuncF = TRUE;
					}
				}
				else {
					irBuf[irp++] = cnt - _cnt;
					
					if (irp >= 512) {
						irp = 0;
						irSuncF = FALSE;
					}
				}
			}
			_cnt = cnt;
	}
}

////////////////////////////////////////////////////////////////////////////////


