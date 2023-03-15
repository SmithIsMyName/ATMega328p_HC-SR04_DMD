/*
 * Matrix_KeyPad_JF4.c
 *
 * Created: 10/2/2022 1:38:45 PM
 * Author : sara.stout-grandy
 *
 * Read a password from the Keypad
 * Fill in the missing code at every "xx"
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>

//Volatile Variable
volatile int row;
volatile int col;


//Function Prototypes
void init_hardware(void);
void set_row_low(int row);
int col_pushed(void);

char get_button(void);
char get_new_button(void);
static	char last_button = 0;
int state = 1;
int main(void)
{

	char b=0;
	init_uart();
	char pin[10]; //string as an array of chars
	char password[]= "4949"; 
	int i=0;


	printf("System Booted, built %s on %s\n ", __TIME__,__DATE__);
	printf("Simple button test to help build a map\n");
	
	init_hardware();
	printf("password: %s\n",password); //Just so you can see it!

	printf("Enter a 4 digit PIN (# to backup, * to erase whole thing): ");


	while (1) 
    {
		b=get_new_button();
		//Clear a digit 
		if (b== '#' && i>0)
		{
			printf("\b \b");
			pin[i--];
			continue;//breaks one iteration
		} else if (b== '*' && state == 1)
		{
			//SetnewPass______________________________________________________________________________________________________________________________________
			printf("\nnew pass\n");
			state = 0;
			i=0;
		}else if (b && b != '#' && b != '*')
		{
			pin[i++] = b; //Store value pin array to "b" 
			
			printf("%c",b);

		}
		if (i>=4)
		{
			
			
			if(state != 0)
			{
				pin[4]='\0'; //Terminate the string with a null terminator...that makes it a string.
				if (strcmp(pin,password)) //need to use string compare!
				{
					printf("PIN Incorrect, try again \n"); //Print "PIN Incorrect, try again" to Putty;
					printf("Re-Enter Password \n"); //Re-ask for password entry
					i=0;
				}
				else
				{
					printf("Pin Correct, Here's a cookie ;3 \n"); //Print "PIN Correct" to Putty
					if(state == 1)
					{
						state = 2 ;
						
					}else if(state == 2 || state == 3)
					{
						state = 1;
						
					}
					i=0;
				}
				printf("State = %d\n",state);
			}else if(state == 0)
			{
				for(int j =0; j<= 3 ; j++)
				{
					printf("ping %d\n", j);
					password[j] = pin[j];
				}
				printf("New Password Set \n");
				i = 0;
				
				state = 1;
			}
		}
	}
	return 0;
}


char get_button(void)
{
			int key_pressed=0;
			char b;
			char buttons[4][3]={{'1','2','3'},
			{'4','5','6'},
			{'7','8','9'},
			{'*','0','#'}}; //Same as in Matrix_KeyPad_1.c

			//Check for button push
			//Cycle through the rows with the for loop
			for (row=0; row< 4 ; row++)
			{
				set_row_low (row);
				_delay_ms(20);
				
				col=col_pushed();

				if (col)
				{
					b = buttons[row][col-1]; //b= look-up table in matrix
					//printf("%d-%d: %c\n", row, col,b );
					_delay_ms(500);
					key_pressed=1; //A key was pressed
				}
			}
			if (key_pressed)
			{
				return b;
			}
			else
			{
				return 0;
			}
}

char get_new_button(void)
{
	char b;
	
	b=get_button(); //Call get_button function
	if (b == last_button)
	{
		return 0;
	}

	last_button = b;
	return b;
}

void init_hardware(void)
{
				//Set pins: rows as output and columns as inputs
				//Rows as outputs
				DDRB |= (1<<PB1);
				DDRB |= (1<<PB0);
				DDRD |= (1<<PD7);
				DDRD |= (1<<PD6);
				//Set outputs to be low
				PORTB ^=(0<<PB1);
				PORTB ^=(0<<PB0);
				PORTD ^=(0<<PD7);
				PORTD ^=(0<<PD6);
				//Set Columns as inputs
				DDRD &= ~(1<<PD5);
				DDRD &=~(1<<PD4);
				DDRD &=~(1<<PD3);
				
				//Enable pull-up resistors on the inputs
				PORTD |=(1<<PD5);
				PORTD |=(1<<PD4);
				PORTD |=(1<<PD3);
}

void set_row_low(int row)
{
	//Hi-Z the rows (make inputs without pull-ups)
	
	PORTD |= (1<<PD5);
	PORTD |= (1<<PD4);
	PORTD |= (1<<PD3);

	//Drive the specified row low
	switch(row)
	{
		case 0: //set Row 1 low
		PORTB &= ~(1<<PB1);
		PORTB |= (1<<PB0);
		PORTD |= (1<<PD7);
		PORTD |= (1<<PD6);
		break;
		case 1: //set Row 2 low
		PORTB &= ~(1<<PB0);
		PORTB |= (1<<PB1);
		PORTD |= (1<<PD7);
		PORTD |= (1<<PD6);
		break;
		case 2: //set Row 3 low
		PORTD &= ~(1<<PD7);
		PORTB |= (1<<PB0);
		PORTB |= (1<<PB1);
		PORTD |= (1<<PD6);
		break;
		case 3: //set Row 4 low
		PORTD &= ~(1<<PD6);
		PORTB |= (1<<PB0);
		PORTD |= (1<<PD7);
		PORTB |= (1<<PB1);
		break;
		default: printf("no row set\n");
	}
}

int col_pushed(void)
{
	
	if ((PIND & (1<<5) )==0) //check column 1
	{
		//	printf("column=1\n");
		return 1;
		
	}
	else if ((PIND & (1<<4) )==0) //check column 2
	{
		//printf("column=2\n");
		return 2;
	}
	else if ((PIND & (1<<3) )==0) //check column 3
	{
		//printf("column=3\n");
		return 3;
	}
	else
	{
		return 0;
	}
	
}
