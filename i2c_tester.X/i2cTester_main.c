/* 
 * File:   i2c_mcp23017_main.c
 * Author: thomas
 *
 * Created on 1. April 2023, 09:22
 */
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/twi.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

//void initUart() {
//    //init UART
//    UBRR0H = 0x00;
//    UBRR0L = 0x67;
//    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
//    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
//
//}
//void sendUart(char *s)
//{
//    while (!(UCSR0A & (1<<UDRE0)))  /* warten bis Senden moeglich                   */
//    {
//    }
//
//    while (*s)
//    {   /* so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)" */
//        UDR0 = *s;
//        s++;
//    }    
//}

void ERROR() {
    while (1) {
        PORTB |= (1 << PB5);
        _delay_ms(200);
        PORTB &= ~(1 << PB5);
        _delay_ms(500);
    }
}

void i2cStart() {
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); //TWSTA = TWI START Condition Bit
    while (!(TWCR & (1 << TWINT)));
    if ((TWSR & 0xF8) != TW_START)
        ERROR(); //Fehlerbehandlung
}

void i2cSendAdr(int value) {
    int result = 0x00;
    TWDR = value;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    result = TWSR;
    result &= 0xF8;
    if (result != TW_MT_SLA_ACK)
        ERROR();
}

void i2cSendData(int reg, int value) {
    int result = 0x00;
    TWDR = reg;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    result = TWSR;
    result &= 0xF8;
    if (result != TW_MT_DATA_ACK)
        ERROR();
    
    TWDR = value;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    result = TWSR;
    result &= 0xF8;
    if (result != TW_MT_DATA_ACK)
        ERROR();
}

void i2cStop() {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

/*
 * 
 */
int main(int argc, char** argv) {
    // Adr (0x20 - 0x27)
    const int SLA_W = ((0x20 << 1) | 0);

    DDRB |= (1 << PB5);
    PORTB |= (1 << PB5);
    // _delay_ms(100);
    PORTB &= ~(1 << PB5);
    // initUart();
    // sendUart("init");
    
    // init i2c Port
    TWSR |= (1 << TWPS1);
    TWBR = 0x40;
    TWAMR = 0xFF;
//    sei();
    
    
    i2cStart();
    i2cSendAdr(SLA_W);
    i2cSendData(0x00, 0x00); // IODIRA => 0x00
//    i2cSendData(0x00); 
    i2cStop();
    while (1) {
        _delay_ms(100);
        i2cStart();
        i2cSendAdr(SLA_W);
        i2cSendData(0x12, 0x01); // GPIOA => 0x01
//        i2cSendData(0x01);
        i2cStop();

        _delay_ms(500);
        i2cStart();
        i2cSendAdr(SLA_W);
        i2cSendData(0x12, 0x00); // GPIOA => 0x01
//        i2cSendData(0x00);
        i2cStop();
    }
    return (EXIT_SUCCESS);
}

