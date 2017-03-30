/**
 *   \file  pwmss.h
 *
 *   \brief This file contains API prototypes for enabling a PWMSS instance
 *
 *
 */

#ifndef _PWMSS_H_
#define _PWMSS_H_

#include "hw_pwmss.h"
#ifdef __cplusplus
extern "C" {
#endif

/*
 * PWMSS REGISTER FIELDS
 */

#define PWMSS_EHRPWM_CLK_EN_ACK          (0x100u)
#define PWMSS_EHRPWM_CLK_STOP_ACK        (0x200u)
#define PWMSS_EHRPWM_CLK_EN_ACK_SHIFT    (0x08u)
#define PWMSS_EHRPWM_CLK_STOP_ACK_SHIFT  (0x09u)

#define PWMSS_EQEP_CLK_EN                (0x00000010u)
#define PWMSS_EQEP_CLK_STOP              (0x00000020u)
#define PWMSS_EQEP_CLK_EN_ACK_SHIFT      (0x04u)
#define PWMSS_EQEP_CLK_STOP_ACK_SHIFT    (0x05u)

#define PWMSS_ECAP_CLK_EN_ACK            (0x01u)
#define PWMSS_ECAP_CLK_STOP_ACK          (0x02u)
#define PWMSS_ECAP_CLK_EN_ACK_SHIFT      (0x00u)
#define PWMSS_ECAP_CLK_STOP_ACK_SHIFT    (0x01u)



/*
 *   API FUNCTION PROTOTYPES
 */

extern void L3L4_driver_init (void);
extern void pwmss_driver_init (void);

extern int L3L4_clock_domain_open (void);
extern int L3L4_clock_domain_close (void);

extern int pwmss_clk_enable (unsigned int device_id);
extern int pwmss_clk_disable (unsigned int device_id);

extern int pwmss_tbclk_enable (unsigned int device_id);
extern int pwmss_tbclk_disable (unsigned int device_id);

extern int pwmss_eqep_clk_enable (unsigned int device_id);
extern int pwmss_eqep_clk_disable (unsigned int device_id);



#ifdef __cplusplus
}
#endif
#endif
