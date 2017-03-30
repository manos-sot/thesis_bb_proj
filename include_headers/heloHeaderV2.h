// heloHeaderV2.h
/*
** edw brisketai PROTOTYPING ka8ws kai DECLARATION twn functions
** pou xreiazontai sto heloWorld.c
**
*/
//INCLUDES
#include "beaglebone.h"
#include "uart_irda_cir.h"
#include "hw_control_AM335x.h"
#include "soc_AM335x.h"
#include "hw_cm_wkup.h"
#include "hw_cm_per.h"
#include "beaglebone.h"
#include "hw_types.h"

//DEFINITIONS
#define BAUD_RATE_115200                  (115200)
#define UART_CONSOLE_BASE                 (0x44E09000)
//PROTOTYPING
void UartBaudRateSet(int baudRate);
void UARTstart_me(int PinMuxSetup,int UARTCONSOLEBASE,int FIFOConfigureA,int FIFOConfigureB,int UARTBAUDRate,int UARTREGMODeEnableB,
                  int UARTLINECharConfigB,int UARTLINECharConfigC,int UARTBreakCtB,int UARTOPERATINGModeSelectB);
//DECLARATIONS:
void UARTstart_me(int PinMuxSetup,int UARTCONSOLEBASE,int FIFOConfigureA,int FIFOConfigureB,int UARTBAUDRate,int UARTREGMODeEnableB,
                  int UARTLINECharConfigB,int UARTLINECharConfigC,int UARTBreakCtB,int UARTOPERATINGModeSelectB)
  {
  //
  UART0ModuleClkConfig();                         /* Configuring the system clocks for UART0 instance. */
  UARTPinMuxSetup(PinMuxSetup);                             /* Performing the Pin Multiplexing for UART0 instance. */

  //UARTStdioInitExpClk(BAUD_RATE_115200, 1, 1);      AUTI EINAI YPERSUNOLO (MALLON) TWN APO KATW:

  UARTModuleReset(UARTCONSOLEBASE);             /* Performing FIFO configurations. */
  UartFIFOConfigure(FIFOConfigureA, FIFOConfigureB);                        /* Performing Baud Rate settings. */
  /* Setting the TX and RX FIFO Trigger levels as 1. No DMA enabled.|| This API configures the FIFO settings for the UART instance.
  *  Specifically, this does the following configurations:
  *         1> Configures the Transmitter and Receiver FIFO Trigger Level granularity
  *         2> Configures the Transmitter and Receiver FIFO Trigger Level
  *         3> Configures the bits which clear/not clear the TX and RX FIFOs
  *         4> Configures the DMA mode of operation
  *  TRELES PERAITERW ODIGIES STO uart_irda_cir.c sto directory drivers.
  */

  UartBaudRateSet(UARTBAUDRate);
  /* Switching to Configuration Mode B. */
  UARTRegConfigModeEnable(UARTCONSOLEBASE, UARTREGMODeEnableB);
  /* Programming the Line Characteristics. */
  UARTLineCharacConfig(UARTCONSOLEBASE,UARTLINECharConfigB,UARTLINECharConfigC);
  /* Disabling write access to Divisor Latches. */
  UARTDivisorLatchDisable(UARTCONSOLEBASE);
  /* Disabling Break Control. */
  UARTBreakCtl(UARTCONSOLEBASE, UARTBreakCtB);
  /* Switching to UART16x operating mode. */
  UARTOperatingModeSelect(UARTCONSOLEBASE, UARTOPERATINGModeSelectB);
}
