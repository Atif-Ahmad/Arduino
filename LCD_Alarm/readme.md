Here is an overhead view of the circuit. 

![alt text](https://github.com/Atif-Ahmad/Arduino/blob/dfe57603237f3e145592921d23af2c7f2429913c/LCD_Alarm/Overhead_view.png?raw=true)

The first board is purely wiring for the Arduino Liquid Crystal Display (LCD). The schematic for the Liquid Crystal Display can be found on their tutorial website here: https://docs.arduino.cc/learn/electronics/lcd-displays. You can find the library code here: https://github.com/arduino-libraries/LiquidCrystal/tree/master. In addition I have included the LiquidCrystal.h file. 
From the website here is the schematic of the LCD. 

![alt text](https://docs.arduino.cc/static/87dafeba444f77d41fe0061e5a34bfde/4ff83/LCD_Base_bb_Schem.png?raw=true)

The next breadboard contains the wiring for the speakers and buttons. The buttons are how the user sets the time for the alarm and how to silence it once it rings. 
To make the speaker sound off, the code uses the pitches.h file, which contains the different tones the speaker can have. This can be found in one of Arduino's tutorials.
The code to record the button's input is in the LCD_ard.ino file. It merely records the voltage input at the specified pin. 


