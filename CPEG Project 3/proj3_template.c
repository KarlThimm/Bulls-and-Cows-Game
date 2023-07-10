/*===================================CPEG222====================================
 * Program:      Project 3 template
 * Authors:     Karl Thimm
 * Date:        10/22/2021
 * This is a template that you can use to write your project 3 code, for mid-stage and final demo.
==============================================================================*/
/*-------------- Board system settings. PLEASE DO NOT MODIFY THIS PART ----------*/
#ifndef _SUPPRESS_PLIB_WARNING          //suppress the plib warning during compiling
#define _SUPPRESS_PLIB_WARNING
#endif
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config POSCMOD = XT             // Primary Oscillator Configuration (XT osc mode)
#pragma config FPBDIV = DIV_8           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
/*----------------------------------------------------------------------------*/

#include <xc.h>   //Microchip XC processor header which links to the PIC32MX370512L header
#include <stdio.h>  // need this for sprintf
#include <time.h>   // need this to randomize the seed of random numbers
#include <sys/attribs.h>
#include "proj3config.h" // Basys MX3 configuration header
#include "proj3led.h"    // Digilent Library for using the on-board LEDs
#include "proj3ssd.h"    // Digilent Library for using the on-board SSDs
#include "proj3lcd.h"    // Digilent Library for using the on-board LCD
#include "proj3acl.h"    // Digilent Library for using the on-board accelerometer


// below are keypad row and column definitions based on the assumption that JB will be used and columns are CN pins
// If you want to use JA or use rows as CN pins, modify this part
#define R4 LATCbits.LATC14
#define R3 LATDbits.LATD0
#define R2 LATDbits.LATD1
#define R1 LATCbits.LATC13
#define C4 PORTDbits.RD9
#define C3 PORTDbits.RD11
#define C2 PORTDbits.RD10
#define C1 PORTDbits.RD8
#define SSD_EMPTY_DIGIT 18
int Number = 0;//NUmber for buttons
int Number_Presses = 0;//number of presses
char arr4[4]; //arr to be entered
char first[16]; //buffer
char second[16];//buffer
char third[16];//buffer
char fourth[16];//buffer
int mode = 0;//mode
int flag = 0;//flag
char arr_secret[4];//secret array
char set_array[4];//set array
unsigned char d4 = SSD_EMPTY_DIGIT; //variable for SSD
unsigned char d3 = SSD_EMPTY_DIGIT; //variable for SSD
unsigned char d2 = SSD_EMPTY_DIGIT; //variable for SSD
unsigned char d1 = SSD_EMPTY_DIGIT; //variable for SSD
char ssdIsOn = 0;
int counter = 0;
int Seconds = 0;
int led_counter=7;
int exist = 0;
int exactmatch = 0;
int try = 0;//tries = 0
char secretfirst[16];//buffer
char secretsecond[16];//buffer
char secretthird[16];//buffer
char secretfourth[16];//buffer
char arr_set[4];
char exactbuf[16];//buffer
char matchbuf[16];//buffer
int Score = 0;//score initialized
char scorebuf[16];//buffer

/*#define R4 LATCbits.LATC3
#define R3 LATDbits.LATG7
#define R2 LATDbits.LATG8
#define R1 LATCbits.LATCG9
#define C4 PORTDbits.RC2
#define C3 PORTDbits.RC1
#define C2 PORTDbits.RC4
#define C1 PORTDbits.RG6*/

typedef enum _KEY {
    K0, K1, K2, K3, K4, K5, K6, K7, K8, K9, K_A, K_B, K_C, K_D, K_E, K_F, K_NONE
} eKey;

typedef enum _STATE {
    INITIAL
} eStates;

eStates state = INITIAL;

// subrountines
void CNConfig();
void Game(eKey key);
void updateLCD();
void secret_number();
void SSD_Timer2();
void toggle_SSD();
//void __ISR(_TIMER_2_VECTOR, ip17SOFT) T2_Handler(void);

int main(void) {
    DDPCONbits.JTAGEN = 0; // Required to use Pin RA0 (connected to LED 0) as IO
    /* Initialization of LED, LCD, SSD, etc */

    //LCD_WriteStringAtPos("Press to Play", 0, 0);
    //LCD_WriteStringAtPos("D-Det. E-Rand", 1, 0);
    LCD_Init();
    ACL_Init();
    LED_Init();
    updateLCD();
    SSD_Init();
    SSD_Timer2();
    //toggle_SSD();

    arr4[3] = -1;
    arr4[2] = -1;
    arr4[1] = -1;
    arr4[0] = -1;
    d4 = 17;
    d3 = 17;
    d2 = 17;
    d1 = 17;
    toggle_SSD();
    float rgACLGVals[3];
    ACL_ReadGValues(rgACLGVals);
    int seed = rgACLGVals[0] * 10000;
    srand((unsigned) seed);
    // below are keypad row and column configurations based on the assumption that JB will be used and columns are CN pins
    // If you want to use JA or use rows as CN pins, modify this part

    // keypad rows as outputs
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    ANSELDbits.ANSD1 = 0;
    TRISCbits.TRISC14 = 0;
    TRISCbits.TRISC13 = 0;

    // keypad columns as inputs
    TRISDbits.TRISD8 = 1;
    TRISDbits.TRISD9 = 1;
    TRISDbits.TRISD10 = 1;
    TRISDbits.TRISD11 = 1;
    // What do I change here to switch ports

    // You need to enable all the rows
    R1 = R2 = R3 = R4 = 0;

    CNConfig();
    updateLCD();

    /* Other initialization and configuration code */

    while (1) {
        //This loop must remain empty
    }
}

void CNConfig() {
    /* Make sure vector interrupts is disabled prior to configuration */
    macro_disable_interrupts;

    //Complete the following configuration of CN interrupts, then uncomment them
    CNCONDbits.ON = 1; //all port D pins to trigger CN interrupts
    CNEND = 0xf00; //configure PORTD pins 8-11 as CN pins
    CNPUD = 0xf00; //enable pullups on PORTD pins 8-11

    IPC8bits.CNIP = 6; // set CN priority to  6
    IPC8bits.CNIS = 3; // set CN sub-priority to 3

    IFS1bits.CNDIF = 0; //Clear interrupt flag status bit
    IEC1bits.CNDIE = 1; //Enable CN interrupt on port D


    int j = PORTD; //read port to clear mismatch on CN pins
    macro_enable_interrupts(); // re-enable interrupts
}

void __ISR(_CHANGE_NOTICE_VECTOR) CN_Handler(void) {
    int i;
    flag = 0;
    eKey key = K_NONE;


    // 1. Disable CN interrupts
    IEC1bits.CNDIE = 0;

    // 2. Debounce keys for 10ms
    for (i = 0; i < 1426; i++) {
    }

    // 3. Decode which key was pressed
/*If a key is pressed this will determine which key it was and it will set that key equal to number while also setting
 flag to 1 which means the button was pressed*/
    // check first row 
    R1 = 0;
    R2 = R3 = R4 = 1;
    if (C1 == 0) {
        Number = 1;
        flag = 1;
    }// first column
    else if (C2 == 0) {
        Number = 2;
        flag = 1;
    }// second column
    else if (C3 == 0) {
        Number = 3;
        flag = 1;
    }// third column
    else if (C4 == 0) {
        Number = 0xA;
        flag = 1;
    } // fourth column

    // check second row 
    R2 = 0;
    R1 = R3 = R4 = 1;
    if (C1 == 0) {
        Number = 4;
        flag = 1;
    } else if (C2 == 0) {
        Number = 5;
        flag = 1;
    } else if (C3 == 0) {
        Number = 6;
        flag = 1;
    } else if (C4 == 0) {
        Number = 0xB;
        flag = 1;
    }

    // check third row 
    R3 = 0;
    R1 = R2 = R4 = 1;
    if (C1 == 0) {
        Number = 7;
        flag = 1;
    } else if (C2 == 0) {
        Number = 8;
        flag = 1;
    } else if (C3 == 0) {
        Number = 9;
        flag = 1;
    } else if (C4 == 0) {
        Number = 0xC;
        flag = 1;
    }

    // check fourth row 
    R4 = 0;
    R1 = R2 = R3 = 1;
    if (C1 == 0) {
        Number = 0;
        flag = 1;
    } else if (C2 == 0) {
        Number = 0xF;
        flag = 1;
    } else if (C3 == 0) {
        Number = 0xE;
        flag = 1;
    } else if (C4 == 0) {
        Number = 0xD;
        flag = 1;
    }

    // re-enable all the rows for the next round
    R1 = R2 = R3 = R4 = 0;

    // if any key has been pressed, update next state and outputs
    if (key != K_NONE) {
        //Game(key) ;
        //updateLCD() ;
    }
    if (flag == 1) {
        Game(key);
    }

    /*if(flag==1)
    {
        
        if(Number>=0 && Number<=9)
        {
            if(Number_Presses<=4)
            {
                arr4[Number_Presses-1]=Number;
                updateLCD();
                LED_SetGroupValue(Number);
            }
            else
            {
            Number_Presses=4;
            }
        }
        if(Number==0xD)
        {
            Number_Presses--;
            arr4[Number_Presses]=-1;
            updateLCD(); 
            LED_SetGroupValue(Number);
        }
        else if(Number==0xC)
        {
            Number_Presses=0;
            arr4[3]=-1;
            arr4[2]=-1;
            arr4[1]=-1;
            arr4[0]=-1;
            updateLCD();
            LED_SetGroupValue(Number);
            
        }
        if(Number_Presses<0)
        {
            Number_Presses=0;
        }
        if(Number==0xA)
        {
        LED_SetGroupValue(Number);
        
        }
        if(Number==0xB)
        {
        LED_SetGroupValue(Number);
        
        }
        if(Number==0xE)
        {
        LED_SetGroupValue(Number);
        
        }
        if(Number==0xF)
        {
        LED_SetGroupValue(Number);
        
        }
        
    }*/




    int j = PORTD; //read port to clear mismatch on CN pints

    // 4. Clear the interrupt flag
    IFS1bits.CNDIF = 0;

    // 5. Reenable CN interrupts
    IEC1bits.CNDIE = 1;
}

void Game(eKey key) {
    switch (mode) {
        case 0: //Idle
            //LCD_WriteStringAtPos("Press to Play", 0, 0);
            //LCD_WriteStringAtPos("D-Det. E-Rand", 1, 0);
            //updateLCD();

            if (flag == 1) {
                if (Number == 0xD) { //If button d is pressed from mode 0 the array is set to 1234
                    updateLCD();
                    mode = 1;
                    arr_secret[4] = 4;
                    arr_secret[3] = 3;
                    arr_secret[2] = 2;
                    arr_secret[1] = 1;
                    T2CONbits.ON = 1; //turn timer on
                    d4 = 17; //clear
                    d3 = 1; //1
                    d2 = 0; //0
                    d1 = 0; //0
                    toggle_SSD(); //Turn on SSD
                }

                if (Number == 0xE) {//If button 
                    updateLCD();
                    mode = 2;
                    secret_number();
                    //LCD_WriteStringAtPos("Mode 2", 0, 0);
                    T2CONbits.ON = 1; //timer on
                    d4 = 17; //clear
                    d3 = 2; //2
                    d2 = 0;///0
                    d1 = 0;//0
                    toggle_SSD();//Tunr on SSD
                }
            }
            break;

        case 1: //Deterministic number 
            //LCD_WriteStringAtPos("Mode 1", 0, 0);

            updateLCD();
            if (flag == 1) {
                if (Number >= 0 && Number <= 9) { //If button 0-9 is prressed write to LCD
                    if (Number_Presses < 4) {
                        arr4[Number_Presses] = Number;
                        updateLCD();
                        LED_SetGroupValue(Number);
                        Number_Presses += 1;
                    } else {
                        Number_Presses = 4;
                    }
                }
                if (Number == 0xD) { //If D is pressed delete 1 character
                    Number_Presses--;
                    arr4[Number_Presses] = -1;
                    updateLCD();
                    LED_SetGroupValue(Number);
                } else if (Number == 0xC) {
                    Number_Presses = 0;
                    arr4[3] = -1; //CLears 0 in space
                    arr4[2] = -1;//CLears 0 in space
                    arr4[1] = -1;//CLears 0 in space
                    arr4[0] = -1;//CLears 0 in space
                    updateLCD();
                    LED_SetGroupValue(Number);

                }
                if (Number_Presses < 0) { //Stops from deleting past array
                    Number_Presses = 0;
                }
                if (Number == 0xF) { //if button F is pressed mode is 4
                    mode = 4;

                }
                if(Number==0xE) //If button pressed is e
                    if(Number_Presses==4) //Only executes if 4 numbers are entered
                    {
                        Number_Presses=0;
                        checker();
                        arr4[3] = -1;
                        arr4[2] = -1;
                        arr4[1] = -1;
                        arr4[0] = -1;
                        sprintf(matchbuf, "%dB", exactmatch);
                        LCD_WriteStringAtPos(matchbuf, 0, 14);
                        
                        sprintf(exactbuf, "    %dA", exist);
                        LCD_WriteStringAtPos(exactbuf, 0, 8);
                        exactmatch = 0;
                        exist = 0;
                        try++;
                        if(try==10) //If tires = 10 game over
                        {
                            mode = 4;
                        }
                    }

            }
            break;


        case 2: //Random number
            //LCD_WriteStringAtPos("Mode 2", 0, 0);
            //secret_number();
            //updateLCD();
            if (flag == 1) { //If buttn is pressed
                if (Number >= 0 && Number <= 9) { //If number between 0 and 9
                    if (Number_Presses < 4) { //If number pressses less than 4 execute
                        arr4[Number_Presses] = Number; //Set number presses array to number
                        updateLCD();
                        LED_SetGroupValue(Number);
                        Number_Presses += 1; //Add 1 to mumber presses
                       } 
                    else {
                        Number_Presses = 4; //else number is 4
                    }
                }
                if (Number == 0xD) { //Pressing button d will remove 1 element of the arrray
                    Number_Presses--;
                    arr4[Number_Presses] = -1;
                    updateLCD();
                    LED_SetGroupValue(Number);
                } else if (Number == 0xC) { //Pressing button c clears the array entered
                    Number_Presses = 0;
                    arr4[3] = -1; //Clears 0 from space
                    arr4[2] = -1;//Clears 0 from space
                    arr4[1] = -1;//Clears 0 from space
                    arr4[0] = -1;//Clears 0 from space
                    updateLCD();
                    LED_SetGroupValue(Number);

                }
                if (Number_Presses < 0) {
                    Number_Presses = 0;
                }
                if (Number == 0xF) {
                    mode = 4;

                }
                if(Number==0xE) //If button E is pressed
                    if(Number_Presses==4) //Only execute if number presses is 4
                    {
                        Number_Presses=0;//set number_presses to 0
                        checker(); //Checks if array entered is correct
                        arr4[3] = -1; //Clears 0 from space
                        arr4[2] = -1;//Clears 0 from space
                        arr4[1] = -1;//Clears 0 from space
                        arr4[0] = -1;//Clears 0 from space
                        sprintf(matchbuf, "%dB", exactmatch);//Prints onto LCD
                        LCD_WriteStringAtPos(matchbuf, 0, 14);//Prints buffer onto LCD
                        
                        LCD_WriteStringAtPos("                ", 1, 0); //Clears space
                        
                        sprintf(exactbuf, "    %dA", exist);
                        LCD_WriteStringAtPos(exactbuf, 0, 8);
                        exactmatch = 0;
                        exist = 0;
                        try++;
                    }
                if(try==10) //If tries is 10 game over
                {
                    mode = 4; //enter mode 4
                }

            }
            break;

        case 3: //Win
            updateLCD();
            Score = Seconds + (10 - try)*20; //Calculates score based on tries and time left
            
            LCD_WriteStringAtPos("Passed:", 0, 0); //ASll belwo write onto LCD the score and correct array
            
        sprintf(secretfirst, "%d", arr_secret[0]);
        LCD_WriteStringAtPos(secretfirst, 0, 7);

        sprintf(secretsecond, "%d", arr_secret[1]);
        LCD_WriteStringAtPos(secretsecond, 0, 8);

        sprintf(secretthird, "%d", arr_secret[2]);
        LCD_WriteStringAtPos(secretthird, 0, 9);

        sprintf(secretfourth, "%d", arr_secret[3]);
        LCD_WriteStringAtPos(secretfourth, 0, 10);
        
        sprintf(scorebuf, "Score=%d            ", Score);
        LCD_WriteStringAtPos(scorebuf, 1, 0);
            
            T2CONbits.ON = 0;//timer off
            break;
            
        case 4: //Time up / give up
            T2CONbits.ON = 0; //timer off
            Score=0; //Score is 0
            sprintf(scorebuf, "Score=%d            ", Score); //Prints score and array onto LCD
            LCD_WriteStringAtPos(scorebuf, 1, 0);
            
            LCD_WriteStringAtPos("Failed:",0,0);
            
            sprintf(secretfirst, "%d", arr_secret[0]);
            LCD_WriteStringAtPos(secretfirst, 0, 7);

            sprintf(secretsecond, "%d", arr_secret[1]);
            LCD_WriteStringAtPos(secretsecond, 0, 8);

            sprintf(secretthird, "%d", arr_secret[2]);
            LCD_WriteStringAtPos(secretthird, 0, 9);

            sprintf(secretfourth, "%d      ", arr_secret[3]);
            LCD_WriteStringAtPos(secretfourth, 0, 10);

    }
}

void updateLCD() { //Updating LCD function to display text
    if (mode == 0); //In mode 0
    {
        LCD_WriteStringAtPos("Press To Play", 0, 0);
        LCD_WriteStringAtPos("D-Det. E-Rand", 1, 0);
    }
    if (mode == 1) { //In mode 1
        LCD_WriteStringAtPos("Try:", 0, 0);

        sprintf(first, "%d", arr4[0]);
        LCD_WriteStringAtPos(first, 0, 4);

        sprintf(second, "%d", arr4[1]);
        LCD_WriteStringAtPos(second, 0, 5);

        sprintf(third, "%d", arr4[2]);
        LCD_WriteStringAtPos(third, 0, 6);

        sprintf(fourth, "%d", arr4[3]);
        LCD_WriteStringAtPos(fourth, 0, 7);

        LCD_WriteStringAtPos(" ", 0, 8);
        if (arr4[3] == -1) {
            LCD_WriteStringAtPos(" ", 0, 7);
        }
        if (arr4[2] == -1) {
            LCD_WriteStringAtPos(" ", 0, 6);
        }
        if (arr4[1] == -1) {
            LCD_WriteStringAtPos(" ", 0, 5);
        }
        if (arr4[0] == -1) {
            LCD_WriteStringAtPos(" ", 0, 4);
        }
    }
    if (mode == 2) { //In mode 2
        LCD_WriteStringAtPos("Try:", 0, 0);

        sprintf(first, "%d", arr4[0]);
        LCD_WriteStringAtPos(first, 0, 4);

        sprintf(second, "%d", arr4[1]);
        LCD_WriteStringAtPos(second, 0, 5);

        sprintf(third, "%d", arr4[2]);
        LCD_WriteStringAtPos(third, 0, 6);

        sprintf(fourth, "%d", arr4[3]);
        LCD_WriteStringAtPos(fourth, 0, 7);

        LCD_WriteStringAtPos(" ", 0, 8);
        if (arr4[3] == -1) {
            LCD_WriteStringAtPos(" ", 0, 7);
        }
        if (arr4[2] == -1) {
            LCD_WriteStringAtPos(" ", 0, 6);
        }
        if (arr4[1] == -1) {
            LCD_WriteStringAtPos(" ", 0, 5);
        }
        if (arr4[0] == -1) {
            LCD_WriteStringAtPos(" ", 0, 4);
        }
    }
}

void secret_number() { //Generates secret number, if there is a duplicate it will try again until there isnt
    int i = 0;
    int Dont_Add;
    while (i < 4) {
        Dont_Add = 0;
        arr_secret[i] = rand() % 10;
        for (int j = 0; j < i; j++) {
            if (arr_secret[i] == arr_secret[j]) {
                Dont_Add = 1;
            }
        }
        if (Dont_Add == 1) {
            i++;
        }
    }
}

void SSD_Timer2() { //SSD timer setup
    PR2 = 4882; //Can it be a decimal
    T2CONbits.TCKPS = 7; //1:264
    T2CONbits.TGATE = 0;
    T2CONbits.TCS = 0;
    T2CONbits.ON = 0; //timer off
    IPC2bits.T2IP = 7; //priority
    IPC2bits.T2IS = 3; //sub priority
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;
    macro_enable_interrupts();
}

void __ISR(_TIMER_2_VECTOR, ipl7) Timer2ISR(void)  { //Sets up timer ISR
    //lat_SSD_AN1 = lat_SSD_AN1 = lat_SSD_AN1 = lat_SSD_AN1 = 1;
    IEC0bits.T2IE = 0;

    counter++;
    if (mode == 1) { //during mode 1
        Seconds = 100 - (counter / 8); //Counter is 100
        if (counter >= 8) {
            d4 = 17;
            d3 = 17;
            d2 = Seconds / 10; //Counts down from 100
            d1 = Seconds % 10;
            toggle_SSD();
        }
        if (counter == 800) { //If counter hits 0 mode is 4
            counter = 0;
            mode = 4;
            updateLCD();
        }
        //Number = (Number << 1) | (Number >> 7); //Left to right every 1/8 second   
    }
    if (mode == 2) { //During mode 2
        Seconds = 200 - (counter / 8);
        if (counter >= 8) {
            d4 = 17;
            d3 = Seconds / 100; //Counts down from 200
            d2 = (Seconds % 100)/10;
            d1 = (Seconds % 100)%10;
            toggle_SSD();
        }
        if (counter == 1600) { //If timer = 0 game over
            counter = 0;
            mode = 4;
        }
    }
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;
}

void toggle_SSD() { //Toggles SSD for timer
    //int number;
    if (ssdIsOn) {
        // clear the SSD
        SSD_WriteDigits(SSD_EMPTY_DIGIT, SSD_EMPTY_DIGIT, SSD_EMPTY_DIGIT, SSD_EMPTY_DIGIT, 0, 0, 0, 0);
    } else {
        // show decimal number with leading zeroes
        /*number = rand() % 256;
        unsigned char d4 = (number / 1000) % 10;
        unsigned char d3 = (number / 100) % 10;
        unsigned char d2 = (number / 10) % 10;
        unsigned char d1 = number % 10; */
        // logic to remove leading zeroes could go here
        SSD_WriteDigits(d1, d2, d3, d4, 0, 0, 0, 0);

    }
}

void checker() //Checks if entered array matches the given array
{
    for(int i = 0; i < 4 ; i++)
    {
        if(arr_secret[i]==arr4[i])
        {
        exactmatch++;    
        }
        if(exactmatch==4)
        {
            mode = 3;
        }
    }
    if(arr_secret[4]==arr4[4])
    {
        exist++;
    }
    if(arr_secret[4]==arr4[3])
    {
        exist++;
    }
    if(arr_secret[4]==arr4[2])
    {
        exist++;
    }
    if(arr_secret[4]==arr4[1])
    {
        exist++;
    }
    if(arr_secret[3]==arr4[4])
    {
        exist++;
    }if(arr_secret[3]==arr4[3])
    {
        exist++;
    }
    if(arr_secret[3]==arr4[2])
    {
        exist++;
    }
    if(arr_secret[3]==arr4[1])
    {
        exist++;
    }
    if(arr_secret[2]==arr4[4])
    {
        exist++;
    }
    if(arr_secret[2]==arr4[3])
    {
        exist++;
    }
    if(arr_secret[2]==arr4[2])
    {
        exist++;
    }
    if(arr_secret[2]==arr4[1])
    {
        exist++;
    }
    if(arr_secret[1]==arr4[4])
    {
        exist++;
    }
    if(arr_secret[1]==arr4[3])
    {
        exist++;
    }
    if(arr_secret[1]==arr4[2])
    {
        exist++;
    }
    if(arr_secret[1]==arr4[1])
    {
        exist++;
    }    
}

