#include "drivers.h"

/**
  * @brief  Tx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *SpiHandle)
{
  if(SpiHandle->Instance == DISP_SPI) {
    dispif_txcplt_callback(SpiHandle);
  }
}

/**
  * @brief  Tx and Rx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *SpiHandle)
{
  if(SpiHandle->Instance == IMU_SPI) {
    imuif_rxtxcplt_callback(SpiHandle);
  } else if(SpiHandle->Instance == SD_SPI) {
    sdif_rxtxcplt_callback(SpiHandle);
  }
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  I2cHandle: I2C handle.
  * @retval None
  */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance == MAG_I2C) {
    magif_txcplt_callback(I2cHandle);
  }
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  I2cHandle: I2C handle
  * @retval None
  */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance == MAG_I2C) {
    magif_rxcplt_callback(I2cHandle);
  }
}

/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
  if(I2cHandle->Instance == MAG_I2C) {
    magif_error_callback(I2cHandle);
  }
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if((IMU_INT1_PIN & GPIO_Pin) != RESET) {
    imuif_int1_callback();
  }
  if((IMU_INT2_PIN & GPIO_Pin) != RESET) {
    imuif_int2_callback();
  }
}
