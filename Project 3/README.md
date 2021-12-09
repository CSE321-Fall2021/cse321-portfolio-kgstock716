-------------------
About
-------------------
Project Description: C++ program for a weather predictor and item recommendor.  
Contribitor List: Katherine Stock  


--------------------
Features
--------------------
The user will place a sensor outside or near an open window which will read temperature and humidity to determine the weather for the day. The expected weather will be communicated via a number that the user has a key to connect to specific weather conditions The system will also make recommendataions to the user based on the decided weather conditions -- items recommmended will be important for the user's wellbeing: sunscreen, water, an umbrella, gloves and a hat, etc.

--------------------
Required Materials
--------------------
-Nucleo L4R5ZI  
-USB a to Micro USB B cable   
-14 male/male jumpers   
-Seven-Segment display (with male/male jumper adaptor)  
-5 external LEDs (one of each color: White, Blue, Yellow, Red, Green)
-DHT11 temperature and humidity sensor 
-5 1kOhm Resistors   
-Breadboard   

--------------------
Resources and References
--------------------
Watchdog Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/watchdog.html, Thread Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/thread.html, DHT11 Documentation -- https://components101.com/sites/default/files/component_datasheet/DHT11-Temperature-Sensor.pdf, Seven Segment Documentation -- https://components101.com/displays/tm1637-grove-4-digit-display-module, EventQueue Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/eventqueue.html 

--------------------
Getting Started
--------------------  
The Nucelo-L4R5ZI can be plugged into the computer and programmed using the provided main file and MBed Studio. This system will now run on the board as long as it has power and isn't reprogrammed. Next, the hardware pieces of the system must be put together.  
From the report 'How to Build the System': "The 3.3 Volt power supply of the Nucleo is connected to one of the power rails on the breadboard so it can be used by both the DHT11 and the 7 segment display. One ground supply of the Nucleo is also connected to one of the ground rails on the breadboard so it can be used by the LEDs, DHT11, and 7 segment display.  

Pins PC_8, PC_9, PC_10, PC_11, and PC_12 should all be connected to the breadboard with an LED (of appropriate color as indicated in the diagram) in sequence with a 1kOhm resistor which is then connected to the ground rail.  

In addition to the 3.3 Volt power rail and the ground rail, the DHT11 should connect the output pin to PC_6 and the 4-Digit-Display should connect the CLK pin to PD_0 and the DIO pin to PD_1."  

The hardware should now follow the specifications given in the schematic.

--------------------
CSE321_project3_kgstock_main.cpp:
--------------------
This file implements a weather predictor and item recommendation system that uses a DHT11 input and LEDs as well as a 7 segment display for output. There are two tickers which time the read of the DHT11 sensor every 5 seconds and the update of the critical section variables via a threaded EventQueue. 

----------
Things Declared
----------
  
----------
API and Built In Elements Used
----------
Built In MBed APIs -- Ticker, Watchdog, EventQueue, Thread  
Custom APIs -- DigitalDisplay.h/.cpp provided API for use with the 7 segment display, DHT.h/.cpp provided API for use with the DHT11  


----------
Custom Functions
----------




