

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>
#include <pic16f18877.h>
#pragma config WDTE = OFF
#pragma config RSTOSC = HFINTPLL

#define NumberDisplay PORTC
#define Segment PORTB

//Prototypes
void configureThings(void);
void playNote(void);
void __interrupt() intHand(void);
void DelayMillis(float numOfMillis);

uint8_t CharDisplay[] = {8,9,10,11}; //This is what displays on the seg

uint16_t TMR_Preload = 0; //Preload variable for the target
uint16_t SegMap[12] = {  0b00111111, // 0
                        0b00000110, // 1
                        0b01011011, // 2
                        0b01001111, // 3
                        0b01100110, // 4
                        0b01101101, // 5
                        0b01111101, // 6
                        0b00000111, // 7 
                        0b01111111, // 8
                        0b01101111, // 9
                        0b00000000, // Blank
                        0b01000000, // -
                        };
uint8_t number;
uint8_t display_index = 0;

void main(void) {
//Main Code
    
    configureThings();
    INTCONbits.GIE = 1; //Global Enable
    
    while(1){
        PORTB = 1; 
    }
    
    return;
}

void __interrupt() intHand(void){
// This interrupt function checks whether the timer flag is high and resets
// the frequency to previously calculated value. It also calls the display code
// Inputs: none
// Outputs: none
    
    if(PIR0bits.TMR0IF){
        //Display Code
        for(display_index = 0; display_index < 4; display_index++){//indexes through
            number = CharDisplay[display_index]; //finds the number that being displayed
            PORTC = ~SegMap[number]; //Displays the number
            DelayMillis(1); //Delay
            PORTC = 0xff; //Clear the number
            if(PORTB < 8){PORTB = PORTB*2;} //Changes the segment display.
            else{PORTB = 1;}   
        }
        
        
        PIR0bits.TMR0IF = 0;   //Clear flag 

        //Initialize Timer0 to frequency by subtracting by our target.
        TMR0H = (TMR_Preload & 0xFF00)>>8;
        TMR0L = TMR_Preload & 0x00FF;    
    }
}

void configureThings(void){
// This function configures all the variables.
// Input: none
// Output: none
    
    T0CON0bits.T016BIT = 1;//Use 16bit timer
    T0CON1bits.T0CS = 3;   // Clock source is HFINTOSC [16MHz]
    T0CON1bits.T0CKPS = 4; // Divide input clock by 2^4
    TMR_Preload = 63535; // 4ms, 250 Hz

    T2CONbits.OUTPS = 2; //Prescaler 2^7 = 128
    T2CONbits.CKPS = 5; //Post: gives us a resolution of 12ms
    T2CLKCONbits.CS = 4; // Clock source is LFINTOSC (32kHZ))
    
    
    //Initialize ports
    BSR = 62;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    BSR = 0;
    PORTB = 0;      //clears ports
    PORTC = 0;
    TRISB = 0;      //configure output
    TRISC = 0;      //configure output
  
    //RB2PPS = 0x18;  //map the TMR0 signal to pin RB2

    //External Interrupt
    PIR0bits.INTF = 0;
    PIE0bits.INTE = 0;
    
    //Timer Interrupt
    T0CON0bits.T0EN = 1; 
    PIR0bits.TMR0IF = 0;
    PIE0bits.TMR0IE = 1;
    
}   


void DelayMillis(float numOfMillis){
// This function delays for the number of milliseconds inputted.
// Inputs: numOf10mSec
// Output: None
    PIR4bits.TMR2IF = 0;
    T2PR = numOfMillis/12;//number of   12ms
    T2CONbits.ON = 1; //Resets counter
    while(PIR4bits.TMR2IF == 0){
        //Wait
    }
    T2CONbits.ON = 0;   

    return;
}