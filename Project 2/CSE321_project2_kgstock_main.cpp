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
#include <iterator>
#include "cstdio"

volatile char x;
int row = 0; 
int accumulator = 0;


//Establish one interrupt for each column we're reading from, use PC 8, 9, 10, 11

InterruptIn column1(PC_8); 
InterruptIn column2(PC_9);
InterruptIn column3(PC_10);
InterruptIn column4(PC_11);

//Establish an isr for each column interrupt to use
void isr_c1(void);
void isr_c2(void);
void isr_c3(void);
void isr_c4(void);

//establish LCD
//CSE321_LCD lcd(); 

// main() runs in its own thread in the OS
int main()
{
    RCC->AHB2ENR |= 0x8; //using port D for LEDs, PD4, 5, 6, 7
    GPIOD->MODER |= 0x5500; //set 1s at -- 0101 0101 0000 0000
    GPIOD->MODER &= ~(0xAA00); //set 0s at -- 1010 1010 0000 0000

    RCC->AHB2ENR |= 0x20;
    GPIOF->MODER |= 0x4400; 
    GPIOF->MODER &= ~(0x8800);


    //when key is pressed and lifted (rising edge), trigger isr for corresponding column
    column1.rise(&isr_c1);
    column2.rise(&isr_c2);
    column3.rise(&isr_c3);
    column4.rise(&isr_c4);

    while (true) {
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

        //print used to check ISR for rows and columns
        thread_sleep_for(500);
        printf("%c\n", x);
    }

    return 0;
}

void isr_c1(void){
    if (row == 1){
        //1
        x = '1';
    }else if(row == 2){
        //4
        x = '4';
    }else if(row == 3){
        //7
        x = '7';
    }
    //if row == 4, * is the key. We have no use for * in this project
}

void isr_c2(void){
    if (row == 1){
        //2
        x = '2';
    }else if(row == 2){
        //5
        x = '5';
    }else if(row == 3){
        //8
        x = '8';
    }else{
        //0
        x = '0';
    }
}

void isr_c3(void){
    if (row == 1){
        //3
        x = '3';
    }else if(row == 2){
        //6
        x = '6';
    }else if(row == 3){
        //9
        x = '6';
    }
    //If row == 4, key is #. We have no use for # in this project
}

void isr_c4(void){
    if (row == 1){
        //A
        x = 'A';
    }else if(row == 2){
        //B
        x = 'B';
    }else if(row == 4){
        //D
        x = 'D';
    }
    //might add in condition for row == 3 if using C for extra credit
}