#include "controlif.h"
#include "string.h"

#include "ringbuffer.h"

/* UART handler declaration */
static UART_HandleTypeDef CTRL_UartHandle;
static DMA_HandleTypeDef CTRL_TxDmaHandle;
static DMA_HandleTypeDef CTRL_RxDmaHandle;

static __IO uint32_t _ctrlif_tx_ready = true;

#define CTRL_RB_CAPACITY                           1024
static ringbuffer_handle ec20_rb;
static uint8_t ringbuffer_cache[CTRL_RB_CAPACITY];

//static uint8_t ctrlif_cache[EC20IF_CACHE_DEPTH];
static uint8_t *ctrlif_cache = NULL;
//static uint32_t _ec20_data_available = 0, _ec20_ptr_out = 0;

status_t ctrlif_init(void)
{
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  CTRL_UartHandle.Instance        = CTRL_UART;

  CTRL_UartHandle.Init.BaudRate   = 115200;
  CTRL_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  CTRL_UartHandle.Init.StopBits   = UART_STOPBITS_1;
  CTRL_UartHandle.Init.Parity     = UART_PARITY_NONE;
  CTRL_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  CTRL_UartHandle.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&CTRL_UartHandle) != HAL_OK) return status_error;
  if(HAL_UART_Init(&CTRL_UartHandle) != HAL_OK) return status_error;

  ringbuffer_init(&ec20_rb, ringbuffer_cache, CTRL_RB_CAPACITY);

//  memset(ctrlif_cache, 0x00, EC20IF_CACHE_DEPTH);
  ctrlif_cache = kmm_alloc(CTRLIF_CACHE_DEPTH);
  if(ctrlif_cache == NULL) return status_nomem;
//  SCB_CleanDCache();
  __HAL_UART_ENABLE_IT(&CTRL_UartHandle, UART_IT_IDLE);
  HAL_UART_Receive_DMA(&CTRL_UartHandle, (uint8_t *)ctrlif_cache, CTRLIF_CACHE_DEPTH);
  return status_ok;
}

status_t ctrlif_tx_string_util(const char *str)
{
  return ctrlif_tx_bytes_util((uint8_t *)str, strlen(str));
}

status_t ctrlif_tx_bytes(uint8_t *p, uint32_t l)
{
  return (status_t)HAL_UART_Transmit_DMA(&CTRL_UartHandle, (uint8_t*)p, l);
}

status_t ctrlif_tx_bytes_util(uint8_t *p, uint32_t l)
{
  status_t ret = status_ok;
  if(l == 0) return ret;
  while(_ctrlif_tx_ready == false) {}
  _ctrlif_tx_ready = false;
  ret = (status_t)HAL_UART_Transmit_DMA(&CTRL_UartHandle, p, l);
  while(_ctrlif_tx_ready == false) {}
  return ret;
}

uint32_t ctrlif_rx_bytes(uint8_t *p, uint32_t l)
{
//  uint32_t len = l;
//  if(_ec20_data_available < l) len = _ec20_data_available;
//  for(uint32_t cnt = 0; cnt < len; cnt ++) {
//    p[cnt] = ctrlif_cache[_ec20_ptr_out ++];
//    _ec20_data_available --;
//    if(_ec20_ptr_out == EC20IF_CACHE_DEPTH) _ec20_ptr_out = 0;
//  }
//  return len;
  return ringbuffer_poll(&ec20_rb, p, l);
}

uint32_t ctrlif_cache_usage(void)
{
  return ringbuffer_usage(&ec20_rb);
}

void ctrlif_txcplt_callback(UART_HandleTypeDef *huart)
{
  _ctrlif_tx_ready = true;
}

void ctrlif_rxcplt_callback(UART_HandleTypeDef *huart)
{
}
uint32_t max_remain = 0;
uint32_t idle_receive = 0;
static void UART_IDLE_Callback(UART_HandleTypeDef *huart)
{
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);

//    uint32_t current_pos = EC20IF_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
//    if(current_pos >= _ec20_ptr_out)
//      _ec20_data_available = current_pos - _ec20_ptr_out;
//    else
//      _ec20_data_available = current_pos + EC20IF_CACHE_DEPTH - _ec20_ptr_out;
    uint32_t len = CTRLIF_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(len > max_remain) max_remain = len;
    idle_receive = len;
    if(len > 0) {
      __HAL_DMA_DISABLE(huart->hdmarx);
      ringbuffer_push(&ec20_rb, ctrlif_cache, len);
      __HAL_DMA_SET_COUNTER(huart->hdmarx, CTRLIF_CACHE_DEPTH);
      __HAL_DMA_ENABLE(huart->hdmarx);
//      __HAL_DMA_CLEAR_FLAG(huart->hdmarx, DMA_FLAG_TCIF0_4);
    }
  }
}

void ctrlif_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  CTRL_UART_TX_GPIO_CLK_ENABLE();
  CTRL_UART_RX_GPIO_CLK_ENABLE();

  /* Enable CTRL_UART clock */
  CTRL_UART_CLK_ENABLE();

  /* Enable DMA clock */
  CTRL_UART_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = CTRL_UART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = CTRL_UART_TX_AF;

  HAL_GPIO_Init(CTRL_UART_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = CTRL_UART_RX_PIN;
  GPIO_InitStruct.Alternate = CTRL_UART_RX_AF;

  HAL_GPIO_Init(CTRL_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  CTRL_TxDmaHandle.Instance                 = CTRL_UART_TX_DMA_STREAM;
  CTRL_TxDmaHandle.Init.Channel             = CTRL_UART_TX_DMA_CHANNEL;
  CTRL_TxDmaHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  CTRL_TxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  CTRL_TxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  CTRL_TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  CTRL_TxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  CTRL_TxDmaHandle.Init.Mode                = DMA_NORMAL;
  CTRL_TxDmaHandle.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&CTRL_TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, CTRL_TxDmaHandle);

  /* Configure the DMA handler for reception process */
  CTRL_RxDmaHandle.Instance                 = CTRL_UART_RX_DMA_STREAM;
  CTRL_RxDmaHandle.Init.Channel             = CTRL_UART_RX_DMA_CHANNEL;
  CTRL_RxDmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  CTRL_RxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  CTRL_RxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  CTRL_RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  CTRL_RxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  CTRL_RxDmaHandle.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  CTRL_RxDmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&CTRL_RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, CTRL_RxDmaHandle);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (UART4_TX) */
  HAL_NVIC_SetPriority(CTRL_UART_DMA_TX_IRQn, INT_PRIO_GSMIF_DMATX, 0);
  HAL_NVIC_EnableIRQ(CTRL_UART_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (UART4_RX) */
  HAL_NVIC_SetPriority(CTRL_UART_DMA_RX_IRQn, INT_PRIO_GSMIF_DMARX, 0);
  HAL_NVIC_EnableIRQ(CTRL_UART_DMA_RX_IRQn);

  /* NVIC for USART, to catch the TX complete */
  HAL_NVIC_SetPriority(CTRL_UART_IRQn, INT_PRIO_GSMIF_PERIPH, 0);
  HAL_NVIC_EnableIRQ(CTRL_UART_IRQn);
}

void ctrlif_msp_deinit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  CTRL_UART_FORCE_RESET();
  CTRL_UART_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure CTRL_UART Tx as alternate function  */
  HAL_GPIO_DeInit(CTRL_UART_TX_GPIO_PORT, CTRL_UART_TX_PIN);
  /* Configure CTRL_UART Rx as alternate function  */
  HAL_GPIO_DeInit(CTRL_UART_RX_GPIO_PORT, CTRL_UART_RX_PIN);

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
  HAL_NVIC_DisableIRQ(CTRL_UART_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(CTRL_UART_DMA_RX_IRQn);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void CTRL_UART_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(CTRL_UartHandle.hdmarx);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void CTRL_UART_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(CTRL_UartHandle.hdmatx);
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void CTRL_UART_IRQHandler(void)
{
  UART_IDLE_Callback(&CTRL_UartHandle);
  HAL_UART_IRQHandler(&CTRL_UartHandle);
}
