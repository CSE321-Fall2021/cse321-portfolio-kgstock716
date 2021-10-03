
#include "mbed.h"

// Create a thread to drive an LED to have an on time of 2000 ms and off time
// 500 ms



void lightOnOff();
void setState(); 
void resetState();

Thread controller;

DigitalOut blueLight(LED2); // establish blue led as an output
InterruptIn button(BUTTON1); 

int state = 0;
int flag = 0; 


int main() {
  // start the allowed execution of the thread
    printf("----------------START----------------\n");
	printf("Starting state of thread: %d\n", controller.get_state());
    controller.start(lightOnOff);
  // Light should turn on, print, turn off, print
    printf("State of thread right after start: %d\n", controller.get_state());
    button.rise(setState);
  // rising edge interrupt -- state should be 1
	button.fall(resetState); 
  // falling edge interrupt -- state should be 0, flag should be 0 or 1
    return 0;
}

// make the handler
void lightOnOff() {
    while (true) {
        if(flag == 0){
            blueLight = !blueLight;
            printf("Light on");
            thread_sleep_for(2000); //Thread_sleep is a time delay function, causes a 2000 unit delay
            blueLight = !blueLight;
            printf("Light off");
            thread_sleep_for(500); //Thread_sleep is a time delay function, causes a 500 unit delay
        }
    }
}

void setState() {
// toggle the state of the thread					  
	state = 1;
}

void resetState() {
    if (state == 1){ 
        flag++; 
        flag %= 2; //flag is always 0 or 1
        state = 0;
    }
}