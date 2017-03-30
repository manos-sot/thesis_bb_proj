/**
 * \file   uartStdio.h
 *
 * \brief  This file contains the prototypes of the functions present in
 *         utils/src/uartStdio.c
 */

#ifndef _UARTSTDIO_H_
#define _UARTSTDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
**                    MACRO DEFINITIONS
****************************************************************************/
#define MAX_DECIMAL_DIGITS		(6) //(12)


/****************************************************************************
**                    FUNCTION PROTOTYPES
****************************************************************************/

extern unsigned int UARTPuts(char *pTxBuffer, int numBytesToWrite);
extern unsigned int UARTGets(char *pRxBuffer, int numBytesToRead);
extern unsigned int UARTwrite(const char *pcBuf, unsigned int len);
extern void UARTprintf(const char *pcString, ...);
extern void UARTPutHexNum(unsigned int hexValue);
extern void UARTPutc(unsigned char byteTx);
extern unsigned int UARTGetHexNum(void);
extern unsigned char UARTGetc(void);
extern void UARTPutNum(int value);
extern void UARTPutDouble(double value);
extern void UARTStdioInit(void);
extern int UARTGetNum(void);

#ifdef __cplusplus
}
#endif
#endif
