// PWM Driver - LPC176x - Single edge only, does not support double edge PWM
//
// Copyright 2017 Brett Fleming
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "pwm_driver.h"
#include "LPC17xx.h"

#include "grbl.h"

//LPC_PWM1_BASE
//LPC_PWM1 is of type LPC_PWM_TypeDef at Address LPC_PWM1_BASE

/*
    Theory of operation: Initialize, start running, change width any time while running.

    pwm_init(PWM1_CH1, 1000, 0);
    pwm_enable(PWM1_CH1);
    pwm_set_width(250);
    sleep(1)
    pwm_set_width(500);
    sleep(1)
    pwm_set_width(999);
    sleep(1)
    pwm_disable(PWM1_CH1);
*/


const PWM_Channel_Config PWM1_CH1 = {
    &(LPC_PWM1->MR1),   //Match Register
    (1 << 9),           //PWM Enable
    (1 << 1)            //Latch Enable Register
};

const PWM_Channel_Config PWM1_CH2 = {
    &(LPC_PWM1->MR2),   //Match Register
    (1 << 10),          //PWM Enable
    (1 << 2)            //Latch Enable Register
};

const PWM_Channel_Config PWM1_CH3 = {
    &(LPC_PWM1->MR3),   //Match Register
    (1 << 11),          //PWM Enable
    (1 << 3)            //Latch Enable Register
};

const PWM_Channel_Config PWM1_CH4 = {
    &(LPC_PWM1->MR4),   //Match Register
    (1 << 12),          //PWM Enable
    (1 << 4)            //Latch Enable Register
};

const PWM_Channel_Config PWM1_CH5 = {
    &(LPC_PWM1->MR5),   //Match Register
    (1 << 13),          //PWM Enable
    (1 << 5)            //Latch Enable Register
};

const PWM_Channel_Config PWM1_CH6 = {
    &(LPC_PWM1->MR6),   //Match Register
    (1 << 14),          //PWM Enable
    (1 << 6)            //Latch Enable Register
};




//Smoothie board uses P2.4 and P2.5 as PWM outputs

//UM10360 LPC17xx Chapter 24 - Pulse Width Modulation

//
// channel - Which of the 6 PWM channels to init.
// period - what is the overall period, in PCLK cycles, for ALL PWMs (not just the provided one)
//
void pwm_init(PWM_Channel_Config* channel, uint32_t period, uint32_t width) {
    //Power up PCONP

    //Match Control - Continous operation (no interrupts, no timer reset)
    LPC_PWM1->MCR = 0x0000;

    //PWM Control Register - Single Edge (0 for bits 2-6) mode for all PWMs
    LPC_PWM1->PCR &= 0xFF00;
    
    //PWM Control Register - Disable output for channel
    LPC_PWM1->PCR &= ~channel->PCR_Enable_Mask;

    //Disable Capture
    LPC_PWM1->CCR = 0x0000;

    //PWM Timer Control Register - Counter Enable, PWM Enable
    LPC_PWM1->TCR = (1 << 0) | (1 << 3);

    //PWM Counter Control Register - Use Timer Mode (Counter Mode is for external clocks, not PWM)
    LPC_PWM1->CTCR = 0x0000;

    pwm_set_period(period);
    pwm_set_width(channel, width);
}

void pwm_set_period(uint32_t period) {
    LPC_PWM1->MR0 = period;

    //If we are running, this will make the MRx register on the next cycle
    LPC_PWM1->LER = (1 << 0);
}

void pwm_set_width(PWM_Channel_Config* channel, uint32_t width) {
    *(channel->MRn) = width;

    //If we are running, this will make the MRx register on the next cycle
    LPC_PWM1->LER |= channel->LER_Enable_Mask;
}

void pwm_enable(PWM_Channel_Config* channel) {
    //PWM Control Register - Enable output for channel
    LPC_PWM1->PCR |= channel->PCR_Enable_Mask;
}

void pwm_disable(PWM_Channel_Config* channel) {
    //PWM Control Register - Disable output for channel
    LPC_PWM1->PCR &= ~channel->PCR_Enable_Mask;
}

