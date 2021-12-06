/** 
  * Author: Katherine Stock (kgstock)
  * 
  * File Purpose: Weather predictor and item carry along suggestor
  * Modules: isr_DHT, isr_ticker
  * Assignment: CSE321 Project 3 
  *
  * Inputs: DHT11
  * Outputs: LEDs, 7 segment display
  * 
  * Constraints: 
  * References: Watchdog Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/watchdog.html, Thread Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/thread.html, DHT11 Documentation -- https://components101.com/sites/default/files/component_datasheet/DHT11-Temperature-Sensor.pdf, Seven Segment Documentation -- https://components101.com/displays/tm1637-grove-4-digit-display-module, EventQueue Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/eventqueue.html
  * Weather Condition Decision References: https://sites.psu.edu/siowfa14/2014/10/24/why-does-humidity-affect-snow/#:~:text=When%20the%20humidity%20is%20high%20there%20is%20a%20large%20amount,then%20the%20pressure%20also%20decreases.&text=In%20relation%20to%20snow%2C%20when,is%20high%20the%20snow%20melts.
  */ 

#include "mbed.h"
#include "DHT.h"
#include "DigitDisplay.h"
#include <cstdio>


/** Sensor reads humidity 20%-90%, temperature 0 celsius (32F) to 50 celsius (122F)
  * Weather Conditions:
  * Snowy (White LED - PC8) -- 90% humidity, 0C temperature (32F)
  * Cold (Blue LED - PC9) -- <90% humidity, 0C - 5C (32F - 41F)
  * Moderate (Yellow LED - PC10) -- <90% humidity, 5C - 16C (41F - 60.8F)
  * Hot (Red LED - PC11) -- <90% humidity, 16C - 50C (60.8F - 122F)
  * Rainy (Green LED - PC12) -- 90% humidity, >0C temperature (>32F)
  */ 


//define weather variables to track state easily
#define SNOWY 1
#define COLD 2
#define MODERATE 3
#define HOT 4
#define RAINY 5
#define START 0


//TODO: testing


int state = START;
int tempF;
int humidity;

//Establish DHT11 as an input
DHT11 sensor(PC_6);

//Establish Seven Segment Display as an output, pd0 - clk, pd1 - dio
DigitDisplay display(PD_0, PD_1);

//Establish Watchdog and timeout
Watchdog &watchdog = Watchdog::get_instance();

const uint32_t TIMEOUT_MS = 9000; //if watchdog fails to get fed for 3 rounds of DHT reading, return to start state

//Establish an isr for setting the flag to read from the sensor, read cannot be done in an interrupt
void isr_flag(void);

//Establish an isr for the DHT to use
void isr_DHT(void);

//Establish an isr for the eventqueue
void isr_ticker(void);

int ret;
char x;

//establish ticker for timing piece and tickerflag to trigger hardware read
Ticker t;
volatile bool tickerFlag = 0;

//establish eventqueue for scheduling and working with timer for 3sec hardware reads, uses a thread for each event
EventQueue queue;

int main()
{
    t.attach(&isr_flag, 10000ms); //attatch function to work with EventQueue that will be called every 3 seconds

    RCC->AHB2ENR |= 0x4; //enable register clock -- using port C for colored LEDs, PC8, 9, 10, 11, 12
    //Next two lines set PC_8,9,10,11,12 to output mode
    GPIOC->MODER |= 0x1550000; //set second bits to 1 -- 0001 0101 0101 0000 0000 0000 0000
    GPIOC->MODER &= ~(0x2AA0000); //set first bits to 0 -- 10 1010 1010 0000 0000 0000 0000

    display.on(); //turn on 7 segment display

    //watchdog.start(TIMEOUT_MS); //start watchdog with 9000 ms timeout

    while (true) {
        if(tickerFlag){
            tickerFlag = 0;
            printf("%d\n",ret);
            ret = sensor.read();
            if(ret == DHTLIB_OK || ret == DHTLIB_ERROR_CHECKSUM){
                printf("Temp: %d\n", (int)sensor.getFahrenheit());
                printf("Hum: %d\n", sensor.getHumidity());
                queue.event(isr_DHT);
                queue.dispatch_once();
            }
        }

        // tempF = sensor.getFahrenheit();
        // humidity = sensor.getHumidity();
        // printf("%c\n",x);

        // printf("Temp: %d\n", tempF);
        // printf("Hum: %d\n", humidity);
        if(state == SNOWY) {
                //condidtions consistent with snow
                //White LED PC8
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x100; //PC8 on
                //7 segment displays 1
                display.clear();
                display.write(1);
         }else if(state == COLD){
                x = 'c';
                //Blue LED PC9
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x200; //PC9 on
                //7 segment displays 2
                display.clear();
                display.write(2);
         }else if(state == MODERATE){
             x = 'm';
                //Yellow LED PC10
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x400; //PC10 on
                //7 segment displays 3
                display.clear();
                display.write(3);
         }else if(state == HOT){
             x = 'h';
                //Red LED PC11
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x800; //PC11 on
                //7 segment displays 4
                display.clear();
                display.write(4);
         }else if(state == RAINY){
             x = 'r';
                //Green LED PC12
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x1000; //PC12 on
                //7 segment displays 5
                display.clear();
                display.write(5);
         }else{
                //reset to starting safe state and clear 7 segment and turn off all lights
                state = START;
                display.clear();
                GPIOC->ODR &= ~(0x1F00);
        }

    }
    return 0;
}

void isr_flag(void){
    tickerFlag = 1;
}

void isr_DHT(void){
        x = 'd';
        //if DHT reads ok, update values and feed watchdog
        tempF = (int)sensor.getFahrenheit();
        humidity = sensor.getHumidity();
        watchdog.kick();

        if(humidity>=90){
            if(tempF<=32){
                state = SNOWY;
            }else{
                state = RAINY;
            }
        }else{
            if(tempF>=32 && tempF<41){
                state = COLD;
            }else if(tempF>=41 && tempF<60.8){
                state = MODERATE;
            }else if(tempF>= 60.8){
                state = HOT;
            }
        }

}

void isr_ticker(void){
    x = 't';
    //add one DHT read to the queue and then dispatch it to update, this happens every 3 seconds
    //EventQueue protects the critical section of tempF and humidity
    queue.event(isr_DHT);
    queue.dispatch_once();
}