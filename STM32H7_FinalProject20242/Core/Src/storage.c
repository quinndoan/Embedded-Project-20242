#include "global.h"
#include "storage.h"
#include "string.h"
#include "stm32h7xx_hal.h"

#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
char tmp[256];

#define BUFFER_SIZE 256
#define MAX_RETRY 3

char tmp[BUFFER_SIZE];
extern uint32_t g_systemtick;
extern SPI_HandleTypeDef hspi2;

extern TIM_HandleTypeDef htim6;

extern UART_HandleTypeDef huart1;
uint32_t g_NbSector;

/*Print Data in queque*/
void ReadData(LIFO_inst *q)
{
	char buffer[128] = {0};
	if(!QueueIsEmpty(q))
	{

		sprintf(buffer, "M[%d] Read Data - front = %d rear = %d",g_systemtick/1000,q->pnt_front,q->pnt_rear);
		HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);

		bzero(buffer, sizeof(buffer));
		sprintf(buffer,"M[%d] Last Data at Rear Pointer:",g_systemtick/1000);
		HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);

		memset(tmp,0,sizeof(tmp));
		USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);
	}
	else
	{
		bzero(buffer, sizeof(buffer));
		sprintf(buffer,"M[%d] Read Data - Queue Empty: front = %d rear = %d",g_systemtick/1000,q->pnt_front,q->pnt_rear);
		HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
	}

}
/*Send data from queue to hecule*/
void SendData(LIFO_inst *q, uint8_t nmb_send_element)
{
	if(!USER_SPI_initialize (0))
	{
		if(!QueueIsEmpty(q))
		{
			char buffer[128] = {0};
			sprintf(buffer,"M[%d] Send Saved Data",g_systemtick/1000);
			HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);

			for(uint8_t i =0;i<nmb_send_element;i++)
			{
				if(QueueIsEmpty(q))
				{

				}
				else
				{
					if(q->pnt_rear> q->pnt_front)
					{
						memset(tmp,0,sizeof(tmp));
						USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);

						q->pnt_rear -= 1;
					}
					else if(q->pnt_rear == q->pnt_front)
					{
						memset(tmp,0,sizeof(tmp));
						USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);


						q->pnt_front=0;

						bzero(buffer, sizeof(buffer));
						sprintf(buffer,"M[%d] Queue is Empty",g_systemtick/1000);
						HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
						break;

					}
					else if (q->pnt_rear < q->pnt_front)
					{
						if(q->pnt_rear>1)
						{
							memset(tmp,0,sizeof(tmp));
							USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);
//							mqtt_debug_send(tmp);

							q->pnt_rear -= 1;
						}
						else
						{
							memset(tmp,0,sizeof(tmp));
							USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);

							q->pnt_rear = g_NbSector;
						}
					}
				}
				bzero(buffer, sizeof(buffer));
				sprintf(buffer, "M[%d] Send Data - front = %d rear = %d",g_systemtick/1000,q->pnt_front,q->pnt_rear);
				HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
			}
		}
	}
	else
	{
		debugPrint("M[%d] SendData - SDcard Failed",g_systemtick/1000);
	}
}
