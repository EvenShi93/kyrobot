#include "periph.h"
#include "log.h"

#if ((!USART6_TX_ENABLE) && (!USART6_RX_ENABLE))
#undef USART6_ENABLE
#define USART6_ENABLE                       (0)
#endif /* !USART6_TX_ENABLE && !USART6_RX_ENABLE */

#if !USART6_ENABLE
#undef USART6_TX_ENABLE
#undef USART6_RX_ENABLE
#define USART6_TX_ENABLE                    (0)
#define USART6_RX_ENABLE                    (0)
#endif /* !USART6_ENABLE */

#if !USART6_TX_ENABLE
#undef USART6_DMA_TX_ENABLE
#define USART6_DMA_TX_ENABLE                (0)
#endif /* !USART6_TX_ENABLE */

#if !USART6_RX_ENABLE
#undef USART6_DMA_RX_ENABLE
#define USART6_DMA_RX_ENABLE                (0)
#endif /* !USART6_RX_ENABLE */

static status_t usart6_init(uint32_t baudrate);
static status_t usart6_deinit(void);
static status_t usart6_tx_bytes(uint8_t *p, uint32_t l);
static status_t usart6_tx_bytes_it(uint8_t *p, uint32_t l);
static status_t usart6_tx_bytes_dma(uint8_t *p, uint32_t l);

static uint32_t usart6_rx_bytes(uint8_t *p, uint32_t l);
static uint32_t usart6_cache_usage(void);

static const char *TAG = "ttyS6";

const uart_dev_t usart6_dev = {
//  "ttyS6",
  usart6_init,
  usart6_deinit,
  usart6_rx_bytes,
  usart6_tx_bytes,
  usart6_tx_bytes_it,
  usart6_tx_bytes_dma,
  usart6_cache_usage,
};

#if USART6_ENABLE

#include "string.h"

#if USART6_TX_ENABLE
#include "cmsis_os.h"
#endif /* USART6_TX_ENABLE */

#if USART6_RX_ENABLE
#include "ringbuffer.h"
#endif /* USART6_RX_ENABLE */

/* UART handler declaration */
static UART_HandleTypeDef *Usart6Handle;
#if USART6_DMA_TX_ENABLE
static DMA_HandleTypeDef *Usart6TxDmaHandle;
#endif /* USART6_DMA_TX_ENABLE */
#if USART6_DMA_RX_ENABLE
static DMA_HandleTypeDef *Usart6RxDmaHandle;
#endif /* USART6_DMA_RX_ENABLE */

#if USART6_TX_ENABLE
static osMutexId u6_tx_mutex;
#endif /* USART6_TX_ENABLE */

#if USART6_RX_ENABLE
#define USART6_DRV_RX_CACHE_SIZE            128
static ringbuffer_handle *usart6_rb;

static uint8_t *_u6_user_rx_cache = NULL;
static uint8_t *_u6_drv_rx_buffer = NULL;
#endif /* USART6_RX_ENABLE */

static void usart6_msp_init(UART_HandleTypeDef *huart);
static void usart6_msp_deinit(UART_HandleTypeDef *huart);

#if USART6_TX_ENABLE
static void usart6_txcplt_callback(UART_HandleTypeDef *huart);
#endif /* USART6_TX_ENABLE */
#if USART6_RX_ENABLE
static void usart6_rxcplt_callback(UART_HandleTypeDef *huart);
#endif /* USART6_RX_ENABLE */

static status_t usart6_init(uint32_t baudrate)
{
  status_t ret = status_ok;

  Usart6Handle = kmm_alloc(sizeof(UART_HandleTypeDef));
#if USART6_RX_ENABLE
  usart6_rb = kmm_alloc(sizeof(ringbuffer_handle));
  _u6_user_rx_cache = kmm_alloc(USART6_USER_RX_CACHE_DEPTH);
  _u6_drv_rx_buffer = kmm_alloc(USART6_DRV_RX_CACHE_SIZE);
#endif /* USART6_RX_ENABLE */
#if USART6_DMA_TX_ENABLE
  Usart6TxDmaHandle = kmm_alloc(sizeof(DMA_HandleTypeDef));
#endif /* USART6_DMA_TX_ENABLE */
#if USART6_DMA_RX_ENABLE
  Usart6RxDmaHandle = kmm_alloc(sizeof(DMA_HandleTypeDef));
#endif /* USART6_DMA_RX_ENABLE */
  if(Usart6Handle == NULL
#if USART6_RX_ENABLE
    || usart6_rb == NULL || _u6_user_rx_cache == NULL || _u6_drv_rx_buffer == NULL
#endif /* USART6_RX_ENABLE */
#if USART6_DMA_TX_ENABLE
    || Usart6TxDmaHandle == NULL
#endif /* USART6_DMA_TX_ENABLE */
#if USART6_DMA_RX_ENABLE
    || Usart6RxDmaHandle == NULL
#endif /* USART6_DMA_RX_ENABLE */
  ) {
    ky_err(TAG, "memory alloc fail");
    ret = status_nomem;
    goto error;
  }
  memset(Usart6Handle, 0, sizeof(UART_HandleTypeDef));
#if USART6_RX_ENABLE
  memset(usart6_rb, 0, sizeof(ringbuffer_handle));
  memset(_u6_user_rx_cache, 0, USART6_USER_RX_CACHE_DEPTH);
  memset(_u6_drv_rx_buffer, 0, USART6_DRV_RX_CACHE_SIZE);
#endif /* USART6_RX_ENABLE */
#if USART6_DMA_TX_ENABLE
  memset(Usart6TxDmaHandle, 0, sizeof(DMA_HandleTypeDef));
#endif /* USART6_DMA_TX_ENABLE */
#if USART6_DMA_RX_ENABLE
  memset(Usart6RxDmaHandle, 0, sizeof(DMA_HandleTypeDef));
#endif /* USART6_DMA_RX_ENABLE */

#if USART6_TX_ENABLE
  /* Create the mutex  */
  osMutexDef(U6TxMutex);
  u6_tx_mutex = osMutexCreate(osMutex(U6TxMutex));
  if(u6_tx_mutex == NULL) {
    ret = status_error;
    goto error;
  }
#endif /* USART6_TX_ENABLE */

  /* Configure the UART peripheral */
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  Usart6Handle->Instance        = USART6;
  Usart6Handle->Init.BaudRate   = baudrate;
  Usart6Handle->Init.WordLength = UART_WORDLENGTH_8B;
  Usart6Handle->Init.StopBits   = UART_STOPBITS_1;
  Usart6Handle->Init.Parity     = UART_PARITY_NONE;
  Usart6Handle->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
#if ((USART6_TX_ENABLE) && (USART6_RX_ENABLE))
  Usart6Handle->Init.Mode       = UART_MODE_TX_RX;
#endif /* USART6_TX_ENABLE && USART6_RX_ENABLE */
#if !USART6_TX_ENABLE
  Usart6Handle->Init.Mode       = UART_MODE_RX;
#endif /* !USART6_TX_ENABLE */
#if !USART6_RX_ENABLE
  Usart6Handle->Init.Mode       = UART_MODE_TX;
#endif /* !USART6_RX_ENABLE */

  /* register callbacks */
  Usart6Handle->MspInitCallback = usart6_msp_init;
  Usart6Handle->MspDeInitCallback = usart6_msp_deinit;
#if USART6_TX_ENABLE
  Usart6Handle->TxCpltCallback = usart6_txcplt_callback;
#endif /* USART6_TX_ENABLE */
#if USART6_RX_ENABLE
  Usart6Handle->RxCpltCallback = usart6_rxcplt_callback;
#endif /* USART6_RX_ENABLE */
  if(HAL_UART_Init(Usart6Handle) != HAL_OK) {
    ret = status_error;
    goto error;
  }

#if USART6_RX_ENABLE
  ringbuffer_init(usart6_rb, _u6_user_rx_cache, USART6_USER_RX_CACHE_DEPTH);
#endif /* USART6_RX_ENABLE */

  /* start receive task */
#if USART6_RX_ENABLE
#if USART6_DMA_RX_ENABLE
  HAL_UART_Receive_DMA(Usart6Handle, (uint8_t *)_u6_drv_rx_buffer, USART6_DRV_RX_CACHE_SIZE);
#else
  HAL_UART_Receive_IT(Usart6Handle, (uint8_t *)_u2_drv_rx_buffer, USART6_DRV_RX_CACHE_SIZE);
#endif /* USART6_DMA_RX_ENABLE */
  __HAL_UART_ENABLE_IT(Usart6Handle, UART_IT_IDLE);
#endif /* USART6_RX_ENABLE */
  return ret;
error:
  kmm_free(Usart6Handle);
#if USART6_RX_ENABLE
  kmm_free(usart6_rb);
  kmm_free(_u6_user_rx_cache);
  kmm_free(_u6_drv_rx_buffer);
#endif /* USART6_RX_ENABLE */
#if USART6_DMA_TX_ENABLE
  kmm_free(Usart6TxDmaHandle);
#endif /* USART6_DMA_TX_ENABLE */
#if USART6_DMA_RX_ENABLE
  kmm_free(Usart6RxDmaHandle);
#endif /* USART6_DMA_RX_ENABLE */
#if USART6_TX_ENABLE
  osMutexDelete(u6_tx_mutex);
#endif /* USART6_TX_ENABLE */
  return ret;
}

static status_t usart6_deinit(void)
{
  HAL_UART_DeInit(Usart6Handle);
  kmm_free(Usart6Handle);
#if USART6_RX_ENABLE
  kmm_free(usart6_rb);
  kmm_free(_u6_user_rx_cache);
  kmm_free(_u6_drv_rx_buffer);
#endif /* USART6_RX_ENABLE */
#if USART6_DMA_TX_ENABLE
  kmm_free(Usart6TxDmaHandle);
#endif /* USART6_DMA_TX_ENABLE */
#if USART6_DMA_RX_ENABLE
  kmm_free(Usart6RxDmaHandle);
#endif /* USART6_DMA_RX_ENABLE */
#if USART6_TX_ENABLE
  osMutexDelete(u6_tx_mutex);
#endif /* USART6_TX_ENABLE */
  return status_ok;
}

#if USART6_TX_ENABLE
static status_t usart6_tx_bytes(uint8_t *p, uint32_t l)
{
  status_t ret;
  osMutexWait(u6_tx_mutex, osWaitForever);
  ret = (status_t)HAL_UART_Transmit(Usart6Handle, (uint8_t*)p, l, USART_HAL_TRANSMIT_TIMEOUT);
  osMutexRelease(u6_tx_mutex);
  return ret;
}

static status_t usart6_tx_bytes_it(uint8_t *p, uint32_t l)
{
  osMutexWait(u6_tx_mutex, osWaitForever);
  return (status_t)HAL_UART_Transmit_IT(Usart6Handle, (uint8_t*)p, l);
}

static status_t usart6_tx_bytes_dma(uint8_t *p, uint32_t l)
{
#if USART6_DMA_TX_ENABLE
  osMutexWait(u6_tx_mutex, osWaitForever);
  return (status_t)HAL_UART_Transmit_DMA(Usart6Handle, (uint8_t*)p, l);
#else
  ky_err(TAG, "DMA TX NOT available");
  return status_error;
#endif /* USART6_DMA_TX_ENABLE */
}
#else
static status_t usart6_tx_bytes(uint8_t *p, uint32_t l)
{
  return status_error;
}

static status_t usart6_tx_bytes_it(uint8_t *p, uint32_t l)
{
  return status_error;
}

static status_t usart6_tx_bytes_dma(uint8_t *p, uint32_t l)
{
  return status_error;
}
#endif /* USART6_TX_ENABLE */

#if USART6_RX_ENABLE
static uint32_t usart6_rx_bytes(uint8_t *p, uint32_t l)
{
  return ringbuffer_poll(usart6_rb, p, l);
}

static uint32_t usart6_cache_usage(void)
{
  return ringbuffer_usage(usart6_rb);
}

#else
static uint32_t usart6_rx_bytes(uint8_t *p, uint32_t l)
{
  return 0;
}

static uint32_t usart6_cache_usage(void)
{
  return 0;
}
#endif /* USART6_RX_ENABLE */

#if USART6_TX_ENABLE
static void usart6_txcplt_callback(UART_HandleTypeDef *huart)
{
  osMutexRelease(u6_tx_mutex);
}
#endif /* USART6_TX_ENABLE */

#if USART6_RX_ENABLE
static void usart6_rxcplt_callback(UART_HandleTypeDef *huart)
{
#if USART6_DMA_RX_ENABLE
  ringbuffer_push(usart6_rb, _u6_drv_rx_buffer, USART6_DRV_RX_CACHE_SIZE);
#else
  ringbuffer_push(usart6_rb, _u2_drv_rx_buffer, USART6_DRV_RX_CACHE_SIZE);
  HAL_UART_Receive_IT(Usart6Handle, (uint8_t *)_u2_drv_rx_buffer, USART6_DRV_RX_CACHE_SIZE);
#endif /* USART6_DMA_RX_ENABLE */
}
#endif /* USART6_RX_ENABLE */
#if USART6_RX_ENABLE
static void usart6_idle_callback(UART_HandleTypeDef *huart)
{
  if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);
#if USART6_DMA_RX_ENABLE
    uint32_t len = USART6_DRV_RX_CACHE_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(len > 0) {
      __HAL_DMA_DISABLE(huart->hdmarx);
      ringbuffer_push(usart6_rb, _u6_drv_rx_buffer, len);
      __HAL_DMA_SET_COUNTER(huart->hdmarx, USART6_DRV_RX_CACHE_SIZE);
      __HAL_DMA_ENABLE(huart->hdmarx);
    }
#else
    uint32_t len = USART6_DRV_RX_CACHE_SIZE - huart->RxXferCount;
    if(len > 0) {
      ringbuffer_push(usart6_rb, _u2_drv_rx_buffer, len);
      huart->RxXferSize = USART6_DRV_RX_CACHE_SIZE;
      huart->RxXferCount = USART6_DRV_RX_CACHE_SIZE;
      huart->pRxBuffPtr = (uint8_t *)_u2_drv_rx_buffer;
    }
#endif /* USART6_DMA_RX_ENABLE */
  }
}
#endif /* USART6_RX_ENABLE */

static void usart6_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable peripherals and GPIO Clocks */
  /* Enable GPIO clock */
  USART6_GPIO_CLK_ENABLE();
  /* Enable USART6's clock */
  __USART6_CLK_ENABLE();

  /* Configure peripheral GPIO */
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
#if USART6_TX_ENABLE
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USART6_TX_PIN;
  GPIO_InitStruct.Alternate = USART6_TX_AF;
  HAL_GPIO_Init(USART6_TX_GPIO_PORT, &GPIO_InitStruct);
#endif /* USART6_TX_ENABLE */
#if USART6_RX_ENABLE
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USART6_RX_PIN;
  GPIO_InitStruct.Alternate = USART6_RX_AF;
  HAL_GPIO_Init(USART6_RX_GPIO_PORT, &GPIO_InitStruct);
#endif /* USART6_RX_ENABLE */

#if USART6_DMA_TX_ENABLE
  /* Enable TX DMA clock */
  USART6_DMA_TX_CLK_ENABLE();

  /* Configure the DMA */
  /* Configure the DMA handler for Transmission process */
  Usart6TxDmaHandle->Instance                 = USART6_TX_DMA_STREAM;
  Usart6TxDmaHandle->Init.Channel             = USART6_TX_DMA_CHANNEL;
  Usart6TxDmaHandle->Init.Direction           = DMA_MEMORY_TO_PERIPH;
  Usart6TxDmaHandle->Init.PeriphInc           = DMA_PINC_DISABLE;
  Usart6TxDmaHandle->Init.MemInc              = DMA_MINC_ENABLE;
  Usart6TxDmaHandle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  Usart6TxDmaHandle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  Usart6TxDmaHandle->Init.Mode                = DMA_NORMAL;
  Usart6TxDmaHandle->Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(Usart6TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, *Usart6TxDmaHandle);

  /* NVIC configuration for USART6 TX DMA */
  HAL_NVIC_SetPriority(USART6_DMA_TX_IRQn, INT_PRIO_USART6_DMATX, 0);
  HAL_NVIC_EnableIRQ(USART6_DMA_TX_IRQn);
#endif /* USART6_DMA_TX_ENABLE */
#if USART6_DMA_RX_ENABLE
  /* Enable RX DMA clock */
  USART6_DMA_RX_CLK_ENABLE();
  /* Configure the DMA handler for reception process */
  Usart6RxDmaHandle->Instance                 = USART6_RX_DMA_STREAM;
  Usart6RxDmaHandle->Init.Channel             = USART6_RX_DMA_CHANNEL;
  Usart6RxDmaHandle->Init.Direction           = DMA_PERIPH_TO_MEMORY;
  Usart6RxDmaHandle->Init.PeriphInc           = DMA_PINC_DISABLE;
  Usart6RxDmaHandle->Init.MemInc              = DMA_MINC_ENABLE;
  Usart6RxDmaHandle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  Usart6RxDmaHandle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  Usart6RxDmaHandle->Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  Usart6RxDmaHandle->Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(Usart6RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, *Usart6RxDmaHandle);

  /* NVIC configuration for USART6 RX DMA */
  HAL_NVIC_SetPriority(USART6_DMA_RX_IRQn, INT_PRIO_USART6_DMARX, 0);
  HAL_NVIC_EnableIRQ(USART6_DMA_RX_IRQn);
#endif /* USART6_DMA_RX_ENABLE */

  /* NVIC for USART6 IRQ */
  HAL_NVIC_SetPriority(USART6_IRQn, INT_PRIO_USART6_PERIPH, 0);
  HAL_NVIC_EnableIRQ(USART6_IRQn);
}

static void usart6_msp_deinit(UART_HandleTypeDef *huart)
{
  /* Reset peripherals */
  __USART6_FORCE_RESET();
  __USART6_RELEASE_RESET();

  /* Disable peripherals and GPIO Clocks */
#if USART6_TX_ENABLE
  /* Configure USART6 Tx as alternate function  */
  HAL_GPIO_DeInit(USART6_TX_GPIO_PORT, USART6_TX_PIN);
#endif /* USART6_TX_ENABLE */
#if USART6_RX_ENABLE
  /* Configure USART6 Rx as alternate function  */
  HAL_GPIO_DeInit(USART6_RX_GPIO_PORT, USART6_RX_PIN);
#endif /* USART6_RX_ENABLE */
  /* Disable the DMA */
#if USART6_DMA_TX_ENABLE
  /* De-Initialize the DMA channel associated to transmission process */
  if(huart->hdmatx != 0) {
    HAL_DMA_DeInit(huart->hdmatx);
  }
  /* Disable the NVIC for DMA */
  HAL_NVIC_DisableIRQ(USART6_DMA_TX_IRQn);
#endif /* USART6_DMA_TX_ENABLE */
#if USART6_DMA_RX_ENABLE
  /* De-Initialize the DMA channel associated to reception process */
  if(huart->hdmarx != 0) {
    HAL_DMA_DeInit(huart->hdmarx);
  }
  /* Disable the NVIC for DMA */
  HAL_NVIC_DisableIRQ(USART6_DMA_RX_IRQn);
#endif /* USART6_DMA_RX_ENABLE */
}

#if USART6_DMA_TX_ENABLE
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void USART6_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Usart6Handle->hdmatx);
}
#endif /* USART6_DMA_TX_ENABLE */
#if USART6_DMA_RX_ENABLE
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void USART6_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Usart6Handle->hdmarx);
}
#endif /* USART6_DMA_RX_ENABLE */
/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void USART6_IRQHandler(void)
{
#if USART6_RX_ENABLE
  usart6_idle_callback(Usart6Handle);
#endif /* USART6_RX_ENABLE */
  HAL_UART_IRQHandler(Usart6Handle);
}

#else /* !USART6_ENABLE */

static status_t usart6_init(uint32_t baudrate)
{
  return periph_disabled(TAG);
}

static status_t usart6_deinit(void)
{
  return periph_disabled(TAG);
}

static status_t usart6_tx_bytes(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

static status_t usart6_tx_bytes_it(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

static status_t usart6_tx_bytes_dma(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

static uint32_t usart6_rx_bytes(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

static uint32_t usart6_cache_usage(void)
{
  return periph_disabled(TAG);
}

#endif /* USART6_ENABLE */
