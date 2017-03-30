/**
 * \file   beaglebone.h
 *
 * \brief  This file contains prototype declarations of functions which 
 *         performs EVM configurations.
 */
#ifndef _BEALGEBONE_H_
#define _BEAGLEBONE_H_

#ifdef __cplusplus
extern "C" {
#endif



/*
 * Macro Definitions
 */
 
#define CTRL_NUM_IOPAD_REGS                 (211)

#define PRINT_WRG(x)                        UARTPuts ("\r\n|WARNING|", -1);\
                                            UARTPuts (x, -1); \
                                            UARTPuts ("|\r\n", -1);            
                                 
#define PRINT_ERR(x)                        UARTPuts ("\r\n|ERROR|", -1); \
                                            UARTPuts (x, -1); \
                                            UARTPuts ("|\r\n", -1);                                                                   
                                 
#define PRINT_RESULT_PASS()                 UARTPuts ("\r\n|RESULT|PASS|\r\n", -1);

#define PRINT_RESULT_FAIL()                 UARTPuts ("\r\n|RESULT|FAIL|\r\n", -1);


/*
 * Structure to store the control register context. More registers
 * can be added to this if need to be saved.
 */

/*
typedef struct 
CTRL_REGCONTEXT_STRUCT
{
    unsigned int pwmssctrl;
    unsigned int gmiisel;
    unsigned int ioPad[CTRL_NUM_IOPAD_REGS];
}
ctrlRegContext_t;
*/


/*
 * BeagleBone Setup and COnfiguration API
 */
/* UART */
extern void UART0ModuleClkConfig(void);
extern void UARTPinMuxSetup(unsigned int instanceNum);

/* CPSW */
extern void CPSWPinMuxSetup(void);
extern void CPSWClkEnable(void);

/* EDMA */
extern void EDMAModuleClkConfig(void);

/* TIMER */
extern void WatchdogTimer1ModuleClkConfig(void);

extern void DMTimer2ModuleClkConfig(void);
extern void DMTimer3ModuleClkConfig(void);
extern void DMTimer4ModuleClkConfig(void);
extern void DMTimer5ModuleClkConfig(void);
extern void DMTimer6ModuleClkConfig(void);
extern void DMTimer7ModuleClkConfig(void);

/* EVM */
extern void EVMMACAddrGet(unsigned int addrIdx, unsigned char *macAddr);
extern void EVMPortMIIModeSelect(void);

/* RTC */
extern unsigned int RTCRevisionInfoGet(void);
extern void RTCModuleClkConfig(void);

/* MMC */
extern void HSMMCSDModuleClkConfig(void);
extern void HSMMCSDPinMuxSetup(void);

/* I2C */
extern void I2C0ModuleClkConfig(void);
extern void I2C1ModuleClkConfig(void);
extern void I2CPinMuxSetup(unsigned int instance);

/* PWMSS */
extern void eqep_1_pinmux_setup(void);
extern void eqep_2_pinmux_setup(void);
extern void ehrpwm_1_pinmux_setup(void);
extern void ehrpwm_2_pinmux_setup(void);

/* GPIO */
extern void GPIO1ModuleClkConfig(void);
extern void GPIO0ModuleClkConfig(void);
extern void GpioModuleEnable(unsigned int module_addr);

extern void GpioPinMuxSetup(unsigned int offsetAddr, unsigned int padConfValue);
extern void GPIO1Pin23PinMuxSetup(void);

extern void gpio_dir_1_setup (void);
extern void gpio_dir_2_setup (void);
extern void gpio_hall_1_setup (void);
extern void gpio_hall_2_setup (void);
extern void gpio_killswitch_setup (unsigned int debounceTime);



#ifdef __cplusplus
}
#endif

#endif



/*
 * EOF
 */
