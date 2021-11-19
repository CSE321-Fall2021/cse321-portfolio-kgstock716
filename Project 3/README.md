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
-male/male jumpers   
-Seven-Segment display   
-16x2 1802 model LCD (comes with male/male jumper adaptor)   
-DHT11 temperature and humidity sensor 
-1kOhm Resistors   
-Breadboard   

--------------------
Resources and References
--------------------
DHT11 Documentation -- https://components101.com/sites/default/files/component_datasheet/DHT11-Temperature-Sensor.pdf  

--------------------
Getting Started
--------------------
Plug the Nucleo into the computer using the micro USB cable.  
Connecting the LCD requires the Nucelo 4 jumpers, the LCD, and the male/male adaptor that comes with the 1802 LCD. To connect the LCD, attatch the given adaptor wires so the LCD is compatible with male/male jumpers. Now, male/male jumpers can be used to connect the GND wire to any ground pin on the Nucleo. Other jumpers should connect VCC to the 5V Nucleo pin, SCL to PB8, and SDA to PB9.  

Connecting the keypad requires 13 male/male jumpers, the Nucelo, the breadboard, the 4x4 matrix keypad, 4 LEDs, and 4 resistors. One jumper should connect any ground pin (besides the one being used for the LCD) to the ground rail of the breadboard. Pins PD7, PD6, PD5, and PD4 should be directly connected to pins 5, 6, 7, and 8 of the keypad, respectively, via jumpers.  PC8, PC9, PC10, and PC11 can be connected to the breadboard anywhere (so long as they are not in the same row of 5) via jumpers. Connected in the same group of 5 for these jumpers should be an LED in seqeunce with a resistor which is connected to ground. Another jumper should be in the same group of 5 as the positive side of the LED and the jumper to the Nucelo which will connect to the keypad. PC8 should share a row with pin 1 of the keypad, PC9 with pin 2, PC10 with pin 3, and PC11 with pin 4.  

An addition 3 LEDs should be placed anywhere on the board connected to PF0, PF1, and PF2; all in sequence with a resistor which is connected to ground.   

--------------------
CSE321_project3_kgstock_main.cpp:
--------------------


----------
Things Declared
----------
  
----------
API and Built In Elements Used
----------



----------
Custom Functions
----------




