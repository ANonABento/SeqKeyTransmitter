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
#include <string.h> // For memset and strcmp
#include <stdio.h>  // For printf

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
	// Sequence buffer
#define SEQ_BUFFER_SIZE 8
char seq_buffer[SEQ_BUFFER_SIZE];
uint8_t seq_index = 0;

	// Word buffer
#define WORD_BUFFER_SIZE 50
char word_buffer[WORD_BUFFER_SIZE];
uint8_t word_index = 0;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int _write(int file, char *ptr, int len) {
	HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	return len;
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  void add_to_sequence(char key) {
      if (seq_index < SEQ_BUFFER_SIZE) {
          seq_buffer[seq_index++] = key;
      }
  }

  char process_sequence() {
      // Example: Map sequences to letters
      if (strcmp(seq_buffer, "TI") == 0) return 'A';  // Thumb + Index = 'A'
      if (strcmp(seq_buffer, "TMM") == 0) return 'B'; // Thumb + Middle + Middle = 'B'
      // Add more mappings...

      // Clear sequence buffer after processing
      memset(seq_buffer, 0, SEQ_BUFFER_SIZE);
      seq_index = 0;

      return 0; // Return 0 if no match
  }

  void send_word(const char *word) {
      // Print the word to the debug console (if UART is enabled)
      printf("Word: %s\n", word);

      // Toggle an LED as feedback
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // toggle LED
      HAL_Delay(300);
      HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // toggle LED
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Poll buttons for input
	  if (HAL_GPIO_ReadPin(thumb_GPIO_Port, thumb_Pin) == GPIO_PIN_RESET) {
		  HAL_Delay(50); // Debounce delay
	      add_to_sequence('T'); // Add "thumb" to sequence
	  }
	  if (HAL_GPIO_ReadPin(index_GPIO_Port, index_Pin) == GPIO_PIN_RESET) {
	      HAL_Delay(50); // Debounce delay
	      add_to_sequence('I'); // Add "index" to sequence
	  }
	  if (HAL_GPIO_ReadPin(middle_GPIO_Port, middle_Pin) == GPIO_PIN_RESET) {
	      HAL_Delay(50); // Debounce delay
	      add_to_sequence('M'); // Add "middle" to sequence
	  }
	  if (HAL_GPIO_ReadPin(ring_GPIO_Port, ring_Pin) == GPIO_PIN_RESET) {
	      HAL_Delay(50); // Debounce delay
	      add_to_sequence('R'); // Add "ring" to sequence
	  }
	  if (HAL_GPIO_ReadPin(pinkie_GPIO_Port, pinkie_Pin) == GPIO_PIN_RESET) {
	      HAL_Delay(50); // Debounce delay
	      add_to_sequence('P'); // Add "pinkie" to sequence
	  }

	  // Confirm letter when the thumb button is pressed again
	  if (HAL_GPIO_ReadPin(thumb_GPIO_Port, thumb_Pin) == GPIO_PIN_RESET) {
	      HAL_Delay(50); // Debounce delay

	      char letter = process_sequence();
	      if (letter) {
	    	  word_buffer[word_index++] = letter; // Add letter to the word
	      }
	      else {
	    	  // If no letter, assume the word is complete
	          send_word(word_buffer);
	          memset(word_buffer, 0, WORD_BUFFER_SIZE);
	          word_index = 0;
	      }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
