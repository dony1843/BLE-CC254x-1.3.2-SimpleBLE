#include "AFE4404_Drv.h"
#include "npi.h"
#include "OSAL.h"
#include "ioCC2541.h"
#include "hal_i2c.h"
#include "hal_sensor.h"
#include "hal_defs.h"

#include <string.h>
#include <stdio.h>

int32 ppg_data[ppg_data_len];
int cnt = 0;

enum {
    WRITE_MODE  =  0x00,
    READ_MODE   =  0x01
};

afe_reg_st afe_config_data[] = {
        {0x00, 0x01},
        {0x01, 0x00},
};

bool isUpdate = FALSE;
int32 *g_buffer_dat_red = NULL;
int32 *g_buffer_dat_green = NULL;
int32 *g_buffer_dat_ir = NULL;
int32 g_buffer_cnt = 0;

int32 afe_adc_convert_val(uint32 inVal, bool *cvtRes)
{
    int32 outVal = 0;
    bool outRes = FALSE;

    if(0x1 == (inVal & 0x00E00000) / 0x200000){
        NPI_PrintValue("Overflow(0x%08X)!", inVal, 16);
    }
    else if(0x6 == (inVal & 0x00E00000) / 0x200000){
        NPI_PrintValue("Underflow(0x%08X)!", inVal, 16);
    }
    else if(0x0 == (inVal & 0x00E00000) / 0x200000){
        outVal = inVal & 0x001FFFFF;
    }
    else if(0x7 == (inVal & 0x00E00000) / 0x200000){
        outVal = (0x200000 - inVal & 0x001FFFFF) * -1;
    }
    else{
        NPI_PrintValue("Unknown Data(0x%08X)!", inVal, 16);
    }
    outRes = TRUE;

    if(NULL != cvtRes) *cvtRes = outRes;
    return outVal;
}

//AFE4404中断处理
void afe4404_interrupt_process(void)
{
    uint8 buf[128];
    bool cvtRes = FALSE;
    uint32 rawBare = 0;
    int32 valLED1 = 0;
    int32 valALED = 0;
    int32 valLED1_ALED1 = 0;

//    int32 valLED2 = 0;
//    int32 valLED3 = 0;
//    int32 valLED2_ALED2 = 0;
//    VMDOUBLE vLED1 = 0;
//    VMDOUBLE vALED = 0;
//    VMDOUBLE vLED1_ALED1 = 0;
//    VMDOUBLE vLED2 = 0;
//    VMDOUBLE vLED3 = 0;
//    VMDOUBLE vLED2_ALED2 = 0;

//    afe_i2c_readReg(LED2VAL, &rawBare);
//    valLED2 = afe_adc_convert_val(rawBare, &cvtRes);
//    vLED2 = valLED2 * 1.2 / 2097152;
//    if(FALSE == cvtRes) vm_log_debug("Convert LED2 failed!");

//    afe_i2c_readReg(LED3VAL, &rawBare);
//    valLED3 = afe_adc_convert_val(rawBare, &cvtRes);
//    vLED3 = valLED3 * 1.2 / 2097152;
//    if(FALSE == cvtRes) vm_log_debug("Convert LED3 failed!");

//    afe_i2c_readReg(LED2_ALED2VAL, &rawBare);
//    valLED2_ALED2 = afe_adc_convert_val(rawBare, &cvtRes);
//    valLED2_ALED2 = valLED2_ALED2 * 1.2 / 2097152;
//    if(FALSE == cvtRes) vm_log_debug("Convert LED3 failed!");

    afe_i2c_readReg(LED1VAL, &rawBare);
    valLED1 = afe_adc_convert_val(rawBare, &cvtRes);
    valLED1 = 0 - valLED1;
//    vLED1 = valLED1 * 1.2 / 2097152;
    if(FALSE == cvtRes) 
      NPI_PrintString("Convert LED1 failed!\r\n");
    else
    {
      // 可以输出一个值，用10进制表示
      if( valLED1 >= 0 )
      {
        _ltoa( valLED1, buf, 10 );
        strcat( buf, "\n" );
        NPI_PrintString(buf);
       // NPI_PrintString( "\n" );
      }
      else
      {
        buf[0] = '-';
        valLED1 = ~valLED1 + 1;
        _ltoa( valLED1, &buf[1], 10 );
        strcat( buf, "\n" );
        NPI_PrintString(buf);
      //  NPI_PrintString( "\n" );
      }
    }
/*
    afe_i2c_readReg(ALED1VAL, &rawBare);
    valALED = afe_adc_convert_val(rawBare, &cvtRes);
//    vALED = valALED * 1.2 / 2097152;
    if(FALSE == cvtRes) 
      NPI_PrintString("Convert ALED failed!\r\n");
    else
    {
      // 可以输出一个值，用10进制表示
      if( valALED >= 0 )
      {
        _ltoa( valALED, buf, 10 );
        NPI_PrintString(buf);
        NPI_PrintString( "\r\n" );
      }
      else
      {
        buf[0] = '-';
        valLED1 = ~valALED + 1;
        _ltoa( valALED, &buf[1], 10 );
        NPI_PrintString(buf);
        NPI_PrintString( "\r\n" );
      }
    }

    afe_i2c_readReg(LED1_ALED1VAL, &rawBare);
    valLED1_ALED1 = afe_adc_convert_val(rawBare, &cvtRes);
//    vLED1_ALED1 = valLED1_ALED1 * 1.2 / 2097152;
    if(FALSE == cvtRes) 
      NPI_PrintString("Convert ALED failed!\r\n");
    else
    {
      // 可以输出一个值，用10进制表示
      if( valLED1_ALED1 >= 0 )
      {
        _ltoa( valLED1_ALED1, buf, 10 );
        NPI_PrintString(buf);
        NPI_PrintString( "\r\n" );
      }
      else
      {
        buf[0] = '-';
        valLED1 = ~valLED1_ALED1 + 1;
        _ltoa( valLED1_ALED1, &buf[1], 10 );
        NPI_PrintString(buf);
        NPI_PrintString( "\r\n" );
      }
    }
 */

#if 0
    ppg_data[cnt] = valLED1;
    cnt++;
    if (cnt == ppg_data_len)
    {
      cnt = 0;
    //  afe_ppg_stop(); //停止AFE4404
    }
#endif

}

//AFE4404 ADC_RDY中断配置
void afe4404_eint(void)
{
  P1SEL &= ~BV(0);  //P1.0作GPIO
  P1DIR &= ~BV(0);  //P1.0作输入
  P1IEN |= BV(0);   //P1.0中断使能
  PICTL |= BV(1);   //P1.0~P1.3下降沿中断
  IEN2 |= BV(4);    //P1端口中断使能
}

bool afe_reg_dump(void)
{
    uint8 ii = 0;
    uint32 regVal = 0;

    for(ii = 0; ii < 0x40; ii++)
    {
      afe_i2c_readReg(ii, &regVal);
//    vm_log_info("0x%2X : 0x%06X", ii, regVal);
    }

    return TRUE;
}

void sample_reg_timing()/* Sample Register Settings */
{
    uint32 val = 0;

    /* set PRPCT */
    val = 39999;
    afe_i2c_writeReg(PRPCOUNT, val);

    /* LED2 start */
    val = 0;
    afe_i2c_writeReg(LED2LEDSTC, val);
    /* LED2 end */
    val = 399;
    afe_i2c_writeReg(LED2LEDENDC, val);

    /* sample LED2 start */
    val = 80;
    afe_i2c_writeReg(LED2STC, val);
    /* sample LED2 end */
    val = 399;
    afe_i2c_writeReg(LED2ENDC, val);

    /* ADC reset phase 0 start */
    val = 401;
    afe_i2c_writeReg(ADCRSTSTCT0, val);
    /* ADC reset phase 0 end */
    val = 407;
    afe_i2c_writeReg(ADCRSTENDCT0, val);

    /* LED2 convert phase start */
    val = 408;
    afe_i2c_writeReg(LED2CONVST, val);
    /* LED2 convert phase end */
    val = 1467;
    afe_i2c_writeReg(LED2CONVEND, val);

    /* LED3 start */
    val = 400;
    afe_i2c_writeReg(LED3LEDSTC, val);
    /* LED3 end */
    val = 799;
    afe_i2c_writeReg(LED3LEDENDC, val);

    /* Sample ambient 2 (or sample LED3) start */
    val = 480;
    afe_i2c_writeReg(ALED2STC, val);
    /* Sample ambient 2 (or sample LED3) end */
    val = 799;
    afe_i2c_writeReg(ALED2ENDC, val);

    /* ADC reset phase 1 start */
    val = 1469;
    afe_i2c_writeReg(ADCRSTSTCT1, val);
    /* ADC reset phase 1 end */
    val = 1475;
    afe_i2c_writeReg(ADCRSTENDCT1, val);

    /* Ambient 2 (or LED3) convert phase start */
    val = 1476;
    afe_i2c_writeReg(ALED2CONVST, val);
    /* Ambient 2 (or LED3) convert phase end */
    val = 2535;
    afe_i2c_writeReg(ALED2CONVEND, val);

    /* LED1 start */
    val = 800;
    afe_i2c_writeReg(LED1LEDSTC, val);
    /* LED1 end */
    val = 1199;
    afe_i2c_writeReg(LED1LEDENDC, val);

    /* sample LED1 start */
    val = 880;
    afe_i2c_writeReg(LED1STC, val);
    /* sample LED1 end */
    val = 1199;
    afe_i2c_writeReg(LED1ENDC, val);

    /* ADC reset phase 2 start */
    val = 2537;
    afe_i2c_writeReg(ADCRSTSTCT2, val);
    /* ADC reset phase 2 end */
    val = 2543;
    afe_i2c_writeReg(ADCRSTENDCT2, val);

    /* LED1 convert phase start */
    val = 2544;
    afe_i2c_writeReg(LED1CONVST, val);
    /* LED1 convert phase end */
    val = 3603;
    afe_i2c_writeReg(LED1CONVEND, val);

    /* Sample ambient 1 start */
    val = 1279;
    afe_i2c_writeReg(ALED1STC, val);
    /* Sample ambient 1 end */
    val = 1598;
    afe_i2c_writeReg(ALED1ENDC, val);

    /* ADC reset phase 3 start */
    val = 3605;
    afe_i2c_writeReg(ADCRSTSTCT3, val);
    /* ADC reset phase 3 end */
    val = 3611;
    afe_i2c_writeReg(ADCRSTENDCT3, val);

    /* Ambient 1 convert phase start */
    val = 3612;
    afe_i2c_writeReg(ALED1CONVST, val);
    /* Ambient 1 convert phase end */
    val = 4671;
    afe_i2c_writeReg(ALED1CONVEND, val);

    /* PDN_CYCLE start */
    val = 5471;
    afe_i2c_writeReg(PDNCYCLESTC, val);
    /* PDN_CYCLE end */
    val = 39199;
    afe_i2c_writeReg(PDNCYCLEENDC, val);
}

bool afe_config()
{
    uint32 val = 0;

#if 0
    uint32 i = 0;

    afe_i2c_writeReg(0, val);
    for(i= 0; i < sizeof(afe_config_data)/sizeof(afe_reg_st); i++) {
        afe_i2c_writeReg(afe_config_data[i].reg, afe_config_data[i].val);
    }
#endif

    /* AFE_CONTROL1 TimerEN = 1; NUMAV = 2 */
    val = 0x000103;
    afe_i2c_writeReg(CONTROL1, val);

    /* DYN1, LEDCurr, DYN2, OSC, DYN3, DYN4 //0x000200); - 0x200 Osc mode //AFE_CONTROL2 */
    val = 0x104218;
    afe_i2c_writeReg(CONTROL2, val);

    /* AFE_TIA_SEP_GAIN (LED2) ENSEPGAIN = 1; LED2/LED3 gain = 250K */
    val = 0x008001;
    afe_i2c_writeReg(TIA_GAIN_SEP, val);

    /* AFE_TIA_GAIN (LED1) LED1/LED1AMB gain = 250K, Cf = 5pF */
    val = 6;     
    afe_i2c_writeReg(TIA_GAIN, val);

    /* LED3 - 4mA; LED2 - 4mA; LED1 - 8mA */
    val = 30;    //val*0.8mA
    afe_i2c_writeReg(LEDCNTRL, val);

    /* AFE_DAC_SETTING_REG */
    val = (4 << 5) + (1 << 9);
    afe_i2c_writeReg(AMBDAC, val);

    return TRUE;
}

//AFE4404软件复位
bool afe_sw_reset()
{
    uint32 val = 0;

    val = 0x09;
    /* reset and enable read out of write registers */
    afe_i2c_writeReg(CONTROL0, val);
    return TRUE;
}

//AFE4404硬件复位
void afe_hw_reset()
{
  P1SEL &= ~BV(2);  //P1.2用作GPIO
  P1DIR |= BV(2);   //P1.2作输出
  
  AFE_RST_PIN = 1;  //拉高
  
  ST_HAL_DELAY(2000); //1ms/125 * 2000 = 16ms
  
  AFE_RST_PIN = 0;  //拉低

  /* AFE4404 reset require 25 ~50 us active low */
  ST_HAL_DELAY(4);  //1000us/125 * 4 = 32us
  
  AFE_RST_PIN = 1;  //拉高
  
  ST_HAL_DELAY(200);  //1.6ms
}

//AFE4404初始化
bool afe_i2c_init(void)
{
    afe_hw_reset();   //硬件复位

    HalSensorInit(AFE4404_I2C_DEFAULT_ADDRESS); //I2C初始化

    return TRUE;
}

/*******************************************************************************
* @fn          afe_i2c_writeReg
* @brief       This function implements the I2C protocol to write to a sensor. he sensor must
*              be selected before this routine is called.
*
* @param       addr - which register to write
* @param       val - data to be written
*
* @return      TRUE if successful write
*/
bool afe_i2c_writeReg(uint8 reg_addr, uint32 val)
{
    bool i;
    uint8 i2c_write_data[3];

    i2c_write_data[0] = ((val & 0x00ff0000) >> 16);
    i2c_write_data[1] = ((val & 0x0000ff00) >> 8);
    i2c_write_data[2] = (val & 0xff);
    
    i = HalSensorWriteReg(reg_addr, i2c_write_data, 3);
    return (i);
}

/**************************************************************************************************
 * @fn          afe_i2c_readReg
 *
 * @brief       This function implements the I2C protocol to read from a sensor. The sensor must
 *              be selected before this routine is called.
 *
 * @param       addr - which register to read
 * @param       data - pointer to buffer to place data
 *
 * @return      TRUE if the required number of bytes are reveived
 **************************************************************************************************/
bool afe_i2c_readReg(uint8 reg_addr, uint32 *data)
{
    bool i;
    uint8 i2c_read_data[3] = {0};
    
    i = HalSensorReadReg(reg_addr, i2c_read_data, 3);
    
    if(i)
    {
      *data = BUILD_UINT32(i2c_read_data[2], i2c_read_data[1], i2c_read_data[0], 0);
      return TRUE;
    }
    else
      return FALSE;   
}

bool afe_ppg_start(void)
{
    afe_i2c_init();
    afe_sw_reset();

    /* write registers mode */
    afe_i2c_writeReg(CONTROL0, WRITE_MODE);

    afe_config();
    sample_reg_timing();

    /* read registers mode */
    afe_i2c_writeReg(CONTROL0, READ_MODE);

    afe4404_eint();
    
    ST_HAL_DELAY(25000);  //1ms/125 * 25000 = 200ms

    return TRUE;
}

bool afe_ppg_stop(void)   //进入power down模式
{
  AFE_RST_PIN = 0;    
  ST_HAL_DELAY(40); //1000us/125 * 40 = 320us
  
  return TRUE;
}
