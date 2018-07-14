/*
 * File:   main.c
 * Author: Max
 *
 * Created on 14 ??????? 2561, 19:19 ?.
 */

#define _XTAL_FREQ 32000000
// #define _XTAL_FREQ 16000000

#include <xc.h>
#include "config.h"
#include "uart.h"

void delay_ms(unsigned int ms) {
    while(ms--){
        __delay_ms(1);
    }
}

#define BAND 9600

int main(int argc, char** argv) {
    
    // Set clock to 32MHz
    OSCCONbits.SCS  = 0b00; // Use INC osc
    OSCCONbits.IRCF = 0b1110; // Use INC 8MHz
    OSCCONbits.SPLLEN = 1; // Use x4 PULL
    while(!OSCSTATbits.HFIOFS);
    
    /*
    // Set clock to 16MHz
    OSCCONbits.SCS  = 0b10; // Use INC osc
    OSCCONbits.IRCF = 0b1111; // Use INC 16MHz
    */
    
    uart_setup(51, 1);

    while(1) {
        while (uart_available() > 0) {
            uart_write(uart_read());
        }
        delay_ms(10);
    }

    return 0;
}
