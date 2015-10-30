/*
 *      Author: Charles Minchau
 *      Program uses a NewHaven display a HC-SR04 ultrasonic module and a atmega328p MCU
 *      The ultrasonic module is queried and the result is displayed as  cm
 */
#define F_CPU 1000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init() {
	DDRD = 0xFF;						// Port D all output. Display: DB0 - DB7 as PD0 - PD7
	DDRC = 0xFF;						// Port C all output. PC0: RW		PC1: RS		PC2: E
	DDRC &= ~(1<<DDC5);					// Set Pin C5 as input to read Echo
	PORTC |= (1<<PORTC5);				// Enable pull up on C5
	PORTC &= ~(1<<PC4);					// Init C4 as low

	PRR &= ~(1<<PRTIM1);				// To activate timer1 module
	TCNT1 = 0;							// Initial timer value
	TCCR1B |= (1<<CS10);				// Timer without prescaller
	TCCR1B |= (1<<ICES1);				// First capture on rising edge

	PCICR = (1<<PCIE1);					// Enable PCINT[14:8] we use pin C5 which is PCINT13
	PCMSK1 = (1<<PCINT13);				// Enable C5 interrupt
	sei();								// Enable Global Interrupts
}
void WriteDisplay(_Bool RS, _Bool RW, uint8_t DB7to0) {
	_delay_ms(2);							// To be sure that busy flag is 0
	PORTC |= (1<<PC2);						// Prepare E for falling edge trigger
	if (RS == 1) {							// Prepare RS
		PORTC |= (1<<PC1);
	} else {
		PORTC &= ~(1<<PC1);
	}
	if (RW == 1) {							// Prepare RW
		PORTC |= (1<<PC0);
	} else {
		PORTC &= ~(1<<PC0);
	}
	PORTD = DB7to0;							// Set DB7-DB0
	PORTC &= ~(1<<PC2);						// Trigger E thereby triggering the transfer
}
uint8_t HexToDisplayCode(uint8_t hex) {
	if (hex == 0x00) {return 0x30;}
	if (hex == 0x01) {return 0x31;}
	if (hex == 0x02) {return 0x32;}
	if (hex == 0x03) {return 0x33;}
	if (hex == 0x04) {return 0x34;}
	if (hex == 0x05) {return 0x35;}
	if (hex == 0x06) {return 0x36;}
	if (hex == 0x07) {return 0x37;}
	if (hex == 0x08) {return 0x38;}
	if (hex == 0x09) {return 0x39;}
	if (hex == 0x0A) {return 0x41;}
	if (hex == 0x0B) {return 0x42;}
	if (hex == 0x0C) {return 0x43;}
	if (hex == 0x0D) {return 0x44;}
	if (hex == 0x0E) {return 0x45;}
	if (hex == 0x0F) {return 0x46;}
	return 0x21;	// 0x21 is an !
}
void Write16BitToDisplay(uint16_t given) {				// 0000 0000 0000 0000 : 1st 2nd 3rd 4th
	uint16_t maskedFirst4 =  0xF000 & given;			// mask given so only 4 most significant bits left
	uint8_t first = maskedFirst4 >> 12;					// shift 4 most significant bits all the way to the right
	uint16_t maskedSecond4 =  0x0F00 & given;
	uint8_t second = maskedSecond4 >> 8;
	uint16_t maskedThird4 =  0x00F0 & given;
	uint8_t third = maskedThird4 >> 4;
	uint16_t maskedFourth4 =  0x000F & given;
	uint8_t fourth = maskedFourth4;

	WriteDisplay(1,0,HexToDisplayCode(first));				// Write first
	WriteDisplay(1,0,HexToDisplayCode(second));				// Write second
	WriteDisplay(1,0,HexToDisplayCode(third));				// Write third
	WriteDisplay(1,0,HexToDisplayCode(fourth));				// Write fourth
}
void Write16BitToDisplayAsDec(uint16_t given) {
	char buffer[5];
	snprintf(buffer, 5, "%d", given);
	for (int i = 0; i < 5; i++) {
		if (buffer[i] == '0') {
			WriteDisplay(1,0,HexToDisplayCode(0x00));
		}
		if (buffer[i] == '1') {
			WriteDisplay(1,0,HexToDisplayCode(0x01));
		}
		if (buffer[i] == '2') {
			WriteDisplay(1,0,HexToDisplayCode(0x02));
		}
		if (buffer[i] == '3') {
			WriteDisplay(1,0,HexToDisplayCode(0x03));
		}
		if (buffer[i] == '4') {
			WriteDisplay(1,0,HexToDisplayCode(0x04));
		}
		if (buffer[i] == '5') {
			WriteDisplay(1,0,HexToDisplayCode(0x05));
		}
		if (buffer[i] == '6') {
			WriteDisplay(1,0,HexToDisplayCode(0x06));
		}
		if (buffer[i] == '7') {
			WriteDisplay(1,0,HexToDisplayCode(0x07));
		}
		if (buffer[i] == '8') {
			WriteDisplay(1,0,HexToDisplayCode(0x08));
		}
		if (buffer[i] == '9') {
			WriteDisplay(1,0,HexToDisplayCode(0x09));
		}
	}
}
void SetDisplay(){
	WriteDisplay(0,0,0b00111100);			// Function Set
	WriteDisplay(0,0,0b00001100);			// Display ON Cursor OFF
}
void ClearDisplay(){
	WriteDisplay(0,0,0b00000001);			// Clear Display
	WriteDisplay(0,0,0b00000010);			// Return Home
}
int main() {
	init();
	SetDisplay();						// Setup the display
	ClearDisplay();
	while (1) {
		_delay_ms(60); 			// To allow sufficient time between queries (60ms min)

		PORTC |= (1<<PC4);		// Set trigger high
		_delay_us(10);			// for 10uS
		PORTC &= ~(1<<PC4);		// to trigger ultrasonic module
	}
}
ISR(PCINT1_vect) {
	if (bit_is_set(PINC,PC5)) {						// Checks if echo is high
		TCNT1 = 0;									// Reset Timer
		PORTC |= (1<<PC3);
	} else {
		uint16_t numuS = TCNT1;					// Save Timer value
		uint8_t oldSREG = SREG;
		cli();									// Disable Global interrupts
		ClearDisplay();							// Clear Display and send cursor home
		Write16BitToDisplayAsDec(numuS/58);		// Write Timer Value / 58 (cm)
		WriteDisplay(1,0,0x20);					// Space
		WriteDisplay(1,0,0x63);					// c
		WriteDisplay(1,0,0x6d);					// m

		SREG = oldSREG;							// Enable interrupts
		PORTC &= ~(1<<PC3);
	}
}
