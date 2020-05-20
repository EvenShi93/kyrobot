#include "periph.h"
#include "log.h"

#if ((!UART4_TX_ENABLE) && (!UART4_RX_ENABLE))
#undef UART4_ENABLE
#define UART4_ENABLE                       (0)
#endif /* !UART4_TX_ENABLE && !UART4_RX_ENABLE */

#if !UART4_ENABLE
#undef UART4_TX_ENABLE
#undef UART4_RX_ENABLE
#define UART4_TX_ENABLE                    (0)
#define UART4_RX_ENABLE                    (0)
#endif /* !UART4_ENABLE */

#if !UART4_TX_ENABLE
#undef UART4_DMA_TX_ENABLE
#define UART4_DMA_TX_ENABLE                (0)
#endif /* !UART4_TX_ENABLE */

#if !UART4_RX_ENABLE
#undef UART4_DMA_RX_ENABLE
#define UART4_DMA_RX_ENABLE                (0)
#endif /* !UART4_RX_ENABLE */

static status_t uart4_init(uint32_t baudrate);
static status_t uart4_deinit(void);
static status_t uart4_tx_bytes(uint8_t *p, uint32_t l);
static status_t uart4_tx_bytes_it(uint8_t *p, uint32_t l);
static status_t uart4_tx_bytes_dma(uint8_t *p, uint32_t l);

static uint32_t uart4_rx_bytes(uint8_t *p, uint32_t l);
static uint32_t uart4_cache_usage(void);

static const char *TAG = "ttyS4";

const uart_dev_t uart4_dev = {
//  "ttyS4",
  &uart4_init,
  &uart4_deinit,
  &uart4_rx_bytes,
  &uart4_tx_bytes,
  &uart4_tx_bytes_it,
  &uart4_tx_bytes_dma,
  &uart4_cache_usage,
};

#if UART4_ENABLE

#include "string.h"

#if UART4_TX_ENABLE
#include "cmsis_os.h"
#endif /* UART4_TX_ENABLE */

#if UART4_RX_ENABLE
#include "ringbuffer.h"
#endif /* UART4_RX_ENABLE */

/* UART handler declaration */
static UART_HandleTypeDef *Uart4Handle;
#if UART4_DMA_TX_ENABLE
static DMA_HandleTypeDef *Uart4TxDmaHandle;
#endif /* UART4_DMA_TX_ENABLE */
#if UART4_DMA_RX_ENABLE
static DMA_HandleTypeDef *Uart4RxDmaHandle;
#endif /* UART4_DMA_RX_ENABLE */

#if UART4_TX_ENABLE
static osMutexId u4_tx_mutex;
#endif /* UART4_TX_ENABLE */

#if UART4_RX_ENABLE
#define UART4_DRV_RX_CACHE_SIZE            128
static ringbuffer_handle *uart4_rb;

static uint8_t *_u4_user_rx_cache = NULL;
static uint8_t *_u4_drv_rx_buffer = NULL;
#endif /* UART4_RX_ENABLE */

static void uart4_msp_init(UART_HandleTypeDef *huart);
static void uart4_msp_deinit(UART_HandleTypeDef *huart);

#if UART4_TX_ENABLE
static void uart4_txcplt_callback(UART_HandleTypeDef *huart);
#endif /* UART4_TX_ENABLE */
#if UART4_RX_ENABLE
static void uart4_rxcplt_callback(UART_HandleTypeDef *huart);
#endif /* UART4_RX_ENABLE */

static status_t uart4_init(uint32_t baudrate)
{
  status_t ret = status_ok;

  Uart4Handle = kmm_alloc(sizeof(UART_HandleTypeDef));
#if UART4_RX_ENABLE
  uart4_rb = kmm_alloc(sizeof(ringbuffer_handle));
  _u4_user_rx_cache = kmm_alloc(UART4_USER_RX_CACHE_DEPTH);
  _u4_drv_rx_buffer = kmm_alloc(UART4_DRV_RX_CACHE_SIZE);
#endif /* UART4_RX_ENABLE */
#if UART4_DMA_TX_ENABLE
  Uart4TxDmaHandle = kmm_alloc(sizeof(DMA_HandleTypeDef));
#endif /* UART4_DMA_TX_ENABLE */
#if UART4_DMA_RX_ENABLE
  Uart4RxDmaHandle = kmm_alloc(sizeof(DMA_HandleTypeDef));
#endif /* UART4_DMA_RX_ENABLE */
  if(Uart4Handle == NULL
#if UART4_RX_ENABLE
    || uart4_rb == NULL || _u4_user_rx_cache == NULL || _u4_drv_rx_buffer == NULL
#endif /* UART4_RX_ENABLE */
#if UART4_DMA_TX_ENABLE
    || Uart4TxDmaHandle == NULL
#endif /* UART4_DMA_TX_ENABLE */
#if UART4_DMA_RX_ENABLE
    || Uart4RxDmaHandle == NULL
#endif /* UART4_DMA_RX_ENABLE */
  ) {
    ky_err(TAG, "memory alloc fail");
    ret = status_nomem;
    goto error;
  }
  memset(Uart4Handle, 0, sizeof(UART_HandleTypeDef));
#if UART4_RX_ENABLE
  memset(uart4_rb, 0, sizeof(ringbuffer_handle));
  memset(_u4_user_rx_cache, 0, UART4_USER_RX_CACHE_DEPTH);
  memset(_u4_drv_rx_buffer, 0, UART4_DRV_RX_CACHE_SIZE);
#endif /* UART4_RX_ENABLE */
#if UART4_DMA_TX_ENABLE
  memset(Uart4TxDmaHandle, 0, sizeof(DMA_HandleTypeDef));
#endif /* UART4_DMA_TX_ENABLE */
#if UART4_DMA_RX_ENABLE
  memset(Uart4RxDmaHandle, 0, sizeof(DMA_HandleTypeDef));
#endif /* UART4_DMA_RX_ENABLE */

#if UART4_TX_ENABLE
  /* Create the mutex  */
  osMutexDef(U4TxMutex);
  u4_tx_mutex = osMutexCreate(osMutex(U4TxMutex));
  if(u4_tx_mutex == NULL) {
    ret = status_error;
    goto error;
  }
#endif /* UART4_TX_ENABLE */

  /* Configure the UART peripheral */
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  Uart4Handle->Instance        = UART4;
  Uart4Handle->Init.BaudRate   = baudrate;
  Uart4Handle->Init.WordLength = UART_WORDLENGTH_8B;
  Uart4Handle->Init.StopBits   = UART_STOPBITS_1;
  Uart4Handle->Init.Parity     = UART_PARITY_NONE;
  Uart4Handle->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
#if ((UART4_TX_ENABLE) && (UART4_RX_ENABLE))
  Uart4Handle->Init.Mode       = UART_MODE_TX_RX;
#endif /* UART4_TX_ENABLE && UART4_RX_ENABLE */
#if !UART4_TX_ENABLE
  Uart4Handle->Init.Mode       = UART_MODE_RX;
#endif /* !UART4_TX_ENABLE */
#if !UART4_RX_ENABLE
  Uart4Handle->Init.Mode       = UART_MODE_TX;
#endif /* !UART4_RX_ENABLE */

  /* register callbacks */
  Uart4Handle->MspInitCallback = uart4_msp_init;
  Uart4Handle->MspDeInitCallback = uart4_msp_deinit;
#if UART4_TX_ENABLE
  Uart4Handle->TxCpltCallback = uart4_txcplt_callback;
#endif /* UART4_TX_ENABLE */
#if UART4_RX_ENABLE
  Uart4Handle->RxCpltCallback = uart4_rxcplt_callback;
#endif /* UART4_RX_ENABLE */
  if(HAL_UART_Init(Uart4Handle) != HAL_OK) {
    ret = status_error;
    goto error;
  }

#if UART4_RX_ENABLE
  ringbuffer_init(uart4_rb, _u4_user_rx_cache, UART4_USER_RX_CACHE_DEPTH);
#endif /* UART4_RX_ENABLE */

  /* start receive task */
#if UART4_RX_ENABLE
#if UART4_DMA_RX_ENABLE
  HAL_UART_Receive_DMA(Uart4Handle, (uint8_t *)_u4_drv_rx_buffer, UART4_DRV_RX_CACHE_SIZE);
#else
  HAL_UART_Receive_IT(Uart4Handle, (uint8_t *)_u4_drv_rx_buffer, UART4_DRV_RX_CACHE_SIZE);
#endif /* UART4_DMA_RX_ENABLE */
  __HAL_UART_ENABLE_IT(Uart4Handle, UART_IT_IDLE);
#endif /* UART4_RX_ENABLE */
  return ret;
error:
  kmm_free(Uart4Handle);
#if UART4_RX_ENABLE
  kmm_free(uart4_rb);
  kmm_free(_u4_user_rx_cache);
  kmm_free(_u4_drv_rx_buffer);
#endif /* UART4_RX_ENABLE */
#if UART4_DMA_TX_ENABLE
  kmm_free(Uart4TxDmaHandle);
#endif /* UART4_DMA_TX_ENABLE */
#if UART4_DMA_RX_ENABLE
  kmm_free(Uart4RxDmaHandle);
#endif /* UART4_DMA_RX_ENABLE */
#if UART4_TX_ENABLE
  osMutexDelete(u4_tx_mutex);
#endif /* UART4_TX_ENABLE */
  return ret;
}

static status_t uart4_deinit(void)
{
  HAL_UART_DeInit(Uart4Handle);
  kmm_free(Uart4Handle);
#if UART4_RX_ENABLE
  kmm_free(uart4_rb);
  kmm_free(_u4_user_rx_cache);
  kmm_free(_u4_drv_rx_buffer);
#endif /* UART4_RX_ENABLE */
#if UART4_DMA_TX_ENABLE
  kmm_free(Uart4TxDmaHandle);
#endif /* UART4_DMA_TX_ENABLE */
#if UART4_DMA_RX_ENABLE
  kmm_free(Uart4RxDmaHandle);
#endif /* UART4_DMA_RX_ENABLE */
#if UART4_TX_ENABLE
  osMutexDelete(u4_tx_mutex);
#endif /* UART4_TX_ENABLE */
  return status_ok;
}

#if UART4_TX_ENABLE
static status_t uart4_tx_bytes(uint8_t *p, uint32_t l)
{
  status_t ret;
  osMutexWait(u4_tx_mutex, osWaitForever);
  ret = (status_t)HAL_UART_Transmit(Uart4Handle, (uint8_t*)p, l, USART_HAL_TRANSMIT_TIMEOUT);
  osMutexRelease(u4_tx_mutex);
  return ret;
}

static status_t uart4_tx_bytes_it(uint8_t *p, uint32_t l)
{
  osMutexWait(u4_tx_mutex, osWaitForever);
  return (status_t)HAL_UART_Transmit_IT(Uart4Handle, (uint8_t*)p, l);
}

static status_t uart4_tx_bytes_dma(uint8_t *p, uint32_t l)
{
#if UART4_DMA_TX_ENABLE
  osMutexWait(u4_tx_mutex, osWaitForever);
  return (status_t)HAL_UART_Transmit_DMA(Uart4Handle, (uint8_t*)p, l);
#else
  ky_err(TAG, "DMA TX NOT available");
  return status_error;
#endif /* UART4_DMA_TX_ENABLE */
}
#else
static status_t uart4_tx_bytes(uint8_t *p, uint32_t l)
{
  return status_error;
}

static status_t uart4_tx_bytes_it(uint8_t *p, uint32_t l)
{
  return status_error;
}

static status_t uart4_tx_bytes_dma(uint8_t *p, uint32_t l)
{
  return status_error;
}
#endif /* UART4_TX_ENABLE */

#if UART4_RX_ENABLE
static uint32_t uart4_rx_bytes(uint8_t *p, uint32_t l)
{
  return ringbuffer_poll(uart4_rb, p, l);
}

static uint32_t uart4_cache_usage(void)
{
  return ringbuffer_usage(uart4_rb);
}

#else
static uint32_t uart4_rx_bytes(uint8_t *p, uint32_t l)
{
  return 0;
}

static uint32_t uart4_cache_usage(void)
{
  return 0;
}
#endif /* UART4_RX_ENABLE */

#if UART4_TX_ENABLE
static void uart4_txcplt_callback(UART_HandleTypeDef *huart)
{
  osMutexRelease(u4_tx_mutex);
}
#endif /* UART4_TX_ENABLE */

#if UART4_RX_ENABLE
static void uart4_rxcplt_callback(UART_HandleTypeDef *huart)
{
#if UART4_DMA_RX_ENABLE
  ringbuffer_push(uart4_rb, _u4_drv_rx_buffer, UART4_DRV_RX_CACHE_SIZE);
#else
  ringbuffer_push(uart4_rb, _u4_drv_rx_buffer, UART4_DRV_RX_CACHE_SIZE);
  HAL_UART_Receive_IT(Uart4Handle, (uint8_t *)_u4_drv_rx_buffer, UART4_DRV_RX_CACHE_SIZE);
#endif /* UART4_DMA_RX_ENABLE */
}
#endif /* UART4_RX_ENABLE */
#if UART4_RX_ENABLE
static void uart4_idle_callback(UART_HandleTypeDef *huart)
{
  if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);
#if UART4_DMA_RX_ENABLE
    uint32_t len = UART4_DRV_RX_CACHE_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(len > 0) {
      __HAL_DMA_DISABLE(huart->hdmarx);
      ringbuffer_push(uart4_rb, _u4_drv_rx_buffer, len);
      __HAL_DMA_SET_COUNTER(huart->hdmarx, UART4_DRV_RX_CACHE_SIZE);
      __HAL_DMA_ENABLE(huart->hdmarx);
    }
#else
    uint32_t len = UART4_DRV_RX_CACHE_SIZE - huart->RxXferCount;
    if(len > 0) {
      ringbuffer_push(uart4_rb, _u4_drv_rx_buffer, len);
      huart->RxXferSize = UART4_DRV_RX_CACHE_SIZE;
      huart->RxXferCount = UART4_DRV_RX_CACHE_SIZE;
      huart->pRxBuffPtr = (uint8_t *)_u4_drv_rx_buffer;
    }
#endif /* UART4_DMA_RX_ENABLE */
  }
}
#endif /* UART4_RX_ENABLE */

static void uart4_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable peripherals and GPIO Clocks */
  /* Enable GPIO clock */
  UART4_GPIO_CLK_ENABLE();
  /* Enable UART4's clock */
  __UART4_CLK_ENABLE();

  /* Configure peripheral GPIO */
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
#if UART4_TX_ENABLE
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = UART4_TX_PIN;
  GPIO_InitStruct.Alternate = UART4_TX_AF;
  HAL_GPIO_Init(UART4_TX_GPIO_PORT, &GPIO_InitStruct);
#endif /* UART4_TX_ENABLE */
#if UART4_RX_ENABLE
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = UART4_RX_PIN;
  GPIO_InitStruct.Alternate = UART4_RX_AF;
  HAL_GPIO_Init(UART4_RX_GPIO_PORT, &GPIO_InitStruct);
#endif /* UART4_RX_ENABLE */

#if UART4_DMA_TX_ENABLE
  /* Enable TX DMA clock */
  UART4_DMA_TX_CLK_ENABLE();

  /* Configure the DMA */
  /* Configure the DMA handler for Transmission process */
  Uart4TxDmaHandle->Instance                 = UART4_TX_DMA_STREAM;
  Uart4TxDmaHandle->Init.Channel             = UART4_TX_DMA_CHANNEL;
  Uart4TxDmaHandle->Init.Direction           = DMA_MEMORY_TO_PERIPH;
  Uart4TxDmaHandle->Init.PeriphInc           = DMA_PINC_DISABLE;
  Uart4TxDmaHandle->Init.MemInc              = DMA_MINC_ENABLE;
  Uart4TxDmaHandle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  Uart4TxDmaHandle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  Uart4TxDmaHandle->Init.Mode                = DMA_NORMAL;
  Uart4TxDmaHandle->Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(Uart4TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, *Uart4TxDmaHandle);

  /* NVIC configuration for UART4 TX DMA */
  HAL_NVIC_SetPriority(UART4_DMA_TX_IRQn, INT_PRIO_UART4_DMATX, 0);
  HAL_NVIC_EnableIRQ(UART4_DMA_TX_IRQn);
#endif /* UART4_DMA_TX_ENABLE */
#if UART4_DMA_RX_ENABLE
  /* Enable RX DMA clock */
  UART4_DMA_RX_CLK_ENABLE();
  /* Configure the DMA handler for reception process */
  Uart4RxDmaHandle->Instance                 = UART4_RX_DMA_STREAM;
  Uart4RxDmaHandle->Init.Channel             = UART4_RX_DMA_CHANNEL;
  Uart4RxDmaHandle->Init.Direction           = DMA_PERIPH_TO_MEMORY;
  Uart4RxDmaHandle->Init.PeriphInc           = DMA_PINC_DISABLE;
  Uart4RxDmaHandle->Init.MemInc              = DMA_MINC_ENABLE;
  Uart4RxDmaHandle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  Uart4RxDmaHandle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  Uart4RxDmaHandle->Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  Uart4RxDmaHandle->Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(Uart4RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, *Uart4RxDmaHandle);

  /* NVIC configuration for UART4 RX DMA */
  HAL_NVIC_SetPriority(UART4_DMA_RX_IRQn, INT_PRIO_UART4_DMARX, 0);
  HAL_NVIC_EnableIRQ(UART4_DMA_RX_IRQn);
#endif /* UART4_DMA_RX_ENABLE */

  /* NVIC for UART4 IRQ */
  HAL_NVIC_SetPriority(UART4_IRQn, INT_PRIO_UART4_PERIPH, 0);
  HAL_NVIC_EnableIRQ(UART4_IRQn);
}

static void uart4_msp_deinit(UART_HandleTypeDef *huart)
{
  /* Reset peripherals */
  __UART4_FORCE_RESET();
  __UART4_RELEASE_RESET();

  /* Disable peripherals and GPIO Clocks */
#if UART4_TX_ENABLE
  /* Configure UART4 Tx as alternate function  */
  HAL_GPIO_DeInit(UART4_TX_GPIO_PORT, UART4_TX_PIN);
#endif /* UART4_TX_ENABLE */
#if UART4_RX_ENABLE
  /* Configure UART4 Rx as alternate function  */
  HAL_GPIO_DeInit(UART4_RX_GPIO_PORT, UART4_RX_PIN);
#endif /* UART4_RX_ENABLE */
  /* Disable the DMA */
#if UART4_DMA_TX_ENABLE
  /* De-Initialize the DMA channel associated to transmission process */
  if(huart->hdmatx != 0) {
    HAL_DMA_DeInit(huart->hdmatx);
  }
  /* Disable the NVIC for DMA */
  HAL_NVIC_DisableIRQ(UART4_DMA_TX_IRQn);
#endif /* UART4_DMA_TX_ENABLE */
#if UART4_DMA_RX_ENABLE
  /* De-Initialize the DMA channel associated to reception process */
  if(huart->hdmarx != 0) {
    HAL_DMA_DeInit(huart->hdmarx);
  }
  /* Disable the NVIC for DMA */
  HAL_NVIC_DisableIRQ(UART4_DMA_RX_IRQn);
#endif /* UART4_DMA_RX_ENABLE */
}

#if UART4_DMA_TX_ENABLE
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void UART4_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart4Handle->hdmatx);
}
#endif /* UART4_DMA_TX_ENABLE */
#if UART4_DMA_RX_ENABLE
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void UART4_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Uart4Handle->hdmarx);
}
#endif /* UART4_DMA_RX_ENABLE */
/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void UART4_IRQHandler(void)
{
#if UART4_RX_ENABLE
  uart4_idle_callback(Uart4Handle);
#endif /* UART4_RX_ENABLE */
  HAL_UART_IRQHandler(Uart4Handle);
}

#else /* !UART4_ENABLE */

static status_t uart4_init(uint32_t baudrate)
{
  return periph_disabled(TAG);
}

static status_t uart4_deinit(void)
{
  return periph_disabled(TAG);
}

static status_t uart4_tx_bytes(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

static status_t uart4_tx_bytes_it(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

static status_t uart4_tx_bytes_dma(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

static uint32_t uart4_rx_bytes(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

static uint32_t uart4_cache_usage(void)
{
  return periph_disabled(TAG);
}

#endif /* UART4_ENABLE */
