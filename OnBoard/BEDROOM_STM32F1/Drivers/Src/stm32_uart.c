/* stm32_uart.c - Minimal UART Implementation for STM32F103C6T6 */

#include "stm32_uart.h"

static StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart);
static StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart);


StatusTypeDef UART_Init(UART_HandleTypeDef *huart)
{
    if (huart == NULL || huart->Instance == NULL)
        return DUNGX_ERROR;

    // Enable clock for USART1 and GPIOA
    if (huart->Instance == USART1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;
    }

    if (huart->Instance == USART1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

        // PA9 - TX
        GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
        GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0;
        GPIOA->CRH |= GPIO_CRH_CNF9_1;

        // PA10 - RX
        GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
        GPIOA->CRH |= GPIO_CRH_CNF10_0; // Input floating

        NVIC_EnableIRQ(USART1_IRQn);
        NVIC_SetPriority(USART1_IRQn, 1);
    }

    else if (huart->Instance == USART2)
    {
        RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

        // PA2 - TX
        GPIOA->CRL &= ~(GPIO_CRL_MODE2 | GPIO_CRL_CNF2);
        GPIOA->CRL |= GPIO_CRL_MODE2_1 | GPIO_CRL_MODE2_0;
        GPIOA->CRL |= GPIO_CRL_CNF2_1;

        // PA3 – RX – input floating
        GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
        GPIOA->CRL |= GPIO_CRL_CNF3_0; // Input floating (01)

        NVIC_EnableIRQ(USART2_IRQn);
        NVIC_SetPriority(USART2_IRQn, 1);
    }
    // Configure baud rate
    uint32_t pclk = 72000000;
    huart->Instance->BRR = (pclk + huart->Init.BaudRate / 2) / huart->Init.BaudRate;

    // Clear relevant bits, then set
    huart->Instance->CR1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE);
    huart->Instance->CR1 |= huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode;

    // ========================
    // CR2 - Stop bits
    // ========================
    huart->Instance->CR2 &= ~(USART_CR2_STOP);
    huart->Instance->CR2 |= huart->Init.StopBits;

    // ========================
    // CR3 - Hardware flow control
    // ========================
    huart->Instance->CR3 &= ~(USART_CR3_CTSE | USART_CR3_RTSE);
    huart->Instance->CR3 |= huart->Init.HwFlowCtl;

    // Enable USART
    huart->Instance->CR1 |= USART_CR1_UE;

    // Set internal state
    huart->gState = UART_STATE_READY;
    huart->RxState = UART_STATE_READY;
    huart->ErrorCode = UART_ERROR_NONE;

    return DUNGX_OK;
}

StatusTypeDef UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (huart == NULL || pData == NULL || Size == 0)
        return DUNGX_ERROR;

    while (Size--)
    {
        // Wait until TXE (Transmit Data Register Empty)
        while (!(huart->Instance->SR & USART_SR_TXE))
        {
            if (Timeout-- == 0)
                return DUNGX_TIMEOUT;
        }

        huart->Instance->DR = (*pData++ & 0xFF);
    }

    // Wait for TC (Transmission Complete)
    while (!(huart->Instance->SR & USART_SR_TC))
    {
        if (Timeout-- == 0)
            return DUNGX_TIMEOUT;
    }

    return DUNGX_OK;
}

StatusTypeDef UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (huart == NULL || pData == NULL || Size == 0)
        return DUNGX_ERROR;

    while (Size--)
    {
        // Wait until RXNE (Read Data Register Not Empty)
        while (!(huart->Instance->SR & USART_SR_RXNE))
        {
            if (Timeout-- == 0)
                return DUNGX_TIMEOUT;
        }

        *pData++ = (uint8_t)(huart->Instance->DR & 0xFF);
    }

    return DUNGX_OK;
}

void UART_IRQHandler(UART_HandleTypeDef *huart)
{
    uint32_t isrflags = huart->Instance->SR;
    uint32_t cr1its = huart->Instance->CR1;

    /* Check for RXNE (data received) */
    if ((isrflags & USART_SR_RXNE) && (cr1its & USART_CR1_RXNEIE))
    {
        UART_Receive_IT(huart); // Gọi hàm xử lý nhận IT
        return;
    }

    if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
      {
        UART_Transmit_IT(huart);
        return;
      }


    /* Check for errors */
    if ((isrflags & (USART_SR_PE | USART_SR_FE | USART_SR_NE | USART_SR_ORE)) != 0)
    {
        if (isrflags & USART_SR_PE)
            huart->ErrorCode |= UART_ERROR_PE;
        if (isrflags & USART_SR_FE)
            huart->ErrorCode |= UART_ERROR_FE;
        if (isrflags & USART_SR_NE)
            huart->ErrorCode |= UART_ERROR_NE;
        if (isrflags & USART_SR_ORE)
            huart->ErrorCode |= UART_ERROR_ORE;

        huart->gState = UART_STATE_ERROR;
    }
}

static StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
    if (huart->RxState != UART_STATE_BUSY_RX)
        return DUNGX_BUSY;

    uint8_t *pdata8bits;
    uint16_t *pdata16bits;

    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
        pdata16bits = (uint16_t *)huart->pRxBuffPtr;
        *pdata16bits = (uint16_t)(huart->Instance->DR & 0x01FF);
        huart->pRxBuffPtr += 2U;
    }
    else
    {
        pdata8bits = (uint8_t *)huart->pRxBuffPtr;

        if ((huart->Init.WordLength == UART_WORDLENGTH_9B) ||
            ((huart->Init.WordLength == UART_WORDLENGTH_8B) && (huart->Init.Parity == UART_PARITY_NONE)))
        {
            *pdata8bits = (uint8_t)(huart->Instance->DR & 0xFF);
        }
        else
        {
            *pdata8bits = (uint8_t)(huart->Instance->DR & 0x7F);
        }
        huart->pRxBuffPtr += 1U;
    }

    if (--huart->RxXferCount == 0U)
    {
        // Tắt ngắt RXNE
        huart->Instance->CR1 &= ~USART_CR1_RXNEIE;

        // Cập nhật trạng thái
        huart->RxState = UART_STATE_READY;

        // Gọi callback
        UART_RxCpltCallback(huart);
    }

    return DUNGX_OK;
}

static StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart)
{
    {
        const uint16_t *tmp;

        /* Check that a Tx process is ongoing */
        if (huart->gState == UART_STATE_BUSY_TX)
        {
            if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
            {
                tmp = (const uint16_t *)huart->pTxBuffPtr;
                huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
                huart->pTxBuffPtr += 2U;
            }
            else
            {
                huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
            }

            if (--huart->TxXferCount == 0U)
            {
                /* Disable the UART Transmit Data Register Empty Interrupt */
                UART_DISABLE_IT(huart, UART_IT_TXE);

                /* Enable the UART Transmit Complete Interrupt */
                UART_ENABLE_IT(huart, UART_IT_TC);
            }
            return DUNGX_OK;
        }
        else
        {
            return DUNGX_BUSY;
        }
    }
}

__weak void UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // Override in up layer
}
