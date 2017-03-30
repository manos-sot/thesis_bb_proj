/**
 *  \file   pwmss.c
 *
 *  \brief  PWMSS driver.
 */


/*
* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* Instructions for Use:
 *
 * In order to successfully use any of the modules contained within
 * the PWMSS, the appropriate functional and interface clock
 * domains must be appropriately managed.
 *
 * Any call to *_open() fucntion will result in the testing of the
 * related *_status or *_clk_status data.
 *
 * These may take the folling values:
 *      i. -1, if the driver has not been initialized (code has just been loaded).
 *     ii. 0, if the driver has been intialzied and/or the device is not active.
 *    iii. 1, if the device has been opened previously and is currently active.
 *
 * When running initial setup code, pwmss_driver_init() MUST be called first to
 * setup the status data (set these to zero). After this, any calls to the *_open()
 * or *_close() fucntions can be done.
 *
 */


/* device headers */
#include "soc_AM335x.h"
#include "hw_types.h"
#include "hw_control_AM335x.h"
#include "hw_pwmss.h"
#include "hw_cm_per.h"

/* driver headers */
#include "pwmss.h"
#include "hw_ehrpwm.h"

/*
 * Driver Defines
 */

#define STATUS_INIT                -1
#define PWMSS_DEVICE_NUM            3

/*
ta parakatw defines ta xw balei egw...
*/
#define CONTROL_PWMSS_CTRL_PWMMS0_TBCLKDIS  (0x01)      //den uparxei disable...page 1403 sto namual(rev L)
#define CONTROL_PWMSS_CTRL_PWMMS1_TBCLKDIS  (0x02)
#define CONTROL_PWMSS_CTRL_PWMMS2_TBCLKDIS  (0x03)
#define CM_PER_EPWMSS1_CLKCTRL_MODULEMODE_DISABLE  (0x00)  //page 1198 sto manual..
#define CM_PER_EPWMSS2_CLKCTRL_MODULEMODE_DISABLE  (0x00)
/*
 * Driver Internal Data
 */

//! must move this to its own .c file scope
static int volatile interconnects_status = -1;

static int volatile pwmss_status[PWMSS_DEVICE_NUM] = {-1,-1,-1};

static int volatile pwmss_ehrpwm_tbclks_status[PWMSS_DEVICE_NUM] = {-1,-1,-1};

static int volatile pwmss_eqep_clk_status[PWMSS_DEVICE_NUM] = {-1,-1,-1};

//! add the respective ecap statuses here...


/*
 * Static Driver Function Prototypes
 */

static unsigned int _L3L4_clock_domain_status_get (void);

static void _L3L4_clock_domain_status_set (unsigned int value);

static unsigned int _pwmss_clk_status_get (unsigned int device_id);

static void _pwmss_clk_status_set (unsigned int device_id, unsigned int value);

static unsigned int _pwmss_ehrpwm_tbclk_status_get (unsigned int device_id);

static void _pwmss_ehrpwm_tbclk_status_set (unsigned int device_id, unsigned int value);

static unsigned int _pwmss_eqep_clk_status_get (unsigned int device_id);

static void _pwmss_eqep_clk_status_set (unsigned int device_id, unsigned int value);

static void _L3L4_clock_domain_open (void);

static void _L3L4_clock_domain_close (void);

static void _pwmss_clk_enable (unsigned int device_id);

static void _pwmss_clk_disable (unsigned int device_id);

static void _pwmss_eqep_clk_enable (unsigned int device_id);

static void _pwmss_eqep_clk_disable (unsigned int device_id);


/*
 *  Public Driver Functions
 */

void L3L4_driver_init (void)
{
    _L3L4_clock_domain_status_set(0);
}

void pwmss_driver_init (void)
{
    _pwmss_clk_status_set(0,0);
    _pwmss_clk_status_set(1,0);
    _pwmss_clk_status_set(2,0);

    _pwmss_ehrpwm_tbclk_status_set(0,0);
    _pwmss_ehrpwm_tbclk_status_set(1,0);
    _pwmss_ehrpwm_tbclk_status_set(2,0);

    _pwmss_eqep_clk_status_set(0,0);
    _pwmss_eqep_clk_status_set(1,0);
    _pwmss_eqep_clk_status_set(2,0);
}


/**
 * \brief  This functions enables/opens the functional and interface clocks for the L3/L4
 *          peripheral & system devices intercconnect NoC.
 *
 **/
int
L3L4_clock_domain_open (void)
{
    if (_L3L4_clock_domain_status_get() != 0)
    {
        return 1;
    }

    _L3L4_clock_domain_open();

    _L3L4_clock_domain_status_set(1);

    return 0;
}

int
L3L4_clock_domain_close (void)
{
    if (_L3L4_clock_domain_status_get() == 0)
    {
        return 1;
    }

    _L3L4_clock_domain_close();

    _L3L4_clock_domain_status_set(0);

    return 0;
}


int
pwmss_clk_enable (unsigned int device_id)
{
    if (device_id > 2)
    {
        return -1;
    }

    if (_pwmss_clk_status_get(device_id) != 0)
    {
        return 1;
    }

    _pwmss_clk_enable(device_id);

    _pwmss_clk_status_set(device_id, 1);

    return 0;
}

int
pwmss_clk_disable (unsigned int device_id)
{
    if (device_id > 2)
    {
        return -1;
    }

    if (_pwmss_clk_status_get(device_id) == 0)
    {
        return 1;
    }

    _pwmss_clk_disable(device_id);

    _pwmss_clk_status_set(device_id, 0);

    return 0;
}


/**
 * \brief  This function Enables TBCLK(Time Base Clock) for specific
 *          EPWM instance of pwmsubsystem.
 *
 * \param   instance  It is the instance number of EPWM of pwmsubsystem.
 *
 **/
int
pwmss_tbclk_enable (unsigned int device_id)
{
    if(_pwmss_ehrpwm_tbclk_status_get(device_id) != 0)
    {
        return 1;
    }

    switch (device_id)
    {
        case 0:
            HWREG(SOC_CONTROL_REGS + CONTROL_PWMSS_CTRL) |= CONTROL_PWMSS_CTRL_PWMSS0_TBCLKEN;
            break;

        case 1:
            HWREG(SOC_CONTROL_REGS + CONTROL_PWMSS_CTRL) |= CONTROL_PWMSS_CTRL_PWMMS1_TBCLKEN;
            break;

        case 2:
            HWREG(SOC_CONTROL_REGS + CONTROL_PWMSS_CTRL) |= CONTROL_PWMSS_CTRL_PWMSS2_TBCLKEN;
            break;

        default:
            return -1;
    }

    _pwmss_ehrpwm_tbclk_status_set(device_id, 1);

    return 0;
}


/**
 * \brief   This function Disables TBCLK(Time Base Clock) for specific
 *          EPWM instance of pwmsubsystem.
 *
 * \param   instance  It is the instance number of EPWM of pwmsubsystem.
 *
 **/
int
pwmss_tbclk_disable (unsigned int device_id)
{
    if(_pwmss_ehrpwm_tbclk_status_get(device_id) == 0)
    {
        return 1;
    }

    switch (device_id)
    {
        case 0:
            HWREG(SOC_CONTROL_REGS + CONTROL_PWMSS_CTRL) &= CONTROL_PWMSS_CTRL_PWMMS0_TBCLKDIS;
            break;

        case 1:
            HWREG(SOC_CONTROL_REGS + CONTROL_PWMSS_CTRL) &= CONTROL_PWMSS_CTRL_PWMMS1_TBCLKDIS;
            break;

        case 2:
            HWREG(SOC_CONTROL_REGS + CONTROL_PWMSS_CTRL) &= CONTROL_PWMSS_CTRL_PWMMS2_TBCLKDIS;
            break;

        default:
            return -1;
    }

    _pwmss_ehrpwm_tbclk_status_set(device_id, 0);

    return 0;
}


/**
 * \brief       enables the \device_id instance of the PWMSS module.
 *
 * \param       \device_id.
 *
 * \return      None.
 *
 * \note        this probably does not need to be called, since on reset the desired
 *              values are already set.
 **/
int
pwmss_eqep_clk_enable (unsigned int device_id)
{
    if (device_id > 2)
    {
        return -1;
    }

    if (_pwmss_eqep_clk_status_get(device_id) != 0)
    {
        return 1;
    }

    _pwmss_eqep_clk_enable(device_id);

    _pwmss_eqep_clk_status_set(device_id, 1);

    return 0;
}


/**
 * \brief       disables the \device_id instance of the PWMSS module.
 *
 * \param       \device_id.
 *
 * \return      None.
 *
 * \note        none.
 *
 **/
int
pwmss_eqep_clk_disable (unsigned int device_id)
{
    if (device_id > 2)
    {
        return -1;
    }

    if(_pwmss_eqep_clk_status_get(device_id) == 0)
    {
        return 1;
    }

    _pwmss_eqep_clk_disable(device_id);

    _pwmss_eqep_clk_status_set(device_id, 0);

    return 0;
}



/*
 * Static Driver Function Definitions
 */

static unsigned int
_L3L4_clock_domain_status_get (void)
{
    return interconnects_status;
}

static void
_L3L4_clock_domain_status_set (unsigned int value)
{
    interconnects_status = value;
}


static unsigned int
_pwmss_clk_status_get (unsigned int device_id)
{
    return pwmss_status[device_id];
}

static void
_pwmss_clk_status_set (unsigned int device_id, unsigned int value)
{
    pwmss_status[device_id] = value;
}


static unsigned int
_pwmss_ehrpwm_tbclk_status_get (unsigned int device_id)
{
    return pwmss_ehrpwm_tbclks_status[device_id];
}

static void
_pwmss_ehrpwm_tbclk_status_set (unsigned int device_id, unsigned int value)
{
    pwmss_ehrpwm_tbclks_status[device_id] = value;
}


static unsigned int
_pwmss_eqep_clk_status_get (unsigned int device_id)
{
    return pwmss_eqep_clk_status[device_id];
}

static void
_pwmss_eqep_clk_status_set (unsigned int device_id, unsigned int value)
{
    pwmss_eqep_clk_status[device_id] = value;
}



/**
 * \brief   This API enables the CM_PER_L4 and CM_PER_L3 clock domains that are
 *          necessary for all PWMSS and other relative device modules to function.
 *
 * \param   None.
 *
 * \return  None.
 *
 **/
static void
_L3L4_clock_domain_open (void)
{
    HWREG(SOC_PRCM_REGS + CM_PER_L3S_CLKSTCTRL) |=
                             CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_PRCM_REGS + CM_PER_L3S_CLKSTCTRL) &
     CM_PER_L3S_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3S_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_PRCM_REGS + CM_PER_L3_CLKSTCTRL) |=
                             CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_PRCM_REGS + CM_PER_L3_CLKSTCTRL) &
     CM_PER_L3_CLKSTCTRL_CLKTRCTRL) != CM_PER_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_PRCM_REGS + CM_PER_L3_INSTR_CLKCTRL) |=
                             CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_PRCM_REGS + CM_PER_L3_INSTR_CLKCTRL) &
                               CM_PER_L3_INSTR_CLKCTRL_MODULEMODE) !=
                                   CM_PER_L3_INSTR_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_PRCM_REGS + CM_PER_L3_CLKCTRL) |=
                             CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_PRCM_REGS + CM_PER_L3_CLKCTRL) &
        CM_PER_L3_CLKCTRL_MODULEMODE) != CM_PER_L3_CLKCTRL_MODULEMODE_ENABLE);

    HWREG(SOC_PRCM_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) |=
                             CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_PRCM_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
                              CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL) !=
                                CM_PER_OCPWP_L3_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_PRCM_REGS + CM_PER_L4LS_CLKSTCTRL) |=
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    while((HWREG(SOC_PRCM_REGS + CM_PER_L4LS_CLKSTCTRL) &
                             CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL) !=
                               CM_PER_L4LS_CLKSTCTRL_CLKTRCTRL_SW_WKUP);

    HWREG(SOC_PRCM_REGS + CM_PER_L4LS_CLKCTRL) |=
                             CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE;


    while((HWREG(SOC_PRCM_REGS + CM_PER_L4LS_CLKCTRL) &
      CM_PER_L4LS_CLKCTRL_MODULEMODE) != CM_PER_L4LS_CLKCTRL_MODULEMODE_ENABLE);

    while(!(HWREG(SOC_PRCM_REGS + CM_PER_L3S_CLKSTCTRL) &
            CM_PER_L3S_CLKSTCTRL_CLKACTIVITY_L3S_GCLK));

    while(!(HWREG(SOC_PRCM_REGS + CM_PER_L3_CLKSTCTRL) &
            CM_PER_L3_CLKSTCTRL_CLKACTIVITY_L3_GCLK));

    while(!(HWREG(SOC_PRCM_REGS + CM_PER_OCPWP_L3_CLKSTCTRL) &
           (CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L3_GCLK |
            CM_PER_OCPWP_L3_CLKSTCTRL_CLKACTIVITY_OCPWP_L4_GCLK)));

    while(!(HWREG(SOC_PRCM_REGS + CM_PER_L4LS_CLKSTCTRL) &
           (CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_L4LS_GCLK )));

}


static void
_L3L4_clock_domain_close (void)
{
    ;//! complete this...
}


/**
 * \brief   This API configures the interface and functional clocks to the
 *          desired instance of the PWMSS module.
 *
 * \param   \device_id is the instance number of the module
 * \param
 *
 * \return  None.
 *
 **/
static void
_pwmss_clk_enable (unsigned int device_id)
{
    if(0 == device_id)
    {
        HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS0_CLKCTRL) |=
            CM_PER_EPWMSS0_CLKCTRL_MODULEMODE_ENABLE;

        while(CM_PER_EPWMSS0_CLKCTRL_MODULEMODE_ENABLE !=
              (HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS0_CLKCTRL) &
               CM_PER_EPWMSS0_CLKCTRL_MODULEMODE));

        while((CM_PER_EPWMSS0_CLKCTRL_IDLEST_FUNC <<
               CM_PER_EPWMSS0_CLKCTRL_IDLEST_SHIFT) !=
              (HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS0_CLKCTRL) &
               CM_PER_EPWMSS0_CLKCTRL_IDLEST));

    }

    else if(1 == device_id)
    {
        HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS1_CLKCTRL) |=
            CM_PER_EPWMSS1_CLKCTRL_MODULEMODE_ENABLE;

        while(CM_PER_EPWMSS1_CLKCTRL_MODULEMODE_ENABLE !=
              (HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS1_CLKCTRL) &
               CM_PER_EPWMSS1_CLKCTRL_MODULEMODE));

        while((CM_PER_EPWMSS1_CLKCTRL_IDLEST_FUNC <<
               CM_PER_EPWMSS1_CLKCTRL_IDLEST_SHIFT) !=
               (HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS1_CLKCTRL) &
               CM_PER_EPWMSS1_CLKCTRL_IDLEST));

    }

    else if(2 == device_id)
    {
        HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS2_CLKCTRL) |=
            CM_PER_EPWMSS2_CLKCTRL_MODULEMODE_ENABLE;

        while(CM_PER_EPWMSS2_CLKCTRL_MODULEMODE_ENABLE !=
              (HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS2_CLKCTRL) &
               CM_PER_EPWMSS2_CLKCTRL_MODULEMODE));

        while((CM_PER_EPWMSS2_CLKCTRL_IDLEST_FUNC <<
               CM_PER_EPWMSS2_CLKCTRL_IDLEST_SHIFT) !=
               (HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS2_CLKCTRL) &
                CM_PER_EPWMSS2_CLKCTRL_IDLEST));
    }

    else
    {
        ;
    }
}


static void
_pwmss_clk_disable (unsigned int device_id)
{
    if(0 == device_id)
    {
        HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS0_CLKCTRL) |=
            CM_PER_EPWMSS0_CLKCTRL_MODULEMODE_DISABLE;

        while(CM_PER_EPWMSS0_CLKCTRL_MODULEMODE_DISABLE !=
              (HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS0_CLKCTRL) &
               CM_PER_EPWMSS0_CLKCTRL_MODULEMODE));

    }

    else if(1 == device_id)
    {
        HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS1_CLKCTRL) |=
            CM_PER_EPWMSS1_CLKCTRL_MODULEMODE_DISABLE;

        while(CM_PER_EPWMSS1_CLKCTRL_MODULEMODE_DISABLE !=
              (HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS1_CLKCTRL) &
               CM_PER_EPWMSS1_CLKCTRL_MODULEMODE));
    }

    else if(2 == device_id)
    {
        HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS2_CLKCTRL) |=
            CM_PER_EPWMSS2_CLKCTRL_MODULEMODE_DISABLE;

        while(CM_PER_EPWMSS2_CLKCTRL_MODULEMODE_DISABLE !=
              (HWREG(SOC_PRCM_REGS + CM_PER_EPWMSS2_CLKCTRL) &
               CM_PER_EPWMSS2_CLKCTRL_MODULEMODE));
    }

    else
    {
        ;
    }
}


static void
_pwmss_eqep_clk_enable (unsigned int device_id)
{
    if (0 == device_id)
    {
        HWREG(SOC_PWMSS0_REGS + PWMSS_CLOCK_CONFIG) &= 0xFFFFFFEFu;
        HWREG(SOC_PWMSS0_REGS + PWMSS_CLOCK_CONFIG) |= PWMSS_EQEP_CLK_EN;
        while(!(HWREG(SOC_PWMSS0_REGS + PWMSS_CLOCK_CONFIG) & PWMSS_EQEP_CLK_EN));
    }

    else if (1 == device_id)
    {
        HWREG(SOC_PWMSS1_REGS + PWMSS_CLOCK_CONFIG) &= 0xFFFFFFEFu;
        HWREG(SOC_PWMSS1_REGS + PWMSS_CLOCK_CONFIG) |= PWMSS_EQEP_CLK_EN;
        while(!(HWREG(SOC_PWMSS1_REGS + PWMSS_CLOCK_CONFIG) & PWMSS_EQEP_CLK_EN));
    }

    else if (2 == device_id)
    {
        HWREG(SOC_PWMSS2_REGS + PWMSS_CLOCK_CONFIG) &= 0xFFFFFFEFu;
        HWREG(SOC_PWMSS2_REGS + PWMSS_CLOCK_CONFIG) |= PWMSS_EQEP_CLK_EN;
        while(!(HWREG(SOC_PWMSS2_REGS + PWMSS_CLOCK_CONFIG) & PWMSS_EQEP_CLK_EN));
    }

    else
    {
        ;
    }
}

static void
_pwmss_eqep_clk_disable (unsigned int device_id)
{
    if(0 == device_id)
    {
        HWREG(SOC_PWMSS0_REGS + PWMSS_CLOCK_CONFIG) &= 0xFFFFFFEFu;
        while(!(HWREG(SOC_PWMSS0_REGS + PWMSS_CLOCK_CONFIG) & 0xFFFFFFEFu));
    }

    else if(1 == device_id)
    {
        HWREG(SOC_PWMSS1_REGS + PWMSS_CLOCK_CONFIG) &= 0xFFFFFFEFu;
        while(!(HWREG(SOC_PWMSS1_REGS + PWMSS_CLOCK_CONFIG) & 0xFFFFFFEFu));
    }

    else if(2 == device_id)
    {
        HWREG(SOC_PWMSS2_REGS + PWMSS_CLOCK_CONFIG) &= 0xFFFFFFEFu;
        while(!(HWREG(SOC_PWMSS2_REGS + PWMSS_CLOCK_CONFIG) & 0xFFFFFFEFu));
    }

    else
    {
        ;
    }
}



/*
 * EOF
 */
