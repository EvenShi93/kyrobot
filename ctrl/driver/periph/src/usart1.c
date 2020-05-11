#include "usart1.h"
#include "log.h"

#if ((!USART1_TX_ENABLE) && (!USART1_RX_ENABLE))
#undef USART1_ENABLE
#define USART1_ENABLE                       (0)
#endif /* !USART1_TX_ENABLE && !USART1_RX_ENABLE */

#if !USART1_ENABLE
#undef USART1_TX_ENABLE
#undef USART1_RX_ENABLE
#define USART1_TX_ENABLE                    (0)
#define USART1_RX_ENABLE                    (0)
#endif /* !USART1_ENABLE */

#if !USART1_TX_ENABLE
#undef USART1_DMA_TX_ENABLE
#define USART1_DMA_TX_ENABLE                (0)
#endif /* !USART1_TX_ENABLE */

#if !USART1_RX_ENABLE
#undef USART1_DMA_RX_ENABLE
#define USART1_DMA_RX_ENABLE                (0)
#endif /* !USART1_RX_ENABLE */

static const char *TAG = "ttyS1";

#if USART1_ENABLE

#include "string.h"

#if USART1_TX_ENABLE
#include "cmsis_os.h"
#endif /* USART1_TX_ENABLE */

#if USART1_RX_ENABLE
#include "ringbuffer.h"
#endif /* USART1_RX_ENABLE */

/* UART handler declaration */
static UART_HandleTypeDef *Usart1Handle;
#if USART1_DMA_TX_ENABLE
static DMA_HandleTypeDef *Usart1TxDmaHandle;
#endif /* USART1_DMA_TX_ENABLE */
#if USART1_DMA_RX_ENABLE
static DMA_HandleTypeDef *Usart1RxDmaHandle;
#endif /* USART1_DMA_RX_ENABLE */

#if USART1_TX_ENABLE
static osMutexId u1_tx_mutex;
#endif /* USART1_TX_ENABLE */

#if USART1_RX_ENABLE
#define USART1_DRV_RX_CACHE_SIZE            128
static ringbuffer_handle *usart1_rb;

static uint8_t *_u1_user_rx_cache = NULL;
static uint8_t *_u1_drv_rx_buffer = NULL;
#endif /* USART1_RX_ENABLE */

static void usart1_msp_init(UART_HandleTypeDef *huart);
static void usart1_msp_deinit(UART_HandleTypeDef *huart);

#if USART1_TX_ENABLE
static void usart1_txcplt_callback(UART_HandleTypeDef *huart);
#endif /* USART1_TX_ENABLE */
#if USART1_RX_ENABLE
static void usart1_rxcplt_callback(UART_HandleTypeDef *huart);
#endif /* USART1_RX_ENABLE */

status_t usart1_init(uint32_t baudrate)
{
  status_t ret = status_ok;

  Usart1Handle = kmm_alloc(sizeof(UART_HandleTypeDef));
#if USART1_RX_ENABLE
  usart1_rb = kmm_alloc(sizeof(ringbuffer_handle));
  _u1_user_rx_cache = kmm_alloc(USART1_USER_RX_CACHE_DEPTH);
  _u1_drv_rx_buffer = kmm_alloc(USART1_DRV_RX_CACHE_SIZE);
#endif /* USART1_RX_ENABLE */
#if USART1_DMA_TX_ENABLE
  Usart1TxDmaHandle = kmm_alloc(sizeof(DMA_HandleTypeDef));
#endif /* USART1_DMA_TX_ENABLE */
#if USART1_DMA_RX_ENABLE
  Usart1RxDmaHandle = kmm_alloc(sizeof(DMA_HandleTypeDef));
#endif /* USART1_DMA_RX_ENABLE */
  if(Usart1Handle == NULL
#if USART1_RX_ENABLE
    || usart1_rb == NULL || _u1_user_rx_cache == NULL || _u1_drv_rx_buffer == NULL
#endif /* USART1_RX_ENABLE */
#if USART1_DMA_TX_ENABLE
    || Usart1TxDmaHandle == NULL
#endif /* USART1_DMA_TX_ENABLE */
#if USART1_DMA_RX_ENABLE
    || Usart1RxDmaHandle == NULL
#endif /* USART1_DMA_RX_ENABLE */
  ) {
    ky_err(TAG, "memory alloc fail");
    ret = status_nomem;
    goto error;
  }
  memset(Usart1Handle, 0, sizeof(UART_HandleTypeDef));
#if USART1_RX_ENABLE
  memset(usart1_rb, 0, sizeof(ringbuffer_handle));
  memset(_u1_user_rx_cache, 0, USART1_USER_RX_CACHE_DEPTH);
  memset(_u1_drv_rx_buffer, 0, USART1_DRV_RX_CACHE_SIZE);
#endif /* USART1_RX_ENABLE */
#if USART1_DMA_TX_ENABLE
  memset(Usart1TxDmaHandle, 0, sizeof(DMA_HandleTypeDef));
#endif /* USART1_DMA_TX_ENABLE */
#if USART1_DMA_RX_ENABLE
  memset(Usart1RxDmaHandle, 0, sizeof(DMA_HandleTypeDef));
#endif /* USART1_DMA_RX_ENABLE */

#if USART1_TX_ENABLE
  /* Create the mutex  */
  osMutexDef(U1TxMutex);
  u1_tx_mutex = osMutexCreate(osMutex(U1TxMutex));
  if(u1_tx_mutex == NULL) {
    ret = status_error;
    goto error;
  }
#endif /* USART1_TX_ENABLE */

  /* Configure the UART peripheral */
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  Usart1Handle->Instance        = USART1;
  Usart1Handle->Init.BaudRate   = baudrate;
  Usart1Handle->Init.WordLength = UART_WORDLENGTH_8B;
  Usart1Handle->Init.StopBits   = UART_STOPBITS_1;
  Usart1Handle->Init.Parity     = UART_PARITY_NONE;
  Usart1Handle->Init.HwFlowCtl  = UART_HWCONTROL_NONE;
#if ((USART1_TX_ENABLE) && (USART1_RX_ENABLE))
  Usart1Handle->Init.Mode       = UART_MODE_TX_RX;
#endif /* USART1_TX_ENABLE && USART1_RX_ENABLE */
#if !USART1_TX_ENABLE
  Usart1Handle->Init.Mode       = UART_MODE_RX;
#endif /* !USART1_TX_ENABLE */
#if !USART1_RX_ENABLE
  Usart1Handle->Init.Mode       = UART_MODE_TX;
#endif /* !USART1_RX_ENABLE */

  /* register callbacks */
  Usart1Handle->MspInitCallback = usart1_msp_init;
  Usart1Handle->MspDeInitCallback = usart1_msp_deinit;
#if USART1_TX_ENABLE
  Usart1Handle->TxCpltCallback = usart1_txcplt_callback;
#endif /* USART1_TX_ENABLE */
#if USART1_RX_ENABLE
  Usart1Handle->RxCpltCallback = usart1_rxcplt_callback;
#endif /* USART1_RX_ENABLE */
  if(HAL_UART_Init(Usart1Handle) != HAL_OK) {
    ret = status_error;
    goto error;
  }

#if USART1_RX_ENABLE
  ringbuffer_init(usart1_rb, _u1_user_rx_cache, USART1_USER_RX_CACHE_DEPTH);
#endif /* USART1_RX_ENABLE */

  /* start receive task */
#if USART1_RX_ENABLE
#if USART1_DMA_RX_ENABLE
  HAL_UART_Receive_DMA(Usart1Handle, (uint8_t *)_u1_drv_rx_buffer, USART1_DRV_RX_CACHE_SIZE);
#else
  HAL_UART_Receive_IT(Usart1Handle, (uint8_t *)_u1_drv_rx_buffer, USART1_DRV_RX_CACHE_SIZE);
#endif /* USART1_DMA_RX_ENABLE */
  __HAL_UART_ENABLE_IT(Usart1Handle, UART_IT_IDLE);
#endif /* USART1_RX_ENABLE */
  return ret;
error:
  kmm_free(Usart1Handle);
#if USART1_RX_ENABLE
  kmm_free(usart1_rb);
  kmm_free(_u1_user_rx_cache);
  kmm_free(_u1_drv_rx_buffer);
#endif /* USART1_RX_ENABLE */
#if USART1_DMA_TX_ENABLE
  kmm_free(Usart1TxDmaHandle);
#endif /* USART1_DMA_TX_ENABLE */
#if USART1_DMA_RX_ENABLE
  kmm_free(Usart1RxDmaHandle);
#endif /* USART1_DMA_RX_ENABLE */
#if USART1_TX_ENABLE
  osMutexDelete(u1_tx_mutex);
#endif /* USART1_TX_ENABLE */
  return ret;
}

status_t usart1_deinit(void)
{
  HAL_UART_DeInit(Usart1Handle);
  kmm_free(Usart1Handle);
#if USART1_RX_ENABLE
  kmm_free(usart1_rb);
  kmm_free(_u1_user_rx_cache);
  kmm_free(_u1_drv_rx_buffer);
#endif /* USART1_RX_ENABLE */
#if USART1_DMA_TX_ENABLE
  kmm_free(Usart1TxDmaHandle);
#endif /* USART1_DMA_TX_ENABLE */
#if USART1_DMA_RX_ENABLE
  kmm_free(Usart1RxDmaHandle);
#endif /* USART1_DMA_RX_ENABLE */
#if USART1_TX_ENABLE
  osMutexDelete(u1_tx_mutex);
#endif /* USART1_TX_ENABLE */
  return status_ok;
}

#if USART1_TX_ENABLE
status_t usart1_tx_bytes(uint8_t *p, uint32_t l)
{
  status_t ret;
  osMutexWait(u1_tx_mutex, osWaitForever);
  ret = (status_t)HAL_UART_Transmit(Usart1Handle, (uint8_t*)p, l, USART_HAL_TRANSMIT_TIMEOUT);
  osMutexRelease(u1_tx_mutex);
  return ret;
}

status_t usart1_tx_bytes_it(uint8_t *p, uint32_t l)
{
  osMutexWait(u1_tx_mutex, osWaitForever);
  return (status_t)HAL_UART_Transmit_IT(Usart1Handle, (uint8_t*)p, l);
}

status_t usart1_tx_bytes_dma(uint8_t *p, uint32_t l)
{
#if USART1_DMA_TX_ENABLE
  osMutexWait(u1_tx_mutex, osWaitForever);
  return (status_t)HAL_UART_Transmit_DMA(Usart1Handle, (uint8_t*)p, l);
#else
  ky_err(TAG, "DMA TX NOT available");
  return status_error;
#endif /* USART1_DMA_TX_ENABLE */
}
#else
status_t usart1_tx_bytes(uint8_t *p, uint32_t l)
{
  return status_error;
}

status_t usart1_tx_bytes_it(uint8_t *p, uint32_t l)
{
  return status_error;
}

status_t usart1_tx_bytes_dma(uint8_t *p, uint32_t l)
{
  return status_error;
}
#endif /* USART1_TX_ENABLE */

#if USART1_RX_ENABLE
uint32_t usart1_rx_bytes(uint8_t *p, uint32_t l)
{
  return ringbuffer_poll(usart1_rb, p, l);
}

uint32_t usart1_cache_usage(void)
{
  return ringbuffer_usage(usart1_rb);
}

#else
uint32_t usart1_rx_bytes(uint8_t *p, uint32_t l)
{
  return 0;
}

uint32_t usart1_cache_usage(void)
{
  return 0;
}
#endif /* USART1_RX_ENABLE */

#if USART1_TX_ENABLE
static void usart1_txcplt_callback(UART_HandleTypeDef *huart)
{
  osMutexRelease(u1_tx_mutex);
}
#endif /* USART1_TX_ENABLE */

#if USART1_RX_ENABLE
static void usart1_rxcplt_callback(UART_HandleTypeDef *huart)
{
#if USART1_DMA_RX_ENABLE
  ringbuffer_push(usart1_rb, _u1_drv_rx_buffer, USART1_DRV_RX_CACHE_SIZE);
#else
  ringbuffer_push(usart1_rb, _u1_drv_rx_buffer, USART1_DRV_RX_CACHE_SIZE);
  HAL_UART_Receive_IT(Usart1Handle, (uint8_t *)_u1_drv_rx_buffer, USART1_DRV_RX_CACHE_SIZE);
#endif /* USART1_DMA_RX_ENABLE */
}
#endif /* USART1_RX_ENABLE */
#if USART1_RX_ENABLE
static void usart1_idle_callback(UART_HandleTypeDef *huart)
{
  if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);
#if USART1_DMA_RX_ENABLE
    uint32_t len = USART1_DRV_RX_CACHE_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(len > 0) {
      __HAL_DMA_DISABLE(huart->hdmarx);
      ringbuffer_push(usart1_rb, _u1_drv_rx_buffer, len);
      __HAL_DMA_SET_COUNTER(huart->hdmarx, USART1_DRV_RX_CACHE_SIZE);
      __HAL_DMA_ENABLE(huart->hdmarx);
    }
#else
    uint32_t len = USART1_DRV_RX_CACHE_SIZE - huart->RxXferCount;
    if(len > 0) {
      ringbuffer_push(usart1_rb, _u1_drv_rx_buffer, len);
      huart->RxXferSize = USART1_DRV_RX_CACHE_SIZE;
      huart->RxXferCount = USART1_DRV_RX_CACHE_SIZE;
      huart->pRxBuffPtr = (uint8_t *)_u1_drv_rx_buffer;
    }
#endif /* USART1_DMA_RX_ENABLE */
  }
}
#endif /* USART1_RX_ENABLE */

static void usart1_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable peripherals and GPIO Clocks */
  /* Enable GPIO clock */
  USART1_GPIO_CLK_ENABLE();
  /* Enable USART1's clock */
  __USART1_CLK_ENABLE();

  /* Configure peripheral GPIO */
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
#if USART1_TX_ENABLE
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = USART1_TX_PIN;
  GPIO_InitStruct.Alternate = USART1_TX_AF;
  HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);
#endif /* USART1_TX_ENABLE */
#if USART1_RX_ENABLE
  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = USART1_RX_PIN;
  GPIO_InitStruct.Alternate = USART1_RX_AF;
  HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);
#endif /* USART1_RX_ENABLE */

#if USART1_DMA_TX_ENABLE
  /* Enable TX DMA clock */
  USART1_DMA_TX_CLK_ENABLE();

  /* Configure the DMA */
  /* Configure the DMA handler for Transmission process */
  Usart1TxDmaHandle->Instance                 = USART1_TX_DMA_STREAM;
  Usart1TxDmaHandle->Init.Channel             = USART1_TX_DMA_CHANNEL;
  Usart1TxDmaHandle->Init.Direction           = DMA_MEMORY_TO_PERIPH;
  Usart1TxDmaHandle->Init.PeriphInc           = DMA_PINC_DISABLE;
  Usart1TxDmaHandle->Init.MemInc              = DMA_MINC_ENABLE;
  Usart1TxDmaHandle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  Usart1TxDmaHandle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  Usart1TxDmaHandle->Init.Mode                = DMA_NORMAL;
  Usart1TxDmaHandle->Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(Usart1TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, *Usart1TxDmaHandle);

  /* NVIC configuration for USART1 TX DMA */
  HAL_NVIC_SetPriority(USART1_DMA_TX_IRQn, INT_PRIO_USART1_DMATX, 0);
  HAL_NVIC_EnableIRQ(USART1_DMA_TX_IRQn);
#endif /* USART1_DMA_TX_ENABLE */
#if USART1_DMA_RX_ENABLE
  /* Enable RX DMA clock */
  USART1_DMA_RX_CLK_ENABLE();
  /* Configure the DMA handler for reception process */
  Usart1RxDmaHandle->Instance                 = USART1_RX_DMA_STREAM;
  Usart1RxDmaHandle->Init.Channel             = USART1_RX_DMA_CHANNEL;
  Usart1RxDmaHandle->Init.Direction           = DMA_PERIPH_TO_MEMORY;
  Usart1RxDmaHandle->Init.PeriphInc           = DMA_PINC_DISABLE;
  Usart1RxDmaHandle->Init.MemInc              = DMA_MINC_ENABLE;
  Usart1RxDmaHandle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  Usart1RxDmaHandle->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  Usart1RxDmaHandle->Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  Usart1RxDmaHandle->Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(Usart1RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, *Usart1RxDmaHandle);

  /* NVIC configuration for USART1 RX DMA */
  HAL_NVIC_SetPriority(USART1_DMA_RX_IRQn, INT_PRIO_USART1_DMARX, 0);
  HAL_NVIC_EnableIRQ(USART1_DMA_RX_IRQn);
#endif /* USART1_DMA_RX_ENABLE */

  /* NVIC for USART1 IRQ */
  HAL_NVIC_SetPriority(USART1_IRQn, INT_PRIO_USART1_PERIPH, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

static void usart1_msp_deinit(UART_HandleTypeDef *huart)
{
  /* Reset peripherals */
  __USART1_FORCE_RESET();
  __USART1_RELEASE_RESET();

  /* Disable peripherals and GPIO Clocks */
#if USART1_TX_ENABLE
  /* Configure USART1 Tx as alternate function  */
  HAL_GPIO_DeInit(USART1_TX_GPIO_PORT, USART1_TX_PIN);
#endif /* USART1_TX_ENABLE */
#if USART1_RX_ENABLE
  /* Configure USART1 Rx as alternate function  */
  HAL_GPIO_DeInit(USART1_RX_GPIO_PORT, USART1_RX_PIN);
#endif /* USART1_RX_ENABLE */
  /* Disable the DMA */
#if USART1_DMA_TX_ENABLE
  /* De-Initialize the DMA channel associated to transmission process */
  if(huart->hdmatx != 0) {
    HAL_DMA_DeInit(huart->hdmatx);
  }
  /* Disable the NVIC for DMA */
  HAL_NVIC_DisableIRQ(USART1_DMA_TX_IRQn);
#endif /* USART1_DMA_TX_ENABLE */
#if USART1_DMA_RX_ENABLE
  /* De-Initialize the DMA channel associated to reception process */
  if(huart->hdmarx != 0) {
    HAL_DMA_DeInit(huart->hdmarx);
  }
  /* Disable the NVIC for DMA */
  HAL_NVIC_DisableIRQ(USART1_DMA_RX_IRQn);
#endif /* USART1_DMA_RX_ENABLE */
}

#if USART1_DMA_TX_ENABLE
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void USART1_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Usart1Handle->hdmatx);
}
#endif /* USART1_DMA_TX_ENABLE */
#if USART1_DMA_RX_ENABLE
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void USART1_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Usart1Handle->hdmarx);
}
#endif /* USART1_DMA_RX_ENABLE */
/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
#if USART1_RX_ENABLE
  usart1_idle_callback(Usart1Handle);
#endif /* USART1_RX_ENABLE */
  HAL_UART_IRQHandler(Usart1Handle);
}

#else /* !USART1_ENABLE */

status_t usart1_init(uint32_t baudrate)
{
  return periph_disabled(TAG);
}

status_t usart1_deinit(void)
{
  return periph_disabled(TAG);
}

status_t usart1_tx_bytes(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

status_t usart1_tx_bytes_it(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

status_t usart1_tx_bytes_dma(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

uint32_t usart1_rx_bytes(uint8_t *p, uint32_t l)
{
  return periph_disabled(TAG);
}

uint32_t usart1_cache_usage(void)
{
  return periph_disabled(TAG);
}

#endif /* USART1_ENABLE */
