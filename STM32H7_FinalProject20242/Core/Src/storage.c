#include "global.h"
#include "storage.h"
#include "string.h"
#include "stm32h7xx_hal.h"

#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"

#define BUFFER_SIZE 256
#define MAX_RETRY 3

char tmp[BUFFER_SIZE];
extern uint32_t g_systemtick;
extern SPI_HandleTypeDef hspi2;

extern TIM_HandleTypeDef htim6;

extern UART_HandleTypeDef huart1;
uint32_t g_NbSector;

/* Hàm debug với timestamp */
void debug_print(const char* format, ...) {
    char buffer[128] = {0};
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);
}

/* Đọc dữ liệu từ queue với xử lý lỗi */
void ReadData(LIFO_inst *q)
{
    if(!q) {
        debug_print("M[%d] Error: Invalid queue pointer\r\n", g_systemtick/1000);
        return;
    }

    char buffer[128] = {0};
    if(!QueueIsEmpty(q))
    {
        sprintf(buffer, "M[%d] Read Data - front = %d rear = %d\r\n", 
                g_systemtick/1000, q->pnt_front, q->pnt_rear);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);

        memset(buffer, 0, sizeof(buffer)); // Thay bzero bằng memset
        sprintf(buffer,"M[%d] Last Data at Rear Pointer:",g_systemtick/1000);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);

        memset(tmp,0,sizeof(tmp));
        DRESULT res = USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);
        if(res != RES_OK) {
            debug_print("M[%d] USER_SPI_read error: %d\r\n", g_systemtick/1000, res);
        }
    }
    else
    {
        memset(buffer, 0, sizeof(buffer)); // Thay bzero bằng memset
        sprintf(buffer,"M[%d] Read Data - Queue Empty: front = %d rear = %d",g_systemtick/1000,q->pnt_front,q->pnt_rear);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);
    }
}

/*Send data from queue to hecule*/
void SendData(LIFO_inst *q, uint8_t nmb_send_element)
{
    // Cải thiện logic kiểm tra khởi tạo SD: chỉ gửi khi khởi tạo thành công (trả về 0)
    if(USER_SPI_initialize(0) == 0)
    {
        if(!QueueIsEmpty(q))
        {
            char buffer[128] = {0};
            sprintf(buffer,"M[%d] Send Saved Data",g_systemtick/1000);
            HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);

            for(uint8_t i =0;i<nmb_send_element;i++)
            {
                if(QueueIsEmpty(q))
                {
                    break; // Thoát vòng lặp nếu queue rỗng
                }
                else
                {
                    DRESULT res = RES_ERROR;
                    if(q->pnt_rear > q->pnt_front)
                    {
                        memset(tmp,0,sizeof(tmp));
                        res = USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);
                        if(res != RES_OK) {
                            debug_print("M[%d] USER_SPI_read error: %d\r\n", g_systemtick/1000, res);
                        }
                        q->pnt_rear -= 1;
                    }
                    else if(q->pnt_rear == q->pnt_front)
                    {
                        memset(tmp,0,sizeof(tmp));
                        res = USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);
                        if(res != RES_OK) {
                            debug_print("M[%d] USER_SPI_read error: %d\r\n", g_systemtick/1000, res);
                        }

                        q->pnt_front=0;

                        memset(buffer, 0, sizeof(buffer)); // Thay bzero bằng memset
                        sprintf(buffer,"M[%d] Queue is Empty",g_systemtick/1000);
                        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100);
                        break;
                    }
                    else if (q->pnt_rear < q->pnt_front)
                    {
                        if(q->pnt_rear > 1)
                        {
                            memset(tmp,0,sizeof(tmp));
                            res = USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);
                            if(res != RES_OK) {
                                debug_print("M[%d] USER_SPI_read error: %d\r\n", g_systemtick/1000, res);
                            }
                            q->pnt_rear -= 1;
                        }
                        else
                        {
                            memset(tmp,0,sizeof(tmp));
                            res = USER_SPI_read(0, (uint8_t*)tmp, q->pnt_rear, 1);
                            if(res != RES_OK) {
                                debug_print("M[%d] USER_SPI_read error: %d\r\n", g_systemtick/1000, res);
                            }
                            q->pnt_rear = g_NbSector;
                        }
                    }
                }
                memset(buffer, 0, sizeof(buffer)); // Thay bzero bằng memset
                sprintf(buffer, "M[%d] Send Data - front = %d rear = %d",g_systemtick/1000,q->pnt_front,q->pnt_rear);
                if(HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 100) != HAL_OK) {
                    debug_print("M[%d] UART transmit error\r\n", g_systemtick/1000);
                }
            }
        }
    }
    else
    {
        debug_print("M[%d] SendData - SDcard Failed\r\n",g_systemtick/1000);
    }
}
