-------------------
About
-------------------
Project Description: C++ program for a bare-metal, count-down timer system  
Contribitor List: Katherine Stock  


--------------------
Features
--------------------
The user will used the keypad to input a value for a timer, an LED will flash when a value is input. The user can press 'C' before inputting the time to toggle between count-down (the default mode) and count-up. They can then press 'D' to input the value, 'A' to start the timer once a value has been input, and 'B' to turn off the timer and reset at any point. The time remaining (or time passed) will be displayed on the external LCD and update as each second passes. Once the timer has reached 0 (or the input time if counting up), the LCD will display 'Time's Up'/'Time Reached' and 3 LEDs will light up. The user can then press 'B' whenever they are ready to use the timer again.  

--------------------
Required Materials
--------------------
-Nucleo L4R5ZI  
-USB a to Micro USB B cable   
-16 male/male jumpers   
-Keypad   
-16x2 1802 model LCD (comes with male/male jumper adaptor)   
-7 external LEDs   
-7 1kOhm Resistors   
-Breadboard   

--------------------
Resources and References
--------------------
Lecture code 10/6  
Ticker Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/ticker.html  
Interrupt Documentation -- https://os.mbed.com/docs/mbed-os/v6.15/apis/interruptin.html  

--------------------
Getting Started
--------------------
Plug the Nucleo into the computer using the micro USB cable.  
Connecting the LCD requires the Nucelo 4 jumpers, the LCD, and the male/male adaptor that comes with the 1802 LCD. To connect the LCD, attatch the given adaptor wires so the LCD is compatible with male/male jumpers. Now, male/male jumpers can be used to connect the GND wire to any ground pin on the Nucleo. Other jumpers should connect VCC to the 5V Nucleo pin, SCL to PB8, and SDA to PB9.  

Connecting the keypad requires 13 male/male jumpers, the Nucelo, the breadboard, the 4x4 matrix keypad, 4 LEDs, and 4 resistors. One jumper should connect any ground pin (besides the one being used for the LCD) to the ground rail of the breadboard. Pins PD7, PD6, PD5, and PD4 should be directly connected to pins 5, 6, 7, and 8 of the keypad, respectively, via jumpers.  PC8, PC9, PC10, and PC11 can be connected to the breadboard anywhere (so long as they are not in the same row of 5) via jumpers. Connected in the same group of 5 for these jumpers should be an LED in seqeunce with a resistor which is connected to ground. Another jumper should be in the same group of 5 as the positive side of the LED and the jumper to the Nucelo which will connect to the keypad. PC8 should share a row with pin 1 of the keypad, PC9 with pin 2, PC10 with pin 3, and PC11 with pin 4.  

An addition 3 LEDs should be placed anywhere on the board connected to PF0, PF1, and PF2; all in sequence with a resistor which is connected to ground.   

--------------------
CSE321_project2_kgstock_main.cpp:
--------------------
This file implements all subproblems of the count-down timer system. The file follows a state machine model to easily track expected behavior of the keypad, LCD, and LEDs at any time. A polling system and ISRs are used to determine keypad input with a wait between each row poll to account for bounce of the keypad switches. The keypad letter column is then used to switch between states depending on what response the user wants. The LCD display also updates depending on the state and will prompt the user to input time, press certian keys, and update the user on how much time is left/how much time has passed and if the timer has finished.

----------
Things Declared
----------
 Variables/Objects  
* 8 defined integer variables to represent the different states.  
* Integer row counter used for polling the keypad rows and determining what key in each column was pressed.  
* Integers mins, secs, cuMins, cuSecs, storedM, and storedS all used to store values related to the time and are initialized to 0. mins and secs are populated when the user inputs values, storedM and storedS are used so that the LCD display only updates when change in time is detected, cuMins and cuSecs store the time passed in count-up mode. These two variables are incremented until they equal mins and secs; mins and secs are directly decremented in count-down mode.
* Integer numbersEntered which is used to build the input time each time a valid key is pressed, mins/secs is populated and the accumulator, numbersEntered, is incremented. The maximum value is 2 since we will have only 3 key reads of the form m:ss.
* Boolean flag countUp which will toggle when 'C' is pressed to determine if the timer is in count-up or count-down mode. The default state is count-down so this flag starts as false.
* Character array of size 15, printString starts as an empty space for the result of a call to sprintf to be stored. sprintf makes it possible to print the current integer time values in a string to the LCD.
* Integer state holds the current state of the machine (one of the 8 integer variables defined).
* CSE321_LCD lcd declares the lcd object being used to print results and prompt the user. The LCD is a 16x2 1802 so it is declared to have 16 columns, 2 rows, each character being 5x8 dots, SDA of PB9, and SCL of PB8 as required by the included documentation.
* 4 InterruptIn: one for each column of the keypad using pins PC8-11. 
* Ticker t, which is used to time the decrement/increment and LCD updates on each second.  
#Functions
* Void function timeChange which is attatched to the ticker. The function decides if the timer's job is done and changes state if so. Otherwise, the function decrements/increments in line with expected behavior of 1 mins = 60 seconds.  
* 4 void ISR functions, one for each column interrupt, that will trigger the behavior associated with that key in the given state.  
----------
API and Built In Elements Used
----------
Interrupt API -- InterruptIn, .rise()  
Ticker API -- Ticker, .attatch()  

----------
Custom Functions
----------
Void ISR functions mentioned above:  
* void isr_c1(void)   
* void isr_c2(void)   
* void isr_c3(void)   
* void isr_c4(void)   
These routines determine what key was pressed based on the current value of row and set the state when appropriate or add to the input time if in a keypad state. Number keys only cause response if in the keypad state, 'B' will reset the timer no matter what state it is currently in, 'A' will start the timer from the keypad state, 'D' will change the state to keypad and allow the user to set the time, and 'C' will switch between the count-up and count-down modes only if it is currently in a start state.  
Void function attatched to the ticker:
* void timeChange   
The function is attatched to the ticker at the start of running the program but will only cause effect if it is currently in a timer state.  
If the state is count-down timer, the function checks for 0:00, which means the timer is up and should move into the count-down end state. If the value is not 0:00 but does have 0 seconds, the function removes a minute from the time and adds 59 seconds. If neither of these are true, the function simply removes 1 second from the time.  
If the state is count-up timer, the function will check if the input time is equal to our time counter and changes the state to count-up end if this is true. If the counter's seconds have reached 59, it rolls over and adds a minute to time counter and sets seconds back to 0. If neither of these cases apply, it simply adds one second.



