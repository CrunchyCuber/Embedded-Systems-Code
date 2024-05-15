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
Mode:		    ds 1 ; Display mode currently selected. Three modes. L-R, R-L, C-O
Change_Mode:	    ds 1 ; 1 when button press commands a change of display
Previous_Mode:	    ds 1 ; Display mode prior to the update about to happen
DelayLoopInner:	    ds 1
DelayLoopOuter:	    ds 1
NumberOfLoops:	    ds 1 ; ds allocates space but does not initilize a value

NumberOfOuterLoops equ 250  
NumberOfInnerLoops equ 125
buttons equ PORTB
LEDs equ PORTC

; Tell the assembler that this should be put in Nonvolatile Program memory     
PSECT code 

main:
    call    ConfigurePorts
    clrf    LEDs    ; blank the display
    movf    buttons,w ; initilize mode to be the current buttons setting
    movwf   Mode 
    clrf    Previous_Mode 
    
flashLEDs:
    clrf Change_Mode
    
    ; If button is being pressed, check if it was different. If not, continue 
    ; to ClearChangeMode.
    movf buttons ,w     
    btfsc STATUS,2  
    bra ClearChangeMode
   
    ; If button is different from last time, continue to update variables.
    ; Else, continue to ClearChangeMode.
    movf buttons,w
    subwf Mode,w   
    btfsc STATUS,2 
    bra ClearChangeMode
   
    ; Update all variables
    movlw 1
    movwf Change_Mode
    movf Mode, w
    movwf Previous_Mode
    movf buttons, w
    movwf Mode
    bra UpdateTheDisplay
	
ClearChangeMode:
    clrf Change_Mode  
    
UpdateTheDisplay:
    call    UpdateDisplay
    goto    flashLEDs
    
spin:
    goto spin
    
; *********************************************
;        Subroutines
; *********************************************    
ConfigurePorts:
    ; This routine will configure the ports as Digital Rather than Analog
    ; as Inputs rather than Outputs, and clear the Data register for LATCH read
    ; Inputs: No such thing for assembly
    ; Output: No such thing for assembly
    
    BANKSEL ANSELA  ; Set BSR to Bank 62
    clrf ANSELA	    ; Make pins digital
    clrf ANSELB
    clrf ANSELC
    clrf ANSELD
    clrf ANSELE
    
    BANKSEL TRISA   ; Set BSR to Bank 0
    clrf PORTA	    ; clear all pins on Ports
    clrf PORTB
    clrf PORTC
    clrf PORTD
    clrf PORTE
    movlw 0x00	    ; make the port pin an output, connected to LEDs 
    movwf TRISC
    movlw 0xFF	    ; make the port pins inputs
    movwf TRISA
    movwf TRISB
    
    movwf TRISD
    movwf TRISE
    
    return

    
Delay:
    ; This routine will delay the code for around 0.5 seconds. Calculations: 
    ; innerLoop: (250 outerloops)*(125 loops)*(4 instruction cycles)*4us= 500ms. 
    ; outerloop: (250 loops)*(6 instruction cycles)*(4us) = 6ms. 
    ; Delay: 4 instruction cycles * 4us = 16us. 
    ; Total: 500ms + 6ms + 16us = 0.5606s
    ; Inputs: NumberOIfInnerLoops, NumberOfOuterLoops
    ; Output: None
    
    movlw NumberOfInnerLoops ;#125
    movwf DelayLoopInner 
    movlw NumberOfOuterLoops ;#250
    movwf DelayLoopOuter
    
    outerLoop:
	innerLoop:
	    decf DelayLoopInner,f ;decriment until variable is zero. 
	    btfss STATUS,2 
	    bra innerLoop 
	movlw NumberOfInnerLoops
	movwf DelayLoopInner 
	decf DelayLoopOuter,f 
	btfss STATUS, 2
	bra outerLoop 
	
delaydone:
    return
      

UpdateDisplay:
    ; This routine will update the display based on what mode the user chooses.
    ; Inputs: Mode, Previous_Mode, LEDs
    ; Output: LEDs
    
    ; Change mode based on if Change_Mode is 1
    movlw 1 
    subwf Mode,w ;If mode is 1, STATUS bit 2 is 1.
    btfss STATUS,2 ;Goes to next check if Mode is not 1
    bra chkRtoL
    clrf LEDs ;clear LEDs
    bra UpdateDisplayDone ;Leave
chkRtoL:
    movlw   2
    subwf   Mode,w 
    btfss   STATUS,2 ;is mode = 2? Yes call UpdateRtoL
    bra	    chkLtoR
    bra	    UpdateRtoL 	
chkLtoR:
    movlw   4
    subwf   Mode,w ;is mode = 4? Yes call UpdateLtoR
    btfss   STATUS,2
    bra	    chkUCO
    bra	    UpdateLtoR
chkUCO:
    movlw   8
    subwf   Mode,w ;is mode = 8? Yes call UpdateUCO
    btfss   STATUS,2
    bra	    noneOfTheAbove
    bra	    UpdateUCO    
noneOfTheAbove:
    clrf    LEDs
    bra	    UpdateDisplayDone ;Leave
UpdateDisplayDone:
    call    Delay
    return
         
;Update Right to Left sequence
UpdateRtoL:    
    ;If Change_Mode is 1, check if previous mode was LtoR. Else check if LEDs
    ;are 0x08
    movlw   1
    subwf   Change_Mode,w
    btfsc   STATUS,2 ;skip if Mode is not 1
    bra	    PreviousModeLtoR   
    
LEDs0x80:
    ;If LEDs are 0x80, then set LEDs to 1. Else logical shift to the left. 
    movlw   0x80
    subwf   LEDs,w
    btfsc   STATUS,2	;if LEDs is not 0x08, skip
    bra	    SetLEDsTo1	;Set LED to 1
    lslf    LEDs, f	;shift LED to the left
    bra	    UpdateDisplayDone ;Return    

PreviousModeLtoR:
    ;If Previous_Mode is 4(LtoR) then check if LEDs are 0x08. Else check if 
    ;Previous_Mode is clear.
    movlw   4
    subwf   Previous_Mode,w
    btfsc   STATUS,2	;if Mode is not 4, skip
    bra	    LEDs0x80	
    
    ;If Previous_Mode was clear, set LEDs to 1. Else check if Previous_Mode is 0x18
    movlw   1
    subwf   Mode,w
    btfsc   STATUS,2 ; if Mode is not 1, skip
    bra	    SetLEDsTo1
    
    ;Previous Mode Center Out
    ;If LEDs is 0x18, then set LEDs to 2
    movlw   0x18    
    subwf   LEDs,w
    btfsc   STATUS,2 ; if LEDs is not 0x18, skip
    bra	    SetLEDsTo20
    ;If LEDs is 0x24, then set LEDs to 40
    movlw   0x24    
    subwf   LEDs,w
    btfsc   STATUS,2 
    bra	    SetLEDsTo40
    ;If LEDs is 0x42, then set LEDs to 80
    movlw   0x42    
    subwf   LEDs,w
    btfsc   STATUS,2 
    bra	    SetLEDsTo80
    ;Everything else
    bra	    SetLEDsTo1 
      
;Update Left to Right sequence  
UpdateLtoR:    
    ;If Change_Mode is 1, check if previous mode is RtoL. Else, check if LEDs 
    ;are 0x01
    movlw   1
    subwf   Change_Mode,w
    btfsc   STATUS,2	;skip if Mode is not 1
    bra	    PreviousModeRtoL	;Check if previous mode is RtoL
 
LEDs0x01:
    ;If LEDs are 0x01, set LEDs to 80. Else, logical shift to the right.
    movlw   0x01
    subwf   LEDs,w
    btfsc   STATUS,2	;if LEDs is not 0x01, skip
    bra	    SetLEDsTo80 ;Set LED to 80
    lsrf    LEDs, f	;shift LED to the left
    bra	    UpdateDisplayDone ;Return    

PreviousModeRtoL:
    ;If Previous_Mode is 2, set LEDs to 0x01. Else, check if previous mode is 1.
    movlw   2
    subwf   Previous_Mode,w
    btfsc   STATUS,2	; if Mode is not 4, skip
    bra	    LEDs0x01	;Set LEDs to 0x01
    
    ;If Previous_Mode is 1(Clear), then set LEDs to 80. Else Check if 
    ;Previous_Mode is 8(Center out)
    movlw   1	;Is Previous_Mode 1 (Clear)?
    subwf   Previous_Mode,w
    btfsc   STATUS,2	;if Mode is not 1, skip
    bra	    SetLEDsTo80
    
    ;Previous Mode Center Out?
    ;If LEDs is 0x18, then set LEDs to 4
    movlw   0x18    ;LEDs0x18?
    subwf   LEDs,w
    btfsc   STATUS,2 ; if LEDs is not 0x18, skip
    bra	    SetLEDsTo4
    ;If LEDs is 0x24, then set LEDs to 2
    movlw   0x24    
    subwf   LEDs,w
    btfsc   STATUS,2 
    bra	    SetLEDsTo2
    ;If LEDs is 0x42, then set LEDs to 1
    movlw   0x42    ;LEDs0x42?
    subwf   LEDs,w
    btfsc   STATUS,2 ; if LEDs is not 0x18, skip
    bra	    SetLEDsTo1
    ;Everything else
    bra	    SetLEDsTo80	
  
;Update Center to Out sequence
UpdateUCO: 
    movlw   0xFF
    movwf   LEDs,w
    ;if LEDs is 0x18, set to 0x24
    movlw   0b00011000
    subwf   LEDs,w
    btfsc   STATUS,2 
    bra	    SetLEDsTo24
    ;else if LEDs is 0x24, set to 0x42
    movlw   0b00100100
    subwf   LEDs,w
    btfsc   STATUS,2
    bra	    SetLEDsTo42
    ;else if LEDs is 0x42, set to 0x81
    movlw   0b01000010
    subwf   LEDs,w
    btfsc   STATUS,2 
    bra	    SetLEDsTo81
    ;else set to 0x18
    bra	    SetLEDsTo18
   
;various LED sets
SetLEDsTo1:
    movlw   1
    movwf   LEDs
    bra	    UpdateDisplayDone
SetLEDsTo2:
    movlw   0x2
    movwf   LEDs
    bra	    UpdateDisplayDone
SetLEDsTo4:
    movlw   0x4
    movwf   LEDs
    bra	    UpdateDisplayDone
SetLEDsTo20:
    movlw   0x20
    movwf   LEDs
    bra	    UpdateDisplayDone
SetLEDsTo40:
    movlw   0x40
    movwf   LEDs
    bra	    UpdateDisplayDone
SetLEDsTo80:
    movlw   0x80
    movwf   LEDs
    bra	    UpdateDisplayDone
SetLEDsTo18:
    movlw   0x18
    movwf   LEDs
    bra	    UpdateDisplayDone
SetLEDsTo24:
    movlw   0x24
    movwf   LEDs
    bra	    UpdateDisplayDone
SetLEDsTo42:
    movlw   0x42
    movwf   LEDs
    bra	    UpdateDisplayDone
SetLEDsTo81:
    movlw   0x81
    movwf   LEDs
    bra	    UpdateDisplayDone
  
    
    
END resetVec


