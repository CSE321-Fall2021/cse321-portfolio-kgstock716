/** 
  * Author: Katherine Stock (kgstock)
  * 
  * File Purpose: Weather predictor and item carry along suggestor
  * Modules: isr_DHT, isr_flag
  * Assignment: CSE321 Project 3 
  *
  * Inputs: DHT11
  * Outputs: LEDs, 7 segment display
  * 
  * Constraints: Sensor must only be read every 2 seconds maximum
  * References: Watchdog Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/watchdog.html, Thread Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/thread.html, DHT11 Documentation -- https://components101.com/sites/default/files/component_datasheet/DHT11-Temperature-Sensor.pdf, Seven Segment Documentation -- https://components101.com/displays/tm1637-grove-4-digit-display-module, EventQueue Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/eventqueue.html
  * Weather Condition Decision References: https://sites.psu.edu/siowfa14/2014/10/24/why-does-humidity-affect-snow/#:~:text=When%20the%20humidity%20is%20high%20there%20is%20a%20large%20amount,then%20the%20pressure%20also%20decreases.&text=In%20relation%20to%20snow%2C%20when,is%20high%20the%20snow%20melts.
  */ 

#include "mbed.h"
#include "DHT.h"
#include "DigitDisplay.h"
#include <cstdio>


/** Sensor reads humidity 20%-90%, temperature 0 celsius (32F) to 50 celsius (122F)
  * Weather Conditions:
  * Snowy (White LED - PC8) -- >85% humidity, 32F
  * Cold (Blue LED - PC9) -- <85% humidity, 32F - 60F
  * Moderate (Yellow LED - PC10) -- <85% humidity, 60F - 80F
  * Hot (Red LED - PC11) -- <85% humidity, 80F - 122F
  * Rainy (Green LED - PC12) -- >85% humidity, >32F
  */ 


//define weather variables to track state easily
#define SNOWY 1
#define COLD 2
#define MODERATE 3
#define HOT 4
#define RAINY 5
#define START 0


int state = START;
int tempF;
int humidity;

//Establish DHT11 as an input
DHT11 sensor(PC_6);

//Establish Seven Segment Display as an output, pd0 - clk, pd1 - dio
DigitDisplay display(PD_0, PD_1);

//Establish Watchdog and timeout
Watchdog &watchdog = Watchdog::get_instance();
const uint32_t TIMEOUT_MS = 15000; //if watchdog fails to get fed for 3 rounds of DHT reading, return to start state

//Establish an isr for setting the flag to read from the sensor, read cannot be done in an interrupt
void isr_flag(void);

//Establish an isr for the DHT to use
void isr_DHT(void);

void isr_critical(void);

//return integer to store condition of the sensor after read, ok or error
volatile int ret;

//establish ticker t1 for timing DHT11 read and ticker t2 to time critical section update with EventQueue, tickerflag tiggered when sensor should be read (no more than every 2 seconds)
Ticker t1;
Ticker t2;
volatile bool tickerFlag = 0;

//establish eventqueue for scheduling and working with timer for 5sec hardware reads, uses a thread for each event
EventQueue q(32 * EVENTS_EVENT_SIZE);

//Create thread that will connect to the queue
Thread qThread;

int main()
{
    //use flag based ISR to signal a new sensor read every 5 seconds
    t1.attach(&isr_flag, 5000ms); 

    //attatch second ticker to work with eventqueue and update critical section
    t2.attach(&isr_DHT, 5000ms);

    //attatch thread to queue, dispatch forever the events that will be added in DHT ISR
    qThread.start(callback(&q, &EventQueue::dispatch_forever));

    //enable register clock -- using port C for colored LEDs, PC8, 9, 10, 11, 12
    RCC->AHB2ENR |= 0x4; 
    
    //Next two lines set PC_8,9,10,11,12 to output mode
    GPIOC->MODER |= 0x1550000; //set second bits to 1 -- 0001 0101 0101 0000 0000 0000 0000
    GPIOC->MODER &= ~(0x2AA0000); //set first bits to 0 -- 10 1010 1010 0000 0000 0000 0000

    display.on(); //turn on 7 segment display

    watchdog.start(TIMEOUT_MS); //start watchdog with 15000 ms timeout
    

    while (true) {
        //add DHT update event to the queue, will be dispatched to safely update the critical section
        q.event(isr_DHT);
        if(tickerFlag){
            //immediately turn off flag to prevent multiple sensor reads
            tickerFlag = 0;
            ret = sensor.read();
            if(ret == DHTLIB_OK  || ret == DHTLIB_ERROR_CHECKSUM){
                //sensor read returned okay, notify the watchdog
                printf("status is %d\n", ret);
                printf("Temp: %d\n", tempF);
                printf("Hum: %d\n", humidity);
                watchdog.kick();
            }
        }

        if(state == SNOWY) {
                //condidtions consistent with snow
                //White LED PC8
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x100; //PC8 on
                //7 segment displays 1
                display.write(1);
         }else if(state == COLD){
                //Blue LED PC9
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x200; //PC9 on
                //7 segment displays 2
                display.write(2);
         }else if(state == MODERATE){
                //Yellow LED PC10
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x400; //PC10 on
                //7 segment displays 3
                display.write(3);
         }else if(state == HOT){
                //Red LED PC11
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x800; //PC11 on
                //7 segment displays 4
                display.write(4);
         }else if(state == RAINY){
                //Green LED PC12
                GPIOC->ODR &= ~(0x1F00); //all LEDs off
                GPIOC->ODR |= 0x1000; //PC12 on
                //7 segment displays 5
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
    //ISR is run off of the EventQueue
    //allows for safe update critical section values tempF, humidity, state
    if(tickerFlag && (ret == DHTLIB_OK || ret == DHTLIB_ERROR_CHECKSUM)){
        //read and update DHT11 values
        tempF = (int)sensor.getFahrenheit();
        humidity = sensor.getHumidity();

        //change state based on newly read information, state boundaries outlined below includes
        if(humidity>=85){
            if(tempF<=32){
                state = SNOWY;
            }else{
                state = RAINY;
            }
        }else{
            if(tempF>=32 && tempF<60){
                state = COLD;
            }else if(tempF>=60 && tempF<80){
                state = MODERATE;
            }else if(tempF>= 80){
                state = HOT;
            }
        } 
    }
}