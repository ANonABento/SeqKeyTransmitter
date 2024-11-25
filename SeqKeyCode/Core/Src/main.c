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

char seq_buffer[SEQ_BUFFER_SIZE] = {0}; // Holds the 4-button sequence
char word_buffer[WORD_BUFFER_SIZE] = {0}; // Holds the constructed string
uint8_t seq_index = 0; // Tracks the sequence length
uint8_t word_index = 0; // Tracks the word length

uint8_t button_state[5] = {0};      // Current button state
uint8_t button_prev_state[5] = {0}; // Previous button state
char button_chars[4] = {'|', '-', '/', '('}; // Buttons for sequence (excluding thumb)

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
void process_sequence();
void reset_buffers();
void send_string();

void update_button_states() {
	// Update previous states
	for (int i = 0; i < 5; i++) {
		button_prev_state[i] = button_state[i];
	}

	// Read current button states
	button_state[0] = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_RESET; // Thumb
	button_state[1] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET; // Index
	button_state[2] = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_RESET; // Middle
	button_state[3] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == GPIO_PIN_RESET; // Ring
	button_state[4] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == GPIO_PIN_RESET; // Pinky
}

void process_sequence() {
	// Map sequence to a letter
	char letter = 0;

	// Letter mappings (A-Z)
	if (strcmp(seq_buffer, "//-") == 0 || strcmp(seq_buffer, "/") == 0) letter = 'A';
	if (strcmp(seq_buffer, "|((") == 0 || strcmp(seq_buffer, "-(") == 0) letter = 'B';
	if (strcmp(seq_buffer, "(") == 0) letter = 'C';
	if (strcmp(seq_buffer, "|(") == 0) letter = 'D';
	if (strcmp(seq_buffer, "---") == 0 || strcmp(seq_buffer, "-") == 0) letter = 'E';
	if (strcmp(seq_buffer, "|--") == 0 || strcmp(seq_buffer, "--") == 0) letter = 'F';
	if (strcmp(seq_buffer, "(-|") == 0) letter = 'G';
	if (strcmp(seq_buffer, "|-|") == 0 || strcmp(seq_buffer, "||") == 0) letter = 'H';
	if (strcmp(seq_buffer, "-|-") == 0 || strcmp(seq_buffer, "|") == 0) letter = 'I';
	if (strcmp(seq_buffer, "-|(") == 0) letter = 'J';
	if (strcmp(seq_buffer, "|//") == 0) letter = 'K';
	if (strcmp(seq_buffer, "|-") == 0) letter = 'L';
	if (strcmp(seq_buffer, "|//|") == 0 || strcmp(seq_buffer, "|//") == 0) letter = 'M';
	if (strcmp(seq_buffer, "|/|") == 0 || strcmp(seq_buffer, "|/") == 0) letter = 'N';
	if (strcmp(seq_buffer, "((") == 0) letter = 'O';
	if (strcmp(seq_buffer, "|(-") == 0 || strcmp(seq_buffer, "/-") == 0) letter = 'P';
	if (strcmp(seq_buffer, "((/") == 0) letter = 'Q';
	if (strcmp(seq_buffer, "|(/") == 0 || strcmp(seq_buffer, "(/") == 0) letter = 'R';
	if (strcmp(seq_buffer, "(-(") == 0 || strcmp(seq_buffer, "(-") == 0) letter = 'S';
	if (strcmp(seq_buffer, "-|") == 0) letter = 'T';
	if (strcmp(seq_buffer, "|(|") == 0 || strcmp(seq_buffer, "(|") == 0) letter = 'U';
	if (strcmp(seq_buffer, "/(/") == 0 || strcmp(seq_buffer, "/(") == 0) letter = 'V';
	if (strcmp(seq_buffer, "////") == 0 || strcmp(seq_buffer, "///") == 0) letter = 'W';
	if (strcmp(seq_buffer, "//") == 0) letter = 'X';
	if (strcmp(seq_buffer, "/|/") == 0 || strcmp(seq_buffer, "/|") == 0) letter = 'Y';
	if (strcmp(seq_buffer, "-/-") == 0 || strcmp(seq_buffer, "-/") == 0) letter = 'Z';

	// Number mappings (1-9, 0)
//	if (strcmp(seq_buffer, "1") == 0) letter = '1';
//	if (strcmp(seq_buffer, "2") == 0) letter = '2';
//	if (strcmp(seq_buffer, "3") == 0) letter = '3';
//	if (strcmp(seq_buffer, "4") == 0) letter = '4';
//	if (strcmp(seq_buffer, "5") == 0) letter = '5';
//	if (strcmp(seq_buffer, "6") == 0) letter = '6';
//	if (strcmp(seq_buffer, "7") == 0) letter = '7';
//	if (strcmp(seq_buffer, "8") == 0) letter = '8';
//	if (strcmp(seq_buffer, "9") == 0) letter = '9';
//	if (strcmp(seq_buffer, "0") == 0) letter = '0';

	// Control Commands
//	if (strcmp(seq_buffer, "combination of (-|/") == 0) letter = '_';  // Reset sequence
//	if (strcmp(seq_buffer, "(/-|") == 0) letter = '-';  // Backspace
//	if (strcmp(seq_buffer, "#") == 0) letter = '>';  // Enter letter
//	if (strcmp(seq_buffer, "##") == 0) letter = '@';  // Send word

	if (letter) {
		// Append the letter to the word buffer
		if (word_index < WORD_BUFFER_SIZE - 1) {
			word_buffer[word_index++] = letter;
			word_buffer[word_index] = '\n'; // Null-terminate the string
			printf("Letter added: %c\r\n", letter);
		}
		else {
			printf("Word buffer full!\r\n");
		}
	}
	else {
		printf("Invalid sequence: %s\r\n", seq_buffer);
	}

	// Clear sequence buffer after processing
	memset(seq_buffer, 0, SEQ_BUFFER_SIZE);
	seq_index = 0;
}

void send_word() {
	printf("String sent: %s\r\n", word_buffer);

	HAL_UART_Transmit(&huart6, (uint8_t *)word_buffer, strlen(word_buffer), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Reset word buffer
	memset(word_buffer, 0, WORD_BUFFER_SIZE);
	word_index = 0;

	// Toggle LED for feedback
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
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */

  printf("UART Initialized \r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
	  update_button_states();

	  // Detect button presses for sequence
	  for (int i = 1; i <= 4; i++) { // Exclude thumb
		  if (button_prev_state[i] == 0 && button_state[i] == 1) { // Button pressed
			  if (seq_index < SEQ_BUFFER_SIZE - 1) {
				  seq_buffer[seq_index++] = button_chars[i - 1];
				  seq_buffer[seq_index] = '\0'; // Null-terminate
				  printf("Button added to sequence: %c\r\n", button_chars[i - 1]);
			  }
			  else {
				  printf("Sequence buffer full!\r\n");
			  }
		  }
	  }

	  // Detect thumb press to process the sequence
	  if (button_prev_state[0] == 0 && button_state[0] == 1) { // Thumb button pressed
	      if (seq_index == 0) { // Sequence is empty
	          if (word_index > 0) { // String exists to be sent
	              send_word();
	          } else {
	              printf("No string to send!\r\n");
	          }
	      } else {
	          printf("Processing sequence: %s\r\n", seq_buffer);
	          process_sequence();
	      }
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
