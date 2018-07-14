/* 
 * File:   uart.h
 * Author: Sonthaya Nongnuch
 *
 * Created on 14 ??????? 2561, 23:11 ?.
 */

#ifndef UART_H
#define	UART_H

extern void uart_setup(int, char);
extern void uart_write(unsigned char) ;
extern int uart_available() ;
extern unsigned char uart_read() ;

#endif	/* UART_H */

