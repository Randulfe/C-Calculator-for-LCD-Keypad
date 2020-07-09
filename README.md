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

You can read the full report on how I did this project and the design specification and choices on [this PDF of the repository](https://github.com/Randulfe/C-Calculator-for-LCD-Keypad/blob/master/ELEC3662_GROUP_10.pdf)

<img src="https://github.com/Randulfe/C-Calculator-for-LCD-Keypad/blob/master/foto.jpg" width="600px"> 
<img src="https://github.com/Randulfe/C-Calculator-for-LCD-Keypad/blob/master/imagen2.jpg" width="500px">

### Instructions

To test the code, you must have the electronic components described above and saved and compiled the folder [elekproject](https://github.com/Randulfe/C-Calculator-for-LCD-Keypad/tree/master/elecprokect) on your microcontroller. In that same folder, the [main C file](https://github.com/Randulfe/C-Calculator-for-LCD-Keypad/blob/master/elecprokect/main.c) calls the rest of the libraries developed by my team, both the low and high level ones to then be able to interact with the different electronic components and perform the calculator operations. 

### License 

MIT License

Copyright (c) 2018 Mateo Randulfe


