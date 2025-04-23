#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "servo.h"
#include "Timer.h"

void servo_init(void){
    SYSCTL_RCGCTIMER_R |= 0x2;
    SYSCTL_RCGCGPIO_R |= 0x0002;
    while ((SYSCTL_PRGPIO_R & 0b10) == 0) {};
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFF0FFFFF) | 0x00700000;
    GPIO_PORTB_DIR_R &= ~0x20;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_DEN_R &= ~0x20;
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_AMSEL_R |= 0x20;
    while ((SYSCTL_PRTIMER_R & 0x2) == 0) {};

    TIMER1_TBMR_R |= 0x8;
    TIMER1_TBMR_R &= ~0x4;
    TIMER1_TBMR_R |= 0x2;
    TIMER1_CTL_R |= 0x0000;
    TIMER1_CFG_R |= 0x4;
    TIMER1_CTL_R |= 0x100;
    TIMER1_TBILR_R = 0xE200;
    TIMER1_TBPR_R = 0x04;

}

void servo_move(uint16_t degrees){
    TIMER1_CTL_R |= 0x100;

    double seconds = ((0.00555555555*(degrees-15))+1);   //Convert wanted degree to seconds
    double clock = seconds*16000;                   //Convert to hex
    TIMER1_TBMATCHR_R = 320000 - clock;
    TIMER1_TBPMR_R = 0x04;

    //TIMER1_TBMATCHR_R = 0xA380;
    //TIMER1_TBPMR_R = 0x04;

    timer_waitMillis(500);
    TIMER1_CTL_R &= ~0x100;
}
