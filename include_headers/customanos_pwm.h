//  name: customanos_pwm.h
/*
//  parameters: 1) desired_freq[Hz] --> epi8umiti frequency tou pwm pou paragetai
                2) desDuty[%]       --> epi8umito duty cycle
    outputs:    8a gurisw 3 pointers: a) pTBPRD: i timi tou TBPRD se Hz
                                      b) pTBCLK: i timi tou TBCLK se Hz
                                      c) pDUTY:  i timi tou COMPARE reg
*/
#include "soc_AM335x.h"                 //SOC_PWMSSx_REGS,SOC_GPIO_x_REGS
#include "pin_mux.h"                    //GPIO_#_## (GPIO_1_29)
#include "ehrpwm.h"
#include "hw_types.h"                   //auto kai to apo panw kaluptoun to bool.MIPWS PREPEI NA MPOUN STO customanos_pwm.h
#include "soc_AM335x.h"                 //auto exei tis d/nseis twn modules (blepe:Base addresses of PWMSS memory mapped registers.)
#include "hw_ehrpwm.h"
#include "gpio_v2.h"
#include "uartStdio.h"


//PROTOTYPING
//void custom_pwm(unsigned int freqnumber, unsigned short int desDuty, int table[4]);
//DEFINES
#define EHRPWM_INP_CLK_FREQ		        (1000000000)	          //input clock freq for the ehrPWM module. i timi den kserw giati einia tosi...
#define SYSCLKOUT                       (EHRPWM_INP_CLK_FREQ)
#define HSP                             (1)
#define CLKDIV                          (14)
//GLOBAL VARIABLES & others
typedef struct
{
  double ftbclk;            //einai Hz
  double tbprd_reg;           //einai ka8aros #
  double reg2comp;            //i timi pou 8a parei to cmpa, ka8aros #double p
}calc_pwm_regs;
//DECLARING
calc_pwm_regs custom_pwm(int entoles[])
{
  double tbclk_reg = 0;           //einai sec
  double tpwm=0;                //einai sec
  double desired_freq = 0;      //einai Hz
  double desDuty = 0;
  //
  calc_pwm_regs pwm_values;
  //
  //initializations
  pwm_values.ftbclk=0;
  pwm_values.tbprd_reg=0;
  pwm_values.reg2comp=0;
  //
  desired_freq = entoles[0];      //entoles[0] = commandade2get[0]
  tpwm = 1/desired_freq;          // einai se sec.
  desDuty = (double) (entoles[1])/100;
  //
  pwm_values.ftbclk =(double) SYSCLKOUT/(HSP*CLKDIV); // einai se Hz, bres struct me tbclk
  tbclk_reg = 1/pwm_values.ftbclk;
  //
  if ((entoles[2] == 1) || (entoles[2] == 2))     //entoles[2]=countMode=up or down
  {
    pwm_values.tbprd_reg = (tpwm*pwm_values.ftbclk)-1;
    //UARTprintf("tpwm*tbclk-1\n");
  }
  else if (entoles[2] == 3)     //if updown
  {
    pwm_values.tbprd_reg = pwm_values.ftbclk/(2*desired_freq);
  }
  else
  {
    UARTprintf("WRONG VALUE \n");
  }
  // DUTY CYCLE
  if (entoles[3] == 1)
  {
    pwm_values.reg2comp = desDuty*pwm_values.tbprd_reg;
    // UARTprintf("1111reg2comp=%d \n",(int) reg2comp);
  }
  else if (entoles[3] == 0)
  {
    pwm_values.reg2comp = pwm_values.tbprd_reg*(1-desDuty);
  }
  else
  {
    UARTprintf("NOT A POSSIBILITY \n");
  }
  //DEBUGGING
  //UARTprintf("tbclk=%d \n",(int) pwm_values.tbclk_reg);

  return pwm_values;
}
