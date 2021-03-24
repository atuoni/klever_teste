/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  *
  * Programa de Avaliação do Desafio Técnico - Empresa Klever (Amauri Tuoni)
  ******************************************************************************
  * This main.c file was written by Amauri Tuoni
  * Date: 24/03/2021
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart2;

QueueHandle_t xUARTQueue;
QueueHandle_t xADCQueue;
QueueHandle_t xLEDQueue;

uint8_t Rx_index=0, Rx_data[2], Rx_Buffer[100], Transfer_complete;  //uart data receiver  buffers
char string_msg[30];      //string message to print on the screen
uint8_t lp_mode = FALSE;  //initial state loop back mode (off)


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);     //clock config
static void MX_GPIO_Init(void);   // i/o config
static void MX_ADC1_Init(void);     //adc config
static void MX_USART2_UART_Init(void);   //uart config
void TaskLED(void const * argument);        //led task
void TaskUART(void const *argument);         //uart task
void TaskADC(void const *argument);         //adc task



/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  xUARTQueue = xQueueCreate(1,1);   // create UART message Queue
  xADCQueue = xQueueCreate(1,2);    // create ADC Queue
  xLEDQueue = xQueueCreate(1,1);    // create LED state queue
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();


  /* Configure the system clock */
  SystemClock_Config();


  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();


  /* Create the thread(s) */
  xTaskCreate((TaskFunction_t)TaskLED,"LED Controller", configMINIMAL_STACK_SIZE,NULL,1,NULL);  //create TASK LED
  xTaskCreate((TaskFunction_t)TaskUART,"UART Controller", configMINIMAL_STACK_SIZE,NULL,1,NULL);  //create TASK UART
  xTaskCreate((TaskFunction_t)TaskADC,"ADC Controller", configMINIMAL_STACK_SIZE,NULL,1,NULL);    //create TASK ADC


  /* Start scheduler */
  osKernelStart();

  while (1)
  {

  }

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

  ADC_ChannelConfTypeDef sConfig = {0};


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


}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{


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
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}


void TaskLED(void const * argument)
{
  uint8_t LED_mode;  //store the led mode from UART
  uint8_t ack;       // acknowledge of the led's state


  while(1)
  {
       if(xQueueReceive(xUARTQueue,&LED_mode,0)==pdPASS)
       {
        switch(LED_mode)
         {

	    case 1:
        	      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5, GPIO_PIN_SET);  //turn the led on
              	      ack=1;
              	      xQueueSend(xLEDQueue,&ack,0);             //send ack to uart task
              	      break;

            case 2:
                      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5, GPIO_PIN_RESET);   // turn the led off
                      ack=2;
                      xQueueSend(xLEDQueue,&ack,0);  //send ack to UART task
                      break;

            case 3:
                     ack=3;
                     xQueueSend(xLEDQueue,&ack,0);
                     while(LED_mode==3)
                     {
                	 HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);  // toggle led 500ms
                	 osDelay(500);
                	 if((xQueueReceive(xUARTQueue,&LED_mode,0)==pdPASS) && LED_mode ==3)  // receive a new led_mode
                	   {
                	     xQueueSend(xLEDQueue,&ack,pdMS_TO_TICKS(1));  //send ack to uart task
                	   }
                     }
                     if(LED_mode==1)    // check new led mode
                     {
                	 ack=1;


                	     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5, GPIO_PIN_SET);  //turn the led on
                	     xQueueSend(xLEDQueue,&ack,pdMS_TO_TICKS(1));   //send ack to uart task

                     }
                     if(LED_mode==2)   //check new led mode
                     {
                	 ack=2;

                	     HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5, GPIO_PIN_RESET);  // turn the led off
                	     xQueueSend(xLEDQueue,&ack,pdMS_TO_TICKS(1));   //send ack to uart task

                     }
                 break;
            }
       }
  }
}

void TaskUART(void const *argument)
{

uint8_t ack_received=0;    //store an acknowledge received

  while(1)
    {
      HAL_UART_Receive_IT(&huart2,(uint8_t*)Rx_data,1);   // Interrupt from UART RX

      if(xQueueReceive(xLEDQueue,&ack_received,0)==pdPASS)  // check ack received from UART task
      {
      	switch(ack_received)
      	{
      	  case 1:
      	      HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
      	      HAL_UART_Transmit(&huart2,(uint8_t *)"THE LED TURNED ON",17,100);  //tx message
      	      HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
      	      break;


      	  case 2:

      	       HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
      	       HAL_UART_Transmit(&huart2,(uint8_t *)"THE LED TURNED OFF",18,100); //tx message
      	       HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
      	       break;

      	  case 3:

      		HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
      		HAL_UART_Transmit(&huart2,(uint8_t *)"LED TOGGLE",10,100); //tx message
      		HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
      		break;

      	  default:
      		HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
      		HAL_UART_Transmit(&huart2,(uint8_t *)"MSG FAIL",8,100);   //tx message
      		HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
           }
	 }
      }
}
void TaskADC(void const *argument)
{
uint16_t adc_msg = 0;

  while(1)
  {
      HAL_ADC_Start(&hadc1);   //start adc read
      adc_msg = HAL_ADC_GetValue(&hadc1);  // get adc value continuous

      xQueueSend(xADCQueue,&adc_msg,0);   // send adc value to UART task
   }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
/* Set transmission flag: transfer complete*/

uint16_t adc_value=0;  //store adc value
uint8_t led_option;   //store led option



xQueueReceiveFromISR(xADCQueue,(uint16_t *)&adc_value,NULL);   //receive the adc value from ADC task

if(huart->Instance==USART2)
  {

    if(Rx_index == 0)
    {
            for(uint8_t position=0;position<100;position++)

                Rx_Buffer[position]=0;    // clean UART rx buffer
    }
    if(Rx_data[0]!='\r')  // any key typed
    {
	Rx_Buffer[Rx_index++]=Rx_data[0];  // move to buffer the typed character
	HAL_UART_Receive_IT(&huart2,(uint8_t *)Rx_data,1);     //get character
	HAL_UART_Transmit(&huart2,Rx_data,strlen((const char *)Rx_data),100);  //echo the character
    }
    else    //enter key pressed
    {

	Rx_index = 0;  // restart buffer index
	Transfer_complete = TRUE;
	HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);


	if(!strcmp((char *)Rx_Buffer,"led on"))
	{
	  led_option = 1;

	  xQueueSendFromISR(xUARTQueue,&led_option,NULL);   //send user message to LED task

	}

        if(!strcmp((char *)Rx_Buffer,"led off"))
	{

	   led_option = 2;
	   xQueueSendFromISR(xUARTQueue,&led_option,NULL);    //send user message to LED task

	}

        if(!strcmp((char *)Rx_Buffer,"led tgl"))
	{
	    led_option = 3;
	    xQueueSendFromISR(xUARTQueue,&led_option,NULL);    //send user message to LED task

	}

        if(!strcmp((char *)Rx_Buffer,"lb on"))
	{
	      lp_mode = TRUE;                                    // turn the loop back mode on
	      HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	      HAL_UART_Transmit(&huart2,(uint8_t *)"Loop Back is ON",15,100);
	      HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	}

        if(!strcmp((char *)Rx_Buffer,"lb off"))
       	{
       	      lp_mode = FALSE;                                       // turn the loop back mode off
       	      HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
       	      HAL_UART_Transmit(&huart2,(uint8_t *)"Loop Back is OFF",16,100);
       	      HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
       	}

        if(!strcmp((char *)Rx_Buffer,"adc"))
	{
	    sprintf(string_msg,"ADC - Current Value: %hu\r\n",adc_value);   //integer data to string

	    HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	    HAL_UART_Transmit(&huart2,(uint8_t *)string_msg,strlen(string_msg),100); //transmit adc read
	    HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);
	}
        if(lp_mode == TRUE)     // if loop back mode is on
        {

            HAL_UART_Transmit(&huart2,(uint8_t *)"Last data packet sent: ",23,100);
	    HAL_UART_Transmit(&huart2,Rx_Buffer,strlen((char *)Rx_Buffer),100);   //transmit the last packet sent by user
            HAL_UART_Transmit(&huart2,(uint8_t *)"\n\r",2,100);

        }
     }

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

