/* 
 * File:   uart.c
 * Author: Sonthaya Nongnuch
 *
 * Created on 14 ??????? 2561, 23:11 ?.
 */

#ifndef UART_C
#define UART_C

#include <xc.h>
#include <stdio.h>
#include <string.h>

// Queue
#define BUFFER_SIZE 10
typedef struct {
	int count;
	int rear;
	int front;
	unsigned char data[BUFFER_SIZE];
} Queue;

void Enqueue(Queue *q, unsigned char data) {
	if (q->count == BUFFER_SIZE) return;

	q->rear = q->rear == BUFFER_SIZE ? 0 : q->rear + 1;
	q->data[q->rear] = data;
	q->count++;
	
	if (q->front == -1) q->front = 0;
}

unsigned char Dequeue(Queue *q) {
	if (q->count == 0 || q->front == -1) return 0;

	unsigned char data = q->data[q->front];
	q->front++;
	if (q->front == BUFFER_SIZE) q->front = 0;
	q->count--;
	
	if (q->count == 0) {
		q->front = -1;
		q->rear  = -1;
	}
	
	return data;
}

Queue qTransmit, qReceiver;

void uart_setup(int SPBRGvalue, char change_pin) {
    // UART queue setup
	qTransmit.front = -1;
	qTransmit.rear  = -1;
	qTransmit.count = 0;
	memset(qTransmit.data, 0, BUFFER_SIZE);
	
	qReceiver.front = -1;
	qReceiver.rear  = -1;
	qReceiver.count = 0;
	memset(qReceiver.data, 0, BUFFER_SIZE);
    
    // UART Setup
    if (change_pin) {
        APFCONbits.RXDTSEL = 1; // Change RXD from RA1 to RA5
        APFCONbits.TXCKSEL = 1; // Change TXD from RA0 to RA4
    }
    
    // int SPBRGvalue = _XTAL_FREQ / (16 * BAND + 1); // SYNC = 0, BRGH = 0, BRG16 = 0, Fosc = 32MHz
    // int SPBRGvalue = 51; // 51 - 9600 band | Table 26-5 page 262
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 0;
    BAUDCONbits.BRG16 = 0;
    SPBRGL = SPBRGvalue&0xFF;     // Set uart speed (LOW byte)
    SPBRGH = (SPBRGvalue>>8)&0xFF; // Set uart speed (HIGH byte)
    
    TXSTAbits.TXEN = 1; // Enable transmit
    RCSTAbits.SPEN = 1; // Enable serial port (needs)
    RCSTAbits.CREN = 1; // Enable receiver
    
    // Interrupt
    di(); // Disable all interrupt
    INTCONbits.PEIE = 1; // pheripheral interrupt
    PIE1bits.RCIE = 1;
    ei(); // Enable all interrupt
}

void uart_write(unsigned char data) {
    Enqueue(&qTransmit, data);
    PIE1bits.TXIE = 1; // Enable transmit interrupt
}

int uart_available() {
	return qReceiver.count;
}

unsigned char uart_read() {
	return Dequeue(&qReceiver);
}

void __interrupt(high_priority) _isr() {
    if (PIE1bits.TXIE && PIR1bits.TXIF) {
        PIR1bits.TXIF = 0;
        if (qTransmit.count > 0) {
            TXREG = Dequeue(&qTransmit);
        } else {
            PIE1bits.TXIE = 0; // Disable transmit interrupt
        }
    } else if (PIE1bits.RCIE && PIR1bits.RCIF) {
        PIR1bits.RCIF = 0;
        Enqueue(&qReceiver, RCREG);
    }
}


#endif