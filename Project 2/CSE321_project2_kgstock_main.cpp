/** 
  * Author: Katherine Stock (kgstock)
  * 
  * File Purpose: Count-down alarm system
  * Modules: lightOnOff, setState, resetState
  * Assignment: Project 2 
  *
  * Inputs:  4x4 Keypad
  * Outputs: 4 external LEDs, LCD 
  * 
  * Constraints: Bounce, communication with Keypad and LCD
  * References: 10/6 Lecture code
  * 
  */ 

#include "mbed.h"
#include "1802.h"
#include <cstdio>

//TODO:
//put everything together
//make sure isr is doing what is specified by the button
//building time
//A, B, D functionality

int mins = 0;
int secs = 0;
int storedM = 0;
int storedS = 0;

//count numbers entered by the user to build mins and seconds. only first 3 numbers are accepted, if user makes a mistake. they must press B to turn off and reset timer.
int numbersEntered = 0;

volatile char x;
int row = 0; 
int accumulator = 0;

//character array to print on each second, pointer will be populated using sprintf so that the lcd print can handle the variable numbers
char *printString;


//Establish one interrupt for each column we're reading from, use PC 8, 9, 10, 11

InterruptIn column1(PC_8, PullDown); 
InterruptIn column2(PC_9, PullDown);
InterruptIn column3(PC_10, PullDown);
InterruptIn column4(PC_11, PullDown);

//Establish an isr for each column interrupt to use
void isr_c1(void);
void isr_c2(void);
void isr_c3(void);
void isr_c4(void);

//establish void function that will be called by timer every 1 second to decrement the time remaining variable
void decrement(void);

//establish flags for different states of the timer
int state = 0; //0 is default state, waiting for input

//establish LCD, 16 columns, 2 rows, PB_9 SDA, PB_8 SCL
CSE321_LCD lcd(16, 2, LCD_5x8DOTS, PB_9, PB_8); 

//establish ticker for timing piece
Ticker t;

// main() runs in its own thread in the OS
int main()
{

    RCC->AHB2ENR |= 0x8; //using port D for LEDs, PD4, 5, 6, 7
    GPIOD->MODER |= 0x5500; //set 1s at -- 0101 0101 0000 0000
    GPIOD->MODER &= ~(0xAA00); //set 0s at -- 1010 1010 0000 0000

    RCC->AHB2ENR |= 0x20;
    GPIOF->MODER |= 0x4400; 
    GPIOF->MODER &= ~(0x8800);

    lcd.begin(); //start LCD

    if(state == 0){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Press D to Enter Time"); 
    }
    //set beginning state
    //prompt user to input time
    if(state == 2){
        //timer is actively running, decrement on each second and change display at each time change
        t.attach(&decrement, 1);
        if(storedM != mins || storedS != secs){
            lcd.clear();
            lcd.setCursor(0, 0);
            sprintf(printString, "Time Remaining: %d min %d sec", mins, secs);
            lcd.print(printString);
        }
    }

    if(state == 3){
        //time is up, print message to display and turn multiple LEDs On, wait for user to press D and input new time
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Time's Up");
        GPIOD->ODR |= 0x10; //set row light on
        GPIOD->ODR |= 0x20; //set row light on
        GPIOD->ODR |= 0x40; //set row light on
        GPIOD->ODR |= 0x80; //set row light on
    }



    //when key is pressed and lifted (rising edge), trigger isr for corresponding column
    column1.rise(&isr_c1);
    column2.rise(&isr_c2);
    column3.rise(&isr_c3);
    column4.rise(&isr_c4);

    while (true) {
            //poll for keypad values and build time value
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
            row = row + 1;
            row %= 4;
            thread_sleep_for(200); //account for bounce by waiting 
    }

    return 0;
}

void isr_c1(void){
    if (row == 1){
        //A
        //start timer from keypad state, otherwise do nothing
        if (state == 1){
            if(secs < 60){
                state = 2;
            }else{

            }
        }
    }else if(row == 2){
        //B
        //stop the timer if the timer is going OR reset to state 0
        state = 0;
    }else if(row == 0){
        //D
        //input time
        if(state != 1){
            state = 1;
        }
    }
}

void isr_c2(void){
    //rows should only do something if in keypad state 1
    if(state == 1){
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
    if(state == 1){
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
    if(state == 1){
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

void decrement(void){
    if(secs == 00){
        if(mins == 00){
            //time is up, enter state for end of timer
            state = 3;
        }else{
            //seconds roll over for next minute, store value to trigger lcd print
            mins--;
            secs = 59;
            storedM = mins;
            storedS = secs;
        }
    }else{
        //decrement as normal
        secs--;
        storedS = secs;
    }
}
