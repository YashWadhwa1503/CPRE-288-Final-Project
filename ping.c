/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping.h"
#include "Timer.h"

// Global shared variables
// Use extern declarations in the header file

volatile uint32_t g_start_time = 0;
volatile uint32_t g_end_time = 0;
volatile state g_state = LOW; // State of ping echo pulse

void ping_init (void){

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();

  // YOUR CODE HERE
    // Configure and enable the timer
    SYSCTL_RCGCTIMER_R |= 0x8;
    SYSCTL_RCGCGPIO_R |= 0x0002;
    while ((SYSCTL_PRGPIO_R & 0b10) == 0) {};
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFFF0FFF) | 0x00007000;
    GPIO_PORTB_DIR_R &= 0x08;
    GPIO_PORTB_AFSEL_R |= 0x08;
    GPIO_PORTB_DEN_R &= ~0x08;
    GPIO_PORTB_DEN_R |= 0x08;
    GPIO_PORTB_AMSEL_R |= 0x08;
    while ((SYSCTL_PRTIMER_R & 0x8) == 0) {};

    TIMER3_TBMR_R |= 0x7;
    TIMER3_TBMR_R &= ~0x10;
    TIMER3_CTL_R |= 0xC00;
    TIMER3_CFG_R |= 0x4;
    TIMER3_CTL_R |= 0x100;
    TIMER3_TBILR_R |= 0xFFFF;
    TIMER3_TBPR_R |= 0xFF;
    TIMER3_IMR_R |= 0x400;

    NVIC_PRI9_R = (NVIC_PRI9_R & 0xFFFFFF0F) | 0x00000020;
    NVIC_EN1_R |= 0x10;

    TIMER3_CTL_R |= 0x100;
}

void ping_trigger (void){
    g_state = LOW;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x100;
    TIMER3_IMR_R &= ~0x400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0x08;

    // YOUR CODE HERE FOR PING TRIGGER/START PULSE
    GPIO_PORTB_DIR_R |= 0x08;
    GPIO_PORTB_DATA_R &= ~0x08;
    GPIO_PORTB_DATA_R |= 0x08;
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= ~0x08;

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0x400;
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0x08;
    TIMER3_IMR_R |= 0x400;
    TIMER3_CTL_R |= 0x100;
}

void TIMER3B_Handler(void){

  // YOUR CODE HERE
  // As needed, go back to review your interrupt handler code for the UART lab.
  // What are the first lines of code in the ISR? Regardless of the device, interrupt handling
  // includes checking the source of the interrupt and clearing the interrupt status bit.
  // Checking the source: test the MIS bit in the MIS register (is the ISR executing
  // because the input capture event happened and interrupts were enabled for that event?
  // Clearing the interrupt: set the ICR bit (so that same event doesn't trigger another interrupt)
  // The rest of the code in the ISR depends on actions needed when the event happens.
    if(TIMER3_MIS_R & 0x400){
        TIMER3_ICR_R |= 0x400;

        if(g_state == LOW){
            g_start_time = TIMER3_TBR_R;
            g_state = HIGH;
        }
        else if(g_state == HIGH){
            //if end value is greater than start value an overflow occurred so we must add the value of the overflow
            g_end_time = TIMER3_TBR_R;
            g_state = DONE;
        }
    }
}

float ping_getDistance (void){
    // YOUR CODE HERE
        return (g_end_time - g_start_time)/73.746;
}
