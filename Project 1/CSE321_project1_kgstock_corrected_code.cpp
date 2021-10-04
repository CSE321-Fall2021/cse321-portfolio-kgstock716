/** 
  * Author: Katherine Stock (kgstock)
  * 
  * File Purpose: Function that uses threading and a button input to turn on and off an LED light
  * Modules: lightOnOff, setState, resetState
  * Assignment: Project 1
  *
  * Inputs:  BUTTON1
  * Outputs: LED2
  * 
  * Constraints:
  * References: https://docs.google.com/document/d/1CfHFVnuIMq4_3XJ1wLBl4revOLBH41sBtM1wGhJN63U/edit
  * 
  */ 


#include "mbed.h"

// Create a thread to drive an LED to have an on time of 2000 ms and off time
// 500 ms


// initialize 3 custom void functions with no input
void lightOnOff();
void setState(); 
void resetState();

Thread controller;

DigitalOut blueLight(LED2); // establish blue led as an output
InterruptIn button(BUTTON1); //establish button as a thread input


// initialize two integers, flag and state to track state of the flag
int state = 0;
int flag = 0; 


int main() {
  // start the allowed execution of the thread
    printf("----------------START----------------\n");
	printf("Starting state of thread: %d\n", controller.get_state());
    controller.start(lightOnOff);
  // start control of the thread
    printf("State of thread right after start: %d\n", controller.get_state());
    button.rise(setState);
  // rising edge interrupt -- state should be 1
	button.fall(resetState); 
  // falling edge interrupt -- state should be 0, flag should be 0 or 1
    return 0;
}

//function w/ no inputs to check the flag, turn the light on/off, and wait for the thread
// make the handler
void lightOnOff() {
    while (true) {
        if(flag == 0){
            blueLight = !blueLight; //turns light on
            printf("Light on");
            thread_sleep_for(2000); //Thread_sleep is a time delay function, causes a 2000 unit delay
            blueLight = !blueLight; //turns light off
            printf("Light off");
            thread_sleep_for(500); //Thread_sleep is a time delay function, causes a 500 unit delay
        }
    }
}


//function w/ no input to set the state of the thread
void setState() {
// toggle the state of the thread					  
	state = 1;
}

//function w/ no inputs to reset the state of the thread to 0 and change the flag
void resetState() {
    if (state == 1){ 
        flag++; 
        flag %= 2; //flag is always 0 or 1
        state = 0;
    }
}