/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== capturepwmdisplay.c ========
 */
/* Driver Header files */
#include <ti/drivers/Capture.h>
#include <ti/display/Display.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>



/* Example/Board Header files */
#include "Board.h"

/* Callback used for blinking LED on timer completion */
void captureCallback(Capture_Handle handle, uint32_t interval);

/* Local Variables */
extern Display_Handle display;
volatile uint32_t curInterval;
static SemaphoreP_Handle captureSem;

/* Global variable*/
SemaphoreP_Params semParams;
Capture_Params captureParams;
Capture_Handle capture;
PWM_Params pwmParams;
PWM_Handle pwm0, pwm1;

/*
 *  ======== secondThread ========
 *  Task that will generate pwm signal, one train of 20ms every 100ms
 */
void *generateSignal(void *arg0)
{
    /* units in microseconds */
    uint32_t   pwmPeriod = 10000;
    uint32_t   duty = 5000;

    /* Sleep time in microseconds */
    uint32_t   time_sleep = 80000;
    uint32_t   time_periode = 20000;

    PWM_init();

    /* PWM Params init */
    PWM_Params_init(&pwmParams);
    pwmParams.dutyUnits = PWM_DUTY_US;
    pwmParams.dutyValue = 0;
    pwmParams.periodUnits = PWM_PERIOD_US;
    pwmParams.periodValue = pwmPeriod;

    /* Open PWM0 */
    pwm0 = PWM_open(Board_PWM0, &pwmParams);

    if (!pwm0)
    {
        Display_printf(display, 0, 0, "Failed to initialized PWM0.\n");
        while (1);
    }
    PWM_start(pwm0);

    /* Open PWM1 */
    //pwm1 = PWM_open(Board_PWM1, &pwmParams);

    //if (!pwm1)
    //{
    //    Display_printf(display, 0, 0, "Failed to initialized PWM1.\n");
    //    while (1);
    //}
    //PWM_start(pwm1);

    //Display_printf(display, 0, 0, "About to Generate!\n");


    while(1) {
        PWM_setDuty(pwm0, duty);
        usleep(time_periode);

        PWM_setDuty(pwm0, 0);
        usleep(time_sleep);
    }
}



/*
 *  ======== mainThread ========
 *  Task that will capture two rising edges and output the time between the
 *  two edges
 */
void *mainThread(void *arg0)
{
    /* Driver Init Functions */
    Capture_init();

    /* Semaphore to wait for capture data */
    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_BINARY;
    captureSem = SemaphoreP_create(0, &semParams);

    if (captureSem == NULL)
    {
        Display_printf(display, 0, 0, "Could not allocate semaphore!\n");
        while(1);
    }

    /* Setting up the Capture driver to detect two rising edges and report
     * the result in microseconds
     */
    Capture_Params_init(&captureParams);
    captureParams.mode = Capture_RISING_EDGE;
    captureParams.periodUnit = Capture_PERIOD_US;
    captureParams.callbackFxn = captureCallback;

    capture = Capture_open(Board_CAPTURE8, &captureParams);
    if (capture == NULL)
    {
        Display_printf(display, 0, 0, "Failed to initialized Capture!\n");
        while(1);
    }

    Display_printf(display, 0, 0, "About to Capture!\n");

    Capture_start(capture);

    while(1)
    {
        /* The value printed should be close to the period of the pwm */
        SemaphoreP_pend(captureSem, SemaphoreP_WAIT_FOREVER);
        Display_printf(display, 0, 0,
                "Period: %d", curInterval);
    }
}

/* Callback function that displays the interval */
void captureCallback(Capture_Handle handle, uint32_t interval)
{
    curInterval = interval;
    SemaphoreP_post(captureSem);
}
