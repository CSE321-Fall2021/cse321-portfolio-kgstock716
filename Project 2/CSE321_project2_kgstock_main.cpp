/** 
  * Author: Katherine Stock (kgstock)
  * 
  * File Purpose: Count-down or count-up alarm system
  * Modules: isr_c1, isr_c2, isr_c3, isr_c4, timeChange 
  * Assignment: CSE321 Project 2 
  *
  * Inputs:  4x4 Keypad
  * Outputs: 7 external LEDs, LCD 
  * 
  * Constraints: Bounce, communication with Keypad and LCD
  * References: 10/6 Lecture code, Ticker documentation - https://os.mbed.com/docs/mbed-os/v6.15/apis/ticker.html 
  * 
  */ 

#include "mbed.h"
#include "1802.h"
#include <cstdio>

//define variables for state machine for readability
#define CDStart 0
#define CDKeypad 1
#define CDTimer 2
#define CDEnd 3

#define CUStart 4
#define CUKeypad 5
#define CUTimer 6
#define CUEnd 7

//storage for time variables
//cuMins and cuSecs are used to count up until mins and secs is reached in count up mode
//mins and secs are directly decremented in count down
//storedM and storedS are used to detect change in the timer (1 second passing) and trigers updating the LCD as infrequently as possible
int mins = 0;
int secs = 00;
int cuMins = 0;
int cuSecs = 00;
int storedM = 0;
int storedS = 00;

//count numbers entered by the user to build mins and seconds. only first 3 numbers are accepted, if user makes a mistake. they must press B to turn off and reset timer.
int numbersEntered = 0;

//counter for polling keypad input
int row = 0; 

//boolean for switching between count up and count down modes
bool countUp = false;

//character array to print on each second, pointer will be populated using sprintf so that the lcd print can handle the variable numbers
char printString[15];


//Establish one interrupt for each column we're reading from, use PC 8, 9, 10, 11
InterruptIn column1(PC_8, PullDown); 
InterruptIn column2(PC_9, PullDown);
InterruptIn column3(PC_10, PullDown);
InterruptIn column4(PC_11, PullDown);

//Establish an isr for each column interrupt to use, the ISR is triggered by keypad input and narrows the row the input was from to determine 
//the value of the key pressed and act accordingly depending on the current state
void isr_c1(void);
void isr_c2(void);
void isr_c3(void);
void isr_c4(void);

//Establish void function that will be called by timer every 1 second to increment/decrement the time remaining variable.
//This function is shared by counting up and down but has different behavior for each
void timeChange(void);

//timer states are defined above. Counting up and down each have 4 states: Start, keypad, timer, and end. Default state upon turn on is count down.
int state = CDStart;

//establish LCD, 16 columns, 2 rows, PB_9 SDA, PB_8 SCL
CSE321_LCD lcd(16, 2, LCD_5x8DOTS, PB_9, PB_8); 

//establish ticker for timing piece
Ticker t;

// main() runs in its own thread in the OS
int main()
{

    RCC->AHB2ENR |= 0x8; //using port D for LEDs, PD4, 5, 6, 7 and port F for LEDS PF8, 7, 9
    GPIOD->MODER |= 0x5500; //set 1s at -- 0101 0101 0000 0000
    GPIOD->MODER &= ~(0xAA00); //set 0s at -- 1010 1010 0000 0000


    RCC->AHB2ENR |= 0x20; //using port F for time's up LEDs PF0, 1, 2
    GPIOF->MODER |= 0x15; //set 1s at 01 0101
    GPIOF->MODER &= ~(0x2A); //set 0s at 10 1010

    lcd.begin(); //start LCD

    t.attach(&timeChange, 1000ms); //attach function to a ticker that will call it every 1 second

    //when key is pressed and lifted (rising edge), trigger isr for corresponding column
    column1.rise(&isr_c1);
    column2.rise(&isr_c2);
    column3.rise(&isr_c3);
    column4.rise(&isr_c4);

    while (true) {
        //poll for keypad values and light up an LED on key press
        if(row == 1){
            //use PD_4 for row 1 LED
            GPIOD->ODR |= 0x10; //set row light on
            GPIOD->ODR &= ~(0x20); //turn others off
            GPIOD->ODR &= ~(0x40);
            GPIOD->ODR &= ~(0x80);      
        }else if(row == 2){
            //use PD_5 for row 2 LED
            GPIOD->ODR |= 0x20; //set row light on
            GPIOD->ODR &= ~(0x10); //turn others off
            GPIOD->ODR &= ~(0x40);
            GPIOD->ODR &= ~(0x80); 
        }else if(row == 3){
            //use PD_6 for row 3 LED
            GPIOD->ODR |= 0x40; //set row light on
            GPIOD->ODR &= ~(0x10); //turn others off
            GPIOD->ODR &= ~(0x20);
            GPIOD->ODR &= ~(0x80); 
        }else{
            //use PD_7 for row 4 LED
            GPIOD->ODR |= 0x80; //set row light on
            GPIOD->ODR &= ~(0x10); //turn others off
            GPIOD->ODR &= ~(0x20);
            GPIOD->ODR &= ~(0x40); 
        }

        thread_sleep_for(100); //account for switch bounce by waiting 100ms before moving on to poll the next row
        row = row + 1;
        row %= 4;

        if(state == CDStart){
            //reset all values to set up for new timer
            //prompt user to press D and then input the time
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CD: Press D");
            lcd.setCursor(0, 1);
            lcd.print("to Enter Time");
            mins = 0;
            secs = 0;
            storedM = 0;
            storedS = 0;
            numbersEntered = 0;
            GPIOF->ODR &= ~(0x7); //turn off time's up LEDs if on
        }

        if(state == CDKeypad || state == CUKeypad){
            //keypad numbers are active
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Set Time:");
            lcd.setCursor(0, 1);
            sprintf(printString, "%d min %d sec", mins, secs);
            lcd.print(printString);
        }

        if(state == CDTimer){
            //timer is actively running, decrement on each second and change display at each time change
            if(storedM != mins || storedS != secs){
                //only update LCD if there has been a change to display (LCD only updates once every second)
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Time Remaining:");
                lcd.setCursor(0, 1);
                sprintf(printString, "%d min %d sec", mins, secs);
                lcd.print(printString);
            }
        }

        if(state == CDEnd){
            //time is up, print message to display and turn multiple LEDs On, wait for user to press D and input new time
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Time's Up");
            GPIOF->ODR |= 0x7; //Turn on all timer lights
        }

        if(state == CUStart){
            //reset all values to set up for new timer
            //prompt user to press D and then input the time
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CU: Press D");
            lcd.setCursor(0, 1);
            lcd.print("to Enter Time");
            mins = 0;
            secs = 0;
            storedM = 0;
            storedS = 0;
            cuMins = 0;
            cuSecs = 0;
            numbersEntered = 0;
            GPIOF->ODR &= ~(0x7); //turn off time reached LEDs if on
        }

        if(state == CUTimer){
            //timer is actively running, decrement on each second and change display at each time change
            if(storedM != cuMins || storedS != cuSecs){
                //only update LCD if there has been a change to display (LCD only updates once every second)
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Time Passed:");
                lcd.setCursor(0, 1);
                sprintf(printString, "%d min %d sec", cuMins, cuSecs);
                lcd.print(printString);
            }
        }

        if(state == CUEnd){
            //time is up, print message to display and turn multiple LEDs On, wait for user to press D and input new time
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Time Reached");
            GPIOF->ODR |= 0x7; //Turn on all timer lights
        }

    }
    return 0;
}

void isr_c1(void){
    if (row == 1){
        //A
        //start timer from keypad state, otherwise do nothing
        if (state == CDKeypad){
            state = CDTimer;
        }else if(state == CUKeypad){
            state = CUTimer;
        }
    }else if(row == 2){
        //B
        //stop the timer if the timer is going OR reset to start state
        if(countUp == false){
            state = CDStart;
        }else{
            state = CUStart;
        }
    }else if(row == 0){
        //D
        //input time if in starting mode, otherwise do nothing
        if(state == CDStart){
            state = CDKeypad;
        }else if(state == CUStart){
            state = CUKeypad;
        }
    }else{
        //row == 3, C
        if(state == CDStart){
            //user can only switch between count down and count up before starting to input values
            countUp = true;
            state = CUStart;
        }else if(state == CUStart){
            countUp = false;
            state = CDStart;
        }
    }
}

void isr_c2(void){
    //rows should only do something if in keypad state 1
    if(state == CDKeypad || state == CUKeypad){
        if (row == 1){
            //3
            if(numbersEntered == 0){
                mins = 3;
                numbersEntered++;
            }else if(numbersEntered == 1 || numbersEntered == 2){
                secs = secs*10 + 3;
                numbersEntered++;
            }
        }else if(row == 2){
            //6
            if(numbersEntered == 0){
                mins = 6;
                numbersEntered++;
            }else if(numbersEntered == 2){
                secs = secs*10 + 6;
                numbersEntered++;
            }
        }else if(row == 3){
            //9
            if(numbersEntered == 0){
                mins = 9;
                numbersEntered++;
            }else if(numbersEntered == 2){
                secs = secs*10 + 9;
                numbersEntered++;
            }
        }
    }
    //If row == 0, key is #. We have no use for # in this project
}

void isr_c3(void){
    if(state == CDKeypad || state == CUKeypad){
        if (row == 1){
            //2
            if(numbersEntered == 0){
                mins = 2;
                numbersEntered++;
            }else if(numbersEntered == 1 || numbersEntered == 2){
                secs = secs*10 + 2;
                numbersEntered++;
            }
        }else if(row == 2){
            //5
            if(numbersEntered == 0){
                mins = 5;
                numbersEntered++;
            }else if(numbersEntered == 1 || numbersEntered == 2){
                secs = secs*10 + 5;
                numbersEntered++;
            }
        }else if(row == 3){
            //8
            if(numbersEntered == 0){
                mins = 8;
                numbersEntered++;
            }else if(numbersEntered == 2){
                secs = secs*10 + 8;
                numbersEntered++;
            }
        }else{
            //0
            if(numbersEntered == 0){
                mins = 0;
                numbersEntered++;
            }else if(numbersEntered == 1 || numbersEntered == 2){
                secs = secs*10 + 0;
                numbersEntered++;
            }
        }
    }
}

void isr_c4(void){
    if(state == CDKeypad || state == CUKeypad){
        if (row == 1){
            //1
            if(numbersEntered == 0){
                mins = 1;
                numbersEntered++;
            }else if(numbersEntered == 1 || numbersEntered == 2){
                secs = secs*10 + 1;
                numbersEntered++;
            }
        }else if(row == 2){
            //4
            if(numbersEntered == 0){
                mins = 4;
                numbersEntered++;
            }else if(numbersEntered == 1 || numbersEntered == 2){
                secs = secs*10 + 4;
                numbersEntered++;
            }
        }else if(row == 3){
            //7
            if(numbersEntered == 0){
                mins = 7;
                numbersEntered++;
            }else if(numbersEntered == 2){
                secs = secs*10 + 7;
                numbersEntered++;
            }
        }
    }
    //if row == 0, key is *. No use for * in this project.
}

void timeChange(void){
    if(state == CDTimer){
        if(secs == 00){
            if(mins == 00){
                //time is up, enter state for end of timer
                state = CDEnd;
            }else{
                //seconds roll over for next minute, store value to trigger lcd print
                storedM = mins;
                storedS = secs;
                mins--;
                secs = 59;

            }
        }else{
            //decrement as normal
            storedS = secs;
            secs--;

        }
    }else if(state == CUTimer){
        if(cuSecs == secs && cuMins == mins){
            //goal minutes and seconds reached, timer done
            state = CUEnd;
        }else{
            if(cuSecs == 59){
                //if seconds is 59, roll over into a minute 
                storedS = cuSecs;
                storedM = cuMins;
                cuSecs = 0;
                cuMins++;
            }else{
                //if none of the above cases apply, just add another second
                storedS = cuSecs;
                cuSecs++;
            }
        }
    }    
}
