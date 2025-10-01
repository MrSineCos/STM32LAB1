/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 * opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DIGIT_COUNT 2
#define COLOR_COUNT 3
#define SEG_COUNT 7

// Thêm enum để quản lý màu sắc
enum TrafficLightColor {
	RED, YELLOW, GREEN
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//redTime = yellowTime + greenTime
static const int yellowTime = 2;
static const int greenTime = 3;
static const int redTime = yellowTime + greenTime;

static const uint16_t LED_PINS[DIGIT_COUNT][COLOR_COUNT] = { { LED_RED_X_Pin,
LED_YELLOW_X_Pin, LED_GREEN_X_Pin }, // Đèn X
		{ LED_RED_Y_Pin, LED_YELLOW_Y_Pin, LED_GREEN_Y_Pin }  // Đèn Y
};

static GPIO_TypeDef *const LED_PORTS[DIGIT_COUNT][COLOR_COUNT] = { {
LED_RED_X_GPIO_Port, LED_YELLOW_X_GPIO_Port, LED_GREEN_X_GPIO_Port }, {
LED_RED_Y_GPIO_Port, LED_YELLOW_Y_GPIO_Port, LED_GREEN_Y_GPIO_Port } };

//---------------------ex 4,5---------------------//
static const uint16_t SEG_PINS[DIGIT_COUNT][SEG_COUNT] = { { A1_Pin, B1_Pin,
C1_Pin, D1_Pin, E1_Pin, F1_Pin, G1_Pin }, // led 7 đoạn cho đèn X
		{ A2_Pin, B2_Pin, C2_Pin, D2_Pin, E2_Pin, F2_Pin, G2_Pin }, // led 7 đoạn cho đèn Y
		};

static GPIO_TypeDef *const SEG_PORTS[DIGIT_COUNT][SEG_COUNT] = { { GPIOB, GPIOB,
GPIOB, GPIOB, GPIOB, GPIOB, GPIOB }, { GPIOB, GPIOB, GPIOB, GPIOB,
GPIOB, GPIOB, GPIOB }, };
static const uint8_t SEG_MAP[10] = { 0b00111111, // 0
		0b00000110, // 1
		0b01011011, // 2
		0b01001111, // 3
		0b01100110, // 4
		0b01101101, // 5
		0b01111101, // 6
		0b00000111, // 7
		0b01111111, // 8
		0b01101111  // 9
		};
//------------------ end ex4,5 ---------------------//
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void set_traffic_light(uint8_t index, enum TrafficLightColor color);
static void update_7seg(uint8_t index, uint8_t number);
static void set_segments_by_mask(uint8_t mask, uint8_t digit_idx);

static void init_display(void);

static void demo_ex3(int counter);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
	/* USER CODE BEGIN 2 */
	init_display();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	int counter = 100;
	while (1) {
		if (counter <= 0) {
			counter = 100;
		}
		demo_ex3(counter);
		HAL_Delay(1000);
		counter--;
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA,
			LED_RED_X_Pin | LED_YELLOW_X_Pin | LED_GREEN_X_Pin | LED_RED_Y_Pin
					| LED_YELLOW_Y_Pin | LED_GREEN_Y_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
			A1_Pin | B1_Pin | C1_Pin | D2_Pin | E2_Pin | F2_Pin | G2_Pin
					| D1_Pin | E1_Pin | F1_Pin | G1_Pin | A2_Pin | B2_Pin
					| C2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : LED_RED_X_Pin LED_YELLOW_X_Pin LED_GREEN_X_Pin LED_RED_Y_Pin
	 LED_YELLOW_Y_Pin LED_GREEN_Y_Pin */
	GPIO_InitStruct.Pin = LED_RED_X_Pin | LED_YELLOW_X_Pin | LED_GREEN_X_Pin
			| LED_RED_Y_Pin | LED_YELLOW_Y_Pin | LED_GREEN_Y_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : A1_Pin B1_Pin C1_Pin D2_Pin
	 E2_Pin F2_Pin G2_Pin D1_Pin
	 E1_Pin F1_Pin G1_Pin A2_Pin
	 B2_Pin C2_Pin */
	GPIO_InitStruct.Pin = A1_Pin | B1_Pin | C1_Pin | D2_Pin | E2_Pin | F2_Pin
			| G2_Pin | D1_Pin | E1_Pin | F1_Pin | G1_Pin | A2_Pin | B2_Pin
			| C2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**
 * @brief  Bật một màu đèn cụ thể cho một cột đèn và tắt các màu khác.
 * @param  index: 0 cho đèn X, 1 cho đèn Y.
 * @param  color: RED, YELLOW, hoặc GREEN.
 * @retval None
 */
static void set_traffic_light(uint8_t index, enum TrafficLightColor color) {
	// Tắt hết các đèn của cột đèn 'index' trước
	HAL_GPIO_WritePin(LED_PORTS[index][RED], LED_PINS[index][RED],
			GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_PORTS[index][YELLOW], LED_PINS[index][YELLOW],
			GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_PORTS[index][GREEN], LED_PINS[index][GREEN],
			GPIO_PIN_SET);

	// Bật đèn mong muốn
	switch (color) {
	case RED:
		HAL_GPIO_WritePin(LED_PORTS[index][RED], LED_PINS[index][RED],
				GPIO_PIN_RESET);
		break;
	case YELLOW:
		HAL_GPIO_WritePin(LED_PORTS[index][YELLOW], LED_PINS[index][YELLOW],
				GPIO_PIN_RESET);
		break;
	case GREEN:
		HAL_GPIO_WritePin(LED_PORTS[index][GREEN], LED_PINS[index][GREEN],
				GPIO_PIN_RESET);
		break;
	}
}

/**
 * @brief  Hiển thị một số lên LED 7 đoạn.
 * @param  index: 0 cho LED X, 1 cho LED Y.
 * @param  number: Số cần hiển thị (0-9). Nếu số > 9, LED sẽ tắt.
 * @retval None
 */
static void update_7seg(uint8_t index, uint8_t number) {
	if (index >= DIGIT_COUNT)
		return;

	if (number > 9) {
		set_segments_by_mask(0x00, index); // Tắt hết segment
	} else {
		uint8_t mask = SEG_MAP[number];
		set_segments_by_mask(mask, index);
	}
}

static void set_segments_by_mask(uint8_t mask, uint8_t digit_idx) {
	for (int s = 0; s < 7; s++) {

		GPIO_TypeDef *port = (GPIO_TypeDef*) SEG_PORTS[digit_idx][s];
		uint16_t pin = SEG_PINS[digit_idx][s];

		// Bit thứ s của mask tương ứng với segment thứ s (A=0, B=1, ..., G=6)
		if ((mask >> s) & 1) {
			HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET); // Bật segment (active low)
		} else {
			HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET); // Tắt segment
		}
	}
}

static void init_display() {
	for (int i = 0; i < DIGIT_COUNT; i++) {
		for (int j = 0; j < COLOR_COUNT; j++) {
			HAL_GPIO_WritePin(LED_PORTS[i][j], LED_PINS[i][j], GPIO_PIN_SET); // SET để tắt
		}
	}
	// Tắt hết 2 led 7 đoạn
	update_7seg(0, 10); // Gửi số > 9 để tắt
	update_7seg(1, 10);
}

static void demo_ex3(int counter) {
	counter %= 10;
	switch (counter) {
	case 0:
		set_traffic_light(0, RED);
		set_traffic_light(1, GREEN);
		break;
	case 7:
		set_traffic_light(1, YELLOW);
		break;
	case 5:
		set_traffic_light(0, GREEN);
		set_traffic_light(1, RED);
		break;
	case 2:
		set_traffic_light(0, YELLOW);
		break;
	default:
		break;
	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
