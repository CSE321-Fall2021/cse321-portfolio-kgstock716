-------------------
About
-------------------
Project Description: C++ program for a count-down timer system  
Contribitor List: Katherine Stock  


--------------------
Features
--------------------
The user will used the keypad to input a value for a timer, an LED will flash when a value is input. They can press 'D' to input the value, 'A' to start the timer, 'B' to stop/turn off the timer. The time remaining once set will be displayed on the external LCD. Once the time is up, the LCD will display time passed and many LEDs will light up.  

--------------------
Required Materials
--------------------
-Nucleo L4R5ZI  
-USB a to Micro USB B cable   
-16 male/male jumpers   
-Keypad   
-16x2 1802 model LCD (comes with male/male jumper adaptor)   
-7 external LEDs   
-7 Resistors   
-Breadboard   

--------------------
Resources and References
--------------------
Lecture code 10/6

--------------------
Getting Started
--------------------
Plug the Nucleo into the computer using the micro USB cable. There are 4 LEDs for each column that should be connected to the specified pins PD4-7 as defined in the code. each LED only needs 1 resistor. These LEDs should connect to the 4 input points on the 4x4 matrix keypad via jumpers 

--------------------
CSE321_project2_kgstock_main.cpp:
--------------------
This file implements all subproblems of the count-down alarm system. Currently, there is a portion using ISRs to handle input via the keypad and specify the key being pressed by the user.

----------
Things Declared
----------
volatile char x which is currently used for testing ISRs.  
int row counter.  
int accumulator for the timer value.  
4 InterruptIn: one for each column of the keypad using pins PC8-11.  
4 void ISR functions, one for each column interrupt.  


----------
API and Built In Elements Used
----------
Interrupt API -- InterruptIn, .rise()

----------
Custom Functions
----------
Void ISR functions mentioned above:  
void isr_c1(void),   
void isr_c2(void),   
void isr_c3(void),   
void isr_c4(void).   
These currently only have testing values and basic conditionals set up to determine key value based on the value in row.
