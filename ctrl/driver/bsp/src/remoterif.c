#include "remoterif.h"
#include "string.h"

/* UART handler declaration */
static UART_HandleTypeDef RMT_UartHandle;
static DMA_HandleTypeDef RMT_TxDmaHandle;
static DMA_HandleTypeDef RMT_RxDmaHandle;

static __IO uint32_t _rmt_tx_ready = true;

//static uint8_t rmtif_cache[UBXBIF_CACHE_DEPTH];
static uint8_t *rmtif_cache = NULL;
static uint32_t _rmt_data_available = 0, _rmt_ptr_out = 0;

status_t rmtif_init(void)
{
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  RMT_UartHandle.Instance        = RMT_UART;

  RMT_UartHandle.Init.BaudRate   = 115200;
  RMT_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  RMT_UartHandle.Init.StopBits   = UART_STOPBITS_1;
  RMT_UartHandle.Init.Parity     = UART_PARITY_NONE;
  RMT_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  RMT_UartHandle.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&RMT_UartHandle) != HAL_OK) return status_error;
  if(HAL_UART_Init(&RMT_UartHandle) != HAL_OK) return status_error;

  rmtif_cache = kmm_alloc(RMTIF_CACHE_DEPTH);
  if(rmtif_cache == NULL) return status_nomem;

  HAL_UART_Receive_DMA(&RMT_UartHandle, (uint8_t *)rmtif_cache, RMTIF_CACHE_DEPTH);
  __HAL_UART_ENABLE_IT(&RMT_UartHandle, UART_IT_IDLE);
  return status_ok;
}

status_t rmtif_tx_bytes(uint8_t *p, uint32_t l)
{
  return (status_t)HAL_UART_Transmit_DMA(&RMT_UartHandle, (uint8_t*)p, l);
}

status_t rmtif_tx_bytes_util(uint8_t *p, uint32_t l)
{
  status_t ret = status_ok;
  if(l == 0) return ret;
  while(_rmt_tx_ready == false) {}
  _rmt_tx_ready = false;
  ret = (status_t)HAL_UART_Transmit_DMA(&RMT_UartHandle, (uint8_t*)p, l);
  while(_rmt_tx_ready == false) {}
  return ret;
}

uint32_t rmtif_rx_bytes(uint8_t *p, uint32_t l)
{
  uint32_t len = l;
  if(_rmt_data_available < l) len = _rmt_data_available;
  for(uint32_t cnt = 0; cnt < len; cnt ++) {
    p[cnt] = rmtif_cache[_rmt_ptr_out ++];
    _rmt_data_available --;
    if(_rmt_ptr_out == RMTIF_CACHE_DEPTH) _rmt_ptr_out = 0;
  }
  return len;
}

status_t rmtif_set_baudrate(uint32_t baudrate)
{
  status_t ret = status_ok;
  while(_rmt_tx_ready == false) {}
  ret = (status_t)HAL_UART_AbortReceive(&RMT_UartHandle); if(ret != status_ok) return ret;
  __HAL_UART_DISABLE_IT(&RMT_UartHandle, UART_IT_IDLE);
  RMT_UartHandle.Init.BaudRate = baudrate;
  ret = (status_t)HAL_UART_Init(&RMT_UartHandle); if(ret != status_ok) return ret;
  _rmt_ptr_out = 0;
  _rmt_data_available = 0;
  __HAL_UART_ENABLE_IT(&RMT_UartHandle, UART_IT_IDLE);
  return (status_t)HAL_UART_Receive_DMA(&RMT_UartHandle, (uint8_t *)rmtif_cache, RMTIF_CACHE_DEPTH);
}

void rmtif_txcplt_callback(UART_HandleTypeDef *huart)
{
  _rmt_tx_ready = true;
}

void rmtif_rxcplt_callback(UART_HandleTypeDef *huart)
{

}

static void UART_IDLE_Callback(UART_HandleTypeDef *huart)
{
  if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);

    uint32_t current_pos = RMTIF_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(current_pos >= _rmt_ptr_out)
      _rmt_data_available = current_pos - _rmt_ptr_out;
    else
      _rmt_data_available = current_pos + RMTIF_CACHE_DEPTH - _rmt_ptr_out;
  }
}

void rmtif_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  RMT_UART_TX_GPIO_CLK_ENABLE();
  RMT_UART_RX_GPIO_CLK_ENABLE();

  /* Enable RMT_UART clock */
  RMT_UART_CLK_ENABLE();

  /* Enable DMA clock */
  RMT_UART_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = RMT_UART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = RMT_UART_TX_AF;

  HAL_GPIO_Init(RMT_UART_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = RMT_UART_RX_PIN;
  GPIO_InitStruct.Alternate = RMT_UART_RX_AF;

  HAL_GPIO_Init(RMT_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  RMT_TxDmaHandle.Instance                 = RMT_UART_TX_DMA_STREAM;
  RMT_TxDmaHandle.Init.Channel             = RMT_UART_TX_DMA_CHANNEL;
  RMT_TxDmaHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  RMT_TxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  RMT_TxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  RMT_TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  RMT_TxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  RMT_TxDmaHandle.Init.Mode                = DMA_NORMAL;
  RMT_TxDmaHandle.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&RMT_TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, RMT_TxDmaHandle);

  /* Configure the DMA handler for reception process */
  RMT_RxDmaHandle.Instance                 = RMT_UART_RX_DMA_STREAM;
  RMT_RxDmaHandle.Init.Channel             = RMT_UART_RX_DMA_CHANNEL;
  RMT_RxDmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  RMT_RxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  RMT_RxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  RMT_RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  RMT_RxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  RMT_RxDmaHandle.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  RMT_RxDmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&RMT_RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, RMT_RxDmaHandle);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (USART2_TX) */
  HAL_NVIC_SetPriority(RMT_UART_DMA_TX_IRQn, INT_PRIO_RMTIF_DMATX, 0);
  HAL_NVIC_EnableIRQ(RMT_UART_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (USART2_RX) */
  HAL_NVIC_SetPriority(RMT_UART_DMA_RX_IRQn, INT_PRIO_RMTIF_DMARX, 0);
  HAL_NVIC_EnableIRQ(RMT_UART_DMA_RX_IRQn);

  /* NVIC for USART, to catch the TX complete */
  HAL_NVIC_SetPriority(RMT_UART_IRQn, INT_PRIO_RMTIF_PERIPH, 0);
  HAL_NVIC_EnableIRQ(RMT_UART_IRQn);
}

void rmtif_msp_deinit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  RMT_UART_FORCE_RESET();
  RMT_UART_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure RMT_UART Tx as alternate function  */
  HAL_GPIO_DeInit(RMT_UART_TX_GPIO_PORT, RMT_UART_TX_PIN);
  /* Configure RMT_UART Rx as alternate function  */
  HAL_GPIO_DeInit(RMT_UART_RX_GPIO_PORT, RMT_UART_RX_PIN);

  /*##-3- Disable the DMA #####################################################*/
  /* De-Initialize the DMA channel associated to reception process */
  if(huart->hdmarx != 0) {
    HAL_DMA_DeInit(huart->hdmarx);
  }
  /* De-Initialize the DMA channel associated to transmission process */
  if(huart->hdmatx != 0) {
    HAL_DMA_DeInit(huart->hdmatx);
  }

  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(RMT_UART_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(RMT_UART_DMA_RX_IRQn);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void RMT_UART_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(RMT_UartHandle.hdmarx);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void RMT_UART_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(RMT_UartHandle.hdmatx);
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void RMT_UART_IRQHandler(void)
{
  UART_IDLE_Callback(&RMT_UartHandle);
  HAL_UART_IRQHandler(&RMT_UartHandle);
}
