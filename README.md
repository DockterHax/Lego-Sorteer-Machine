# Lego-Sorteer-Machine
Mbed program made for a school project.
Because of this all the commends are in Dutch.

The machine only sorts straight LEGO Technic brick with a length of 3 to 15 long.

Used library’s:
Servo: https://os.mbed.com/users/simon/code/Servo/#36b69a7ced07 
TCS3200: https://os.mbed.com/users/grantphillips/code/TCS3200/docs/b98e768bc655/classTCS3200.html 
TekstLCD: https://os.mbed.com/users/wim/code/TextLCD/ 
(Make sure to use the Mbed OS 6 version. This version also works on Mbed OS 6)

The machine uses:
1	Nucleo F411RE running Mbed OS 5
1	20x4 LCD screen with PCF8574 I2C module
2	H-bridge L298N motor controllers
4	MG90S servo motors (SG90’s with metal gears instead of plastic)
1	TCS230-TCS3200 RGB collar sensor
6	IR proximity sensors
3	EATON M22-DL-G NO push buttons with LED
