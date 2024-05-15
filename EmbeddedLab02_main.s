; Including this allows us to use some assembler macros
#include <xc.inc>
CONFIG WDTE=OFF ; Turn off the Watch Dog Timer (necessary for debug)  

; Insert command at Reset Vector location that tells
; the system to go to the start of Main Code
PSECT resetVec, class=CODE, abs
    ORG  0x00000 ; abs and ORG 0 put this at Program Memory 0
resetVec:
    goto main
 
; Allocate memory for program variables    
PSECT udata
Min:	    ds 1
Max:	    ds 1
Avg:	    ds 1
NumArray:   ds 8

; Tell the assembler that this should be put in Nonvolatile Program memory     
PSECT code 

main:
    call ConfigurePorts
    call LoadMemory
    call CalcMin
    call CalcMax
    call CalcAvg
    
disp:
    ; if Sw1 Display Min
    movlw 0b00000001
    subwf PORTB,w   ; Check if the first button was pressed. Subtract 00000001 
		    ; from PortB if they are the same, the result will be zero
    btfss STATUS,2  ; see if the subtraction resulted in a zero, if it did
		    ; jump over the command that goes to the next IF check
    goto CheckMax
    movf Min,w	    ; put Min in WREG
    goto DispW	    ; display it
    
    
; if Sw2 Display Max
CheckMax: 
    movlw 0b00000010
    subwf PORTB,w   ; Check if the second button was pressed. 
    btfss STATUS,2  ; if pressed, skip the next line
    goto CheckAvg
    movf Max,w	    ; put Max in WREG
    goto DispW	    ; display it


CheckAvg:
    ; if Sw3 = 1 Display Avg
    movlw 0b00000100
    subwf PORTB,w   ; Check if the third button was pressed
    btfss STATUS,2  ; if pressed, skip the next line
    goto ClearDisp
    movf Avg,w	    ; put Avg in WREG
    goto DispW	    ; display it
    
    
ClearDisp:
    ; anything else, clear display
    clrw
    
DispW:
    ; display what is in the WREG
    movwf PORTA
    
    goto disp
    
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
    movlw 0x00   ; make the port pins outputs
    movwf TRISA
    movlw 0xFF	; make the port pins inputs
    movwf TRISB
    movwf TRISC
    movwf TRISD
    movwf TRISE
    
    return
    
    
LoadMemory:
    ; This routine will load 8 numbers into an allocated block of memory 
    ; called NumArray. The minimum will exist twice and the sum of the values 
    ; will be less than 225.
    ; Input: None
    ; Output: None
    ; Numbers are: 13,13,22,23,35,44,52,14. Sum: 216, Avg: 27
    movlw 35
    movwf NumArray
    movlw 13
    movwf NumArray+1
    movlw 52
    movwf NumArray+2
    movlw 23
    movwf NumArray+3
    movlw 13
    movwf NumArray+4
    movlw 44
    movwf NumArray+5
    movlw 22
    movwf NumArray+6
    movlw 14
    movwf NumArray+7
    
    return
    
CalcMin:
    ; This routine will find the smallest value in the array and save this 
    ; value to the variable Min
    ; Input: None
    ; Output: Minimum value
    
    movf NumArray,w
    movwf Min
    subwf NumArray+1,w ;is WREG > FileRegister? Yes, C = 0      
    btfsc STATUS,0  ; see if the subtraction resulted in a zero, 
		    ; jump over the command that goes to the next
    goto CalcMin1
    movf NumArray+1,w ;if WREG < FileReg, then it is updated to the new min
    movwf Min
CalcMin1:
    movf Min,w 
    subwf NumArray+2,w ;continues to check NumArray+n   
    btfsc STATUS,0  
    goto CalcMin2
    movf NumArray+2,w
    movwf Min
CalcMin2:
    movf Min,w
    subwf NumArray+3,w ;continues to check NumArray+n   
    btfsc STATUS,0  
    goto CalcMin3
    movf NumArray+3,w
    movwf Min    
CalcMin3:
    movf Min,w 
    subwf NumArray+4,w ;continues to check NumArray+n 
    btfsc STATUS,0  
    goto CalcMin4
    movf NumArray+4,w
    movwf Min
CalcMin4:
    movf Min,w
    subwf NumArray+5,w ;continues to check NumArray+n  
    btfsc STATUS,0  
    goto CalcMin5
    movf NumArray+5,w
    movwf Min
CalcMin5:
    movf Min,w 
    subwf NumArray+6,w ;continues to check NumArray+n 
    btfsc STATUS,0  
    goto CalcMin6
    movf NumArray+6,w
    movwf Min
CalcMin6:
    movf Min,w 
    subwf NumArray+7,w ;continues to check NumArray+n 
    btfsc STATUS,0  
    return 
    movf NumArray+7,w
    movwf Min

    

    
CalcMax:
    ; This routine will find the largest value in the array and save this 
    ; value to the variable Max 
    ; Input: None
    ; Output: Max value

    movf NumArray,w
    movwf Max
    subwf NumArray+1,w ;is WREG =< FileRegister? If yes, C = 1, skip next line    
    btfss STATUS,0 ; set(1): skip next line if C = 1.  
    goto CalcMax1
    movf NumArray+1,w ;if WREG < FileReg, then it is updated to the new max
    movwf Max
CalcMax1:
    movf Max,w
    subwf NumArray+2,w ;continues to check NumArray+n 
    btfss STATUS,0  
    goto CalcMax2
    movf NumArray+2,w 
    movwf Max
CalcMax2:
    movf Max,w
    subwf NumArray+3,w ;continues to check NumArray+n    
    btfss STATUS,0 
    goto CalcMax3
    movf NumArray+3,w 
    movwf Max    
CalcMax3:
    movf Max,w
    subwf NumArray+4,w ;continues to check NumArray+n    
    btfss STATUS,0 
    goto CalcMax4
    movf NumArray+4,w 
    movwf Max
CalcMax4:
    movf Max,w
    subwf NumArray+5,w ;continues to check NumArray+n   
    btfss STATUS,0  
    goto CalcMax5
    movf NumArray+5,w 
    movwf Max    
CalcMax5:
    movf Max,w
    subwf NumArray+6,w ;continues to check NumArray+n    
    btfss STATUS,0 
    goto CalcMax6
    movf NumArray+6,w 
    movwf Max
CalcMax6:
    movf Max,w
    subwf NumArray+7,w ;continues to check NumArray+n   
    btfss STATUS,0  
    return
    movf NumArray+7,w 
    movwf Max
    
CalcAvg:
    ; This routine will calculate the average value in the array and save this 
    ; value to the variable Avg
    ; Input: None
    ; Output: Average value
    clrw
    addwf NumArray,w
    addwf NumArray+1,w
    addwf NumArray+2,w
    addwf NumArray+3,w
    addwf NumArray+4,w
    addwf NumArray+5,w
    addwf NumArray+6,w
    addwf NumArray+7,w
    movwf Avg
    lsrf Avg
    lsrf Avg
    lsrf Avg ; = 27
    return

END resetVec


