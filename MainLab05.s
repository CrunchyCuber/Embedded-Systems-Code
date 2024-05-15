; Including this allows us to use some assembler macros
#include <xc.inc>
CONFIG WDTE=OFF ; Turn off the Watch Dog Timer (necessary for debug) 
CONFIG CSWEN=ON ; allows the oscillator source to change after power up
;CONFIG RSTOSC=LFINT ;Set oscillator to internal 31khz  
CONFIG RSTOSC=HFINT1 ;Set oscillator to internal 1Mhz
;CONFIG RSTOSC=HFINTPLL ;Set oscillator to internal 32Mhz  

; Insert command at Reset Vector location that tells
; the system to go to the start of Main Code
PSECT resetVec, class=CODE, abs
    ORG  0x00000 ; abs and ORG 0 put this at Program Memory 0
resetVec:
    goto main
 
; Allocate memory for program variables    
PSECT udata
LatestKeyPress: ds 1
BtnIsPressed:	ds 1
DelayLoopInner:	ds 1
DelayLoopOuter:	ds 1
Num10MsLoop:	ds 1
Array:		ds 5
NumKeyPress:	ds 1 
NumDisplay:	ds 1
    
buttons		equ PORTB
LEDs		equ PORTC
NumInnerLoops	equ 125 ; 125
NumOuterLoops	equ 5 ; 5
Num10Ms		equ 100 ; for 20ms for now.  

; Tell the assembler that this should be put in Nonvolatile Program memory     
PSECT code 

main:
    call    ConfigurePorts
    clrf    LatestKeyPress
    clrf    NumKeyPress
    clrf    NumDisplay
main2:
    call    ReadKeyPress
    call    DisplayValue
    call    SaveValue
    incf    NumKeyPress,f   ; keeps count of how many presses there were
    movlw   5
    subwf   NumKeyPress,w
    btfss   STATUS,2	    ; if there were 5 presses, go to final display. 
    goto    main2    
    movlw   50		    ; 0.5 seconds (500ms)
    movwf   Num10MsLoop
    call    Delay
    call    FinalDisplay


spin:
    goto spin
    
; *********************************************
;        Subroutines
; *********************************************    

DisplayValue:
    ; This routine will display value in LatestKeyPressed
    ; Inputs: LatestKeyPressed
    ; Output: None
     
    movf    LatestKeyPress,w
    movwf   LEDs
    movlw   200	    ; 2 seconds (2000ms)
    movwf   Num10MsLoop
    call    Delay
    clrf    LEDs

    return
    
SaveValue:
    ; This routine will save the latest key pressed into an array of 5
    ; Inputs: LatestKeyPressed
    ; Output: None
    
    movf    NumKeyPress,w   ;Check if array 0 has been filled. If not, update.
    btfsc   STATUS,2
    bra	    UpdateArray0    
    movlw   1
    subwf   NumKeyPress,w   ;Check if array 1 has been filled and so on...
    btfsc   STATUS,2
    bra	    UpdateArray1    
    movlw   2
    subwf   NumKeyPress,w
    btfsc   STATUS,2
    bra	    UpdateArray2    
    movlw   3
    subwf   NumKeyPress,w
    btfsc   STATUS,2
    bra	    UpdateArray3    
    movlw   4
    subwf   NumKeyPress,w
    btfsc   STATUS,2
    bra	    UpdateArray4
    
UpdateArray0:
    movf    LatestKeyPress,w	
    movwf   Array
    bra	    DoneUpdate 
UpdateArray1:
    movf    LatestKeyPress,w
    movwf   Array + 1
    bra	    DoneUpdate    
UpdateArray2:
    movf    LatestKeyPress,w
    movwf   Array + 2
    bra	    DoneUpdate    
UpdateArray3:
    movf    LatestKeyPress,w
    movwf   Array + 3
    bra	    DoneUpdate      
UpdateArray4:
    movf    LatestKeyPress,w
    movwf   Array + 4
DoneUpdate:
    return
    
FinalDisplay:
    ; This routine will display all values in the array. 
    ; Inputs: Array
    ; Output: None
    
    movf    Array,w ; Display value in array 0
    movwf   LEDs
    movlw   100	    ; 1 seconds (1000ms)
    movwf   Num10MsLoop
    call    Delay
    clrf    LEDs
    movf    Array+1,w ; Display value in array 1 and so on...
    movwf   LEDs
    movlw   100
    movwf   Num10MsLoop
    call    Delay
    clrf    LEDs
    movf    Array+2,w
    movwf   LEDs
    movlw   100
    movwf   Num10MsLoop
    call    Delay
    clrf    LEDs
    movf    Array+3,w
    movwf   LEDs
    movlw   100
    movwf   Num10MsLoop
    call    Delay
    clrf    LEDs
    movf    Array+4,w
    movwf   LEDs
    movlw   100
    movwf   Num10MsLoop
    call    Delay
    clrf    LEDs
    clrf    NumKeyPress
    incf    NumDisplay,f
    
    movlw   150		; 1.5 seconds before displaying twice
    movwf   Num10MsLoop
    call    Delay
    movlw   150		; 1.5 seconds before displaying twice
    movwf   Num10MsLoop
    call    Delay
    
    movlw   2
    subwf   NumDisplay,w
    btfss   STATUS, 2	; make sure to display twice
    bra	    FinalDisplay

    return
    
ReadKeyPress:
    ; This routine will read button pressed by the user. It will have a routine
    ; to account for button bounce. 
    ; Inputs: 
    ; Output: LatestKeyPress
    
    call    IsAnyKeyPressed	; if no, keep inquiring. 
    movf    BtnIsPressed,w
    btfsc   STATUS,2
    bra	    ReadKeyPress
    movlw   2	    ; 20 ms
    movwf   Num10MsLoop
    call    Delay
    call    IsAnyKeyPressed	; if the second check is no, return to first.
    movf    BtnIsPressed,w
    btfsc   STATUS,2
    bra	    ReadKeyPress
    
    call    WhatKeyWasIt		; if confirmed button was pressed. 

ReadKeyPress2:
    call    IsAnyKeyPressed	; if yes, keep inquiring
    movf    BtnIsPressed,w
    btfss   STATUS,2
    bra	    ReadKeyPress2  
    movlw   2	    ; 20 ms
    movwf   Num10MsLoop
    call    Delay
    call    IsAnyKeyPressed	; if yes, go back to first inquiry. 
    movf    BtnIsPressed,w
    btfss   STATUS,2
    bra	    ReadKeyPress2
    
    ; if button is confirmed not pressed, then exit
    return
  
    
IsAnyKeyPressed:
    ; This routine will check to see if a button is pressed. 1 is pressed. 
    ; 0 is not pressed.
    ; Inputs: Current button press
    ; Output: BtnIsPressed 
    
    ;set the first three bits of PORTD to high
    movlw   0b111
    movwf   PORTD   ; keypad

    movlw   1	    ; 10 ms to make sure the pic has time to make it a high
    movwf   Num10MsLoop
    call    Delay
    
    ;check the last four bits of PORTD to see if any of them are high. If so, SetButtTo1. If not, clrf BtnIsPressed.
    movlw   0b11110000
    andwf   PORTD
    btfsc   STATUS,2	; checks if a button was pressed
    clrf    BtnIsPressed; set to zero if there is nothing pressed
    
    btfss   STATUS,2
    bra	    SetButtTo1
    bra	    GoHome
    
    
SetButtTo1:
    movlw   1
    movwf   BtnIsPressed 

GoHome:
    return
    
    
    
Delay:
    ; This routine will delay for NumTenMs * 10ms. Number of instruction cycles
    ; is... 4us/cycle * 4 cycles/ins.cycle * 125 loops = 2ms (1000us)
    ; Outer loop runs inner loop 5 times. 2ms * 5 loops = 10ms. Before using 
    ; Delay, set NumTenMs to the value desired. 
    ; Inputs: NumTenMs
    ; Output: None
    
    movlw   NumInnerLoops   ; 125
    movwf   DelayLoopInner 
    movlw   NumOuterLoops   ; 5
    movwf   DelayLoopOuter 
    
    LoopNumTenMs: ; run for the NumTenMs
	OuterLoop: ; run 5 times for 10ms
	    InnerLoop: ; 2ms 
		decf	DelayLoopInner,f    ; 1 ins. cycle
		btfss	STATUS,2	    ; 1(2) ins. cycle
		bra	InnerLoop	    ; 2 ins. cycles
	    movlw   NumInnerLoops
	    movwf   DelayLoopInner 
	    decf    DelayLoopOuter,f
	    btfss   STATUS,2
	    bra	    OuterLoop
	movlw	NumOuterLoops
	movwf	DelayLoopOuter 
	decf	Num10MsLoop,f
	btfss	STATUS,2
	bra LoopNumTenMs

    return
    
WhatKeyWasIt:
    ; This routine will check to see what button was pressed. 
    ; Inputs: Current button press
    ; Output: LatestKeyPress
CheckColumn1:      
    ; set keypad to 001 to check the left most column
    movlw   0b001
    movwf   PORTD
    ; if it was 0001 = 1, 0x01
    btfss   PORTD,4
    bra	    CheckC1R2
    movlw   0x01
    movwf   LatestKeyPress
    bra	    DoneCheck
CheckC1R2:     ; 0010 = 4, 0x04
    btfss   PORTD,5 ;row 2 is bit 5
    bra	    CheckC1R3
    movlw   0x04
    movwf   LatestKeyPress
    bra	    DoneCheck    
CheckC1R3:    ; 0100 = 7, 0x07
    btfss   PORTD,6 ;row 3 is bit 6
    bra	    CheckC1R4
    movlw   0x07
    movwf   LatestKeyPress
    bra	    DoneCheck 
CheckC1R4:    
    btfss   PORTD,7
    bra	    CheckColumn2
    movlw   0x0E
    movwf   LatestKeyPress
    bra	    DoneCheck 
CheckColumn2:      
    ; set keypad to 001 to check the left most column
    movlw   0b010
    movwf   PORTD
    ; if it was 0001 = 1, 0x01
    btfss   PORTD,4
    bra	    CheckC2R2
    movlw   0x02
    movwf   LatestKeyPress
    bra	    DoneCheck
CheckC2R2:     ; 0010 = 4, 0x04
    btfss   PORTD,5 ;row 2 is bit 5
    bra	    CheckC2R3
    movlw   0x05
    movwf   LatestKeyPress
    bra	    DoneCheck    
CheckC2R3:    ; 0100 = 7, 0x07
    btfss   PORTD,6 ;row 3 is bit 6
    bra	    CheckC2R4
    movlw   0x08
    movwf   LatestKeyPress
    bra	    DoneCheck 
CheckC2R4:    
    btfss   PORTD,7
    bra	    CheckColumn3
    movlw   0x0A
    movwf   LatestKeyPress
    bra	    DoneCheck 
CheckColumn3:      
    ; set keypad to 001 to check the left most column
    movlw   0b100
    movwf   PORTD
    ; if it was 0001 = 1, 0x01
    btfss   PORTD,4
    bra	    CheckC3R2
    movlw   0x03
    movwf   LatestKeyPress
    bra	    DoneCheck
CheckC3R2:     ; 0010 = 4, 0x04
    btfss   PORTD,5 ;row 2 is bit 5
    bra	    CheckC3R3
    movlw   0x06
    movwf   LatestKeyPress
    bra	    DoneCheck    
CheckC3R3:    ; 0100 = 7, 0x07
    btfss   PORTD,6 ;row 3 is bit 6
    bra	    CheckC3R4
    movlw   0x09
    movwf   LatestKeyPress
    bra	    DoneCheck 
CheckC3R4:    
    movlw   0x0F
    movwf   LatestKeyPress
DoneCheck:
    return
 
    
    
ConfigurePorts:
    ; This routine will configure the ports as Digital Rather than Analog
    ; as Inputs rather than Outputs, and clear the Data register for LATCH read
    ; Inputs: No such thing for assembly
    ; Output: No such thing for assembly
    
    BANKSEL ANSELA ; Set BSR to Bank 62
    clrf ANSELA    ; Make pins digital
    clrf ANSELB
    clrf ANSELC
    clrf ANSELD
    clrf ANSELE
    
    BANKSEL TRISA ; Set BSR to Bank 0
    clrf PORTA    ; clear all pins on Ports
    clrf PORTB
    clrf PORTC
    clrf PORTD
    clrf PORTE
    movlw 0x00	 ; make the port pin output
    movwf TRISC	 ; using
    movlw 0xFF   ; make the port pins inputs
    movwf TRISB	 ; using
    movwf TRISA
    movlw 0b11110000
    movwf TRISD
    movwf TRISE
    
    return

FlashLEDs: ;debugging purposes only
    clrf LEDs
    movlw   200	    ; 2 seconds (2000ms)
    movwf   Num10MsLoop
    call Delay
    movlw 0x55
    movwf LEDs
    movlw   200	    ; 2 seconds (2000ms)
    movwf   Num10MsLoop
    call Delay
    clrf LEDs
    movlw   200	    ; 2 seconds (2000ms)
    movwf   Num10MsLoop
    call Delay
    movlw 0x55
    movwf LEDs
    movlw   200	    ; 2 seconds (2000ms)
    movwf   Num10MsLoop
    call Delay
    clrf LEDs
    return

    
    
END resetVec


