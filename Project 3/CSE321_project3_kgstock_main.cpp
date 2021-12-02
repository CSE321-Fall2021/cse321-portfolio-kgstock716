/** 
  * Author: Katherine Stock (kgstock)
  * 
  * File Purpose: Weather predictor and item carry along suggestor
  * Modules: 
  * Assignment: CSE321 Project 3 
  *
  * Inputs:  
  * Outputs: 
  * 
  * Constraints:
  * References: Watchdog Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/watchdog.html, Thread Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/thread.html, DHT11 Documentation -- https://components101.com/sites/default/files/component_datasheet/DHT11-Temperature-Sensor.pdf, Seven Segment Documentation -- https://components101.com/displays/tm1637-grove-4-digit-display-module
  * Weather Condition Decision References: https://sites.psu.edu/siowfa14/2014/10/24/why-does-humidity-affect-snow/#:~:text=When%20the%20humidity%20is%20high%20there%20is%20a%20large%20amount,then%20the%20pressure%20also%20decreases.&text=In%20relation%20to%20snow%2C%20when,is%20high%20the%20snow%20melts.
  */ 

#include "mbed.h"
#include "DHT.h"
#include "DigitDisplay.h"
#include <cstdio>


/** Sensor reads humidity 20%-90%, temperature 0 celsius (32F) to 50 celsius (122F)
  * Weather Conditions:
  * Snowy (White LED) -- 90% humidity, 0C temperature (32F)
  * Cold (Blue LED) -- <90% humidity, 0C - 5C (32F - 41F)
  * Moderate (Yellow LED) -- <90% humidity, 5C - 16C (41F - 60.8F)
  * Hot (Red LED) -- <90% humidity, 16C - 50C (60.8F - 122F)
  * Rainy (Green) -- 90% humidity, >0C temperature (>32F)
  */ 



//Establish DHT11 as an input
DHT11 sensor(PC_8);

//Establish Seven Segment Display as an output
DigitDisplay(PD_11, PD_12);

//Establish an isr for the DHT to use
void isr_DHT(void);

//establish ticker for timing piece
Ticker t;

// main() runs in its own thread in the OS
int main()
{

    while (true) {
        

    }
    return 0;
}

void isr_DHT(void){
    
}
