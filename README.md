# CPEG 222 Project 3 Keypad Calculator
For the third project, our goal is to implement a 4-digit decimal calculator. By pressing the number keys on the keypad, the user enters the first number for calculation, which is displayed on two SSDs (4-digits). By pressing the operation button (button A-F), user can insert the second number which is displayed on the SSDs as well. After finishing the input of the operands, the results will be displayed on the SSDs. While numbers are entered only as decimal values, the values can be displayed on the SSDs in either decimal or hexadecimal form.  The display and stored numbers can be cleared by pressing on-board btn1.
## Goals of this project
*	Learn how to use Pmodkeypad
*	Learn how to use polling to capture input 
## Minimal Hardware
* One chipKIT Pro MX7 Board
* One Pmod keypad
* Two Pmod SSDs
## Inputs
One on-board button: Btn1
The following buttons on the keypad will be used. Their definitions are listed below: 

Inputs: 0-9	A	B	C	D	E	F	BTN1

Functions: 	Input digit Addition	Subtraction	Multiplication	Division	Equal	HEX/DEC Transition	Clear

## Outputs
* Two external Pmod SSDs displaying the absolute value ranging from 0 to (9999)10. 
* On-board LEDs showing the sign of sum, errors in the calculation, and current decimal/hex display. 
* LED4 is ON if and only if the value being displayed is negative. LED1 & LED2 are ON if the value is displayed in hex format. 
## Functional description
* General requirements for all modes
    *	When one button is pressed (and not released yet), any additional button pressing will be ignored. 
    *	Except for the overflow mode, any unnecessary “0” characters will not be displayed. For example, “345” should not be displayed as “0345”. 
* Conceptually the calculator has three modes
    *	Mode 1 – Initial
        *	All on-board LEDs are off.
        *	Sum and current input number is set to 0 by default. 
        *	SSDs display “0” in 1-digit (rightmost digit), and other digits are off. 
        *	Number is displayed in decimal and sign of sum is positive, by default. 
        *	The controller enters mode 2 automatically.
    *	Mode 2 – Calculation
        *	By default, all displayed numbers are in decimal. LED1 is ON and LED2 is OFF.
        *	Number buttons ‘0’-‘9’ - when a number button is pressed,
            *	The input numbers are recognized as decimal all the time.
            *	If the last pressed button is not a number button, this will be the first digit for the new operand. The entered digit will be displayed at the rightmost position. 
            *	If the last pressed button is a number button, the current displayed number will shift one digit to the left and the just entered number will be displayed at the rightmost position (least significant digit). For example, if ‘1’,’2’,’3’,’4’ and ‘9’ have been pressed in sequence, the displayed number (which is identical to the value recorded as the operand) would be (1→12→123→1234→2349).
        *	Operation buttons ‘A’-‘F’ and Btn1 - when an operation button is pressed:
        *	Addition ‘A’(+) , Subtraction ‘B’ (-) , Multiplication ‘C’ (*) and Division ‘D’ (/): 
            1.	Whatever value on the SSD (e.g., the value entered by the user or the result left from previous calculation) will be used as the first operand. The display will be set to “0” to accept the second operand. 
            2.	If the last calculation hasn’t been finished yet (e.g., you entered “a-b*c”), the calculator will first finish the last calculation (a-b), store the result to sum, and then set the display to “0” to accept the next operand “c”. 
            3.	If one presses different operation buttons for multiple times, only the most recently pressed button will be considered (e.g., pressing “a + - b” will be considered as “a – b”). 
        *	Equal ‘E’ (=): calculate the sum and display it on SSDs.
            1.	“E” will be ignored if the two operands and the operation have not been entered properly. 
            2.	Pressing “E” multiple times in a row will only trigger one calculation (the first time).
        *	Decimal/Hex ‘F’: convert decimal to hex and vice versa.
            1.	When the number is displayed in hex, LED1 and LED2 are ON. If Display in Hex mode, pressing all the other keys will be ignored, only ‘Btn1’ and ‘F’ works.
        *	Clear ‘btn1’: Reset the controller to Mode 1 immediately. 
        *	If user performs a division by zero, enters mode 3 immediately.
        *	If the result overflows (> 9999) or underflows (<-9999), which may happen when either A, B, C or E is pressed, the controller enters mode 3 immediately.
    * Mode 3 – Overflow
        *	ALL on-board LEDs are OFF. 
        *	If overflow occurs, “HHHH” will flash on the SSDs at a frequency of approximately 2Hz. 
        *	If underflow occurs, “LLLL” will flash on the SDDs in a frequency of approximately 2Hz. 
        *	If division error (divided by o) occurs, “EEEE” will flash on the SSDs at a frequency of approximately 2Hz. 
        *	At this mode, all keys are invalid expect for ‘Btn1’. Pressing ‘Btn1’ will reset he calculator back to Mode 1 immediately.
## Display quality and keypad input requirements
*	All 4 digits of two SSDs should be always illuminated without observable flickering, and the luminance of each digit should be the same (at least cannot see the difference with eyes).
*	Button pressing should not interfere the display.
*	The calculator is sensitive to inputs. Mode switch, if any, happens as soon as a button is pressed. However, if a button is pressed less than 0.5 second, it is only recognized as one button press.

## Customer Satisfaction Requirements
Any of the following may account for deduction of your score
*	Incorrect/unexpected operation
*	Unfamiliarity with any aspect of your project
*	Late for your demo
*	Poorly presented demo
*	Sloppy, undocumented program code









