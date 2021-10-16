
#include "main.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

char Rx_Data[2];
UART_HandleTypeDef huart1;
uint32_t IC_Val1 = 0;
uint32_t IC_Val2 = 0;
uint32_t Difference = 0;
uint8_t Is_First_Captured = 0;  
int d1,d2 =0;
int Distance=0;
static uint8_t vatcan=0;
static int counter=0;
static int tuhanh=0;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM1_Init(void);
void motor1_up()
{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
}
void motor1_back()
{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);
}
void motor1_stop()
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
}
void motor2_up()
{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
}
void motor2_back()
{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
}
void motor2_stop()
{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET);
}

void motor_up()
{
	motor1_up();
	motor2_up();
}
void motor_back()
{
	motor1_back();
	motor2_back();
}
void motor_left()
{
	motor1_up();
	motor2_stop();
}
void motor_right()
{
	motor1_stop();
	motor2_up();
}
void motor_stop()
{
	motor1_stop();
	motor2_stop();
}


void delay(uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim1,0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < time);
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)  												// if the interrupt source is channel1
	{
		if (Is_First_Captured==0) 																						// if the first value is not captured
		{
			IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); 					// read the first value
			Is_First_Captured = 1;  																						// set the first captured as true
																																					// Now change the polarity to falling edge
		}

		else if (Is_First_Captured==1)   																				// if the first is already captured
		{
			IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);  						// read second value
			__HAL_TIM_SET_COUNTER(htim, 0);  																			// reset the counter

			if (IC_Val2 > IC_Val1)
			{
				Difference = IC_Val2-IC_Val1;
			}

			else if (IC_Val1 > IC_Val2)
			{
				Difference = (0xffff - IC_Val1) + IC_Val2;
			}
			if(vatcan==1)
			{
				switch(counter)
				{
					case '0' : d1 = Difference * .034/2;
						counter++;
						break;
					case '1' : d2 = Difference * .034/2;
						counter=0;
						vatcan=0;
						break;
					
				}
			}else 
			{
				Distance = Difference * .034/2;
			}
			Is_First_Captured = 0; 																								// set it back to false

			
			
			__HAL_TIM_DISABLE_IT(&htim1, TIM_IT_CC1);
		}
	}
}
void HCSR04_Read (void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);  // pull the TRIG pin low

	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
}
void tuhanh_func()
{
	HCSR04_Read();
	if(Distance <= 20)
	{
		motor_stop();
		HAL_Delay(100);
		htim3.Instance->CCR1=350;
		htim3.Instance->CCR2=350;
		motor_back();
    HAL_Delay(300);
    motor_stop();
    HAL_Delay(200);
		htim3.Instance->CCR3=0;
		vatcan=1;
		HCSR04_Read();
		HAL_Delay(100);
		htim3.Instance->CCR3=300;
		HAL_Delay(100);
		htim3.Instance->CCR3=600;
		HAL_Delay(100);
		htim3.Instance->CCR3=900;
		HAL_Delay(100);
		htim3.Instance->CCR3=1000;
		HCSR04_Read();
		HAL_Delay(100);
		htim3.Instance->CCR3=500;
		if((d1 < 20)&& (d2 < 20))
    {
      motor_back();
      HAL_Delay(300);
      motor_stop();
      HAL_Delay(300);
    }
    else
    if( d1 >d2)
    {
      motor_right();
      HAL_Delay(750);
    }
    else
    if( d1 <=d2)
    {
      motor_left();
      HAL_Delay(750);
    }
    
	}else 
	{
		motor_up();
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(Rx_Data[0]=='0')
	{
		htim3.Instance->CCR1=100;
		htim3.Instance->CCR2=100;
	}else
	if(Rx_Data[0]=='1')
	{
		 htim3.Instance->CCR1=150;
		htim3.Instance->CCR2=150;
	}else if(Rx_Data[0]=='2')
	{
		htim3.Instance->CCR1=250;
		htim3.Instance->CCR2=250;
	}
	else if(Rx_Data[0]=='3')
	{
		htim3.Instance->CCR1=350;
		htim3.Instance->CCR2=350;
	}
	else if(Rx_Data[0]=='4')
	{
		htim3.Instance->CCR1=450;
		htim3.Instance->CCR2=450;
	}
	else if(Rx_Data[0]=='5')
	{
		htim3.Instance->CCR1=550;
		htim3.Instance->CCR2=550;
	}
	else if(Rx_Data[0]=='6')
	{
		htim3.Instance->CCR1=650;
		htim3.Instance->CCR2=650;
	}
	else if(Rx_Data[0]=='7')
	{
		htim3.Instance->CCR1=750;
		htim3.Instance->CCR2=750;
	}
	else if(Rx_Data[0]=='8')
	{
		htim3.Instance->CCR1=850;
		htim3.Instance->CCR2=850;
	}
	else if(Rx_Data[0]=='9')
	{
		htim3.Instance->CCR1=900;
		htim3.Instance->CCR2=900;
	}
	else if(Rx_Data[0]=='q')
	{
		htim3.Instance->CCR1=1000;
		htim3.Instance->CCR2=1000;
	}
	if(Rx_Data[0]=='F')
	{
		motor_up();
		HAL_UART_Transmit(&huart1,(uint8_t *)Rx_Data,1,1000);
		HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_Data,1);	
	}
	else
	{	
		if(Rx_Data[0]=='L')
		{
		if(tuhanh==0)
			motor_left();
			HAL_UART_Transmit(&huart1,(uint8_t *)Rx_Data,1,1000);
			HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_Data,1);	
		}
		else if(Rx_Data[0]=='R')
			{
				if(tuhanh==0)
					motor_right();
					HAL_UART_Transmit(&huart1,(uint8_t *)Rx_Data,1,1000);
					HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_Data,1);
			}
		else if(Rx_Data[0]=='S')
			{
				if(tuhanh==0)
					motor_stop();
					HAL_UART_Transmit(&huart1,(uint8_t *)Rx_Data,1,1000);
					HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_Data,1);
			}
		else if(Rx_Data[0]=='B')
			{
				if(tuhanh==0)
					motor_back();
					HAL_UART_Transmit(&huart1,(uint8_t *)Rx_Data,1,1000);
					HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_Data,1);
			}
		else if(Rx_Data[0]=='H')
			{
					tuhanh=1;
					HAL_UART_Transmit(&huart1,(uint8_t *)Rx_Data,1,1000);
					HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_Data,1);
			}
		else if(Rx_Data[0] == 'B')
			{
					tuhanh=0;
					HAL_UART_Transmit(&huart1,(uint8_t *)Rx_Data,1,1000);
					HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_Data,1);
			}	
	}
	HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_Data,1);
}

int main(void)
{
  
  HAL_Init();

  
  SystemClock_Config();
	HAL_UART_Receive_IT(&huart1,(uint8_t *)Rx_Data,1);
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  while (1)
  {
    if(tuhanh==1)
		{
			tuhanh_func();
		}
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 8-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 160;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  huart1.Init.BaudRate = 9600;
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
