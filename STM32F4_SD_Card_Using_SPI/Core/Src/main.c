/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"
#include "string.h"
#include "fatfs_sd.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//#define f_size(fp)  ((fp)->obj.objsize)

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void SD_List_File(void);
void SD_creatSubDir(char* filename);
void SD_deleteFolder(char* foldername);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FATFS fs;
FIL fil;
FRESULT fresult;
char buffer[1024];

UINT br, bw;


FATFS *pfs;			// con trỏ đến hệ thống file system
DWORD fre_clust;
uint32_t total, free_space;
char option[4];
char namebuf[128];
char filebuf[1024];
FRESULT fres;

void send_uart(char *string)
{
	uint16_t len = strlen(string);
  char buf1[len];
  memset(buf1, 0, sizeof(buf1));
  strcpy(buf1, string);
  HAL_UART_Transmit(&huart1,(char*) buf1, sizeof(buf1), 2000);

}

int bufsize(char *buf){
	int i=0;
	while (*buf++ != '\0' )i++;

	return i;
}

void clear_buffer (void)
{
	for (int i=0; i<1024; i++) buffer[i] = '\0';
}

// Hàm nhận chuỗi từ UART (blocking, kết thúc khi gặp '!' hoặc '\n')
void uart_receive_string(char* buf, uint16_t maxlen) {
    uint16_t i = 0;
    char c;
    while (i < maxlen - 1) {
        HAL_UART_Receive(&huart1, (uint8_t*)&c, 1, HAL_MAX_DELAY);
        if (c == '!' || c == '\n') break;
        buf[i++] = c;
        HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, 100); // Echo lại ký tự
    }
    buf[i] = '\0';
    char crlf[2] = "\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)crlf, 2, 100);
}

// Hàm nhận 1 ký tự từ UART (dùng cho chọn option)
char uart_receive_char(void) {
    char c;
    HAL_UART_Receive(&huart1, (uint8_t*)&c, 1, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart1, (uint8_t*)&c, 1, 100); // Echo lại ký tự
    char crlf[2] = "\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t*)crlf, 2, 100);
    return c;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_FATFS_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(500);
  // Mount the SD card

    fresult = f_mount(&fs, "/", 1);
      	if (fresult != FR_OK) send_uart ("ERROR in mounting SD CARD...\n\n");
      	else send_uart("SD CARD mounted successfully...\n\n");


      	/*************** Card capacity details ********************/

      	/* Check free space */
      	f_getfree("", &fre_clust, &pfs);

      	total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
      	sprintf (buffer, "SD CARD Total Size: \t%lu\n",total);
      	send_uart(buffer);
      	clear_buffer();
      	free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
      	sprintf (buffer, "SD CARD Free Space: \t%lu\n\n",free_space);
      	send_uart(buffer);
      	clear_buffer();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */
	  send_uart("==== SD Card Menu ====\r\n");
	  	       send_uart("0. Stop program\r\n");
	  	       send_uart("1. List files\r\n");
	  	       send_uart("2. Create directory\r\n");
	  	       send_uart("3. Delete directory\r\n");
	  	       send_uart("4. Create file \r\n");
	  	       send_uart("5. Delete file\r\n");
	  	       send_uart("6. Update an existing file\r\n");
	  	       send_uart("Please enter option number: ");

	  	       char opt = uart_receive_char();

	  	       switch (opt) {
	  	           case '0':
	  	               send_uart("Stopping program.\r\n");
	  	               while(1); // Dừng chương trình
	  	               break;
	  	           case '1':
	  	               send_uart("Files on SD card:\r\n");
	  	               SD_List_File();
	  	               break;
	  	           case '2':
	  	               send_uart("Please enter directory name: ");
	  	               uart_receive_string(namebuf, sizeof(namebuf));
	  	               SD_creatSubDir(namebuf);
	  	               send_uart("Directory created!\r\n");
	  	               break;
	  	           case '3':
	  	               send_uart("Please enter directory name to delete: ");
	  	               uart_receive_string(namebuf, sizeof(namebuf));
	  	               SD_deleteFolder(namebuf);
	  	               send_uart("Directory deleted!\r\n");
	  	               break;
	  	           case '4':
	  	               send_uart("Please enter file name to create: ");
	  	               uart_receive_string(namebuf, sizeof(namebuf));
	  	               fres = f_open(&fil, namebuf, FA_CREATE_ALWAYS | FA_WRITE);
	  	               if (fres == FR_OK) {
	  	                   send_uart("Enter content (end with !):\r\n");

	  	                   uart_receive_string(filebuf, sizeof(filebuf));
	  	                   fres = f_write(&fil, filebuf, bufsize(filebuf), &bw);
	  	                   if (fres == FR_OK) send_uart("File created and written!\r\n");
	  	                   else send_uart("Failed to write file!\r\n");
	  	                   f_close(&fil);
	  	               } else {
	  	                   send_uart("Failed to create file!\r\n");
	  	               }
	  	               break;
	  	           case '5':
	  	               send_uart("Please enter file name to delete: ");
	  	               uart_receive_string(namebuf, sizeof(namebuf));
	  	               char pathbuf[64] = {0};
	                   snprintf(pathbuf, sizeof(pathbuf), "/%s", namebuf);
	  	               fres = f_unlink(pathbuf);
	  	               if (fres == FR_OK) send_uart("File deleted!\r\n");
	  	               else send_uart("Failed to delete file!\r\n");
	  	               break;


	  	           case '6':
	  	               send_uart("Please enter file name to update: ");
	  	               uart_receive_string(namebuf, sizeof(namebuf));
	  	               fres = f_open(&fil, namebuf, FA_OPEN_EXISTING | FA_WRITE | FA_READ);
	  	               if (fres == FR_OK) {
	  	                   f_lseek(&fil, f_size(&fil));
	  	                   memset(filebuf, '0', sizeof(filebuf));
	  	                   send_uart("Enter content to append (end with !):\r\n");

	  	                   uart_receive_string(filebuf, sizeof(filebuf));
	  	                   fres = f_puts(filebuf, &fil);
	  	                   f_close(&fil);
	  	               } else {
	  	                   send_uart("Failed to open file for update!\r\n");
	  	               }
	  	               break;
	  	           default:
	  	               send_uart("Invalid option!\r\n");
	  	               break;
	  	       }

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
