#include "usart2.h"
#include "log.h"

#if ((!USART2_TX_ENABLE) && (!USART2_RX_ENABLE))
#undef USART2_ENABLE
#define USART2_ENABLE                       (0)
#endif /* !USART2_TX_ENABLE && !USART2_RX_ENABLE */

#if !USART2_ENABLE
#undef USART2_TX_ENABLE
#undef USART2_RX_ENABLE
#define USART2_TX_ENABLE                    (0)
#define USART2_RX_ENABLE                    (0)
#endif /* !USART2_ENABLE */

#if !USART2_TX_ENABLE
#undef USART2_DMA_TX_ENABLE
#define USART2_DMA_TX_ENABLE                (0)
#endif /* !USART2_TX_ENABLE */

#if !USART2_RX_ENABLE
#undef USART2_DMA_RX_ENABLE
#define USART2_DMA_RX_ENABLE                (0)
#endif /* !USART2_RX_ENABLE */

static const char *TAG = "ttyS2";

#if USART2_ENABLE

#include "string.h"

#if USART2_TX_ENABLE
#include "cmsis_os.h"
#endif /* USART2_TX_ENABLE */

#if USART2_RX_ENABLE
#include "ringbuffer.h"
#endif /* USART2_RX_ENABLE */

/* UART handler declaration */
static UART_HandleTypeDef *Usart2Handle;
#if USART2_DMA_TX_ENABLE
static DMA_HandleTypeDef *Usart2TxDmaHandle;
#endif /* USART2_DMA_TX_ENABLE */
#if USART2_DMA_RX_ENABLE
static DMA_HandleTypeDef *Usart2RxDmaHandle;
#endif /* USART2_DMA_RX_ENABLE */

#if USART2_TX_ENABLE
static osMutexId u2_tx_mutex;
#endif /* USART2_TX_ENABLE */

#if USART2_RX_ENABLE
#define USART2_DRV_RX_CACHE_SIZE            128
static ringbuffer_handle *usart2_rb;

static uint8_t *_u2_user_rx_cache = NULL;
static uint8_t *_u2_drv_rx_buffer = NULL;
#endif /* USART2_RX_ENABLE */

static void usart2_msp_init(UART_HandleTypeDef *huart);
static void usart2_msp_deinit(UART_HandleTypeDef *huart);

#if USART2_TX_ENABLE
static void usart2_txcplt_callback(UART_HandleTypeDef *huart);
#endif /* USART2_TX_ENABLE */
#if USART2_RX_ENABLE
static void usart2_rxcplt_callback(UART_HandleTypeDef *huart);
#endif /* USART2_RX_ENABLE */

status_t usart2_init(uint32_t baudrate)
{
  status_t ret = status_ok;

  Usart2Handle = kmm_alloc(sizeof(UART_HandleTypeDef));
#if USART2_RX_ENABLE
  usart2_rb = kmm_alloc(sizeof(ringbuffer_handle));
  _u2_user_rx_cache = kmm_alloc(USART2_USER_RX_CACHE_DEPTH);
  _u2_drv_rx_buffer = kmm_alloc(USART2_DRV_RX_CACHE_SIZE);
#endif /* USART2_RX_ENABLE */
#if USART2_DMA_TX_ENABLE
  Usart2TxDmaHandle = kmm_alloc(sizeof(DMA_HandleTypeDef));
#endif /* USART2_DMA_TX_ENABLE */
#if USART2_DMA_RX_ENABLE
  Usart2RxDmaHandle = kmm_alloc(sizeof(DMA_HandleTypeDef));
#endif /* USART2_DMA_RX_ENABLE */
  if(Usart2Handle == NULL
#if USART2_RX_ENABLE
    || usart2_rb == NULL || _u2_user_rx_cache == NULL || _u2_drv_rx_buffer == NULL
#endif /* USART2_RX_ENABLE */
#if USART2_DMA_TX_ENABLE
    || Usart2TxDmaHandle == NULL
#endif /* USART2_DMA_TX_ENABLE */
#if USART2_DMA_RX_ENABLE
    || Usart2RxDmaHandle == NULL
#endif /* USART2_DMA_RX_ENABLE */
  ) {
    ky_err(TAG, "memory alloc fail");
    ret = status_nomem;
    goto error;
  }
  memset(Usart2Handle, 0, sizeof(UART_HandleTypeDef));
#if USART2_RX_ENABLE
  memset(usart2_rb, 0, sizeof(ringbuffer_handle));
  memset(_u2_user_rx_cache, 0, USART2_USER_RX_CACHE_DEPTH);
  memset(_u2_drv_rx_buffer, 0, USART2_DRV_RX_CACHE_SIZE);
#endif /* USART2_RX_ENABLE */
#if USART2_DMA_TX_ENABLE
  memset(Usart2TxDmaHandle, 0, sizeof(DMA_HandleTypeDef));
#endif /* USART2_DMA_TX_ENABLE */
#if USART2_DMA_RX_ENABLE
  memset(Usart2RxDmaHandle, 0, sizeof(DMA_HandleTypeDef));
#endif /* USART2_DMA_RX_ENABLE */

#if USART2_TX_ENABLE
  /* Create the mutex  */
  osMutexDef(U2TxMutex);
  u2_tx_mutex = osMutexCreate(osMutex(U2TxMutex));
  if(u2_tx_mutex == NULL) {
    ret = status_error;
    goto error;
  }
#endif /* USART2_TX_ENABLE */

  /* Configure the UART peripheral */
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  Usart2Handle->Instance        = USART2;
  Usart2Handle->Init.BaudRate   = baudrate;
  Usart2Handle->Init.WordLength = UART_WORDLENGTH_8B;
  Usart2Handle->Init.StopBits   = UART_STOPBITS_1;
  Usart2Handle->Init.Parity     = UART_PARITY_NONE;
  Usart2Handle->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
#if ((USART2_TX_ENABLE) && (USART2_RX_ENABLE))
  Usart2Handle->Init.Mode       = UART_MODE_TX_RX;
#endif /* USART2_TX_ENABLE && USART2_RX_ENABLE */
#if !USART2_TX_ENABLE
  Usart2Handle->Init.Mode       = UART_MODE_RX;
#endif /* !USART2_TX_ENABLE */
#if !USART2_RX_ENABLE
  Usart2Handle->Init.Mode       = UART_MODE_TX;
#endif /* !USART2_RX_ENABLE */

  /* register callbacks */
  Usart2Handle->MspInitCallback = usart2_msp_init;
  Usart2Handle->MspDeInitCallback = usart2_msp_deinit;
#if USART2_TX_ENABLE
  Usart2Handle->TxCpltCallback = usart2_txcplt_callback;
#endif /* USART2_TX_ENABLE */
#if USART2_RX_ENABLE
  Usart2Handle->RxCpltCallback = usart2_rxcplt_callback;
#endif /* USART2_RX_ENABLE */
  if(HAL_UART_Init(Usart2Handle) != HAL_OK) {
    ret = status_error;
    goto error;
  }

#if USART2_RX_ENABLE
  ringbuffer_init(usart2_rb, _u2_user_rx_cache, USART2_USER_RX_CACHE_DEPTH);
#endif /* USART2_RX_ENABLE */

  /* start receive task */
#if USART2_RX_ENABLE
#if USART2_DMA_RX_ENABLE
  HAL_UART_Receive_DMA(Usart2Handle, (uint8_t *)_u2_drv_rx_buffer, USART2_DRV_RX_CACHE_SIZE);
#else
  HAL_UART_Receive_IT(Usart2Handle, (uint8_t *)_u2_drv_rx_buffer, USART2_DRV_RX_CACHE_SIZE);
#endif /* USART2_DMA_RX_ENABLE */
  __HAL_UART_ENABLE_IT(Usart2Handle, UART_IT_IDLE);
#endif /* USART2_RX_ENABLE */
  return ret;
error:
  kmm_free(Usart2Handle);
#if USART2_RX_ENABLE
  kmm_free(usart2_rb);
  kmm_free(_u2_user_rx_cache);
  kmm_free(_u2_drv_rx_buffer);
#endif /* USART2_RX_ENABLE */
#if USART2_DMA_TX_ENABLE
  kmm_free(Usart2TxDmaHandle);
#endif /* USART2_DMA_TX_ENABLE */
#if USART2_DMA_RX_ENABLE
  kmm_free(Usart2RxDmaHandle);
#endif /* USART2_DMA_RX_ENABLE */
#if USART2_TX_ENABLE
  osMutexDelete(u2_tx_mutex);
#endif /* USART2_TX_ENABLE */
  return ret;
}

status_t usart2_deinit(void)
{
  HAL_UART_DeInit(Usart2Handle);
  kmm_free(Usart2Handle);
#if USART2_RX_ENABLE
  kmm_free(usart2_rb);
  kmm_free(_u2_user_rx_cache);
  kmm_free(_u2_drv_rx_buffer);
#endif /* USART2_RX_ENABLE */
#if USART2_DMA_TX_ENABLE
  kmm_free(Usart2TxDmaHandle);
#endif /* USART2_DMA_TX_ENABLE */
#if USART2_DMA_RX_ENABLE
  kmm_free(Usart2RxDmaHandle);
#endif /* USART2_DMA_RX_ENABLE */
#if USART2_TX_ENABLE
  osMutexDelete(u2_tx_mutex);
#endif /* USART2_TX_ENABLE */
  return status_ok;
}

#if USART2_TX_ENABLE
status_t usart2_tx_bytes(uint8_t *p, uint32_t l)
{
  status_t ret;
  osMutexWait(u2_tx_mutex, osWaitForever);
  ret = (status_t)HAL_UART_Transmit(Usart2Handle, (uint8_t*)p, l, USART_HAL_TRANSMIT_TIMEOUT);
  osMutexRelease(u2_tx_mutex);
  return ret;
}

status_t usart2_tx_bytes_it(uint8_t *p, uint32_t l)
{
  osMutexWait(u2_tx_mutex, osWaitForever);
  return (status_t)HAL_UART_Transmit_IT(Usart2Handle, (uint8_t*)p, l);
}

status_t usart2_tx_bytes_dma(uint8_t *p, uint32_t l)
{
#if USART2_DMA_TX_ENABLE
  osMutexWait(u2_tx_mutex, osWaitForever);
  return (status_t)HAL_UART_Transmit_DMA(Usart2Handle, (uint8_t*)p, l);
#else
  ky_err(TAG, "DMA TX NOT available");
  return status_error;
#endif /* USART2_DMA_TX_ENABLE */
}
#else
status_t usart2_tx_bytes(uint8_t *p, uint32_t l)
{
  return status_error;
}

status_t usart2_tx_bytes_it(uint8_t *p, uint32_t l)
{
  return status_error;
}

status_t usart2_tx_bytes_dma(uint8_t *p, uint32_t l)
{
  return status_error;
}
#endif /* USART2_TX_ENABLE */

#if USART2_RX_ENABLE
uint32_t usart2_rx_bytes(uint8_t *p, uint32_t l)
{
  return ringbuffer_poll(usart2_rb, p, l);
}

uint32_t usart2_cache_usage(void)
{
  return ringbuffer_usage(usart2_rb);
}

#else
uint32_t usart2_rx_bytes(uint8_t *p, uint32_t l)
{
  return 0;
}

uint32_t usart2_cache_usage(void)
{
  return 0;
}
#endif /* USART2_RX_ENABLE */

#if USART2_TX_ENABLE
static void usart2_txcplt_callback(UART_HandleTypeDef *huart)
{
  osMutexRelease(u2_tx_mutex);
}
#endif /* USART2_TX_ENABLE */

#if USART2_RX_ENABLE
static void usart2_rxcplt_callback(UART_HandleTypeDef *huart)
{
#if USART2_DMA_RX_ENABLE
  ringbuffer_push(usart2_rb, _u2_drv_rx_buffer, USART2_DRV_RX_CACHE_SIZE);
#else
  ringbuffer_push(usart2_rb, _u2_drv_rx_buffer, USART2_DRV_RX_CACHE_SIZE);
  HAL_UART_Receive_IT(Usart2Handle, (uint8_t *)_u2_drv_rx_buffer, USART2_DRV_RX_CACHE_SIZE);
#endif /* USART2_DMA_RX_ENABLE */
}
#endif /* USART2_RX_ENABLE */
#if USART2_RX_ENABLE
static void usart2_idle_callback(UART_HandleTypeDef *huart)
{
  if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);
#if USART2_DMA_RX_ENABLE
    uint32_t len = USART2_DRV_RX_CACHE_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(len > 0) {
      __HAL_DMA_DISABLE(huart->hdmarx);
      ringbuffer_push(usart2_rb, _u2_drv_rx_buffer, len);
      __HAL_DMA_SET_COUNTER(huart->hdmarx, USART2_DRV_RX_CACHE_SIZE);
      __HAL_DMA_ENABLE(huart->hdmarx);
    }
#else
    uint32_t len = USART2_DRV_RX_CACHE_SIZE - huart->RxXferCount;
    if(len > 0) {
      ringbuffer_push(usart2_rb, _u2_drv_rx_buffer, len);
      huart->RxXferSize = USART2_DRV_RX_CACHE_SIZE;
      huart->RxXferCount = USART2_DRV_RX_CACHE_SIZE;
      huart->pRxBuffPtr = (uint8_t *)_u2_drv_rx_buffer;
    }
#endif /* USART2_DMA_RX_ENABLE */
  }
}
#endif /* USART2_RX_ENABLE */

static void usart2_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable peripherals and GPIO Clocks */
  /* Enable GPIO clock */
  USART2_GPIO_CLK_ENABLE();
  /* Enable USART2's clock */
  __USART2_CLK_ENABLE();

  /* Configure peripheral GPIO */
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
#if USART2_TX_ENABLE
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USART2_TX_PIN;
  GPIO_InitStruct.Alternate = USART2_TX_AF;
  HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);
#endif /* USART2_TX_ENABLE */
#if USART2_RX_ENABLE
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USART2_RX_PIN;
  GPIO_InitStruct.Alternate = USART2_RX_AF;
  HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);
#endif /* USART2_RX_ENABLE */

#if USART2_DMA_TX_ENABLE
  /* Enable TX DMA clock */
  USART2_DMA_TX_CLK_ENABLE();

  /* Configure the DMA */
  /* Configure the DMA handler for Transmission process */
  Usart2TxDmaHandle->Instance                 = USART2_TX_DMA_STREAM;
  Usart2TxDmaHandle->Init.Channel             = USART2_TX_DMA_CHANNEL;
  Usart2TxDmaHandle->Init.Direction           = DMA_MEMORY_TO_PERIPH;
  Usart2TxDmaHandle->Init.PeriphInc           = DMA_PINC_DISABLE;
  Usart2TxDmaHandle->Init.MemInc              = DMA_MINC_ENABLE;
  Usart2TxDmaHandle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  Usart2TxDmaHandle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  Usart2TxDmaHandle->Init.Mode                = DMA_NORMAL;
  Usart2TxDmaHandle->Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(Usart2TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, *Usart2TxDmaHandle);

  /* NVIC configuration for USART2 TX DMA */
  HAL_NVIC_SetPriority(USART2_DMA_TX_IRQn, INT_PRIO_USART2_DMATX, 0);
  HAL_NVIC_EnableIRQ(USART2_DMA_TX_IRQn);
#endif /* USART2_DMA_TX_ENABLE */
#if USART2_DMA_RX_ENABLE
  /* Enable RX DMA clock */
  USART2_DMA_RX_CLK_ENABLE();
  /* Configure the DMA handler for reception process */
  Usart2RxDmaHandle->Instance                 = USART2_RX_DMA_STREAM;
  Usart2RxDmaHandle->Init.Channel             = USART2_RX_DMA_CHANNEL;
  Usart2RxDmaHandle->Init.Direction           = DMA_PERIPH_TO_MEMORY;
  Usart2RxDmaHandle->Init.PeriphInc           = DMA_PINC_DISABLE;
  Usart2RxDmaHandle->Init.MemInc              = DMA_MINC_ENABLE;
  Usart2RxDmaHandle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  Usart2RxDmaHandle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  Usart2RxDmaHandle->Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  Usart2RxDmaHandle->Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(Usart2RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, *Usart2RxDmaHandle);

  /* NVIC configuration for USART2 RX DMA */
  HAL_NVIC_SetPriority(USART2_DMA_RX_IRQn, INT_PRIO_USART2_DMARX, 0);
  HAL_NVIC_EnableIRQ(USART2_DMA_RX_IRQn);
#endif /* USART2_DMA_RX_ENABLE */

  /* NVIC for USART2 IRQ */
  HAL_NVIC_SetPriority(USART2_IRQn, INT_PRIO_USART2_PERIPH, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

static void usart2_msp_deinit(UART_HandleTypeDef *huart)
{
  /* Reset peripherals */
  __USART2_FORCE_RESET();
  __USART2_RELEASE_RESET();

  /* Disable peripherals and GPIO Clocks */
#if USART2_TX_ENABLE
  /* Configure USART2 Tx as alternate function  */
  HAL_GPIO_DeInit(USART2_TX_GPIO_PORT, USART2_TX_PIN);
#endif /* USART2_TX_ENABLE */
#if USART2_RX_ENABLE
  /* Configure USART2 Rx as alternate function  */
  HAL_GPIO_DeInit(USART2_RX_GPIO_PORT, USART2_RX_PIN);
#endif /* USART2_RX_ENABLE */
  /* Disable the DMA */
#if USART2_DMA_TX_ENABLE
  /* De-Initialize the DMA channel associated to transmission process */
  if(huart->hdmatx != 0) {
    HAL_DMA_DeInit(huart->hdmatx);
  }
  /* Disable the NVIC for DMA */
  HAL_NVIC_DisableIRQ(USART2_DMA_TX_IRQn);
#endif /* USART2_DMA_TX_ENABLE */
#if USART2_DMA_RX_ENABLE
  /* De-Initialize the DMA channel associated to reception process */
  if(huart->hdmarx != 0) {
    HAL_DMA_DeInit(huart->hdmarx);
  }
  /* Disable the NVIC for DMA */
  HAL_NVIC_DisableIRQ(USART2_DMA_RX_IRQn);
#endif /* USART2_DMA_RX_ENABLE */
}

#if USART2_DMA_TX_ENABLE
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void USART2_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Usart2Handle->hdmatx);
}
#endif /* USART2_DMA_TX_ENABLE */
#if USART2_DMA_RX_ENABLE
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void USART2_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Usart2Handle->hdmarx);
}
#endif /* USART2_DMA_RX_ENABLE */
/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
#if USART2_RX_ENABLE
  usart2_idle_callback(Usart2Handle);
#endif /* USART2_RX_ENABLE */
  HAL_UART_IRQHandler(Usart2Handle);
}

#else /* !USART2_ENABLE */

status_t usart2_init(uint32_t baudrate)
{
  return periph_disabled(TAG);
}

status_t usart2_deinit(void)
{
  return periph_disabled(TAG);
}

status_t usart2_tx_bytes(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

status_t usart2_tx_bytes_it(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

status_t usart2_tx_bytes_dma(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

uint32_t usart2_rx_bytes(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

uint32_t usart2_cache_usage(void)
{
  return periph_disabled(TAG);
}

#endif /* USART2_ENABLE */
