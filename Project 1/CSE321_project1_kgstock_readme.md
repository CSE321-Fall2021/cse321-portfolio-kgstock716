-------------------
About
-------------------
Project Description: C++ program for a button triggered, threaded LED on the Nucleo L4R5ZI
Contribitor List: Dr. Winikus, Katherine Stock


--------------------
Features
--------------------
The program uses threads and interrupts to time turning on the onboard blue LED via the onboard button.

--------------------
Required Materials
--------------------
-Nucleo L4R5ZI
-USBC cable

--------------------
Resources and References
--------------------
For information on interrupts in Mbed (documentation provided in lecture):
https://os.mbed.com/docs/mbed-os/v6.14/mbed-os-api-doxy/classmbed_1_1_interrupt_in.html#af0810e0eb56d5504c46a89d1e86c045b

--------------------
Getting Started
--------------------
Only a USBC cord and the Nucleo are necessary since this project uses the onboard blue LED and button.
Plug the Nucleo into your computer using the USBC cord.

--------------------
CSE321_project1_kgstock_corrected_code.cpp:
--------------------
This is not a bare metal project due to use of threads. These threads are used to used to create periodic events with the onboard button and blue LED.
The project uses the items defined in the declaration section to create a threaded program to turn the LED on and off based on the button input.
Three custom functions also described below break down the tasks -- threading, turning the LED on/off, and waiting -- into smaller problems.


----------
Things Declared
----------
Two integer variables: state and flag.
A DigitalOut onboard blue LED: blueLight.
An InterruptIn onboard button: button.
A Thread controller.
Our three custom functions described below: setState, resetState, lightOnOff.
The mbed.h file is included for use of the Mbed API.

----------
API and Built In Elements Used
----------
The API used is Mbed and is included with mbed.h.
Build In Elements used are the Blue LED (LED2) and the button (BUTTON1).

----------
Custom Functions
----------
setState:
	Sets thread state to 1. 
	Inputs:
		None
	Globally referenced things used:
	state

resetState:
	Increments and mods count if thread state is 1. 
	Inputs:
		None
	Globally referenced things used:
	flag, state 

lightOnOff:
	Toggles the LED on, prints confirmation, waits, toggles the LED off, prints confirmation, waits. 
	Inputs:
		None
	Globally referenced things used:
	flag, blueLight, printf


