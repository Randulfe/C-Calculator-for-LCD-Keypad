# Calculator written in C for an electronic system made with a keypad and an LCD
I created for this uni project a simple calcuator with some added extra features that could perform the following:
* Basic operations (addition, substraction, multiplication and division). 
* Floating point operations.
* Nested calculations up to 6 operands.
* Alternative functions (sin, cos, tan, ^2, square root, Pi, asin, acos, atan, lg10 and ln)
* User password.
* Interfacing cursor shifting functions for avoiding to “write” out of the LCD. 

The challenging part of this project was to addecuately interface with C and Assembly the Keypad (4x4 AK-1607) and LCD (16x2 HD44780) with a microcontroller (Tiva TM4C123GH6PM) using Keil uVision. 
For doing I had to first implement the hardware connections and then write the sofware for interfacing each device.

You can read the full report on how I did this project on this repository.

![Photo of the actual circuit](https://github.com/Randulfe/C-Calculator-for-LCD-Keypad/blob/master/foto.jpg)
![Circuit diagram](https://github.com/Randulfe/C-Calculator-for-LCD-Keypad/blob/master/imagen2.jpg)


