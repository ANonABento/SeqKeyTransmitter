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

#define SEQ_BUFFER_SIZE 32
#define WORD_BUFFER_SIZE 32

char seq_buffer[SEQ_BUFFER_SIZE] = {0};
char word_buffer[WORD_BUFFER_SIZE] = {0};
uint8_t seq_index = 0;
uint8_t word_index = 0;

uint8_t button_state[5] = {0};      // Current button state
uint8_t button_prev_state[5] = {0}; // Previous button state
char button_chars[5] = {'#', '|', '-', '/', '('};

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

void update_button_states();
void add_to_sequence(char key);
char process_sequence();
void send_word(const char *word);
void reset_buffers();
void log_button_press(void);

void update_button_states() {
	button_prev_state[0] = button_state[0];
	button_prev_state[1] = button_state[1];
	button_prev_state[2] = button_state[2];
	button_prev_state[3] = button_state[3];
	button_prev_state[4] = button_state[4];

	button_state[0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_RESET; //thumb
	button_state[1] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET; //index
	button_state[2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_RESET; //middle
	button_state[3] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == GPIO_PIN_RESET; //ring
	button_state[4] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == GPIO_PIN_RESET; //pink
}

void log_button_press(void) {
	for (int i = 0; i < 5; i++) {
		if (button_prev_state[i] == 0 && button_state[i] == 1) { // Button pressed
			printf("Button %c was pressed.\r\n", button_chars[i]);
		}
	}
}

void add_to_sequence(char key) {
	if (seq_index < SEQ_BUFFER_SIZE - 1) {
		seq_buffer[seq_index++] = key;
		seq_buffer[seq_index] = '\0'; // Null-terminate the sequence
	}
}

char process_sequence() {
	// Letter mappings (A-Z)
	if (strcmp(seq_buffer, "//-") == 0 || strcmp(seq_buffer, "/") == 0) return 'A';
	if (strcmp(seq_buffer, "|((") == 0 || strcmp(seq_buffer, "-(") == 0) return 'B';
	if (strcmp(seq_buffer, "(") == 0) return 'C';
	if (strcmp(seq_buffer, "|(") == 0) return 'D';
	if (strcmp(seq_buffer, "---") == 0 || strcmp(seq_buffer, "-") == 0) return 'E';
	if (strcmp(seq_buffer, "|--") == 0 || strcmp(seq_buffer, "--") == 0) return 'F';
	if (strcmp(seq_buffer, "(-|") == 0) return 'G';
	if (strcmp(seq_buffer, "|-|") == 0 || strcmp(seq_buffer, "||") == 0) return 'H';
	if (strcmp(seq_buffer, "-|-") == 0 || strcmp(seq_buffer, "|") == 0) return 'I';
	if (strcmp(seq_buffer, "-|(") == 0) return 'J';
	if (strcmp(seq_buffer, "|//") == 0) return 'K';
	if (strcmp(seq_buffer, "-|") == 0) return 'L';
	if (strcmp(seq_buffer, "|//|") == 0 || strcmp(seq_buffer, "|//") == 0) return 'M';
	if (strcmp(seq_buffer, "|/|") == 0 || strcmp(seq_buffer, "|/") == 0) return 'N';
	if (strcmp(seq_buffer, "((") == 0) return 'O';
	if (strcmp(seq_buffer, "|(-") == 0 || strcmp(seq_buffer, "/-") == 0) return 'P';
	if (strcmp(seq_buffer, "((/") == 0) return 'Q';
	if (strcmp(seq_buffer, "|(/") == 0 || strcmp(seq_buffer, "(/") == 0) return 'R';
	if (strcmp(seq_buffer, "(-(") == 0 || strcmp(seq_buffer, "(-") == 0) return 'S';
	if (strcmp(seq_buffer, "-|") == 0) return 'T';
	if (strcmp(seq_buffer, "|(|") == 0 || strcmp(seq_buffer, "(|") == 0) return 'U';
	if (strcmp(seq_buffer, "/(/") == 0 || strcmp(seq_buffer, "/(") == 0) return 'V';
	if (strcmp(seq_buffer, "////") == 0 || strcmp(seq_buffer, "///") == 0) return 'W';
	if (strcmp(seq_buffer, "//") == 0) return 'X';
	if (strcmp(seq_buffer, "/|/") == 0 || strcmp(seq_buffer, "/|") == 0) return 'Y';
	if (strcmp(seq_buffer, "-/-") == 0 || strcmp(seq_buffer, "-/") == 0) return 'Z';

	// Number mappings (1-9, 0)
	if (strcmp(seq_buffer, "1") == 0) return '1';
	if (strcmp(seq_buffer, "2") == 0) return '2';
	if (strcmp(seq_buffer, "3") == 0) return '3';
	if (strcmp(seq_buffer, "4") == 0) return '4';
	if (strcmp(seq_buffer, "5") == 0) return '5';
	if (strcmp(seq_buffer, "6") == 0) return '6';
	if (strcmp(seq_buffer, "7") == 0) return '7';
	if (strcmp(seq_buffer, "8") == 0) return '8';
	if (strcmp(seq_buffer, "9") == 0) return '9';
	if (strcmp(seq_buffer, "0") == 0) return '0';

	// Control Commands
	if (strcmp(seq_buffer, "combination of (-|/") == 0) return '_';  // Reset sequence
	if (strcmp(seq_buffer, "(/-|") == 0) return '-';  // Backspace
	if (strcmp(seq_buffer, "#") == 0) return '>';  // Enter letter
	if (strcmp(seq_buffer, "##") == 0) return '@';  // Send word

	// Clear sequence buffer after processing
	memset(seq_buffer, 0, SEQ_BUFFER_SIZE);
	seq_index = 0;

	return 0; // Return 0 if no match
}

void send_word(const char *word) {
	printf("Word sent: %s\r\n", word);

	// Toggle an LED as feedback
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	HAL_Delay(300);
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void reset_buffers() {
	memset(seq_buffer, 0, SEQ_BUFFER_SIZE);
	memset(word_buffer, 0, WORD_BUFFER_SIZE);
	seq_index = 0;
	word_index = 0;
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

  printf("UART Initialized\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
	  update_button_states();
	  log_button_press();

	  // Detect button press and release
	  for (int i = 0; i < 5; i++) {
		  if (button_prev_state[i] == 0 && button_state[i] == 1) { // Button pressed
			  add_to_sequence(button_chars[i]);
			  printf("Button pressed: %c\r\n", button_chars[i]);
		  }
	  }

	  // Check for control commands
	  char command = process_sequence();
	  if (command) {
		  printf("Command executed: %c\r\n", command);

		  if (command == '>') { // Enter letter
			  char letter = process_sequence();
			  if (letter) {
				  word_buffer[word_index++] = letter;
				  word_buffer[word_index] = '\0';
				  printf("Letter added: %c\r\n", letter);
			  }
		  } else if (command == '@') { // Send word
			  send_word(word_buffer);
			  reset_buffers();
		  } else if (command == '_') { // Reset sequence
			  reset_buffers();
		  } else if (command == '-') { // Backspace
			  if (word_index > 0) {
				  word_buffer[--word_index] = '\0';
				  printf("Backspace executed\r\n");
			  }
		  }

		  // Clear sequence buffer after processing
		  memset(seq_buffer, 0, SEQ_BUFFER_SIZE);
		  seq_index = 0;
	  }

	  HAL_Delay(50); // Reduce CPU load
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
