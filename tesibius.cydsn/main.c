/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */
#include "buttons_low.h"
#include "controls.h"
#include "knobs_low.h"
#include "leds.h"
#include "task.h"
#include <FreeRTOS.h>
#include <project.h>
#include <stdio.h>

extern void
xPortPendSVHandler(void);
extern void
xPortSysTickHandler(void);
extern void
vPortSVCHandler(void);

#define CORTEX_INTERRUPT_BASE (16)
void
setupFreeRTOS()
{
    /* Handler for Cortex Supervisor Call (SVC, formerly SWI) - address 11 */
    CyIntSetSysVector(CORTEX_INTERRUPT_BASE + SVCall_IRQn,
                      (cyisraddress)vPortSVCHandler);

    /* Handler for Cortex PendSV Call - address 14 */
    CyIntSetSysVector(CORTEX_INTERRUPT_BASE + PendSV_IRQn,
                      (cyisraddress)xPortPendSVHandler);

    /* Handler for Cortex SYSTICK - address 15 */
    CyIntSetSysVector(CORTEX_INTERRUPT_BASE + SysTick_IRQn,
                      (cyisraddress)xPortSysTickHandler);
}

StaticTask_t xLEDTaskCB;
#define TASK_STACK_DEPTH 80
StackType_t xLEDTaskStack[TASK_STACK_DEPTH];

void
LED_Task(void* arg)
{
    (void)arg;

    int led = 0x01;
    while (1)
    {
        leds_set(led);
        led ^= 0x01;
        vTaskDelay(500);
    }
}

/* configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide
an implementation of vApplicationGetIdleTaskMemory() to provide the memory that
is used by the Idle task. */
void
vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
                              StackType_t** ppxIdleTaskStackBuffer,
                              uint32_t* pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be
    allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so
the application must provide an implementation of
vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void
vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
                               StackType_t** ppxTimerTaskStackBuffer,
                               uint32_t* pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be
    allocated on the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

int
main()
{
    CyGlobalIntEnable; /* Enable global interrupts */
    uart_debug_Start();
    buttons_init();
    controls_init();
    leds_set(0);
    knobs_start();

    setupFreeRTOS();
    /* Create LED task, which will control the intensity of the LEDs */
    xTaskCreateStatic(LED_Task,         /* Task function */
                      "LED Blink",      /* Task name (string) */
                      TASK_STACK_DEPTH, /* Task stack depth*/
                      0,                /* No param passed to task function */
                      1,                /* Low priority */
                      xLEDTaskStack,
                      &xLEDTaskCB); /* Not using the task handle */
    vTaskStartScheduler();
}
