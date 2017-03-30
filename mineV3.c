// mineV3.c
// idio me mineV2 me moni diafora, tin xrisi tis UARTGetNum(void)!!!
// kai profanws epistrofi se commandade2get san pinaka me ints.

/* kwdikas gia paragwgi pwm.
** based on billys and starterware
**
** title:         mineV3.c
** orismata:      [frequency],[duty cycle],[counter mode=up down combos],?[active high or low]?,??????????[dif i dir],
** result:
**
** sto build prepei na kaneis link ta: gpio_v2.c, pwmss.c, gpio.c, ehrpwm.c,
** bb_uart.c, bb_uartConsole.c, uart_irda_cir.c, uartStdio.c.
*/

/* BHMATA Starterware:
** 1)enable the module in the Power Sleep Controller by using
**   PSCModuleControl()  <-------------->DEN UPARXEI (giati?)
** 2)pin multiplexing <----> EHRPWMPinMuxSetup()
** 3)enable the AINTC (to receive interrupts)
** 4)twist whichever subModules u need from the
**   following:
**              -timebase (8es:EHRPWMTimebaseClkConfig,
                                EHRPWMPWMOpFreqSet,
                                EHRPWMTTimebaseSyncEnable,
                                EHRPWMTEmulationModeSet,
                                EHRPWMTSyncOutModeSet)
**              -counterCompare (8es:EHRPWMTLoadCMPx)
**              -actionQualifier (8es: EHRPWMTConfigureAQActionONx)
**              -deadBand
**              -pwmChopper (8es: EHRPWMConfigureChopperFreq/Duty/OSPW)
**              -HR
*/

//INCLUDEs
#include "customanos_pwm.h"
#include "heloHeaderV2.h"               //gia tin UARTstart_me
#include "soc_AM335x.h"                 //SOC_PWMSSx_REGS,SOC_GPIO_x_REGS dld oi d/nseis twn modules
#include "beaglebone.h"
#include "hw_types.h"                   //auto kai to apo panw kaluptoun to bool.
#include "ehrpwm.h"
#include "hw_ehrpwm.h"
#include "pwmss.h"
#include "gpio_v2.h"
#include "uart_irda_cir.h"
#include "uartStdio.h"
//#include "hw_gpio.h"
//DEFINEs
#define GPIOP8_34           (GPIO_2_17)     //apo bb user guide(mode7) gpio1[18]=ehrpwm1a
#define GPIOP8_36           (GPIO_2_16)     //gpio1[19]=ehrpwm1b
#define GPIO_PIN_NUMBER_A   (17)  					//opws leei stin GPIODirModeSetfunction:pairnei times apo 0-31
#define GPIO_PIN_NUMBER_B   (16)  					//prokuptei apo user guide bb
#define BAUD_RATE_115200    (115200)
// #define RX_BUFF_SIZE        (128)
// #define BUFFER_TEXT         (RX_BUFF_SIZE)
//
//GLOBAL VARs
int commandade2get[4] = {0,0,0,0};
//
void main(int argc, char *argv[])
{
//LOCAL VARIABLES
	int PinMuxSetup = 0;
	int UARTCONSOLEBASE = UART_CONSOLE_BASE;
	int FIFOConfigureA = 1;
	int FIFOConfigureB = 1;
	int UARTBAUDRate = BAUD_RATE_115200;
	int UARTREGMODeEnableB = UART_REG_CONFIG_MODE_B;
	int UARTLINECharConfigB = (UART_FRAME_WORD_LENGTH_8 | UART_FRAME_NUM_STB_1);  //edw kindinos 8anatos...
	int UARTLINECharConfigC = UART_PARITY_NONE;
	int UARTBreakCtB = UART_BREAK_COND_DISABLE;
	int UARTOPERATINGModeSelectB = UART16x_OPER_MODE;
//
	calc_pwm_regs pwmValues_reg;
//
	UARTstart_me(PinMuxSetup, UARTCONSOLEBASE,FIFOConfigureA, FIFOConfigureB,UARTBAUDRate,UARTREGMODeEnableB,UARTLINECharConfigB,
                 UARTLINECharConfigC, UARTBreakCtB,UARTOPERATINGModeSelectB);     //initialize + 8ese se leitourgia to UART
// ask and get all the infoz u need:
	UARTprintf("Desired Freq of the produced PWM signal in Hz=\n");
	commandade2get[0] = UARTGetNum();			//commandade2get[0]=entoles[0]=desired_freq
	UARTprintf("commandade2get[0]=desired_freq=%d\n",commandade2get[0]);          //gia epibebaiwsi
	UARTprintf("Desired Duty Cycle (in %%) =\n");
	commandade2get[1] = UARTGetNum();     //commandade2get[1]=desDuty
	UARTprintf("select mode (up=1,down=2,updown=3):\n");
	commandade2get[2] = UARTGetNum();     //commandade2get[2]=countMode
  UARTprintf("active high or low?[1:high,0:low]\n");  //relative 2:duty proportional to high or low to CMPx
  commandade2get[3] = UARTGetNum();                   //commandade2get[3]=activeWHAT
//
	GPIO1ModuleClkConfig();     //paroti exw gpio2[#] den exei problima.                                                  //energopoihsh L3L4 interface clocks+functional rologi tou GPIO
	GpioPinMuxSetup(GPIOP8_34, 6);                                                //pin multiplexing//1ο arg= OFFSE ADD,2O arg= mode tou pin
	GPIODirModeSet(SOC_GPIO_1_REGS, GPIO_PIN_NUMBER_A, GPIO_DIR_OUTPUT);          //set gpio as output
//extra pins pou xreiazomai| posa xreiazomai gia to pwm?=2
	GpioPinMuxSetup(GPIOP8_36, 6);                                                //pin multiplexing//to deutero argument einai to mode tou pin
	GPIODirModeSet(SOC_GPIO_1_REGS, GPIO_PIN_NUMBER_B, GPIO_DIR_OUTPUT);          //set gpio as output
//
	L3L4_driver_init();
	L3L4_clock_domain_open();
	pwmss_driver_init();                                                    	  //enabling interconnection (with processor?)
	//to apo panw einai aparaitito alliws krasarei to EHRPWMClockEnable!!to opoio einai paralogo giati apla orizei flags
	//ta opoia omws den ta checkarei kaneis...MALLON to pwmss_clk_enable exei kati ti
	pwmss_clk_enable(1);                                                          //enabling and configuring the PWMss Module
	UARTprintf("test-reached edw\n");
//
	pwmss_tbclk_enable(1);
//
	UARTprintf("test-reached here2bsdfsdafsdgdsgdg\n");
	EHRPWMClockEnable(SOC_PWMSS1_REGS);                                           //enabling the EHRPWM clock
	UARTprintf("test-reached here2c\n");
//
	pwmValues_reg = custom_pwm(commandade2get);
	UARTprintf("tbclk=%d \n",(int) pwmValues_reg.ftbclk);
	UARTprintf("tbprd_reg=%d \n",(int) pwmValues_reg.tbprd_reg);
	UARTprintf("reg2comp=%d \n",(int) pwmValues_reg.reg2comp);
//
	int arftbclk = (int) pwmValues_reg.ftbclk;
	int artbprd = (int) pwmValues_reg.tbprd_reg;
	int arreg2comp = (int) pwmValues_reg.reg2comp;
 	EHRPWMTimebaseClkConfig(SOC_EPWM_1_REGS, arftbclk, EHRPWM_INP_CLK_FREQ);
 	UARTprintf("test-reached here5\n");
 	EHRPWMPWMOpFreqSet(SOC_EPWM_1_REGS, arftbclk, (int) commandade2get[0],
                        commandade2get[2], EHRPWM_SHADOW_WRITE_ENABLE);
 	UARTprintf("test-reached here6\n");
 	EHRPWMLoadCMPA(SOC_EPWM_1_REGS, arreg2comp, EHRPWM_SHADOW_WRITE_ENABLE, EHRPWM_COMPA_LOAD_COUNT_EQUAL_ZERO_OR_PERIOD,
                    EHRPWM_CMPCTL_OVERWR_SH_FL);
	UARTprintf("test-reached here7\n");
	EHRPWMConfigureAQActionOnA(SOC_EPWM_1_REGS, EHRPWM_AQCTLA_ZRO_EPWMXAHIGH, EHRPWM_AQCTLA_PRD_EPWMXATOGGLE,
                               EHRPWM_AQCTLA_CAU_DONOTHING, EHRPWM_AQCTLA_CAD_DONOTHING, EHRPWM_AQCTLA_CBU_DONOTHING,
                               EHRPWM_AQCTLA_CBD_DONOTHING, EHRPWM_AQSFRC_ACTSFA_DONOTHING);
	UARTprintf("test-reached END\n");

}
