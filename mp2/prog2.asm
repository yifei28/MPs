;
;
;
.ORIG x3000
; This is a calculator that takes postfix expressiions and calculates their results.
; This calculator will incessantly take inputs from user until '=' is typed.
; The result will be printed on screen immdiately after '=' is typed.
; If the expressions entered by user is invalid, an exception will be thrown on screen.
; Supported operations on this calculator are: addition, subtraction, mutiply, divide, and power.
;
; Register chart
; R0: take input from user's keyboard & output result or exception
; R1: stores temporary value
; R2: stores temporary value
; R3: stores the first operand for an operation
; R4: stores the second operand for an operation
; R5: stores the result of calculation
; R6: stores temporary value
; R7: primarily used as the pointer for JSP & RET
	
	; take the characters from user and load them to the stack
INPUT	GETC			; read next character and show user's input on screen
	OUT			;
	JSR EVALUATE		; jump to evaluation subroutine after reading an input

        ; keep loading characters to the stack until '=' is typed by user	
	BR INPUT		; branch back to read next input
	
	; where RET + 1 will jump to
	JSR PRINT_HEX		; jump to PRINT_HEX subroutine to print out the result
	HALT			;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; this function stores the result of calculation in R5 & prints it out to screen
; R0: stores the hexadecimal output
; R1: temporary register
; R2: counter for main loop
; R3: temporarily stores value of the result for extraction
; R4: counter for extraction loop
; R5: stores the result of calculation
PRINT_HEX
	ST R7, PRINT_SaveR7     ; save address in R7 for nested subroutines
	JSR POP			; extract the result using POP
	ADD R5, R0, #0		; stores the result of calculation into R5
	ADD R3, R0, #0		; stores the result of calculation into R3 for extraction
	AND R2, R2, #0		; clear R2
	ADD R2, R2, #4		; set the main loop counter to 4
NUM_LOOP
        AND R4, R4, #0          ; clear value of R4
        ADD R4, R4, #4          ; initialize extraction counter to 4
	AND R0, R0, #0		; clear R0 to store output
	
	; loop to extract the highest 4 bits and print them out to console as hexadecimal number
EXTRACT_LOOP
	ADD R0, R0, R0		; left shift R0 to make space for the next bit
	ADD R3, R3, #0 		; check the highest bit of R3
	BRzp SHIFT		; if highest bit is 0, extract a 0 to R0
	ADD R0, R0, #1		; if highest bit is 1, extract a 1 to R0

SHIFT	ADD R3, R3, R3		; left shift R3
	ADD R4, R4, #-1		; decrement loop counter R4
	BRp EXTRACT_LOOP	; if R4 > 0, loop the extraction process again

	; otherwise R0 already has 4 bits extracted, print the hexadecimal number out
	ADD R1, R0, #-10	; check if R0 is a number of a character
	BRzp IS_CHAR		; if R0 contains a character (>=10), select the corresponding character to print out
	LD R1, ASCII_ZERO	; load R1 with the ASCII code of '0', x0030
	ADD R0, R0, R1		; add '0' (x0030) and the number stored in R0 to get the corresponding number in ASCII code
	OUT			; print out the number
	BRnzp NEXT_NUM		; 

	; if the hexadecimal number is a character, find it and print it out
IS_CHAR	
	LD R1, ASCII_CHAR	; load R1 with the ASCII code that corresponds to 'A'-10 (x0037)
	ADD R0, R0, R1		; add x0037 and the character stored in R0 to get the corresponding ASCII code for character A-F
	OUT			; print out the character

	; move to the next hexadecimal number to be printed 
NEXT_NUM
	ADD R2, R2, #-1 	; decrement loop counter
	BRp NUM_LOOP		; if R2 > 0, print the next hexadecimal number of the result
	LD R7, PRINT_SaveR7	; otherwise program is finished, return and halt
	RET			;


PRINT_SaveR7	.BLKW #1        ; 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; this function identifies what character is inputed by user and calls corresponding subroutines to perform calculations
; R0: character input from keyboard
; R1: stores temporary ASCII values
; R2: stores the result of checking corresponding values
; R3: temporary register
; R4: temporary register
; R5: overflow/underflow indicator
EVALUATE

	; identify if user types a '=' character
        LD R1, NEG_ASCII_EQUAL  ; load the value of -x003D ('=') to R1
        ADD R2, R1, R0          ; subtract ASCII code of '=' by input in R0
        BRz RESULT 	        ; if input is '=', branch to check if there is only 1 number in stack

	; identify if user types space ' ' character
	LD R1, NEG_ASCII_SPACE	; load the value of -x0020 (' ') to R1
        ADD R2, R1, R0          ; subtract ASCII code of ' ' by input in R0
	BRnp NUM_CHECK		; if input is not ' ', branch to identification of whether or not this is a number
	RET			; if input is ' ', return to read next input

	; identiy if user types a number/operator
NUM_CHECK
	LD R1, NEG_ASCII_ZERO	; load the value of -x0030 ('0') to R1
	ADD R2, R1, R0		; subtract ASCII code of '0' by input in R0
	BRn INPUT_NON_NUM	; if R0 < x0030, branch to non-number part 
	ADD R2, R2, #-9		; subtract ASCII code of '9' by input in R2
	BRp INPUT_NON_NUM	; if R0 > x0039, branch to non-number part
	BR INPUT_NUM		; otherwise branch to number part
	
	; if user typed an number (operand), subtract it by '0' and push it to the stack
INPUT_NUM
	ST R7, EVAL_SaveR7      ; save address in R7 for nested subroutines
	ADD R0, R0, R1		; subtract ASCII code of '0' by the ASCII code of number in R0
	JSR PUSH		; jump to push function
	ADD R5, R5, #0		; check overflow
	BRz #1			; if R5 != 0, return and get next input
	JSR THROW_EXCEPTION	; otherwise print invalid message
	BR DONE_EVAL		; after push operation, check overflow

	; identify if user types a operator, if so, jump to the corresponding operation
INPUT_NON_NUM
	ST R7, EVAL_SaveR7      ; save address in R7 for nested subroutines
	LD R1, NEG_ASCII_MUL	; load the value of -x002A ('*') to R1
	ADD R2, R1, R0		; check if user types '*' (multiply)
	BRz J_MUL		; if R0 == x002A, branch to multiplication
	ADD R1, R1, #-1		; change the value of R1 to -x002B ('+')
	ADD R2, R1, R0		; check if user types '+' (addition)
	BRz J_PLUS		; if R0 == x002B, branch to addition
	ADD R1, R1, #-2		; change the value of R1 to -x002D ('-')
	ADD R2, R1, R0		; check if user types '-' (subtraction)
	BRz J_MIN		; if R0 == x002D, branch to subtraction
	ADD R1, R1, #-2		; change the value of R1 to -x002F ('/')
	ADD R2, R1, R0		; check if user types '/' (divide)
	BRz J_DIV		; if R0 == x002F, branch to divide
	LD R1, NEG_ASCII_POWER	; load the value of -x005E ('^') to R1
	ADD R2, R1, R0		; check if user types '^' (exponent)
	BRz J_EXP		; if R0 == x005E, branch to exponent

	; otherwise the character can not be recoginized
	JSR THROW_EXCEPTION	; print invalid message

	; jump to corresponding operations
J_MUL
	JSR LOAD_INPUTS		; loads the previous two numbers entered by the user to R3 and R4
	JSR MUL			;
	BR DONE_EVAL		; after operation, return and get the next input value from user

J_DIV
	JSR LOAD_INPUTS		; similar procedure as the first one
	JSR DIV			;
	BR DONE_EVAL		;

J_EXP
	JSR LOAD_INPUTS		;
	JSR EXP			;
	BR DONE_EVAL		;
	
J_PLUS
        JSR LOAD_INPUTS		;
	JSR PLUS		;
	BR DONE_EVAL		;

J_MIN
        JSR LOAD_INPUTS		;
	JSR MIN			;
	
	; the evaluation is done and program is returned to GETC the next character typed or to print the result
DONE_EVAL
	LD R7, EVAL_SaveR7	; load the orginial R7
	RET			; return to addr in R7

	; if '=' is typed, evaluation will be branched to here
RESULT
	LD R3, STACK_TOP	; check if the result is the only number in stack
	LD R4, STACK_START	;
	ADD R4, R4, #-1		; if TOS is 1 address above the STACK_TOP, there is only one number
	NOT R4, R4		; negate R4
	ADD R4, R4, #1		;
	ADD R3, R3, R4		; subtract 'STACK_START - 1' by STACK_TOP
	BRz PRINT		; if true, increment R7 and return to print the result
	JSR THROW_EXCEPTION	; otherwise print invalid message
PRINT	ADD R7, R7, #1		; increment R7 to break the loop and print the result in hexadecimal number
	RET			;


EVAL_SaveR7	.BLKW #1	; 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; check if there are at least 2 numbers in stack
CHECK_STACK
	LD R3, STACK_TOP        ; 
        LD R4, STACK_START      ;
	ADD R4, R4, #-2		; check if TOS is at least 2 addresses above the STACK_START
        NOT R4, R4              ; negate R4
        ADD R4, R4, #1          ;
        ADD R3, R3, R4          ; subtract 'STACK_START - 2' by STACK_TOP
	BRp #1			; if TOS is below 'STACK_START - 2', print invaild message
	RET			; otherwise there are at least 2 numbers in stack and return
	JSR THROW_EXCEPTION	;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; this function print invaild message
; R0: stores the address of the invalid message string
THROW_EXCEPTION
        LEA R0, EXCEPTION       ; stores the address of invalid message string to R0
        PUTS                    ; print the invalid message
        HALT                    ;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; this function pops two values and load them into R3 and R4
; R3: stores the second POP value
; R4: stores the first POP value
LOAD_INPUTS
	ST R7, LOAD_IN_SaveR7	; save R7 because this is a nested subroutine
	JSR CHECK_STACK		; check if there are at least 2 numbers in stack for operation
	JSR POP			; pop the first value and stores it to R4
        ADD R4, R0, #0          ;
	JSR POP			; pop the second value and stores the value into R3
	ADD R3, R0, #0		;
	LD R7, LOAD_IN_SaveR7	; load R7 and return when finished loading the two values
	RET			;


LOAD_IN_SaveR7     .BLKW #1        ; 


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;input R3, R4
;out R0
;R3+R4
PLUS	
	ST R7, OPRT_SaveR7	; save R7	
	ADD R0, R3, R4		; sum is stored in R0
	JSR PUSH		; push the sum into stack
	LD R7, OPRT_SaveR7	;
	RET			;
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;input R3, R4
;out R0
;R3-R4
MIN	
        ST R7, OPRT_SaveR7      ; save R7       
        NOT R4, R4	        ; negate R4
	ADD R4, R4, #1		;
	ADD R0, R3, R4		; difference is stored in R0
        JSR PUSH                ; push the difference into stack
        LD R7, OPRT_SaveR7      ;
        RET                     ;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;input R3, R4
;out R0
;R3*R4
MUL	
        ST R7, OPRT_SaveR7      ; save R7    
	ST R4, MUL_SaveR4	; save R4
	AND R0, R0, #0		; clear R0 

	; perform calculation when there is negative values, 
	ADD R4, R4, #0		; check if R4 is negative
	BRzp MUL_LOOP		; if R4 is >= 0, perform mulitplication
	NOT R3, R3		; otherwise flip both sign
  	ADD R3, R3, #1		;
	NOT R4, R4		;
	ADD R4, R4, #1		;

MUL_LOOP
	ADD R4, R4, #0		; check if R4 is 0
	BRz MUL_DONE		; if R4 = 0, push the result and return       
	ADD R0, R0, R3		; use addition and loop to implement multiplication
	ADD R4, R4, #-1		; decrement R4
	BRp MUL_LOOP		; if R4 > 0, repeat this process
        JSR PUSH                ; push the product into stack
MUL_DONE
	LD R4, MUL_SaveR4	;
        LD R7, OPRT_SaveR7      ;
        RET                     ;

MUL_SaveR4	.BLKW #1	; save R4 for exponent function

	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;input R3, R4
;out R0
;R3/R4
DIV	
        ST R7, OPRT_SaveR7      ; save R7 
	AND R0, R0, #0		; clear R0
	ADD R4, R4, #0		; check if R4 is 0
	BRnp #1			; if R4 = 0, throw an exception
	JSR THROW_EXCEPTION 	;  
	NOT R4, R4		; simple division is performed using subtraction
	ADD R4, R4, #1		; negate R4
DIV_LOOP
	ADD R3, R3, R4		; subtract R4 by R3
	BRn DIV_DONE		; if R4 < 0, break the loop
	ADD R0, R0, #1		; increment R0 for every R4 subtracted
	BR DIV_LOOP		;
DIV_DONE
        JSR PUSH                ; push the quotient into stack
        LD R7, OPRT_SaveR7      ;
        RET                     ;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;input R3, R4
;counter for main loop R7
;counter for multiplication loop R4
;out R0
;R3^R4
EXP
        ST R7, OPRT_SaveR7      ; save R7 
	ST R3, EXP_COUNTER	; stores R3 as the counter for multiplication
	ADD R7, R4, #-1		; load R7 - 1 as the counter for EXP_LOOP
	
EXP_LOOP
        ADD R0, R3, #0          ; load R3 to R0, or load the result of multiplication to R0
        AND R3, R3, #0          ; clear R3
	ADD R7, R7, #0 		; verify if R7 = 0
	BRz EXP_DONE		; if R7 = 0, exponent operation is done and return
	LD R4, EXP_COUNTER	; load value of R3 to R4, which is used as a multiplication counter
	
	; this loop do multiplication inside the exponent operation
EXP_MUL_LOOP
	ADD R3, R3, R0		; stores the result of multiplication to R3
	ADD R4, R4, #-1		; decrement R4
	BRp EXP_MUL_LOOP	; if R4 > 0, repeat mutiplication
	ADD R7, R7, #-1		; decrement R7
	BR EXP_LOOP		; otherwise back to main loop

EXP_DONE
        JSR PUSH                ; push the result of exponent into stack
        LD R7, OPRT_SaveR7      ;
        RET                     ;


EXP_COUNTER	.BLKW #1	; 


OPRT_SaveR7	.BLKW #1	;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;IN:R0, OUT:R5 (0-success, 1-fail/overflow)
;R3: STACK_END R4: STACK_TOP
;
PUSH	
	ST R3, PUSH_SaveR3	;save R3
	ST R4, PUSH_SaveR4	;save R4
	AND R5, R5, #0		;
	LD R3, STACK_END	;
	LD R4, STACk_TOP	;
	ADD R3, R3, #-1		;
	NOT R3, R3		;
	ADD R3, R3, #1		;
	ADD R3, R3, R4		;
	BRz OVERFLOW		;stack is full
	STR R0, R4, #0		;no overflow, store value in the stack
	ADD R4, R4, #-1		;move top of the stack
	ST R4, STACK_TOP	;store top of stack pointer
	BRnzp DONE_PUSH		;
OVERFLOW
	ADD R5, R5, #1		;
DONE_PUSH
	LD R3, PUSH_SaveR3	;
	LD R4, PUSH_SaveR4	;
	RET


PUSH_SaveR3	.BLKW #1	;
PUSH_SaveR4	.BLKW #1	;


;OUT: R0, OUT R5 (0-success, 1-fail/underflow)
;R3 STACK_START R4 STACK_TOP
;
POP	
	ST R3, POP_SaveR3	;save R3
	ST R4, POP_SaveR4	;save R3
	AND R5, R5, #0		;clear R5
	LD R3, STACK_START	;
	LD R4, STACK_TOP	;
	NOT R3, R3		;
	ADD R3, R3, #1		;
	ADD R3, R3, R4		;
	BRz UNDERFLOW		;
	ADD R4, R4, #1		;
	LDR R0, R4, #0		;
	ST R4, STACK_TOP	;
	BRnzp DONE_POP		;
UNDERFLOW
	ADD R5, R5, #1		;
DONE_POP
	LD R3, POP_SaveR3	;
	LD R4, POP_SaveR4	;
	RET


POP_SaveR3	.BLKW #1	;
POP_SaveR4	.BLKW #1	;
STACK_END	.FILL x3FF0	;
STACK_START	.FILL x4000	;
STACK_TOP	.FILL x4000	;
ASCII_ZERO	.FILL x0030	; ASCII code of '0'
ASCII_CHAR	.FILL x0037	; ASCII code of 'A' - 10
NEG_ASCII_ZERO	.FILL xFFD0	; negated ASCII code of '0'
NEG_ASCII_MUL	.FILL xFFD6	; negated ASCII code of '*'
NEG_ASCII_POWER .FILL xFFA2	; negated ASCII code of '^'
NEG_ASCII_SPACE	.FILL xFFE0	; negated ASCII code of ' '
NEG_ASCII_EQUAL .FILL xFFC3	; negated ASCII code of '='
EXCEPTION	.STRINGZ "Invaild Expression"	;

.END
