/* 
 * File:   Lab_9.c
 * Author: Ben Huang
 */

#include <xc.h>
#include <stdint.h>
#pragma  config WDTE = OFF
#pragma  config RSTOSC = HFINTPLL

// Define Note Frequencies
#define  A       440.00
#define  B       493.88
#define  C       261.63
#define  D       293.66
#define  E       329.63
#define  F       349.23
#define  G       392.00

#define  As      466.16
#define  Cs      277.18
#define  Ds      311.13
#define  Fs      369.99
#define  Gs      415.30

// Global Variables
uint16_t TMR_Preload;   // Value for the Timer to Reset to After reaching 0xFFFF
// Define Song 1
float Notes1[] = {20, D, C, A, D, C, C/2, C, G/2, G, D, D/2, D, D/2, C, A/2, A, D, D, D, A};
float Duration1[] = {3, 2, 1, 3, 1.5, 0.5, 0.5, 0.5, 3, 1.5, 0.5, 0.5, 0.5, 1.5, 0.5, 1, 1, 1, 1, 1};
// uint8_t Octave1[] = {2, 2, 2, 2, 2, 3, 2, 3, 2, 2, 3, 2, 3, 2, 3, 2, 2, 2, 2, 2};
// Define Song 2
// This first one is the entire song, I shortened it for you in the second one
float Notes2[] = {137, Fs, 0, A, Cs*2, 0, A, 0, Fs, D, D, D, 0, D*2, D*2, D*2, Cs, D, Fs, A, Cs*2, 0, A, 0, Fs, E*2, Ds*2, D*2, 0, G, Cs*2, Fs, 0, Cs*2, 0, Gs, 0, Cs*2, 0, G, Fs, 0, Cs, 0, C, C, C, 0, C, C, C, 0, Ds, D, Cs, A, Cs*2, 0, A, 0, Fs, E, E, E, 0, E*2, E*2, E*2, 0, Fs, A, Cs*2, 0, Fs, 0, F, Cs*2, B, 0, B, G, D, Cs, B, G, Cs, A, Fs, C, B/2, F, D, B/2, E, E, E, 0, As, B, Cs*2, D*2, Fs*2, A*2, 0, A/2, As/2, B/2, As/2, B/2, A/2, As/2, B/2, Fs, Cs, B/2, As/2, B/2, B/2, C, Cs, C, Cs, Cs, C, Cs, Gs, Ds, Cs, Ds, B/2, C, D, A, D, Gs, Gs, Gs, 0};
// float Notes2[] = {27, Fs, 0, A, Cs*2, 0, A, 0, Fs, D, D, D, 0, D*2, D*2, D*2, Cs, D, Fs, A, Cs*2, 0, A, 0, Fs, E*2, Ds*2, D*2, 0, G, Cs*2, Fs, 0, Cs*2, 0, Gs, 0, Cs*2, 0, G, Fs, 0, Cs, 0, C, C, C, 0, C, C, C, 0, Ds, D, Cs, A, Cs*2, 0, A, 0, Fs, E, E, E, 0, E*2, E*2, E*2, 0, Fs, A, Cs*2, 0, Fs, 0, F, Cs*2, B, 0, B, G, D, Cs, B, G, Cs, A, Fs, C, B/2, F, D, B/2, E, E, E, 0, As, B, Cs*2, D*2, Fs*2, A*2, 0, A/2, As/2, B/2, As/2, B/2, A/2, As/2, B/2, Fs, Cs, B/2, As/2, B/2, B/2, C, Cs, C, Cs, Cs, C, Cs, Gs, Ds, Cs, Ds, B/2, C, D, A, D, Gs, Gs, Gs, 0};
float Duration2[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1.5, .5, 1, 1, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1.5, 0.5, 0.5, 0.5, 1.5, 1, 1, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 2, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 1, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 2, 0.5, 0.5, 0.5, 0.5, 0.5, 1, 3,1, 1, 1.5,  0.5, 3, 0.5, 0.5, 0.5, 1, 0.5, 1.5, 0.5, 4, 1, 1, 1.5, 0.5, 3, 0.5, 0.5, 0.5, 1, 0.5, 1.5, 0.5, 1.5, 0.5, 0.5, 1, 0.5, 0.5, 0.5, 0.5, 0.5};
// uint8_t Octave2[] = {2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 1, 1, 1, 2, 2, 1, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2};

// Function Prototypes
void ConfigurePorts(void);
void ConfigureTimer0(void);
void ConfigureTimer2(void);
void ConfigureADC(void);
uint16_t AdjustUpperLimit(void);
uint16_t AdjustLowerLimit(void);
void PlaySong(uint8_t Song, uint16_t Index, float tempo);
void Delay(float ms);

// Functions
void main(void) {
    ConfigurePorts();
    ConfigureTimer0();
    ConfigureTimer2();
    ConfigureADC();
    
    uint16_t POT, LightSensor;
    uint16_t upperLimit = 0x03FF;
    uint16_t lowerLimit = 0;
    uint16_t noteIndex = 0;
    float tempo;
    
    INTCONbits.GIE = 1; // Enable Interrupts
    
    while(1){
        if(PORTC & 0x01){
            upperLimit = AdjustUpperLimit();
        }
        if(PORTC & 0x02){
            lowerLimit = AdjustLowerLimit();
        }
        
        ADPCH = 8; // 8 for PortB0, 9 for Port B1
        Delay(12);
        PIR1bits.ADIF = 0;  // Clear ADC Flag
        ADCON0bits.ADGO = 1;
        
        while(!PIR1bits.ADIF){  // Wait until conversion is finished
        }
        LightSensor = ADRES;
        
        if(LightSensor < lowerLimit) {
            PlaySong(0, noteIndex, tempo);
            noteIndex = noteIndex + 1;
            
            if(noteIndex >= Notes1[0]) { // If reached the end of the song, reset
                noteIndex = 0;
            }
        }
        else if (LightSensor > upperLimit) { 
            PlaySong(1, noteIndex, tempo);
            noteIndex = noteIndex + 1;
            
            if(noteIndex >= Notes2[0]) { // If reached the end of the song, reset
                noteIndex = 0;
            }
        }
        else {
            noteIndex = 0;
        }
        
        ADPCH = 9;
        Delay(12);
        PIR1bits.ADIF = 0;  // Clear ADC Flag
        ADCON0bits.ADGO = 1;
        
        while(!PIR1bits.ADIF){  // Wait until conversion is finished
        }
        POT = ADRES;
        
        tempo = 1 - 0.000879765395894 * POT; // Set tempo
    }
    
    return;
}

void __interrupt() int_handler(void) {
    // This function handles all interrupts occurred throughout the code. For
    // this code, the interrupt should only be triggered when Timer0
    // overflows.
    //
    // Inputs:  No such thing for interrupts
    // Outputs: No such thing for interrupts
    
        PIR0bits.TMR0IF = 0; // Clear Timer0 Flag

        // Set Timer to Preload
        TMR0H = TMR_Preload >> 8;   // Set the High Byte for the Timer Preload
        TMR0L = TMR_Preload & 0x0F; // Set the Low  Byte for the Timer Preload
}

void ConfigurePorts(void) {
    // This routine will configure the ports as Digital rather than Analog, as 
    // configure which pins should be Inputs and which should be Outputs,
    // and clear the Data register for LATCH read
    //
    // Inputs:  None
    // Outputs: None
    
    ANSELA = 0;     // Make pins digital
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    
    ANSELB = 0x03;  // Make pins 0-1 analog and others digital
    
    PORTA = 0;  // Clear all pins on Ports
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    PORTE = 0;
    
    TRISA = 0xFF;   // Make the port pins inputs
    TRISD = 0xFF;
    TRISE = 0xFF;
    
    TRISB = 0x03;     // Make pins 0 and 1 inputs and the rest outputs
    TRISC = 0x7F;     // Make pin 7 an output and the rest inputs
}

void ConfigureTimer0(void){
    // This function configures Timer0 to be in 16bit mode.
    // The function assumes that internal oscillator is 16Mhz.
    //
    // Timer0 out signal can be sent to any pin on PortB or PortC by writing
    //    0x18 to the RxyPPS register PortB2 = RB2PPS
    //
    // Inputs:  None
    // Outputs: None
    
    T0CON0bits.T016BIT = 1; // Run in 16bit mode
    T0CON1bits.T0CS = 3;   // Clock source is HFINTOSC
    T0CON1bits.T0CKPS = 2; // Divide input clock by 2^prescale
    T0CON0bits.T0OUTPS = 0; // Postscaler is 1:1
    RB2PPS = 0x18; // Set the output to Pin PortB2
    T0CON0bits.T0EN = 0; // Turn Timer on
    PIE0bits.TMR0IE = 1; // Enable Timer0 Interrupts
}

void ConfigureTimer2(void) {
    // This function configures Timer2 to have a 12 ms resolution between target
    // values
    //
    // Inputs:  None
    // Outputs: None
    T2CLKCONbits.CS = 4; // Clock source is LFINTOSC
//     T2CONbits.CKPS = 0; // Divide input clock by 2^prescale
//     T2CONbits.OUTPS = 0; // Postscaler is 1:1
//     T2PR = 32; // Set Timer target to run at 1 ms
    T2CONbits.CKPS = 5; // Divide input clock by 2^prescale
    T2CONbits.OUTPS = 11; // Postscaler is 12:1
}

void ConfigureADC(void) {
    // This function configures the Analog to Digital Converter for use, right
    // justifying the result for full 10-bit resolution
    
    // Inputs:  None
    // Outputs: None
    ADCON0bits.ADCS = 1; // Select Fosc as Clock
    ADCLK = 15; // Divide by 2(15+1)
    
    ADREFbits.ADNREF = 0;   // Configure V+ and V- to VDD and VSS
    ADREFbits.ADPREF = 0;
    
    ADCON0bits.ADON = 1; // Turn ADC On
    
    ADCON0bits.ADFRM0 = 1;  // Right Justify the result
}

void PlaySong(uint8_t Song, uint16_t Index, float tempo) {
    // This function will play one of the two predefined global songs based on
    // the light level read by the photoresistor. If the light level leaves the 
    // range needed to play a song, it will immediately stop playing the song
    // 
    // Inputs:  uint8_t Song - Which song should be played
    //          uint16_t Index - Next index of the note that should be played
    //          float tempo - the time length in seconds of a quarter note
    // Outputs: None
    
    float multiplier;
    if (!Song) {
        if (Notes1[Index+1]) {  // To be used as a rest if frequency is 0
            if(Notes1[Index+1] < C) { // Set Octave depending on frequency of note
                multiplier = 2;
                T0CON1bits.T0CKPS = 3;
            }
            else if(Notes1[Index+1] > B) {
                multiplier = 0.5;
                T0CON1bits.T0CKPS = 1;
            }
            else {
                multiplier = 1;
                T0CON1bits.T0CKPS = 2;
            }
            TMR_Preload = 65535 - (4000 /(2 * Notes1[Index+1] * multiplier)) * 1000; // Set Timer Preload
            //T0CON1bits.T0CKPS = Octave1[i];
            T0CON0bits.T0EN = 1;
        }

        Delay(Duration1[Index] * tempo * 1000 - 24); // Play note for specified period of time
        T0CON0bits.T0EN = 0;
        Delay(0.20 * tempo * 1000); // Wait 20% of quarter note after
    }
    else {  // Else, play Song 2
        if (Notes2[Index+1]) {  // To be used as a rest if frequency is 0
            if(Notes2[Index+1] < C) { // Set Octave depending on frequency of note
                multiplier = 2;
                T0CON1bits.T0CKPS = 3;
            }
            else if(Notes2[Index+1] > B) {
                multiplier = 0.5;
                T0CON1bits.T0CKPS = 1;
            }
            else {
                multiplier = 1;
                T0CON1bits.T0CKPS = 2;
            }
            TMR_Preload = 65535 - (4000 /(2 * Notes2[Index+1] * multiplier)) * 1000; // Set Timer Preload
            //T0CON1bits.T0CKPS = Octave2[i];
            T0CON0bits.T0EN = 1;
        }

        Delay(Duration2[Index] * tempo * 1000 - 24); // Play note for specified period of time
        T0CON0bits.T0EN = 0;
        Delay(0.20 * tempo * 1000); // Wait 20% of quarter note after
    }
}

uint16_t AdjustUpperLimit(void) {
    // This function adjusts the upper light level limit to match the
    // the potentiometer reading while the button on PortC0 is pressed
    //
    // Inputs:  None
    // Outputs: uint16_t upperLimit - the new upper limit
    
    uint16_t upperLim = 0x03FF;
    uint16_t lightLevel;
    
    while(PORTC & 0x01) {
        ADPCH = 9;
        Delay(12);
        PIR1bits.ADIF = 0;  // Clear ADC Flag
        ADCON0bits.ADGO = 1;
        
        while(!PIR1bits.ADIF){  // Wait until conversion is finished
        }
        upperLim = ADRES;
        
        ADPCH = 8;
        Delay(12);
        PIR1bits.ADIF = 0;  // Clear ADC Flag
        ADCON0bits.ADGO = 1;
        
        while(!PIR1bits.ADIF){  // Wait until conversion is finished
        }
        lightLevel = ADRES;
        
        if(lightLevel > upperLim) {
            PORTC = PORTC | 0x80; // Turn on LED on PORTC7
        }
        else {
            PORTC = PORTC & 0x7F; // Turn off LED on PORTC7
        }
    }
    
    PORTC = PORTC & 0x7F;   // Clear PORTC7
    return upperLim;
}

uint16_t AdjustLowerLimit(void) {
    // This function adjusts the lower light level limit to match the
    // the potentiometer reading while the button on PortC1 is pressed
    //
    // Inputs:  None
    // Outputs: uint16_t lowerLimit - the new lower limit
    
    uint16_t lowerLim = 0x0000;
    uint16_t lightLevel;
    
    while(PORTC & 0x02) {
        ADPCH = 9;
        Delay(12);
        PIR1bits.ADIF = 0;  // Clear ADC Flag
        ADCON0bits.ADGO = 1;
        
        while(!PIR1bits.ADIF){  // Wait until conversion is finished
        }
        lowerLim = ADRES;
        
        ADPCH = 8;
        Delay(12);
        PIR1bits.ADIF = 0;  // Clear ADC Flag
        ADCON0bits.ADGO = 1;
        
        while(!PIR1bits.ADIF){  // Wait until conversion is finished
        }
        lightLevel = ADRES;
        
        if(lightLevel < lowerLim) {
            PORTC = PORTC | 0x80; // Turn on LED on PORTC7
        }
        else {
            PORTC = PORTC & 0x7F; // Turn off LED on PORTC7
        }
    }
    
    PORTC = PORTC & 0x7F;   // Clear PORTC7
    return lowerLim;
}

//void Delay(float ms) {
//    for(uint16_t i = 0; i < ms; i++) {
//        T2CONbits.ON = 1;
//        while(!PIR4bits.TMR2IF) {  
//        }
//        
//        T2CONbits.ON = 0;
//        PIR4bits.TMR2IF = 0;
//    }
//}

void Delay(float ms) {
    // This function uses Timer2 to delay ms milliseconds, taking into account
    // that Timer2 only has 12 ms precision
    //
    // Inputs:  ms (float) - the number of milliseconds to delay
    // Outputs: None
    
    T2PR = ms/12; // Set target so Timer2 alarms after ms milliseconds
    
    T2CONbits.ON = 1;
    while(!PIR4bits.TMR2IF) { 
    }
    
    T2CONbits.ON = 0;
    PIR4bits.TMR2IF = 0;
}
