#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <stdint.h>
#include <pic16f18877.h>
#pragma config WDTE = OFF
#pragma config RSTOSC = HFINTPLL

#define CButtons PORTC
#define BButtons PORTB

//Prototypes
void configureThings(void);
void playNote(float frequency, float delaySec);
void __interrupt() intHand(void);
void DelayMillis(float numOfMillis);
uint16_t ReadPotLevel(void);
void AdjustTempo(void);
void playNumOfBeats(uint8_t numOfBeats);

//Define the note frequencies
#define C1 130.8128
#define Cs1 138.5913
#define D1 146.8324
#define Eb1 155.5635
#define E1 164.8138
#define F1 174.6141
#define Fs1 184.9972
#define G1 195.9977
#define Ab1 207.6523
#define A1 220.0000
#define Bb1 233.0819
#define B1 246.9417

#define C2 261.63
#define Cs2 277.18
#define D2 293.66
#define Eb2 311.1270
#define E2 329.63
#define F2 349.23
#define Fs2 369.99
#define G2 392.00
#define Ab2 415.30
#define A2 440.00 
#define Bb2 466.16
#define B2 493.88

#define C3 523.25
#define Cs3 554.36
#define D3 587.32
#define Eb3 622.25
#define E3 659.25
#define F3 698.45
#define Fs3 739.98
#define G3 783.99
#define Ab3 830.60
#define A3 880.00
#define Bb3 932.32
#define B3 987.76

//Globals
uint16_t TMR_Preload = 0; //Preload variable for the target
float tempo = 0.6; //quater note length in seconds.

//Song Arrays
float notesSong1[] = {Ab2,A2,Fs2,E2,Fs2,Ab2,B2,Cs3,Fs2,1000000,Cs3,E3,G3,Fs3,D3,A2,B2};
float durationSong1[] = {3,1,3,0.5,0.5,3,0.5,1,2,0.5,0.5,0.5,1.5,0.5,1,0.5,4.5,0};

float notesSong2[] = {Eb3, 1000000, Bb2, Eb3, Bb2, C3, D3, G2, G2, C3, Bb2, Ab2, Bb2, 
                     Eb2, Eb2, F2, F2, G2, Ab2, Ab2, Bb2, C3, D3, Eb3, F3, Bb2, 
                    G3, F3, Eb3, F3, D3, Bb2, Eb3, D3, C3, D3, G2, G2, C3, Bb2, 
                    Ab2, Bb2, Eb2, Eb2, Eb3, D3, C3, Bb2,100000,G3,F3,Eb3,D3,Eb3,
                    F3,Bb2,Bb2,1000000,Eb3,D3,C3,Bb2,C3,D3,G2,G2,Bb2,C3,D3,
                    Eb3,C3,D3,Eb3,C3,D3,Eb3,C3,Eb3,Ab3,Ab3,Ab3,Ab3,Ab3,Ab3,Ab3,
                    G3,F3,Eb3,F3,G3,Eb3,Eb3,F3,Eb3,D3,C3,D3,Eb3,C3,C3,G2,1000000,
                    G2,Eb3,D3,C3,Bb2,Eb2,Eb2,Eb3,D3,C3,Bb2,Bb2,Bb2};
float durationSong2[] = {3, 0.5, 0.5, 1, 0.75, 0.25, 1, 0.5, 0.5, 1, 0.75, 0.25,
                1, 0.5, 0.5, 1, 0.75, 0.25, 1, 0.75, 0.25, 1, 0.5, 0.5, 1.5, 
                0.5,1,0.75,0.25,1,0.5,0.5,1,0.75,0.25,1,0.5,0.5,1,0.75,0.25,
                1,0.75,0.25,1,0.75,0.25,1.5,0.5,2,0.5,0.5,0.5,0.5,1.5,0.25,1.5,
                0.5,2,0.5,0.5,0.5,0.5,1.5,0.5,0.5,0.5,0.5,0.5,
                1,0.75,0.25,1,0.75,0.25,1,0.5,0.5,0.5,0.33,0.33,0.33,
                        0.5,0.5,2,0.5,0.5,0.5,0.5,1.75,0.5,2,
                2,0.5,0.5,0.5,0.5,1.5,0.5,2,0.5,0.5,0.5,1,0.75,0.25,1,0.5,0.5,1,
                0.75,0.25,1,0.5,0.5,0};


void main(void) {
    configureThings();
    INTCONbits.GIE = 1; 
    uint8_t mode = 0;
    float potval;
    float prevpotval;

    //Remembers what mode we are in. Button container state or beaker mdoe.
    while(1){
        potval = ReadPotLevel()/1023.0;
        if(BButtons == 128){
            mode = 0;
        } else if(BButtons == 64){
            mode = 1;
        }

        if(mode == 0){
            //Button container mode
            //Case statement for each button
            if(CButtons == 1){
                playNumOfBeats(1);
            }else if(CButtons == 2){
                playNumOfBeats(2);
            }else if(CButtons == 4){
                playNumOfBeats(3);
            }else if(CButtons == 8){
                playNumOfBeats(4);
            }else if(CButtons == 16){
                playNumOfBeats(5);
            }else if(CButtons == 32){
                playNumOfBeats(6);
            }
        }else if(mode == 1){
            //Beaker mode
            //If POT volume changes, the number of beeps will play indicating the volume
            if(CButtons == 1){
                if(potval >= 0.99){
                    playNumOfBeats(7);
                }else if((potval > 0.99)&&(potval >= 0.75)){
                    playNumOfBeats(6);
                }else if(potval >= 0.66){
                    playNumOfBeats(5);
                }else if(potval >= 0.5){
                    playNumOfBeats(4);
                }else if(potval >= 0.33){
                    playNumOfBeats(3);
                }else if(potval >= 0.25){
                    playNumOfBeats(2);
                }else if(potval >= 0.125){
                    playNumOfBeats(1);
                } 
                prevpotval = potval;
            }

        }        
    }
    
}

void playNumOfBeats(uint8_t numOfBeats){
    
    while(numOfBeats != 0){
        playNote(440.00, 0.2);
        numOfBeats = numOfBeats - 1;
    }
}

uint16_t ReadPotLevel(void){
// This function reads the potentiometer value.
// Input: none
// Output: potentiometer reading
    
    uint16_t result;
    ADPCH = 0b001001; //Set potentiometer as our input
    ADCON0bits.ADGO = 1; //start conversion
    while(PIR1bits.ADIF == 0){
        // wait for conversion to finish
    }
    PIR1bits.ADIF = 0;
    result = ADRES;
    return result;
}


void playNote(float frequency, float delaySec){
// This function calculates the timer preload variable to play the note and 
// duration inputted.
// Inputs: frequency, delaySec
// Outputs: none
    
    DelayMillis(0.1*delaySec*1000);
    T0CON0bits.T0EN = 1; //Enable clock
    TMR_Preload = 65535 - (16000/(2*frequency*16)*1000);
    DelayMillis(delaySec*1000);
    T0CON0bits.T0EN = 0; //Disable clock
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
    
    //ADC configure 
    //Configure the ADC module
    ADCON0bits.ADCS = 1;
    ADCLK = 15; //divide by 2(15+1)
    //configure the voltage references
    ADREFbits.ADNREF = 0;
    ADREFbits.ADPREF = 0;
    //Select ADC input channel
    ADPCH = 0b001001; //Set potentiometer as our input
    //ADPCH = 0b001000; //Set light sensor as our input
    //Turn on ADC module
    ADCON0bits.ADON = 1;
    ADCON0bits.ADCONT = 0; //continuous?
    ADCON0bits.ADFRM = 1; // right justified
    ADCON0bits.ADGO = 1; //start conversion
    
    //POT ADC to Port1-1. ANB1 001001
    //Light sensor to Port1-0. ANB0 001000
    
    T0CON0bits.T016BIT = 1;//Use 16bit timer
    T0CON1bits.T0CS = 3;   // Clock source is HFINTOSC [16MHz]
    T0CON1bits.T0CKPS = 4; // Divide input clock by 2^4


    T2CONbits.OUTPS = 2; //Prescaler 2^7 = 128
    T2CONbits.CKPS = 7; //Post: gives us a resolution of 12ms
    T2CLKCONbits.CS = 4; // Clock source is LFINTOSC (32kHZ))
    
    //Initialize ports
    BSR = 62;
    ANSELB = 0;
    ANSELBbits.ANSB0 = 1; //Light Sensor
    ANSELBbits.ANSB1 = 1; //Potentiometer
    ANSELC = 0;
    ANSELD = 0;
    BSR = 0;
    PORTB = 0;          //clears ports
    PORTC = 0;
    TRISB = 0b11111011; //configure 3rd bit to output, rest input
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


