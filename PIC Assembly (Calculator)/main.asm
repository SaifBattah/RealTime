; Done by Saif, Khaled, Nour and Najwa.
PROCESSOR 16F877A
INCLUDE "P16F877A.INC"	
__config 0X3731

; Define variables for the first & second 5-digit number
number1_tenthous EQU 0x20
number1_thous EQU 0x21
number1_hunds EQU 0x22
number1_tens EQU 0x23
number1_ones EQU 0x24
number2_tenthous EQU 0x25
number2_thous EQU 0x26
number2_hunds EQU 0x27
number2_tens EQU 0x28
number2_ones EQU 0x29

MAX_COUNT  EQU 7 ; Maximum value for the COUNT variable 
V_SEVEN EQU 7
MAX_PERIOD EQU 2
TIME_TO_CLEAR_LCD EQU 3
COUNT EQU 0x31
TMR1_OVERFLOW EQU 0x32
ADDITION_SIGN EQU 0x33
DIVISION_SIGN EQU 0x34
MOD_SIGN  EQU 0x35
SIGN_COUNT EQU 3
V_ZERO EQU 0
V_ONE EQU 1
COUNT_FLAG EQU 0x36
SAVED_OPERATION EQU 0x37
MAX_NINE EQU D'10'
MAX_FIVE EQU 6
MAX_THREE EQU 4
IS_THIS_DIGIT_1 EQU 0x38
IS_THIS_DIGIT_2 EQU 0x39
IS_THIS_DIGIT_3 EQU 0x40
IS_THIS_DIGIT_4 EQU 0x41
DOES_D1_EQU_6 EQU 0x42
DOES_D2_EQU_5 EQU 0x43
DOES_D3_EQU_5 EQU 0x44
DOES_D4_EQU_3 EQU 0x45
temp EQU 0x46
temp2 EQU 0x47
Sdigit1 EQU 0x48 ; sum ones
Sdigit2 EQU 0x49
Sdigit3 EQU 0x50
Sdigit4 EQU 0x51
Sdigit5 EQU 0x52 ; sum tenthous
Sdigit6 EQU 0x53 ; carry
carry EQU 0x54
DIV_COUNT EQU 0x55

temp1 equ 0x56
sub1 equ 0x57
sub2 equ 0x58
sub3 equ 0x59
sub4 equ 0x5A
sub5 equ 0x5B
divR1 equ 0x5C
divR2 equ 0x5D
divR3 equ 0x5E
divR4 equ 0x5F
divR5 equ 0x60
remainder1 equ 0x61
remainder2 equ 0x62
remainder3 equ 0x63
remainder4 equ 0x64
remainder5 equ 0x65
compare equ 0x66
compare2 equ 0x67
Y_N_RESULT EQU 0x68
SKIP_NUMBER2 EQU 0x69
; The instructions should start from here
ORG 0x00
GOTO init

ORG 0x04
GOTO ISR

; The init for our program
init:

BANKSEL TRISB
BSF TRISB, TRISB0

BANKSEL INTCON
BSF INTCON, GIE
BSF INTCON, INTE

BANKSEL TRISD
CLRF TRISD

BANKSEL PORTD

; Initialize the data memory for the variables
CLRF number1_tenthous
CLRF number1_thous
CLRF number1_hunds
CLRF number1_tens
CLRF number1_ones
CLRF number2_tenthous
CLRF number2_thous
CLRF number2_hunds
CLRF number2_tens
CLRF number2_ones
CLRF COUNT
CLRF TMR1_OVERFLOW
CLRF COUNT_FLAG
CLRF SAVED_OPERATION
CLRF temp
CLRF temp2
CLRF Sdigit1
CLRF Sdigit2
CLRF Sdigit3
CLRF Sdigit4
CLRF Sdigit5
CLRF Sdigit6
CLRF carry 
CLRF SKIP_NUMBER2

MOVLW D'10'
MOVWF temp
MOVLW 0x01
MOVWF IS_THIS_DIGIT_1
MOVLW 0x01
MOVWF IS_THIS_DIGIT_2
MOVLW 0x01
MOVWF IS_THIS_DIGIT_3
MOVLW 0x01
MOVWF IS_THIS_DIGIT_4
MOVLW 0x01
MOVWF DOES_D1_EQU_6
MOVLW 0x01
MOVWF DOES_D2_EQU_5
MOVLW 0x01
MOVWF DOES_D3_EQU_5
MOVLW 0x01
MOVWF DOES_D4_EQU_3

MOVLW '+'
MOVWF ADDITION_SIGN
MOVLW '/'
MOVWF DIVISION_SIGN
MOVLW '%'
MOVWF MOD_SIGN

GOTO start

; When intruput happend the program will enter here
ISR:
BANKSEL INTCON
BCF INTCON, INTE
BCF INTCON, INTF

MOVLW 0x00
XORWF COUNT_FLAG, W
BTFSS STATUS, Z
GOTO Button_ISR2 ; this is for the operations
GOTO Button_ISR ; this is for the numbers 1 and 2

RETFIE ; Return from ISR


Timer1_SET_1:
; 1- set value of number1_tenthous to COUNT
BANKSEL number1_tenthous
MOVF COUNT, W
MOVWF number1_tenthous

; check if DIGIT at max value
MOVLW 0x06
SUBWF number1_tenthous, W
BTFSC STATUS, Z
CLRF DOES_D1_EQU_6

; the value is set -> set the flag of this digit that its not his turn anymore
CLRF IS_THIS_DIGIT_1

; 2- set cursor at cell 0 row 2
MOVLW 0xC0 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number1_tenthous, W
ADDLW '0'
BSF Select, RS
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO loop2
;----------------------------------------------------------------------------
Timer1_SET_2:
; 1- set value of number1_thous to COUNT
BANKSEL number1_thous
MOVF COUNT, W
MOVWF number1_thous

; check if DIGIT at max value
MOVLW 0x05
SUBWF number1_thous, W
BTFSC STATUS, Z
CLRF DOES_D2_EQU_5

; the value is set -> set the flag of this digit that its not his turn anymore
CLRF IS_THIS_DIGIT_2


; 2- set cursor at cell 0 row 2
MOVLW 0xC1 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number1_thous, W
ADDLW '0'
BSF Select, RS
CALL send

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 4- reset COUNT
CLRF COUNT 

; 5- start the second loop
GOTO loop3
;---------------------------------------------------------------------------
Timer1_SET_3:
; 1- set value of number1_hunds to COUNT
BANKSEL number1_hunds
MOVF COUNT, W
MOVWF number1_hunds

; check if DIGIT at max value
MOVLW 0x05
SUBWF number1_hunds, W
BTFSC STATUS, Z
CLRF DOES_D3_EQU_5

; the value is set -> set the flag of this digit that its not his turn anymore
CLRF IS_THIS_DIGIT_3

; 2- set cursor at cell 0 row 2
MOVLW 0xC2 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number1_hunds, W
ADDLW '0'
BSF Select, RS
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO loop4
;---------------------------------------------------------------------------
Timer1_SET_4:
; 1- set value of number1_tens to COUNT
BANKSEL number1_tens
MOVF COUNT, W
MOVWF number1_tens

; check if DIGIT at max value
MOVLW 0x03
SUBWF number1_tens, W
BTFSC STATUS, Z
CLRF DOES_D4_EQU_3

; the value is set -> set the flag of this digit that its not his turn anymore
CLRF IS_THIS_DIGIT_4

; 2- set cursor at cell 0 row 2
MOVLW 0xC3 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number1_tens, W
ADDLW '0'
BSF Select, RS
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO loop5
;----------------------------------------------------------------------------
Timer1_SET_OPERATION:
; 1- set value of number1_tenthous to COUNT
BANKSEL number1_ones
MOVF COUNT, W
MOVWF number1_ones

; 2- set cursor at cell 0 row 2
MOVLW 0xC4 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number1_ones, W
ADDLW '0'
BSF Select, RS
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW


; 5- start the second loop
MOVLW 0x01
MOVWF IS_THIS_DIGIT_1
MOVLW 0x01
MOVWF IS_THIS_DIGIT_2
MOVLW 0x01
MOVWF IS_THIS_DIGIT_3
MOVLW 0x01
MOVWF IS_THIS_DIGIT_4
MOVLW 0x01
MOVWF DOES_D1_EQU_6
MOVLW 0x01
MOVWF DOES_D2_EQU_5
MOVLW 0x01
MOVWF DOES_D3_EQU_5
MOVLW 0x01
MOVWF DOES_D4_EQU_3

GOTO loop6
;----------------------------------------------------------------------------
Timer1_SET_21:
; 1- Save the operation
MOVLW 0x00
MOVWF SAVED_OPERATION

; 2- set cursor at cell 0 row 2
MOVLW 0xC5 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVLW '+' ; E
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO loop7

Timer1_SET_22:
; 1- Save the operation
MOVLW 0x01
MOVWF SAVED_OPERATION

; 2- set cursor at cell 0 row 2
MOVLW 0xC5 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVLW '/' ; E
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO loop7

Timer1_SET_23:
; 1- Save the operation
MOVLW 0x02
MOVWF SAVED_OPERATION

; 2- set cursor at cell 0 row 2
MOVLW 0xC5 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVLW '%' ; E
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO loop7

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

;-------------START OF NUMBER2--------------------------------------------
Timer1_SET_201:
; 1- set value of number2_tenthous to COUNT
BANKSEL number2_tenthous
MOVF COUNT, W
MOVWF number2_tenthous

; check if DIGIT at max value
MOVLW 0x06
SUBWF number2_tenthous, W
BTFSC STATUS, Z
CLRF DOES_D1_EQU_6

; the value is set -> set the flag of this digit that its not his turn anymore
CLRF IS_THIS_DIGIT_1


; 2- set cursor at cell 0 row 2
MOVLW 0xC6 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number2_tenthous, W
ADDLW '0'
BSF Select, RS
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO loop8
;----------------------------------------------------------------------------
Timer1_SET_202:
; 1- set value of number2_thous to COUNT
BANKSEL number2_thous
MOVF COUNT, W
MOVWF number2_thous

; check if DIGIT at max value
MOVLW 0x05
SUBWF number2_thous, W
BTFSC STATUS, Z
CLRF DOES_D2_EQU_5

; the value is set -> set the flag of this digit that its not his turn anymore
CLRF IS_THIS_DIGIT_2

; 2- set cursor at cell 0 row 2
MOVLW 0xC7 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number2_thous, W
ADDLW '0'
BSF Select, RS
CALL send

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 4- reset COUNT
CLRF COUNT 

; 5- start the second loop
GOTO loop9
;---------------------------------------------------------------------------
Timer1_SET_203:
; 1- set value of number2_hunds to COUNT
BANKSEL number2_hunds
MOVF COUNT, W
MOVWF number2_hunds

; check if DIGIT at max value
MOVLW 0x05
SUBWF number2_hunds, W
BTFSC STATUS, Z
CLRF DOES_D3_EQU_5

; the value is set -> set the flag of this digit that its not his turn anymore
CLRF IS_THIS_DIGIT_3

; 2- set cursor at cell 0 row 2
MOVLW 0xC8 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number2_hunds, W
ADDLW '0'
BSF Select, RS
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO loop10
;---------------------------------------------------------------------------
Timer1_SET_204:
; 1- set value of number2_tens to COUNT
BANKSEL number2_tens
MOVF COUNT, W
MOVWF number2_tens

; check if DIGIT at max value
MOVLW 0x03
SUBWF number2_tens, W
BTFSC STATUS, Z
CLRF DOES_D4_EQU_3

; the value is set -> set the flag of this digit that its not his turn anymore
CLRF IS_THIS_DIGIT_4

; 2- set cursor at cell 0 row 2
MOVLW 0xC9 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number2_tens, W
ADDLW '0'
BSF Select, RS
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO loop11
;---------------------------------
Timer1_SET_205:
; 1- set value of number2_ones to COUNT
BANKSEL number2_ones
MOVF COUNT, W
MOVWF number2_ones

; 2- set cursor at cell 0 row 2
MOVLW 0xCA ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVF number2_ones, W
ADDLW '0'
BSF Select, RS
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; 5- start the second loop
GOTO CLEAR_LCD
;-------------END OF NUMBER2----------------------------------------------
;--------------- FOR Y/N -------------------------------------------------
Timer1_SET_YN:
; 1- set value of number2_ones to COUNT
BANKSEL Y_N_RESULT
MOVF COUNT, W
MOVWF Y_N_RESULT

; check if its 1 or 2
MOVLW 0x1
XORWF Y_N_RESULT, W
BTFSS STATUS, Z
GOTO CHOOSE_N ; 2 -> N 
GOTO CHOOSE_Y ; 1 -> Y


CHOOSE_Y:
; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW
;task
GOTO RE_SELECT_OPERATION
;end of task


CHOOSE_N:
; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW

; TASK
goto init
; END OF TASK
;--------------- END Y/N -------------------------------------------------


RETFIE ; Return from ISR


    
Button_ISR:
; is this digit 1 ?
MOVLW 0x1
XORWF IS_THIS_DIGIT_1, W
BTFSS STATUS, Z
GOTO CHECK_AFTERD1 ; not digit 1 
GOTO ISR_MAX_SIX ; yes digit 1

CHECK_AFTERD1:
; is this digit 2 ?
MOVLW 0x1
XORWF IS_THIS_DIGIT_2, W
BTFSS STATUS, Z
GOTO CHECK_AFTERD2 ; not digit 2 
GOTO CHECK_IF_D1_IS_MAX ; yes digit 2

CHECK_IF_D1_IS_MAX:
MOVLW 0x0
XORWF DOES_D1_EQU_6, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D1 != 6
GOTO ISR_MAX_FIVE ; if D1 == 6

CHECK_AFTERD2:
; is this digit 3 ?
MOVLW 0x1
XORWF IS_THIS_DIGIT_3, W
BTFSS STATUS, Z
GOTO CHECK_AFTERD3 ; not digit 3
GOTO CHECK_IF_D1_IS_MAX_v2 ; yes digit 3

CHECK_IF_D1_IS_MAX_v2:
MOVLW 0x0
XORWF DOES_D1_EQU_6, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D1 != 6
GOTO CHECK_IF_D2_IS_MAX ; if D1 == 6

CHECK_IF_D2_IS_MAX:
MOVLW 0x0
XORWF DOES_D2_EQU_5, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D2 != 5
GOTO ISR_MAX_FIVE ; if D2 == 5

CHECK_AFTERD3:
; is this digit 4 ?
MOVLW 0x1
XORWF IS_THIS_DIGIT_4, W
BTFSS STATUS, Z
GOTO CHECK_IF_D1_IS_MAX_v4 ; not digit 4
GOTO CHECK_IF_D1_IS_MAX_v3 ; yes digit 4

CHECK_IF_D1_IS_MAX_v3:
MOVLW 0x0
XORWF DOES_D1_EQU_6, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D1 != 6
GOTO CHECK_IF_D2_IS_MAX_v2 ; if D1 == 6

CHECK_IF_D2_IS_MAX_v2:
MOVLW 0x0
XORWF DOES_D2_EQU_5, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D2 != 5
GOTO CHECK_IF_D3_IS_MAX ; if D2 == 5

CHECK_IF_D3_IS_MAX:
MOVLW 0x0
XORWF DOES_D3_EQU_5, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D3 != 5
GOTO ISR_MAX_THREE ; if D3 == 5

CHECK_IF_D1_IS_MAX_v4:
MOVLW 0x0
XORWF DOES_D1_EQU_6, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D1 != 6
GOTO CHECK_IF_D2_IS_MAX_v3 ; if D1 == 6

CHECK_IF_D2_IS_MAX_v3:
MOVLW 0x0
XORWF DOES_D2_EQU_5, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D2 != 5
GOTO CHECK_IF_D3_IS_MAX_v2 ; if D2 == 5

CHECK_IF_D3_IS_MAX_v2:
MOVLW 0x0
XORWF DOES_D3_EQU_5, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D3 != 5
GOTO CHECK_IF_D4_IS_MAX ; if D3 == 5

CHECK_IF_D4_IS_MAX:
MOVLW 0x0
XORWF DOES_D4_EQU_3, W
BTFSS STATUS, Z
GOTO ISR_MAX_NINE ; if D4 != 3
GOTO ISR_MAX_FIVE ; if D4 == 3







Button_ISR2:
MOVLW 0x01
XORWF COUNT_FLAG, W
BTFSS STATUS, Z
GOTO Button_ISR_ROUNDS ; this is for the operations
GOTO Button_ISR_OP ; this is for the numbers 1 and 2

Button_ISR_OP:

; TASK
CLRF TMR1_OVERFLOW
INCF COUNT ; increments count
; check if count > 2 to reset to 0
MOVLW SIGN_COUNT
SUBWF COUNT, W
BTFSC STATUS, Z
CLRF COUNT
;End of TASK

BANKSEL INTCON
BSF INTCON, INTE

BANKSEL PORTD
retfie

Button_ISR_ROUNDS:
; TASK
CLRF TMR1_OVERFLOW
INCF COUNT ; increments count
; check if count > 2 to reset to 0
MOVLW SIGN_COUNT
SUBWF COUNT, W
BTFSC STATUS, Z
GOTO SET_TO_ONE

;End of TASK
BANKSEL INTCON
BSF INTCON, INTE

BANKSEL PORTD
retfie


SET_TO_ONE:
MOVLW 0x01
MOVWF COUNT
BANKSEL INTCON
BSF INTCON, INTE

BANKSEL PORTD
retfie


INCLUDE "LCDIS_PORTA.INC" ; IF U WANT TO USE LCD ON PORT A
ISR_MAX_NINE:
; TASK
CLRF TMR1_OVERFLOW
INCF COUNT ; increments count
; check if count > 9 to reset to 0
MOVLW MAX_NINE
SUBWF COUNT, W
BTFSC STATUS, Z
CLRF COUNT
;End of TASK

BANKSEL INTCON
BSF INTCON, INTE

BANKSEL PORTD
retfie

ISR_MAX_SIX:
; TASK
CLRF TMR1_OVERFLOW
INCF COUNT ; increments count
; check if count > 6 to reset to 0
MOVLW MAX_COUNT
SUBWF COUNT, W
BTFSC STATUS, Z
CLRF COUNT
;End of TASK

BANKSEL INTCON
BSF INTCON, INTE

BANKSEL PORTD
retfie

ISR_MAX_FIVE:
; TASK
CLRF TMR1_OVERFLOW
INCF COUNT ; increments count
; check if count > 5 to reset to 0
MOVLW MAX_FIVE
SUBWF COUNT, W
BTFSC STATUS, Z
CLRF COUNT
;End of TASK

BANKSEL INTCON
BSF INTCON, INTE

BANKSEL PORTD
retfie

ISR_MAX_THREE:
; TASK
CLRF TMR1_OVERFLOW
INCF COUNT ; increments count
; check if count > 3 to reset to 0
MOVLW MAX_THREE
SUBWF COUNT, W
BTFSC STATUS, Z
CLRF COUNT
;End of TASK

BANKSEL INTCON
BSF INTCON, INTE

BANKSEL PORTD
retfie
    
    
; The main code for our program
start:

CALL xms
CALL xms

CALL inid
; Move the cursor to the beginning of the first line (address 0x00)
BCF Select, RS
MOVLW 0x80
CALL send
BSF Select, RS
; Print "Enter Operation" on the LCD
MOVLW 'E' ; E
CALL send
MOVLW 'n' ; n
CALL send
MOVLW 't' ; t
CALL send
MOVLW 'e' ; e
CALL send
MOVLW 'r' ; r
CALL send
MOVLW ' ' ; Space
CALL send
MOVLW 'O' ; O
CALL send
MOVLW 'p' ; p
CALL send
MOVLW 'e' ; e
CALL send
MOVLW 'r' ; r
CALL send
MOVLW 'a' ; a
CALL send
MOVLW 't' ; t
CALL send
MOVLW 'i' ; i
CALL send
MOVLW 'o' ; o
CALL send
MOVLW 'n' ; n
CALL send
    
    ;----------------------- end of line 1-------------------------------

loop:

MOVLW 0xC0 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_1
call xms

GOTO loop


loop2:

MOVLW 0xC1 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_2
call xms

GOTO loop2

loop3:

MOVLW 0xC2 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_3
call xms

GOTO loop3

loop4:

MOVLW 0xC3 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_4
call xms

GOTO loop4

loop5:

MOVLW 0xC4 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_OPERATION
call xms

GOTO loop5

loop6:

MOVLW 0x01
MOVWF COUNT_FLAG

MOVLW 0xC5 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS


; Check if 0 to print + 
;BANKSEL COUNT
MOVLW 0x00;V_ZERO
XORWF COUNT, W
BTFSS STATUS, Z
CALL PRINT_OTHER
CALL PRINT_ADD


GOTO loop6

PRINT_ADD:
MOVLW '+' ; E
CALL send

; check if reset or not
MOVLW 0x01
XORWF SKIP_NUMBER2, W
BTFSS STATUS, Z
CALL CONT_NORMALLY
CALL SKIP_IT

CONT_NORMALLY:
BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_21
call xms
GOTO loop6

SKIP_IT:
MOVLW 0x00
MOVWF SAVED_OPERATION

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
GOTO DELAY_IT

DELAY_IT:
; 2- set cursor at cell 0 row 2
MOVLW 0xC5 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVLW '+' ; E
CALL send

; 4- reset COUNT
CLRF COUNT 
; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW
call xms
GOTO CLEAR_LCD


PRINT_OTHER:

MOVLW 0x01
XORWF COUNT, W
BTFSS STATUS, Z
CALL PRINT_MOD
CALL PRINT_DIV

PRINT_DIV:
MOVLW '/' ; E
CALL send

; check if reset or not
MOVLW 0x01
XORWF SKIP_NUMBER2, W
BTFSS STATUS, Z
CALL CONT_NORMALLY1
CALL SKIP_IT1


CONT_NORMALLY1:
BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_22
call xms
GOTO loop6

SKIP_IT1:
MOVLW 0x01
MOVWF SAVED_OPERATION

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
GOTO DELAY_IT1

DELAY_IT1:
; 2- set cursor at cell 0 row 2
MOVLW 0xC5 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVLW '/' ; E
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW
call xms
GOTO CLEAR_LCD

PRINT_MOD:
MOVLW '%' ; E
CALL send

; check if reset or not
MOVLW 0x01
XORWF SKIP_NUMBER2, W
BTFSS STATUS, Z
CALL CONT_NORMALLY2
CALL SKIP_IT2

CONT_NORMALLY2:
BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_23
call xms
GOTO loop6

SKIP_IT2:
; 1- Save the operation
MOVLW 0x02
MOVWF SAVED_OPERATION

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
GOTO DELAY_IT2

DELAY_IT2:
; 2- set cursor at cell 0 row 2
MOVLW 0xC5 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

; 3- print the value on screen
MOVLW '%' ; E
CALL send

; 4- reset COUNT
CLRF COUNT 

; Clear Timer1 overflow flag
BANKSEL TMR1_OVERFLOW
CLRF TMR1_OVERFLOW
call xms
GOTO CLEAR_LCD

;---------- LOOPS for NUMBER2

loop7:
CLRF COUNT_FLAG

MOVLW 0xC6 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_201
call xms

GOTO loop7


loop8:
CLRF COUNT_FLAG

MOVLW 0xC7 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_202
call xms

GOTO loop8

loop9:
CLRF COUNT_FLAG

MOVLW 0xC8 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_203
call xms

GOTO loop9

loop10:
CLRF COUNT_FLAG

MOVLW 0xC9 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW MAX_PERIOD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_204
call xms

GOTO loop10


loop11:
CLRF COUNT_FLAG

MOVLW 0xCA ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW TIME_TO_CLEAR_LCD
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_205
call xms

GOTO loop11
;--------END OF LOOPS FOR NUMBER2

CLEAR_LCD: ; this loop to clear the lcd 
call inid

BCF Select, RS
MOVLW 0x80
CALL send
BSF Select, RS
MOVLW 'r'
CALL send
MOVLW 'e'
CALL send
MOVLW 's'
CALL send
MOVLW 'u'
CALL send
MOVLW 'l'
CALL send
MOVLW 't'
CALL send


BCF Select, RS
MOVLW 0xC0
CALL send
BSF Select, RS
MOVLW '='
CALL send



GOTO EXECUTE_OPERATION

EXECUTE_OPERATION:

MOVLW 0x00
XORWF SAVED_OPERATION, W
BTFSS STATUS, Z
GOTO DIV_OR_MOD ; no, its either div or mod
GOTO ADD_OPERATION_EXECUTE ; yes, its add

ADD_OPERATION_EXECUTE:
movf number1_ones, w
addwf number2_ones, w
movwf temp2
movwf Sdigit1
movf temp, w
subwf Sdigit1, f

    ; Check if the result is negative
btfss STATUS, C
goto NegativeResult1
movlw 0x01
movwf carry

goto add2
; If the result is positive, do something here
   
      
NegativeResult1:
movf temp2 , w
movwf Sdigit1
    
goto add2

add2:
movf number1_tens, w
addwf number2_tens, w
addwf carry, w
movwf temp2
movwf Sdigit2
movf temp, w
subwf Sdigit2, f

; Check if the result is negative
btfss STATUS, C
goto NegativeResult2
movlw 0x01
movwf carry

goto add3
; If the result is positive, do something here
   
      
NegativeResult2:
movf temp2 , w
movwf Sdigit2
clrf carry
goto add3

add3:
movf number1_hunds, w
addwf number2_hunds, w
addwf carry, w
movwf temp2
movwf Sdigit3
movf temp, w
subwf Sdigit3, f

; Check if the result is negative
btfss STATUS, C
goto NegativeResult3
movlw 0x01
movwf carry

goto add4
; If the result is positive, do something here
   
      
NegativeResult3:
movf temp2 , w
movwf Sdigit3
clrf carry
goto add4

add4:
movf number1_thous, w
addwf number2_thous, w
addwf carry, w
movwf temp2
movwf Sdigit4
movf temp, w
subwf Sdigit4, f

; Check if the result is negative
btfss STATUS, C
goto NegativeResult4
movlw 0x01
movwf carry

goto add5
; If the result is positive, do something here
   
      
NegativeResult4:
movf temp2 , w
movwf Sdigit4
clrf carry
goto add5

add5:
movf number1_tenthous, w
addwf number2_tenthous, w
addwf carry, w
movwf temp2
movwf Sdigit5
movf temp, w
subwf Sdigit5, f

; Check if the result is negative
btfss STATUS, C
goto NegativeResult5
movlw 0x01
movwf carry

goto finish
; If the result is positive, do something here
   
      
NegativeResult5:
movf temp2 , w
movwf Sdigit5
clrf carry
goto finish	

finish:  
movf carry, w
movwf Sdigit6

MOVLW 0xC1 ; Set cursor to the 2nd cell address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF Sdigit6, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF Sdigit5, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF Sdigit4, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF Sdigit3, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF Sdigit2, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF Sdigit1, W
ADDLW '0'
BSF Select, RS
CALL send
GOTO LCD_QUESTIONING
;-------Division / MODE ------
DIV_OR_MOD:
	CLRF divR1
	CLRF divR2
	CLRF divR3
	CLRF divR4
	CLRF divR5

	movf number1_tenthous, w
	movwf compare2
	movf number2_tenthous, w
	subwf compare2
	btfss STATUS, C
	goto cant

load:
	MOVF number1_ones, W    ; Move the value of number1_ones to the WREG (Working Register)
	MOVWF temp1      ; Move the value from WREG to temp1

	div1:
	MOVF number2_ones, W
    SUBWF temp1
	BTFSs STATUS,C
	goto borrow1
	MOVF temp1, W
	MOVWF sub1
	goto load2
	
	;goto div2


	
borrow1:
	MOVF number1_tens, W
	BTFSC STATUS, Z
	goto borrow2
	continue1:
	DECF number1_tens, 1 
	;BTFSS STATUS, C
	MOVF number1_ones, W
	ADDLW .10
	MOVWF number1_ones
	goto load
	;GOTO div2	

borrow2:
    MOVF number1_hunds, W
	BTFSC STATUS, Z
	goto borrow3
	continue2:
	DECF number1_hunds, 1
	;BTFSS STATUS, C
	MOVF number1_tens, W
	ADDLW .10
	MOVWF number1_tens
	goto continue1

borrow3:
    MOVF number1_thous, W
	BTFSC STATUS, Z
	goto borrow4
    continue3:
	DECF number1_thous, 1
	;BTFSS STATUS, C
	MOVF number1_hunds, W
	ADDLW .10
	MOVWF number1_hunds
	goto continue2

borrow4:
	MOVF number1_tenthous, W
	BTFSC STATUS, Z
	goto cant
	DECF number1_tenthous, 1
	;BTFSS STATUS, C
	MOVF number1_thous, W
	ADDLW .10
	MOVWF number1_thous
	goto continue3


load2:
    MOVF number1_tens, W    ; Move the value of number1_ones to the WREG (Working Register)
	MOVWF temp1      ; Move the value from WREG to temp1

	div2:
	MOVF number2_tens, W
    SUBWF temp1
	BTFSs STATUS,C
	goto borrow12
	MOVF temp1, W
	MOVWF sub2
	goto load3
	
borrow12:
	MOVF number1_hunds, W
	BTFSC STATUS, Z
	goto borrow22
	continue12:
	DECF number1_hunds, 1
	;BTFSS STATUS, C
	MOVF number1_tens, W
	ADDLW .10
	MOVWF number1_tens
	goto load2
	;GOTO div2	

borrow22:
    MOVF number1_thous, W
	BTFSC STATUS, Z
	goto borrow32
	continue22:
	DECF number1_thous, 1
	;BTFSS STATUS, C
	MOVF number1_hunds, W
	ADDLW .10
	MOVWF number1_hunds
	goto continue12

borrow32:
	MOVF number1_tenthous, W
	BTFSC STATUS, Z
	goto cant
	DECF number1_tenthous, 1
	;BTFSS STATUS, C
	MOVF number1_thous, W
	ADDLW .10
	MOVWF number1_thous
	goto continue22

load3:
    MOVF number1_hunds, W    ; Move the value of number1_ones to the WREG (Working Register)
	MOVWF temp1      ; Move the value from WREG to temp1

	div3:
	MOVF number2_hunds, W
    SUBWF temp1
	BTFSs STATUS,C
	goto borrow13
	MOVF temp1, W
	MOVWF sub3
	goto load4
	
borrow13:
	MOVF number1_thous, W
	BTFSC STATUS, Z
	goto borrow23
	continue13:
	DECF number1_thous, 1
	;BTFSS STATUS, C
	MOVF number1_hunds, W
	ADDLW .10
	MOVWF number1_hunds
	goto load3
	;GOTO div2	

borrow23:
	MOVF number1_tenthous, W
	BTFSC STATUS, Z
	goto cant
	DECF number1_tenthous, 1
	;BTFSS STATUS, C
	MOVF number1_thous, W
	ADDLW .10
	MOVWF number1_thous
	goto continue13


load4:
    MOVF number1_thous, W    ; Move the value of number1_ones to the WREG (Working Register)
	MOVWF temp1      ; Move the value from WREG to temp1

	div4:
	MOVF number2_thous, W
    SUBWF temp1
	BTFSs STATUS,C
	goto borrow14
	MOVF temp1, W
	MOVWF sub4
	goto load5
	
borrow14:
	MOVF number1_tenthous, W
	BTFSC STATUS, Z
	goto cant
	DECF number1_tenthous, 1
	;BTFSS STATUS, C
	MOVF number1_thous, W
	ADDLW .10
	MOVWF number1_thous
	goto load4

load5:
    MOVF number1_tenthous, W    ; Move the value of number1_ones to the WREG (Working Register)
	MOVWF temp1      ; Move the value from WREG to temp1

	div5:
	MOVF number2_tenthous, W
    SUBWF temp1
	BTFSs STATUS,C
	goto cant
	MOVF temp1, W
	MOVWF sub5
	goto incR1

cant:
	MOVF number1_ones, W
	MOVWF remainder1
	MOVF number1_tens, W
	MOVWF remainder2
	MOVF number1_hunds, W
	MOVWF remainder3
	MOVF number1_thous, W
	MOVWF remainder4
	MOVF number1_tenthous, W
	MOVWF remainder5

	MOVLW 0x00
	MOVWF sub1
	MOVWF sub2
	MOVWF sub3
	MOVWF sub4
	MOVWF sub5
	goto done

incR1:
	MOVF divR1, W
	MOVWF compare
	MOVLW 0x09
    SUBWF compare
	BTFSC STATUS,Z
	goto incR2
	INCF divR1, f

	MOVF sub1, W
	MOVWF number1_ones
	MOVF sub2, W
	MOVWF number1_tens
	MOVF sub3, W
	MOVWF number1_hunds
	MOVF sub4, W
	MOVWF number1_thous
	MOVF sub5, W
	MOVWF number1_tenthous

	goto load

incR2:
	clrf divR1
	MOVF divR2, W
	MOVWF compare
	MOVLW 0x09
    SUBWF compare
	BTFSC STATUS,Z
	goto incR3
	INCF divR2, f
	
	MOVF sub1, W
	MOVWF number1_ones
	MOVF sub2, W
	MOVWF number1_tens
	MOVF sub3, W
	MOVWF number1_hunds
	MOVF sub4, W
	MOVWF number1_thous
	MOVF sub5, W
	MOVWF number1_tenthous

	goto load
	

incR3:
	clrf divR2
	clrf divR1
	MOVF divR3, W
	MOVWF compare
	MOVLW 0x09
    SUBWF compare
	BTFSC STATUS,Z
	goto incR4
	INCF divR3, f

	MOVF sub1, W
	MOVWF number1_ones
	MOVF sub2, W
	MOVWF number1_tens
	MOVF sub3, W
	MOVWF number1_hunds
	MOVF sub4, W
	MOVWF number1_thous
	MOVF sub5, W
	MOVWF number1_tenthous

	goto load

incR4:
	clrf divR3
	clrf divR2
	clrf divR1
	MOVF divR4, W
	MOVWF compare
	MOVLW 0x09
    SUBWF compare
	BTFSC STATUS,Z
	goto incR5
	INCF divR4, f

	MOVF sub1, W
	MOVWF number1_ones
	MOVF sub2, W
	MOVWF number1_tens
	MOVF sub3, W
	MOVWF number1_hunds
	MOVF sub4, W
	MOVWF number1_thous
	MOVF sub5, W
	MOVWF number1_tenthous

	goto load

incR5:
	clrf divR4
	clrf divR3
	clrf divR2
	clrf divR1
	MOVF divR5, W
	MOVWF compare
	MOVLW 0x09
    SUBWF compare
	BTFSC STATUS,Z
	goto done
	INCF divR5, f

done:
	movf carry, w
	movwf Sdigit6

	MOVLW 0xC1 ; Set cursor to the 2nd cell address of the second line
	BCF Select, RS 
	CALL send ; Send command to set cursor position
	BSF Select, RS
	MOVLW 0x01
        XORWF SAVED_OPERATION, W
	BTFSS STATUS, Z
	GOTO PRINT_MOD_RESULT ; no, its mod
	GOTO PRINT_DIV_RESULT ; yes, its div

PRINT_DIV_RESULT:
	MOVF divR5, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF divR4, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF divR3, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF divR2, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF divR1, W
	ADDLW '0'
	BSF Select, RS
	CALL send
	; Remainder
	MOVLW ' '
	BSF Select, RS
	CALL send

	MOVLW 'R'
	BSF Select, RS
	CALL send
       
        MOVLW ':'
	BSF Select, RS
	CALL send

	MOVF remainder5, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF remainder4, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF remainder3, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF remainder2, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF remainder1, W
	ADDLW '0'
	BSF Select, RS
	CALL send
	GOTO LCD_QUESTIONING
PRINT_MOD_RESULT:
	; Remainder
	MOVLW ' '
	BSF Select, RS
	CALL send

	MOVF remainder5, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF remainder4, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF remainder3, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF remainder2, W
	ADDLW '0'
	BSF Select, RS
	CALL send

	MOVF remainder1, W
	ADDLW '0'
	BSF Select, RS
	CALL send
	GOTO LCD_QUESTIONING
	
LCD_QUESTIONING
call xms
call xms
call xms
; wait for 3 seconds
; clear lcd 
call inid
BCF Select, RS
MOVLW 0x80
CALL send
BSF Select, RS
MOVLW 'K'
CALL send
MOVLW 'e'
CALL send
MOVLW 'e'
CALL send
MOVLW 'p'
CALL send
MOVLW '?'
CALL send
MOVLW ' '
CALL send
MOVLW '['
CALL send
MOVLW '1'
CALL send
MOVLW ':'
CALL send
MOVLW 'Y'
CALL send
MOVLW ','
CALL send
MOVLW ' '
CALL send
MOVLW '2'
CALL send
MOVLW ':'
CALL send
MOVLW 'N'
CALL send
MOVLW ']'
CALL send

BCF Select, RS
MOVLW 0xC0
CALL send
BSF Select, RS

goto loop_rounds


loop_rounds:

MOVLW 0x10
MOVWF COUNT_FLAG

MOVLW 0xC0 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS

MOVF COUNT, W
ADDLW '0'
BSF Select, RS
CALL send

BANKSEL TMR1_OVERFLOW
INCF TMR1_OVERFLOW, F ; Increment Timer1 overflow flag
; Timer1 ISR
MOVLW SIGN_COUNT
SUBWF TMR1_OVERFLOW, W
BTFSC STATUS, Z
CALL Timer1_SET_YN
call xms

GOTO loop_rounds


RE_SELECT_OPERATION:
CLRF COUNT
MOVLW 0x01
MOVWF SKIP_NUMBER2

call inid

BCF Select, RS
MOVLW 0x80
CALL send
BSF Select, RS
MOVLW 'E'
CALL send
MOVLW 'n'
CALL send
MOVLW 't'
CALL send
MOVLW 'e'
CALL send
MOVLW 'r'
CALL send
MOVLW ' '
CALL send
MOVLW 'O'
CALL send
MOVLW 'p'
CALL send
MOVLW 'e'
CALL send
MOVLW 'r'
CALL send
MOVLW 'a'
CALL send
MOVLW 't'
CALL send
MOVLW 'i'
CALL send
MOVLW 'o'
CALL send
MOVLW 'n'
CALL send

; 2- set cursor at cell 0 row 2
MOVLW 0xC0 ; Set cursor to the starting address of the second line
BCF Select, RS 
CALL send ; Send command to set cursor position
BSF Select, RS


MOVF number1_tenthous, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF number1_thous, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF number1_hunds, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF number1_tens, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF number1_ones, W
ADDLW '0'
BSF Select, RS
CALL send

MOVLW '+'
CALL send

MOVF number2_tenthous, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF number2_thous, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF number2_hunds, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF number2_tens, W
ADDLW '0'
BSF Select, RS
CALL send

MOVF number2_ones, W
ADDLW '0'
BSF Select, RS
CALL send
call xms 
call xms 
call xms 
goto loop6
end