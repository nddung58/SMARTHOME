/* stm32_uart.h - Minimal UART Interface for STM32F103C6T6 */

#ifndef __STM32_UART_H
#define __STM32_UART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx_hal_def.h"

  /* UART Initialization Structure */
  typedef struct
  {
    uint32_t BaudRate;     // Baud rate e.g. 9600, 115200
    uint32_t WordLength;   // UART_WORDLENGTH_8B
    uint32_t StopBits;     // UART_STOPBITS_1
    uint32_t Parity;       // UART_PARITY_NONE
    uint32_t Mode;         // UART_MODE_TX_RX
    uint32_t HwFlowCtl;    // UART_HWCONTROL_NONE
    uint32_t OverSampling; // UART_OVERSAMPLING_16
  } UART_InitTypeDef;

  /**
   * @brief UART Reception type definition
   * @note  UART Reception type value aims to identify which type of Reception is ongoing.
   *        This parameter can be a value of @ref UART_Reception_Type_Values :
   *           UART_RECEPTION_STANDARD         = 0x00U,
   *           UART_RECEPTION_TOIDLE           = 0x01U,
   */
  typedef uint32_t UART_RxTypeTypeDef;

#define UART_RECEPTION_STANDARD (0x00000000U) /*!< Standard reception                       */
#define UART_RECEPTION_TOIDLE (0x00000001U)   /*!< Reception till completion or IDLE event  */

  // UART State definitions
  typedef enum
  {
    UART_STATE_RESET = 0x00U,      /*!< Peripheral is not yet Initialized
                                           Value is allowed for gState and RxState */
    UART_STATE_READY = 0x20U,      /*!< Peripheral Initialized and ready for use
                                           Value is allowed for gState and RxState */
    UART_STATE_BUSY = 0x24U,       /*!< an internal process is ongoing
                                           Value is allowed for gState only */
    UART_STATE_BUSY_TX = 0x21U,    /*!< Data Transmission process is ongoing
                                           Value is allowed for gState only */
    UART_STATE_BUSY_RX = 0x22U,    /*!< Data Reception process is ongoing
                                           Value is allowed for RxState only */
    UART_STATE_BUSY_TX_RX = 0x23U, /*!< Data Transmission and Reception process is ongoing
                                           Not to be used for neither gState nor RxState.
                                           Value is result of combination (Or) between gState and RxState values */
    UART_STATE_TIMEOUT = 0xA0U,    /*!< Timeout state
                                           Value is allowed for gState only */
    UART_STATE_ERROR = 0xE0U       /*!< Error
                                           Value is allowed for gState only */
  } UART_StateTypeDef;

  /* UART Handle Structure */
  typedef struct UART_HandleTypeDef
  {
    USART_TypeDef *Instance; // USARTx instance (e.g. USART1)
    UART_InitTypeDef Init;   // UART configuration

    uint8_t *pTxBuffPtr;       // Pointer to TX buffer
    uint16_t TxXferSize;       // Size of TX transfer
    __IO uint16_t TxXferCount; // TX transfer count

    uint8_t *pRxBuffPtr;       // Pointer to RX buffer
    uint16_t RxXferSize;       // Size of RX transfer
    __IO uint16_t RxXferCount; // RX transfer count

    __IO UART_RxTypeTypeDef ReceptionType; // Reception type

    __IO UART_StateTypeDef gState; // UART state (busy, ready, etc.)

    __IO UART_StateTypeDef RxState; // Reception state (busy, ready, etc.)

    __IO uint32_t ErrorCode; // UART error code (parity error, framing error, etc.)

  } UART_HandleTypeDef;

/* UART Word Length */
#define UART_WORDLENGTH_8B ((uint32_t)0x00000000U)
#define UART_WORDLENGTH_9B ((uint32_t)USART_CR1_M)

/* UART Stop Bits */
#define UART_STOPBITS_1 ((uint32_t)0x00000000U)

/* UART Parity */
#define UART_PARITY_NONE ((uint32_t)0x00000000U)

/* UART Mode */
#define UART_MODE_TX ((uint32_t)USART_CR1_TE)
#define UART_MODE_RX ((uint32_t)USART_CR1_RE)
#define UART_MODE_TX_RX ((uint32_t)(USART_CR1_TE | USART_CR1_RE))

/* UART Hardware Flow Control */
#define UART_HWCONTROL_NONE ((uint32_t)0x00000000U)

/* UART Oversampling */
#define UART_OVERSAMPLING_16 ((uint32_t)0x00000000U)

/* UART Interrupt Enable*/
// === USART_CR1 Interrupts ===
#define UART_IT_PE ((uint32_t)USART_CR1_PEIE)     // Parity error interrupt
#define UART_IT_TXE ((uint32_t)USART_CR1_TXEIE)   // TX register empty interrupt
#define UART_IT_TC ((uint32_t)USART_CR1_TCIE)     // Transmission complete interrupt
#define UART_IT_RXNE ((uint32_t)USART_CR1_RXNEIE) // RX register not empty interrupt
#define UART_IT_IDLE ((uint32_t)USART_CR1_IDLEIE) // IDLE line detected interrupt

// === USART_CR2 Interrupts ===
#define UART_IT_LBD ((uint32_t)USART_CR2_LBDIE) // LIN break detection interrupt

// === USART_CR3 Interrupts ===
#define UART_IT_ERR ((uint32_t)USART_CR3_EIE << 16)   // Error interrupt: framing, noise, overrun
#define UART_IT_CTS ((uint32_t)USART_CR3_CTSIE << 16) // CTS interrupt (if flow control used)

// UART Error codes
#define UART_ERROR_NONE 0x00000000U // No error
#define UART_ERROR_PE 0x00000001U   // Parity error
#define UART_ERROR_NE 0x00000002U   // Noise error
#define UART_ERROR_FE 0x00000004U   // Framing error
#define UART_ERROR_ORE 0x00000008U  // Overrun error
#define UART_ERROR_DMA 0x00000010U  // DMA transfer error (nếu bạn dùng DMA)

#define UART_IT_MASK 0x0000FFFFU
#define UART_CR1_REG_INDEX 1U
#define UART_CR2_REG_INDEX 2U
#define UART_CR3_REG_INDEX 3U

#define UART_ENABLE_IT(__HANDLE__, __INTERRUPT__)                          \
    ( (((__INTERRUPT__) >> 28U) == UART_CR1_REG_INDEX) ?                   \
        ((__HANDLE__)->Instance->CR1 |= ((__INTERRUPT__) & UART_IT_MASK)) : \
      (((__INTERRUPT__) >> 28U) == UART_CR2_REG_INDEX) ?                   \
        ((__HANDLE__)->Instance->CR2 |= ((__INTERRUPT__) & UART_IT_MASK)) : \
        ((__HANDLE__)->Instance->CR3 |= ((__INTERRUPT__) & UART_IT_MASK)) )

#define UART_DISABLE_IT(__HANDLE__, __INTERRUPT__)                         \
    ( (((__INTERRUPT__) >> 28U) == UART_CR1_REG_INDEX) ?                   \
        ((__HANDLE__)->Instance->CR1 &= ~((__INTERRUPT__) & UART_IT_MASK)) : \
      (((__INTERRUPT__) >> 28U) == UART_CR2_REG_INDEX) ?                   \
        ((__HANDLE__)->Instance->CR2 &= ~((__INTERRUPT__) & UART_IT_MASK)) : \
        ((__HANDLE__)->Instance->CR3 &= ~((__INTERRUPT__) & UART_IT_MASK)) )

  /* UART Exported Functions */
  StatusTypeDef UART_Init(UART_HandleTypeDef *huart);
  StatusTypeDef UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
  StatusTypeDef UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);

  /* UART Interrupt-related APIs */
  void UART_IRQHandler(UART_HandleTypeDef *huart); ///< Gọi trong hàm ISR
  void UART_RxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_UART_H */
