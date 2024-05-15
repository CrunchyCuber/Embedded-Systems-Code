
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>
#pragma config WDTE = OFF
#pragma config RSTOSC = HFINTPLL

#define Buttons PORTC
#define Octave PORTB

//Prototypes
void configureThings(void);
void playNote(void);
void __interrupt() intHand(void);

uint16_t TMR_Preload = 0; //Preload variable for the target

void main(void) {
    configureThings();
    INTCONbits.GIE = 1; //Global Enable
    
    uint8_t octaveUp = 5;
    uint8_t octaveDown = 3;
    uint8_t octave = 4;
    
    while(1){
        if(Buttons != 0){
            T0CON0bits.T0EN = 1; 
            playNote();
            if(PORTBbits.RB0){  //PORTB Button 0
                T0CON1bits.T0CKPS = octaveUp;
            }else if(PORTBbits.RB1){//PORTB Button 1
                T0CON1bits.T0CKPS = octaveDown;
            }else{
                T0CON1bits.T0CKPS = octave;
            }
        }else{
            T0CON0bits.T0EN = 0; 
        }
    }

    return;
}


void playNote(void){
// This function calculates the number to be placed into the target bit of the
// timer.
// Inputs: none
// Outputs: none
    
    float C = 261.63;
    float D = 293.66;
    float E = 329.63;
    float F = 349.23;
    float G = 392.00;
    float A = 440.00; 
    float B = 493.88;
    float des_freq = 0;

    //Checks which button is pressed.
    if(Buttons == 1){
        des_freq = C;
    }else if(Buttons == 2){
        des_freq = D;
    }else if(Buttons == 4){
        des_freq = E;
    }else if(Buttons == 8){
        des_freq = F;
    }else if(Buttons == 16){
        des_freq = G;
    }else if(Buttons == 32){
        des_freq = A;
    }else if(Buttons == 64){
        des_freq = B;
    }
    
    //Calculate global variable for the timer target
    TMR_Preload = 65535 - (16000/(2*des_freq*16)*1000);

}

void __interrupt() intHand(void){
// This interrupt function checks whether the timer flag is high and resets
// the frequency to previously calculated value.
// Inputs: none
// Outputs: none
    
    if(PIR0bits.TMR0IF){
        PIR0bits.TMR0IF = 0;   //Clear flag 

        //Initialize Timer0 to frequency by subtracting by our target.
        TMR0H = (TMR_Preload & 0xFF00)>>8;
        TMR0L = TMR_Preload & 0x00FF;    
    }
}

void configureThings(void){
    T0CON0bits.T016BIT = 1;//Use 16bit timer
    T0CON1bits.T0CS = 3;   // Clock source is HFINTOSC [16MHz]
    T0CON1bits.T0CKPS = 4; // Divide input clock by 2^4

    //Initialize ports
    BSR = 62;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    BSR = 0;
    PORTB = 0;          //clears ports
    PORTC = 0;
    TRISB = 0b11111011; //configure output
    TRISC = 0xFF;       //configure input
  
    RB2PPS = 0x18;  //map the TMR0 signal to pin RB2

    //External Interrupt
    PIR0bits.INTF = 0;
    PIE0bits.INTE = 0;
    
    //Timer Interrupt
    T0CON0bits.T0EN = 1; 
    PIR0bits.TMR0IF = 0;
    PIE0bits.TMR0IE = 1;
    
}