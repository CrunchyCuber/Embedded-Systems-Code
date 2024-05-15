/*
 * File:   lab11.c
 * Author: whoknows
 *
 *
 */


#include <xc.h>
#include <stdint.h>
#include <pic16f18877.h>
#pragma config WDTE = OFF
#pragma config RSTOSC = HFINTPLL

void ConfigurePorts (void); 
void ConfigureTimer0 (void);
    uint8_t SegMap[] = {0xc0, 0xf9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x98, 0xFF, 0xBF}; // this array goes in order from 0-8 on 7 segment display
    uint8_t CharDisplay[] = {9,10,11,11}; // this is what the user changes
    uint8_t number = 2;
    uint8_t *display_index= 0; // this initializes the indexing to 0
    uint8_t choosedisplay = 1; // choosedisplay 


void ConfigurePorts(void) {
    ANSELA = 0x00;
    ANSELB = 0b00000000;
    ANSELC = 0x00; 
    ANSELD = 0x00;
    ANSELE = 0x00;

    TRISA = 0x00;
    TRISB = 0x00; 
    TRISC = 0x00; 
    TRISD = 0X00;
    TRISE = 0x00;
    
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
return;
}
    
void ConfigureTimer0 (void) {
// This function configures Timer0 so that it will increment every 16usec
// The function assumes that internal oscillator is 32Mhz.
//   therefor, the unscaled clock fosc/4 is 8Mhz
//
// Timer0 out signal can be sent to any pin on PortB or PortC by writing
//    0x18 to the RxyPPS register PortB2 = RB2PPS

    // Configure Timer0
    T0CON0bits.T016BIT = 0; // 
    T0CON1bits.T0CS = 3 ;// HFINTOSC is the clock source (16MHz)
    T0CON1bits.T0CKPS = 4; //  Prescale by dividing by 2^2
    PIE0bits.TMR0IE = 1 ;        // enable timer0 interrupts
    T0CON0bits.T0EN = 1; // turn on timer
    T0CON0bits.T0OUTPS = 0; // postscale 1:1
    TMR0H = 0xFE;
    }

void __interrupt() int_handler(void){
    PIR0bits.TMR0IF = 0; //clear TMR0 int flag
    PORTB = choosedisplay;
    PORTC = SegMap[*display_index];
    display_index++;
    choosedisplay= choosedisplay *2;
    
    if (choosedisplay > 8) {
    choosedisplay = 1;
    display_index = CharDisplay;
    }
}

void main(void) {
   
   ConfigurePorts();
   ConfigureTimer0();
   INTCONbits.GIE = 1;//enable interrupts
   while (1)
   {
    }
}

