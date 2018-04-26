 ATMega328p_HC-SR04_DMD

Program uses a ATMega328p MCU to query a HC-SR04 ultrasonic module. Then Calculates and displays the distance to a object in cm on the NewHaven display.
 
 The ultrasonic module is triggered every 60 ms by setting pin PC4 (the trigger) high for 10 us. A change in state on pin PC5 (the echo)
 causes a interrupt. The interrupt checks if PC5 is high. If it is a timer is started that increments every micro second. Once PC5 triggers a
 interrupt again and is low the result is calculated and displayed in centimeters on the NewHaven display. Distance in cm is calculated
 by the amount of microseconds PC5 (the echo) is active high divided by 58.

 
 Pin placement of ATMega328p:
 
  Port D[7-0]		NewHavenDisplay DB[7-0]
  Pin PC0				NewHavenDisplay RW
  Pin PC1				NewHavenDisplay RS
  Pin PC2				NewHavenDisplay E
  Pin PC3				Debugging LED Active High
  Pin PC4				HC-SR04 Trig
  Pin PC5				HC-SR04 Echo
