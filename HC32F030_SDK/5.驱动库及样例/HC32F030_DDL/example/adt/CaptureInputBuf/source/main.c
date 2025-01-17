/******************************************************************************
* Copyright (C) 2017, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file main.c
 **
 ** A detailed description is available at
 ** @link Sample Group Some description @endlink
 **
 **   - 2017-06-20 LiuHL    First Version
 **
 ******************************************************************************/

/******************************************************************************
 * Include files
 ******************************************************************************/
#include "adt.h"
#include "gpio.h"

/******************************************************************************
 * Local pre-processor symbols/macros ('#define')                            
 ******************************************************************************/


/******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/******************************************************************************
 * Local type definitions ('typedef')                                         
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/


/******************************************************************************
 * Local variable definitions ('static')                                      *
 ******************************************************************************/
static uint16_t u16Capture;
static uint16_t u16CaptureBuf;

/*****************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

void Adt4CaptureBCalllback(void)
{    
    Adt_GetCaptureValue(AdtTIM4, AdtCHxB, &u16Capture);
    Adt_GetCaptureBuf(AdtTIM4, AdtCHxB, &u16CaptureBuf);
}

void Adt4CompACalllback(void)
{
    static uint32_t i = 0;
    
    if (i%2)
    {
        Adt_SetCompareValue(AdtTIM4, AdtCompareC, 0x6000);
    }
    else
    {
        Adt_SetCompareValue(AdtTIM4, AdtCompareC, 0x3000);
    }
    i++;
}

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/
int32_t main(void)
{
    en_adt_unit_t            enAdt;
    uint16_t                 u16Period;
    en_adt_compare_t         enAdtCompare;
    uint16_t                 u16Compare;
    stc_adt_basecnt_cfg_t    stcAdtBaseCntCfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4ACfg;
    stc_adt_CHxX_port_cfg_t  stcAdtTIM4BCfg;
    stc_gpio_config_t        stcTIM4APort;
    stc_gpio_config_t        stcTIM4BPort;

    DDL_ZERO_STRUCT(stcAdtBaseCntCfg);
    DDL_ZERO_STRUCT(stcAdtTIM4ACfg);
    DDL_ZERO_STRUCT(stcAdtTIM4BCfg);
    DDL_ZERO_STRUCT(stcTIM4APort);
    DDL_ZERO_STRUCT(stcTIM4BPort);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); //端口外设时钟使能
    
    //PA08设置为TIM4_CHA
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin8);
    stcTIM4APort.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin8, &stcTIM4APort);
    Gpio_SetAfMode(GpioPortA,GpioPin8,GpioAf6);
    
    //PA07设置为TIM4_CHB
    //Gpio_ClrAnalogMode(GpioPortA, GpioPin7);
    stcTIM4BPort.enDir  = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin7, &stcTIM4BPort);
    Gpio_SetAfMode(GpioPortA,GpioPin7,GpioAf7);

    if (Ok != Sysctrl_SetPeripheralGate(SysctrlPeripheralAdvTim, TRUE))  //ADT外设时钟使能
    {
        return Error;
    }
    enAdt = AdtTIM4;
    
    stcAdtBaseCntCfg.enCntMode = AdtSawtoothMode;
    stcAdtBaseCntCfg.enCntDir = AdtCntUp;
    stcAdtBaseCntCfg.enCntClkDiv = AdtClkPClk0Div1024;
    Adt_Init(enAdt, &stcAdtBaseCntCfg);                           //ADT载波、计数模式、时钟配置
    
    u16Period = 0xC000;
    Adt_SetPeriod(enAdt, u16Period);                              //周期设置
    
    enAdtCompare = AdtCompareA;
    u16Compare = 0x6000;
    Adt_SetCompareValue(enAdt, enAdtCompare, u16Compare);         //通用比较基准值寄存器A设置
    enAdtCompare = AdtCompareC;
    u16Compare = 0x3000;
    Adt_SetCompareValue(enAdt, enAdtCompare, u16Compare);         //通用比较基准值寄存器B设置
    
    stcAdtTIM4ACfg.enCap = AdtCHxCompareOutput;                   //CHA比较输出
    stcAdtTIM4ACfg.bOutEn = TRUE;                                 //输出使能
    stcAdtTIM4ACfg.enPerc = AdtCHxPeriodLow;
    stcAdtTIM4ACfg.enCmpc = AdtCHxCompareHigh;
    stcAdtTIM4ACfg.enStaStp = AdtCHxStateSelSS;
    stcAdtTIM4ACfg.enStaOut = AdtCHxPortOutLow;
    Adt_CHxXPortConfig(enAdt, AdtCHxA, &stcAdtTIM4ACfg);          //端口CHA配置,比较输出功能
    Adt_EnableValueBuf(enAdt, AdtCHxA, TRUE);                     //比较基准值缓存传送功能打开
    
    stcAdtTIM4BCfg.enCap = AdtCHxCompareInput;                    //CHB设为捕获输入
    Adt_CHxXPortConfig(enAdt, AdtCHxB, &stcAdtTIM4BCfg);          //端口CHA配置,捕获输入功能
    Adt_EnableValueBuf(enAdt, AdtCHxB, TRUE);                     //捕获值缓存传送功能打开
    
    Adt_ConfigHwCaptureB(enAdt, AdtHwTrigCHxBRise);               //硬件捕获B条件配置
    Adt_ConfigIrq(enAdt, AdtCMBIrq, TRUE, Adt4CaptureBCalllback); //捕获输入B中断配置
    Adt_ConfigIrq(enAdt, AdtCMAIrq, TRUE, Adt4CompACalllback);    //比较匹配A中断配置
    
    Adt_StartCount(enAdt);
    
    while(1);
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/


