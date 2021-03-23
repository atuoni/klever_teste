/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart2;

QueueHandle_t xUARTQueue;
QueueHandle_t xADCQueue;

uint8_t Rx_indx, Rx_data[2], Rx_Buffer[100], Transfer_cplt;
char msg[30];
//uint16_t adcvalue=0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);
void Task1(void const * argument);
void Task2(void const *argument);
void Task3(void const *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  xUARTQueue = xQueueCreate(1,1);
  xADCQueue = xQueueCreate(1,2);

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
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  xTaskCreate((TaskFunction_t)Task1,"LED Controller", configMINIMAL_STACK_SIZE,NULL,10,NULL);
  xTaskCreate((TaskFunction_t)Task2,"UART Controller", configMINIMAL_STACK_SIZE,NULL,10,NULL);
  xTaskCreate((TaskFunction_t)Task3,"ADC Controller", configMINIMAL_STACK_SIZE,NULL,10,NULL);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PF7 PF9 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void Task1(void const * argument)
{
  uint8_t modo;
  /* USER CODE BEGIN 5 */
  /* Infinite loop */


  while(1)
  {
       if(xQueueReceive(xUARTQueue,&modo,0)==pdPASS)
              {
        	if(modo == 1)
                {
              	      HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9, GPIO_PIN_SET);

                }
                    if(modo == 2)
                        {
                        	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9, GPIO_PIN_RESET);

                        }
                    if(modo == 3)
                        {
                        	HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_9);

                        }
               }

  }
}

void Task2(void const *argument)
{
//uint16_t adc;
//char msg[15];
  while(1)
    {

      HAL_UART_Receive_IT(&huart2,(uint8_t*)Rx_data,1);

      //if(opt==1)
	//{
	//  xQueueSend(xUARTQueue,&opt,pdMS_TO_TICKS(10));
	//}
      //else
	//xQueueSend(xUARTQueue,&opt,pdMS_TO_TICKS(10));
    }
}
void Task3(void const *argument)
{
uint16_t adcvalue=0;
  while(1)
    {
      HAL_ADC_Start(&hadc1);
      adcvalue = HAL_ADC_GetValue(&hadc1);

      xQueueSend(xADCQueue,&adcvalue,0);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
/* Set transmission flag: transfer complete*/
uint8_t i, opt=0;
uint16_t adc =0;
//char msg[15];

BaseType_t xHigherPriorityTaskWoken = pdFALSE;
BaseType_t xTaskWokenByReceive = pdFALSE;


xQueueReceiveFromISR(xADCQueue,(uint16_t *)&adc,&xTaskWokenByReceive);

if( xTaskWokenByReceive != pdFALSE )
{
    taskYIELD ();
}

if(huart->Instance==USART2)
  {

    if(Rx_indx == 0)
      {
	for(i=0;i<100;i++)
	  Rx_Buffer[i]=0;
	//HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9, GPIO_PIN_RESET);
	//HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	//HAL_UART_Transmit(&huart2,(uint8_t *)"Digite o Comando: ",18,100);

      }
    if(Rx_data[0]!='\r')
      {
	Rx_Buffer[Rx_indx++]=Rx_data[0];
	HAL_UART_Receive_IT(&huart2,(uint8_t *)Rx_data,1);
	HAL_UART_Transmit(&huart2,Rx_data,strlen((const char *)Rx_data),100);
      }
    else
      {
	Rx_indx=0;
	Transfer_cplt=1;
	HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	//HAL_UART_Transmit(&huart2,(uint8_t *)"Digite o Comando: ",18,100);
	//HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	//HAL_UART_Transmit(&huart2,Rx_Buffer,strlen((char *)Rx_Buffer),100);

	if(!strcmp((char *)Rx_Buffer,"LED ON"))
	{
	    //HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9, GPIO_PIN_SET);
	  opt = 1;
	  xQueueSendFromISR(xUARTQueue,&opt,&xHigherPriorityTaskWoken);
	  HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	  HAL_UART_Transmit(&huart2,(uint8_t *)"LED ACESO",9,100);
	  HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	  if(xHigherPriorityTaskWoken != pdFALSE)
	  {
	      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	  }

	}
	if(!strcmp((char *)Rx_Buffer,"LED OFF"))
	{

	    opt = 2;
	    xQueueSendFromISR(xUARTQueue,&opt,&xHigherPriorityTaskWoken);
	    HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	    HAL_UART_Transmit(&huart2,(uint8_t *)"LED APAGADO",11,100);
	    HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	    	  if(xHigherPriorityTaskWoken != pdFALSE)
	    	    {
	    	      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	    	    }
	}
	if(!strcmp((char *)Rx_Buffer,"LED T"))
	{
	    opt = 3;
	    xQueueSendFromISR(xUARTQueue,&opt,&xHigherPriorityTaskWoken);
	    HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	    HAL_UART_Transmit(&huart2,(uint8_t *)"LED Toggle",10,100);
	    HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	    	  if(xHigherPriorityTaskWoken != pdFALSE)
	    	    {
	    	      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	    	    }
	}
	if(!strcmp((char *)Rx_Buffer,"Loop Back"))
	{
	    opt = 4;
	    HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	    HAL_UART_Transmit(&huart2,Rx_Buffer,strlen((char *)Rx_Buffer),100);
	    HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	}
	if(!strcmp((char *)Rx_Buffer,"ADC"))
	{


			sprintf(msg,"rawValue: %hu\r\n",adc);
			HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
			HAL_UART_Transmit(&huart2,(uint8_t *)msg,strlen(msg),100);
			HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);


	}
	//else
	//{
	    //opt=0;
	    //HAL_UART_Transmit(&huart2,(uint8_t *)"Comando nao identificado!",25,100);
	    //HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	    //osMessagePut(MsgBox, opt, osWaitForever);
	//}
      }
    //HAL_UART_Receive_IT(&huart2,(uint8_t *)Rx_data,1);
    //HAL_UART_Transmit(&huart2,Rx_data,strlen((const char *)Rx_data),100);
  }
}



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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
