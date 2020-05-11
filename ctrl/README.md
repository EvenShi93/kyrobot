# Gimbal controller

#### MCU:
```STM32F732RET6, Cortex-M7F, Freq(216MHz), ROM(512K), RAM(256K), LQFP64```
#### IMU:
```ICM42605, 3-Axis gyroscope, 3-Axis accelerometer, 2.5x3x0.91mm 14-pin LGA package```
#### GCC:
```arm-none-eabi-gcc version 7.2.1 20170904 (release)```
#### CONNECTION:
```
     SPI1       -> ICM42605 IMU Sensor
     SPI2       -> microSD
     SPI3       -> IPS displayer
     USB(CDC)   -> Communicate with upper
     USART1     -> EXT_PORT
     IIC1       -> EXT_PORT
     USART2     -> Remote controller
     USART4     -> Communicate with upper
     USART6     -> logger
     CAN        -> External sensor
     TIM1_CH1/2 -> Motor
     TIM2_CH2   -> IPS backlight
     TIM3_CH1/2 -> Encoder
     TIM8_CH4   -> Steer
     ADC1_IN9   -> Battery
```

#### Hardware Resources:
```
    6-Dof IMU Sensor <ICM42605>: SPI_CS(PA4) SPI_MOSI(PA7) SPI_MISO(PA6) SPI_SCLK(PA5) INT1(PC4) INT2(PB0)
    microSD Card: MOSI(PB15) MISO(PB14) SCLK(PB13) NSS(PB12) Detect(PB2)
    IPS screen: BL(PB3) RST(PD2) MOSI(PC12) NSS(PC11) SCLK(PC10) DC(PA15)
    LED Indicator: GREEN(PC3) BLUE(PC2)
    COM PORT: RX(PA1) TX(PA0)
    RMT PORT: RX(PA3) TX(PA2)
    EXT PORT: TX/SCL(PB6) RX/SDA(PB7)
    CAN BUS: RX(PB8) TX(PB9)
    LOG PORT: RX(PC7) TX(PC6)
    USB Device: DM(PA11) DP(PA12) ID(PA10) SWT(PC13)
    ADC IN: (PB1)
    PWR CTRL: (PC1)
    MOTOR: INA(PA8) INB(PA9)
    STEER: PWM(PC9)
    BUTTON: KEY1(PB11) KEY2(PB10) KEY3(PC8) KEY_PWR(PC0)
```

#### DMA requests for each channel:
```
    DMA1:
    +---------+-----------+---------+---------+---------+---------+---------+---------+
    | STREAM0 |  STREAM1  | STREAM2 | STREAM3 | STREAM4 | STREAM5 | STREAM6 | STREAM7 |
    +---------+-----------+---------+---------+---------+---------+---------+---------+
    |   xxx   |  xxxxxxx  |  U4_RX  |  xxxxx  |  U4_TX  |  U2_RX  |  U2_TX  | SPI3_TX |
    +---------+-----------+---------+---------+---------+---------+---------+---------+
    DMA2:
    +---------+-----------+-----------+---------+---------+---------+-----------+---------+
    | STREAM0 |  STREAM1  |  STREAM2  | STREAM3 | STREAM4 | STREAM5 |  STREAM6  | STREAM7 |
    +---------+-----------+-----------+---------+---------+---------+-----------+---------+
    | SPI1_RX |  xxxxxxx  |  (COM_RX) | SPI1_TX | ADC1_C9 |  U1_RX  |  (COM_TX) |  U1_TX  |
    +---------+-----------+-----------+---------+---------+---------+-----------+---------+
```

                                                       kyChu@2020/04/14
