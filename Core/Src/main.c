/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RELAYS_COUNT 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define STR1(x) #x
#define STR(x) STR1(x)

// #define HAL_UART_Transmit(&huart1, text) HAL_UART_Transmit(&huart1, text, strlen(text), 1000, 100);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

GPIO_TypeDef *relayPorts[] = {
    RELAY0_GPIO_Port,
    RELAY1_GPIO_Port,
    RELAY2_GPIO_Port,
    RELAY3_GPIO_Port};

uint16_t relayPins[] = {
    RELAY0_Pin,
    RELAY1_Pin,
    RELAY2_Pin,
    RELAY3_Pin};

GPIO_TypeDef *ledPorts[] = {
    LED0_GPIO_Port,
    LED1_GPIO_Port,
    LED2_GPIO_Port,
    LED3_GPIO_Port};

uint16_t ledPins[] = {
    LED0_Pin,
    LED1_Pin,
    LED2_Pin,
    LED3_Pin};

uint8_t currentlyActiveRelay;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
GPIO_PinState reversePinStateIf(GPIO_PinState state, uint8_t reverse)
{
  return reverse ? !state : state;
}

void turnOffAllRelays()
{
  for (int i = 0; i < RELAYS_COUNT; i++)
  {
    HAL_GPIO_WritePin(relayPorts[i], relayPins[i], reversePinStateIf(GPIO_PIN_RESET, i == 0));
    HAL_GPIO_WritePin(ledPorts[i], ledPins[i], GPIO_PIN_SET);
  }
}

void relayWrite(uint8_t relayNo, uint8_t set)
{
  turnOffAllRelays();
  HAL_GPIO_WritePin(relayPorts[relayNo], relayPins[relayNo], reversePinStateIf(set ? GPIO_PIN_SET : GPIO_PIN_RESET, relayNo == 0));
  HAL_GPIO_WritePin(ledPorts[relayNo], ledPins[relayNo], !set ? GPIO_PIN_SET : GPIO_PIN_RESET);
  currentlyActiveRelay = relayNo;
}
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_UART_Transmit(&huart1, STR(BUILD_ID) "\r\n", 2 + strlen(STR(BUILD_ID)), 100);
  // printf("UID: 0x%08X 0x%08X 0x%08X\r\n", HAL_GetUIDw0(), HAL_GetUIDw1(), HAL_GetUIDw2());

  uint8_t activePin = 0;
  relayWrite(activePin, 1);

  // there is some magic issue and button is pressed right after reset.
  // while (GPIO_PIN_SET == HAL_GPIO_ReadPin(CHANGE_GPIO_Port, CHANGE_Pin))
  HAL_Delay(100);

  char UART1_rxBuffer[1] = {0};
  while (1)
  {

    if (currentlyActiveRelay != activePin)
    {
      relayWrite(activePin, 1);
      HAL_UART_Transmit(&huart1, "change\r\n", 8, 100);
    }

    if (HAL_OK == HAL_UART_Receive(&huart1, UART1_rxBuffer, 1, 50) && UART1_rxBuffer[0] != 0x00)
    {
      HAL_UART_Transmit(&huart1, "got uart data\r\n", 15, 100);
      HAL_UART_Transmit(&huart1, UART1_rxBuffer, 1, 100);
      HAL_UART_Transmit(&huart1, "\r\n", 2, 100);
      switch (UART1_rxBuffer[0])
      {
      case '0':
      case '1':
      case '2':
      case '3':
        HAL_UART_Transmit(&huart1, "activating pin X: X\r\n", 18, 100);
        activePin = UART1_rxBuffer[0] - '0';
        break;
      case '?':
        HAL_UART_Transmit(&huart1, "active pin: X\r\n", 15, 100);
      }
    }

    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(CHANGE_GPIO_Port, CHANGE_Pin))
    {
      activePin++;
      activePin %= RELAYS_COUNT;
      // debounce
      do
      {
        HAL_Delay(50);
      } while (GPIO_PIN_RESET == HAL_GPIO_ReadPin(CHANGE_GPIO_Port, CHANGE_Pin));
    }

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
