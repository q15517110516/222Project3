/*==================================CPEG222=============================
* Program:		Project3.c
* Authors: 		Yuchen Tang & Mingrui Liu
* Date: 		4/2/2017
* Inputs : On board Btn1 & keypad
* Outputs: One on board LED (LD1 to 4) and SSD
========================================================================*/
#include <plib.h>   //include peripheral library
#pragma config ICESEL = ICS_PGx1    //configure on board licensed debugger
#pragma config FNOSC = PRIPLL       //configure system clock 80 MHz
#pragma config POSCMOD = EC, FPLLIDIV = DIV_2,FPLLMUL = MUL_20,FPLLODIV = DIV_1
#pragma config FPBDIV = DIV_8   //configure peripheral bus clock to 10 MHz
#define SYS_FREQ    (80000000L)   // 80MHz system clock
#define INT_SEC     100           // interrupt at 100th of a second
#define CORE_TICK_RATE      (SYS_FREQ/2/INT_SEC)

//Define keymap to the values on the PmodKYPD
int keyTable [4][4] = {{1, 4, 7, 0},      // col 1
                      {2, 5, 8, 15},      // col 2
                      {3, 6, 9, 14},      // col 3
                      {10, 11, 12, 13}};  // col 4

int Poll_KYPD(int col); 
void ConverDecimalToHex(int value ,int Sw);
void DelayCount(int count);
void DisplayChar(int num,int SSD);
void clear();
void showres(int value);
void claculateres();


unsigned int mode = 1;  // 2 modes, mode 1 is decimal mode, mode 2 is hex mode
unsigned int one = 0;  // Initialize the ones digit with 0
unsigned int ten = 0;  // Initialize the tens digit with 0
unsigned int hun = 0; // Initialize the hunds digit with 0
unsigned int tho = 0; // Initialize the thous digit with 0
unsigned int TempOne = 0;  // Initialize the TempOnes with 0
unsigned int TempTen = 0;  // Initialize the TempTens with 0
unsigned int TempHun = 0; // Initialize the TempHunds with 0
unsigned int TempTho = 0; // Initialize the TempThous with 0
unsigned int counter = 0;   // variable to count the times of number has been read
unsigned int column = 0;    // variable to set column
unsigned int cur_row = 0;   // variable to set current row
unsigned int lastCol = 0;   // variable to set last column
unsigned int readNum = 0;   // variable to read number from keypad
unsigned int current_num = 0;   // variable to store current number
char hexadecimalNumber[100];    // Initialize the array hexadecimalNumber with 100 elements
char decimalahexNumber[100];    // Initialize the array decimalahexNumber with 100 elements
unsigned int remainder = 0; // variable to store current remainder
unsigned int quotient1 = 0;  // variable to store current quotient
unsigned int quotient2 = 0;  // variable to store current quotient
unsigned int left_SSD = 0;  // Initialize the left_SSD with 0
unsigned int keypadLock = 0;    // variable to "lock" the Pmod keypad
unsigned int buttonLock = 0;    // Variable to "lock" the button
int swi = 0;
int res = 0;
int neg = 0;
int err = 0;
int sto1 = 0;
int sto2 = 0;
int sto3 = 0;
int step = 0;
int pause = 1;
char op;
int show_0 =1;
int show_1 =1;
int show_2 =1;
int show_3 =1;
int op1_ready;
int op2_ready;


#define DELAY_CNT   125000          // Counter to set delay length (1250000 ~ 1sec)
#define DELAY_SSD   1250000/200
#define Btn1        PORTGbits.RG6   // Define Btn1 to appropriate port bit
#define Btn2        PORTGbits.RG7   // Define Btn2 to appropriate port bit
#define Btn3        PORTAbits.RA0   // Define Btn3 to appropriate port bit
#define Led1        LATGbits.LATG12 // Define Led1 to appropriate port bit
#define Led2        LATGbits.LATG13 // Define Led2 to appropriate port bit
#define Led3        LATGbits.LATG14 // Define Led3 to appropriate port bit
#define Led4        LATGbits.LATG15 // Define Led4 to appropriate port bit
// SSD Pmod1 (2 rightmost SSDs)using the bottom rows of JA & JB jumpers
#define SegA1       LATBbits.LATB7
#define SegB1       LATBbits.LATB8
#define SegC1       LATBbits.LATB9
#define SegD1       LATBbits.LATB10
#define SegE1       LATEbits.LATE4
#define SegF1       LATEbits.LATE5
#define SegG1       LATEbits.LATE6
#define DispSel1    LATEbits.LATE7 // Select between the cathodes of the 2 SSDs
// SSD Pmod2 (2 leftmost SSDs)using the bottom rows of JC & JD jumpers
#define SegA2       LATBbits.LATB15
#define SegB2       LATDbits.LATD5
#define SegC2       LATDbits.LATD4
#define SegD2       LATBbits.LATB14
#define SegE2       LATDbits.LATD1
#define SegF2       LATDbits.LATD2
#define SegG2       LATDbits.LATD3
#define DispSel2    LATDbits.LATD12 // Select between the cathodes of the 2 SSDs
// KYPD Pmod attached to MX7 jumper JF
#define COL4        LATFbits.LATF12
#define COL3        LATFbits.LATF5
#define COL2        LATFbits.LATF4
#define COL1        LATFbits.LATF13
#define ROW4        PORTEbits.RE9
#define ROW3        PORTAbits.RA1
#define ROW2        PORTAbits.RA4
#define ROW1        PORTAbits.RA5

void __ISR(_CORE_TIMER_VECTOR, IPL2SOFT) CoreTimerHandler(void) {
    mCTClearIntFlag(); // Clear the interrupt flag 
    if(!left_SSD){ 
        if(show_1){
            DisplayChar(ten, 1);
        }
        else
        {
            SegA1=0; SegB1=0; SegC1=0; SegD1=0;
            SegE1=0; SegF1=0; SegG1=0;
        }
        if(show_3){
            DisplayChar(tho, 3);
        }
        else
        {
            SegA2=0; SegB2=0; SegC2=0; SegD2=0;
            SegE2=0; SegF2=0; SegG2=0;
        }
    }
    else{ 
        if(show_0){
            DisplayChar(one, 0); 
        }
        else
        {
            SegA1=0; SegB1=0; SegC1=0; SegD1=0;
            SegE1=0; SegF1=0; SegG1=0;
        }
        if(show_2){
            DisplayChar(hun, 2);
        }
        else
        {
            SegA2=0; SegB2=0; SegC2=0; SegD2=0;
            SegE2=0; SegF2=0; SegG2=0;
        }
    }
    left_SSD = !left_SSD;
    UpdateCoreTimer(CORE_TICK_RATE);    // Update Core timer
    if(!pause && mode==3){// when overflow set ssd to "HHHH"
        step++;
        if(step==50){
            one = 17;
            ten = 17;
            hun = 17;
            tho = 17;
        }
        else if(step==100){
            one = 16;
            ten = 16;
            hun = 16;
            tho = 16;
            step=0;  
        }
    }
    else if(!pause && mode==4){// when underflow set ssd to "LLLL"
        step++;
        if(step == 50){
            one = 18;
            ten = 18;
            hun = 18;
            tho = 18;
        }
        else if(step == 100){
            one = 16;
            ten = 16;
            hun = 16;
            tho = 16;
            step = 0;  
        }
    }
    else if(!pause && mode==5){// when overflow set ssd to "EEEE"
        step++;
        if(step==50){
            one = 14;
            ten = 14;
            hun = 14;
            tho = 14;
        }
        else if(step==100){
            one = 16;
            ten = 16;
            hun = 16;
            tho = 16;
            step=0;  
        }
    }
}

void main() {
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
    OpenCoreTimer(CORE_TICK_RATE);  // set up the core timer interrupt with a prioirty of 2 and zero sub-priority
    mConfigIntCoreTimer((CT_INT_ON | CT_INT_PRIOR_2 | CT_INT_SUB_PRIOR_0));  
    INTEnableSystemMultiVectoredInt();  // enable multi-vector interrupts
    DDPCONbits.JTAGEN = 0; // Shutoff JTAG
    PORTSetPinsDigitalOut (IOPORT_G, BIT_12|BIT_13|BIT_14|BIT_15); // Set LD1 through LD4 as digital output
    PORTClearBits (IOPORT_G, BIT_12|BIT_13|BIT_14|BIT_15); // Turn all 4 LEDs OFF
    PORTSetPinsDigitalIn (IOPORT_G, BIT_6|BIT_7); // Set Btn1 and Btn2 as inputs
    PORTSetPinsDigitalIn (IOPORT_E, BIT_9); // Set MX7 Port E as input for keypad
    PORTSetPinsDigitalIn (IOPORT_A, BIT_1|BIT_4|BIT_5); // Set MX7 Port A as input for keypad
    PORTSetPinsDigitalOut (IOPORT_F, BIT_12|BIT_5|BIT_4|BIT_13); // Set MX7 Port F as output for keypad
    PORTSetPinsDigitalOut (IOPORT_B, BIT_7|BIT_8|BIT_9|BIT_10|BIT_14|BIT_15);   // Set MX7 Port A as output for SSD1R
    PORTSetPinsDigitalOut (IOPORT_E, BIT_4|BIT_5|BIT_6|BIT_7);  // Set MX7 Port B as output for SSD1R
    PORTSetPinsDigitalOut (IOPORT_D, BIT_1|BIT_2|BIT_3|BIT_4|BIT_5|BIT_12); // Set MX7 Port C and D as output for SSD2
 
    SegA2=0; SegB2=0; SegC2=0; SegD2=0;
    SegE2=0; SegF2=0; SegG2=0;//set ssd pins display nothing
    
    
    while (1){   
        if(mode == 1){
           // Light up Led1
            Led1=Led2 = Led3 = Led4 = 0; // Turn off Led2, 3, 4
            clear();
        }
        
        if(mode == 2){
            Led1 = 1;// Light up Led1
            Led2 = Led3 = Led4 = 0;// Turn off Led2, 3, 4
            
        }
        
        cur_row = Poll_KYPD(column);// Call function Poll_KYPD to get hte current row
        if(cur_row && !keypadLock) {// Check if curr_row is not 0 and keypad is unlocked
            mode = 2;
            DelayCount(1800000);// Wait about 100ms for KYPD key to settle
            keypadLock = 1;// Lock the keypad
            counter++;// counter add up 1
            lastCol = column;// Pass column value to lastCol
            readNum = keyTable[column][cur_row-1];//need to decrement row since row = 0 means no key pressed 
            if (readNum<10){ 
                if (counter == 1) {// Check if counter is 1
                    tho = 0;
                    hun = 0;
                    ten = 0;
                    one = readNum;// Replace current ones with readNum value
                    show_1 = 0;
                    show_2 = 0;
                    show_3 = 0;
                }
                else if (counter == 2) {// Check if counter is 2
                    show_1 = 1;
                    ten = one;// Replace current tens with ones
                    one = readNum;// Replace current ones with readNum value
                }
                else if (counter == 3) {// Check if counter is 3
                    show_2 = 1;
                    hun = ten;// Pass tens value to hunds
                    ten = one;// Replace current tens with ones
                    one = readNum;// Replace current ones with readNum value
                }
                else if (counter == 4) {// Check if counter is 4
                    show_3 = 1;
                    tho = hun;// Pass thous value to hunds
                    hun = ten;// Pass tens value to hunds
                    ten = one;// Replace current tens with ones
                    one = readNum;// Replace current ones with readNum value
                    counter = 0;
                }
            }
            else
            {
                if(readNum == 10){//when press A do addition
                    calcluateres();
                    op = 'A';
                                  
                }
                if(readNum == 11){//when press B do subtraction
                    calcluateres();
                    op = 'S';
                }
                if(readNum == 12){//when press C do multiplcation
                    calcluateres();
                    op = 'M';
                }
                if(readNum == 13){//when press D do division
                    calcluateres();
                    op = 'D';
                }
                if(readNum == 15){
                    swi = (swi+1)%2;
                    ConverDecimalToHex(current_num, swi);
                }
                if (readNum == 14){
                    if(op == 'A'){
                        calcluateres();
                        showres(res);
                        }
                    
                    if(op == 'S'){
                        calcluateres();
                        showres(res);
                    }
                    if(op == 'M'){
                       calcluateres();
                       showres(res);
                   }

                   if(op == 'D'){
                       calcluateres();
                       showres(res);
                   }
                }
            }
            TempOne = one;// Pass current ones to temporary ones
            TempTen = ten;// Pass current tens to temporary tens 
            TempHun = hun;// Pass current hunds to temporary hunds 
            TempTho = tho;// Pass current thous to temporary thous
            current_num = one + (10 * ten) + (100 * hun) + (1000 * tho);// To calculate the current decimal number
        }
        else if ((column==lastCol) && (cur_row==0) && keypadLock){// key that was pressed has been released
            keypadLock = 0;//unlock the keypad 
        }
        if( res > 9999){//When overflow 
            mode = 3;
            show_1 = show_2 = show_3 = 1;
            Led1 = Led2 = Led3 = Led4 = 0;
            pause = 0;
        }
        if (neg < (0-9999)){//when underflow
            mode = 4;
            show_1 = show_2 = show_3 = 1;
            Led1 = Led2 = Led3 = Led4 = 0;
            pause = 0;  
        }
        if ( err == 1){
            mode = 5;
            show_1 = show_2 = show_3 = 1;
            Led1 = Led2 = Led3 = Led4 = 0;
            pause = 0; 
        }
        if ( Btn1 && !buttonLock)
        {
            mode = 1;
            buttonLock = 1;
            pause = 1;
        }
        else if (!Btn1 && buttonLock){
            DELAY_CNT;
            buttonLock = 0;
        }
        
        column = (column+1)%4;//increment column so next time thru //it polls the next column of keys
    }
}

void clear()
{
    show_1 = show_2 = show_3 = 0;
    one = 0;
    ten = 0; 
    hun = 0; 
    tho = 0;  
    counter = 0;
}
void showres(int value)
{
    one = res%10;
    ten = (res/10)%10;
    hun = (res/100)%10;
    tho = res/1000;
    if(hun > 0 || ten > 0 || tho > 0){
        show_1 = 1;
    }
    if(tho > 0 || hun > 0){
        show_2 = 1;
    }
    if((tho) > 0){
        show_3 = 1;
    }
}
void calcluateres()
{
    if(!op1_ready)
    {
        sto1 = current_num;
        clear();
        op1_ready = 1;
    }
    else 
    {
        sto2 = current_num;
        clear();
        if(op == 'A')
        {
            res = sto1 + sto2;
            if (neg < 0)
            {
                res = neg + sto2;
                neg = res;
            }
        }
        if(op == 'S')
        {   
            if (neg < 0)
            {
                res = neg - sto2;
            }
            else
            {
                res = sto1 - sto2;
            }
            if (res < 0)
            {
                neg = res;
            }
        }
        if(op == 'M')
        {
            res = sto1 * sto2;
            if (neg < 0)
            {
                res = neg * sto2;
                neg = res;
            }
        }
        if(op == 'D')
        {   
            if (sto2 == 0){
                err = 1;
            }
            else{
                res = sto1 / sto2;
                if (neg < 0)
                {
                    res = neg / sto2;
                    neg = res;
                }
            }
        }
        sto1 = res;
        if (res < 0)
        {
            res = 0 - res;
        }
        showres(res);
    }
}
void ConverDecimalToHex (int value, int sw) {
    unsigned int i = 1;
    if(sw == 1){
        quotient1 = value;// Pass parameter value to quotient
        while (quotient1 != 0)  {// if quotient is not equal to 0, the code inside while loop will always execute
            remainder = quotient1 % 16;// Calculate remainder
            hexadecimalNumber[i++] = remainder;// Pass each calculated remainder into an array
            quotient1 = quotient1 / 16;// Get the new quotient number
        }
        one = hexadecimalNumber[1];// Pass the first element in hexadecimalNumber to ones 
        ten = hexadecimalNumber[2];// Pass the second element in hexadecimalNumber to tens 
        hun = hexadecimalNumber[3];// Pass the third element in hexadecimalNumber to hunds 
        tho = hexadecimalNumber[4];// Pass the fourth element in hexadecimalNumber to thous 
    }
    else{
        quotient2 = value;// Pass parameter value to quotient
        while (quotient2 != 0)  {// if quotient is not equal to 0, the code inside while loop will always execute
            remainder = quotient2 % 10;// Calculate remainder
            decimalahexNumber[i++] = remainder;// Pass each calculated remainder into an array
            quotient2 = quotient2 / 10;// Get the new quotient number
        }
        one = decimalahexNumber[1];// Pass the first element in hexadecimalNumber to ones 
        ten = decimalahexNumber[2];// Pass the second element in hexadecimalNumber to tens 
        hun = decimalahexNumber[3];// Pass the third element in hexadecimalNumber to hunds 
        tho = decimalahexNumber[4];// Pass the fourth element in hexadecimalNumber to thous 
    }
}

int Poll_KYPD (int col) {
    int row;
    switch(col) {
      
        case 0: COL1 = 0; COL2 = 1; COL3 = 1; COL4 = 1; break;
       
        case 1: COL1 = 1; COL2 = 0; COL3 = 1; COL4 = 1; break;
       
        case 2: COL1 = 1; COL2 = 1; COL3 = 0; COL4 = 1; break;
       
        case 3: COL1 = 1; COL2 = 1; COL3 = 1; COL4 = 0; break;
        default:break;
    }
    DelayCount(200000);
    
    if (!ROW1) {
        row = 1;
    }
    else if (!ROW2) {
        row = 2;
    }
    else if (!ROW3) {
        row = 3;
    }
    else if (!ROW4) {
        row = 4;
    }
    else {
        row = 0;
    }
    return row;
}
void DisplayChar(int num,int SSD)
{
    if(SSD > 1)
    {      
        DispSel2 = SSD-2;
        switch(num)
        {
            case 0:
                SegA2=1; SegB2=1; SegC2=1; SegD2=1;
                SegE2=1; SegF2=1; SegG2=0;break;
            case 1:
                SegA2=0; SegB2=1; SegC2=1; SegD2=0;
                SegE2=0; SegF2=0; SegG2=0;break;
            case 2:
                SegA2=1; SegB2=1; SegC2=0; SegD2=1;
                SegE2=1; SegF2=0; SegG2=1;break;
            case 3:
                SegA2=1; SegB2=1; SegC2=1; SegD2=1;
                SegE2=0; SegF2=0; SegG2=1;break;
            case 4:
                SegA2=0; SegB2=1; SegC2=1; SegD2=0;
                SegE2=0; SegF2=1; SegG2=1;break;
            case 5:
                SegA2=1; SegB2=0; SegC2=1; SegD2=1;
                SegE2=0; SegF2=1; SegG2=1;break;
            case 6:
                SegA2=1; SegB2=0; SegC2=1; SegD2=1;
                SegE2=1; SegF2=1; SegG2=1;break;
            case 7:
                SegA2=1; SegB2=1; SegC2=1; SegD2=0;
                SegE2=0; SegF2=0; SegG2=0;break;
            case 8:
                SegA2=1; SegB2=1; SegC2=1; SegD2=1;
                SegE2=1; SegF2=1; SegG2=1;break;
            case 9:
                SegA2=1; SegB2=1; SegC2=1; SegD2=1;
                SegE2=0; SegF2=1; SegG2=1;break;
            case 10:
                SegA2=1; SegB2=1; SegC2=1; SegD2=0;
                SegE2=1; SegF2=1; SegG2=1;break;
            case 11:
                SegA2=0; SegB2=0; SegC2=1; SegD2=1;
                SegE2=1; SegF2=1; SegG2=1;break;
            case 12:
                SegA2=1; SegB2=0; SegC2=0; SegD2=1;
                SegE2=1; SegF2=1; SegG2=0;break;
            case 13:
                SegA2=0; SegB2=1; SegC2=1; SegD2=1;
                SegE2=1; SegF2=0; SegG2=1;break;
            case 14:
                SegA2=1; SegB2=0; SegC2=0; SegD2=1;
                SegE2=1; SegF2=1; SegG2=1;break;
            case 15:
                SegA2=1; SegB2=0; SegC2=0; SegD2=0;
                SegE2=1; SegF2=1; SegG2=1;break;
            case 16:
                SegA2=0; SegB2=0; SegC2=0; SegD2=0;
                SegE2=0; SegF2=0; SegG2=0;break;
            case 17://H
                SegA2=0; SegB2=1; SegC2=1; SegD2=0;
                SegE2=1; SegF2=1; SegG2=1;break;
            case 18://L
                SegA2=0; SegB2=0; SegC2=0; SegD2=1;
                SegE2=1; SegF2=1; SegG2=0;break;
            default:
                break;
        }   
    }
    else
    {
        DispSel1 = SSD;
        switch(num)
        {
            case 0:
                SegA1=1; SegB1=1; SegC1=1; SegD1=1;
                SegE1=1; SegF1=1; SegG1=0;break;
            case 1:
                SegA1=0; SegB1=1; SegC1=1; SegD1=0;
                SegE1=0; SegF1=0; SegG1=0;break;
            case 2:
                SegA1=1; SegB1=1; SegC1=0; SegD1=1;
                SegE1=1; SegF1=0; SegG1=1;break;
            case 3:
                SegA1=1; SegB1=1; SegC1=1; SegD1=1;
                SegE1=0; SegF1=0; SegG1=1;break;
            case 4:
                SegA1=0; SegB1=1; SegC1=1; SegD1=0;
                SegE1=0; SegF1=1; SegG1=1;break;
            case 5:
                SegA1=1; SegB1=0; SegC1=1; SegD1=1;
                SegE1=0; SegF1=1; SegG1=1;break;
            case 6:
                SegA1=1; SegB1=0; SegC1=1; SegD1=1;
                SegE1=1; SegF1=1; SegG1=1;break;
            case 7:
                SegA1=1; SegB1=1; SegC1=1; SegD1=0;
                SegE1=0; SegF1=0; SegG1=0;break;
            case 8:
                SegA1=1; SegB1=1; SegC1=1; SegD1=1;
                SegE1=1; SegF1=1; SegG1=1;break;
            case 9:
                SegA1=1; SegB1=1; SegC1=1; SegD1=1;
                SegE1=0; SegF1=1; SegG1=1;break;
            case 10:
                SegA1=1; SegB1=1; SegC1=1; SegD1=0;
                SegE1=1; SegF1=1; SegG1=1;break;
            case 11:
                SegA1=0; SegB1=0; SegC1=1; SegD1=1;
                SegE1=1; SegF1=1; SegG1=1;break;
            case 12:
                SegA1=1; SegB1=0; SegC1=0; SegD1=1;
                SegE1=1; SegF1=1; SegG1=0;break;
            case 13:
                SegA1=0; SegB1=1; SegC1=1; SegD1=1;
                SegE1=1; SegF1=0; SegG1=1;break;
            case 14:
                SegA1=1; SegB1=0; SegC1=0; SegD1=1;
                SegE1=1; SegF1=1; SegG1=1;break;
            case 15:
                SegA1=1; SegB1=0; SegC1=0; SegD1=0;
                SegE1=1; SegF1=1; SegG1=1;break;
            case 16:
                SegA1=0; SegB1=0; SegC1=0; SegD1=0;
                SegE1=0; SegF1=0; SegG1=0;break;
            case 17://H
                SegA1=0; SegB1=1; SegC1=1; SegD1=0;
                SegE1=1; SegF1=1; SegG1=1;break;
            case 18://L
                SegA1=0; SegB1=0; SegC1=0; SegD1=1;
                SegE1=1; SegF1=1; SegG1=0;break;
            default:
                break;
        }
    }
}
void DelayCount(int count)
{
    int     counter;
    for (counter=0; counter<count; counter++){}
}
