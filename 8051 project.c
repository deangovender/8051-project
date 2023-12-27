#include <reg51.h>

sbit highTemp = P3^0;		//high temp led
sbit adcRD = P3^5;			//ADC read signal
sbit adcWR = P3^6;			//adc write signal
sbit adcINTR = P3^7;		//adc INTR signal
sbit hexEncoder = P3^2;	//hex encoder interruptPin

float a = 0.353; 				//Scaling factor = Max sensor reading / Max ADC = 90/255						
int hundred = 100;			//overflow value

unsigned char raw_adc;
unsigned int x;					//used for overflow
unsigned char temp;			//8 bit used to calc temp
unsigned char adc_val;	//8 bit read value of ADC

void conv();							//function to tell ADC to convert tempreture signal to ADC code
void read();							//function to read ADC code from ADC
void delay2(void);				//2 second delay for start characters on 7 seg
void delay1(void);				//1 second delay to display ADC raw value
void INCHAR();						//interrupts subroutine for hex pad


void main(void)
{
	P1 = 0;									//Port 1 set as output
//------- DELAY FOR LEDS 1.0 - 1.1 - 1.6 - 1.7 -----------------------------------------------------------------------------
	TMOD = 0X01; 						//timer 0 mode 1
	P1 = 0xC3;							//7 seg- display - 11000011
	delay2(); 							//wait 2 seconds
	P1 = 0X00;							//LEDS set to zero
	
//---- EXTERNAL INTERRUPT--------------------------------------------------------------------------------------------------
	IE = 0X81;							//enable gloabal interrupts and INT0
	IT0 = 1;								//enable EXT0 to be negative edge triggered signal
	
//-----	MAIN PROGRAM -------------------------------------------------------------------------------------------------------
while(1) 									//repeat forever
	{	
//---- READ ADC REPEATIDLY--------------------------------------------------------------------------------------------------
		conv();								//jump to function to convert tempreture sensor signal to digital value
		read();								//jump to fucntion to read ADC value into port 0
		temp = adc_val * a;		//ADC value from read function, converted to actual tempreture
		if(temp < 40)					//if tempreture is less than 40 degrees
			highTemp = 0;				//switch led connected to P3.0 OFF
		else						
			highTemp = 1;				//else, led ON
		P1 = temp;						//display tempreture value across port 1
		adcRD = 1;						//adc read pin brought high
		adc_val = 0;					//reset raw adc value for next loop
		}
}

void conv()
{
	adcWR = 0;							//ADC write pin brought low
	adcWR = 1;							//ADC write pin brought high, to trigger INTR
	while(adcINTR == 1);		//wait for INTR to toggle
}


void read()
{
	adcRD = 1;							//ADC read pin brought high
	adcRD = 0;							//ADC read pin brought low to end reception of ADC data
	adc_val = P0;						//take recieved data from ADC on Port 0 and place it in adc_val variable
	adcRD = 1;							//reset ADC read pin to normally high
}


void delay2()							//2 second delay
{
	x = hundred;
	do
	{
		TH0 = 0XB7;						//18433 high byte 
		TL0 = 0XA9;						//18433 low byte
		TR0 = 1;							//start timer 0	
		while(TF0 != 1);			//wait for over flow
		TF0 = 0;							//reset flag
		TR0 = 0;							//stop timer
		x--;									//decrement x
	}while(x != 0);				  //wait for 100 overflows

}

void INCHAR() interrupt 0				//when interrupted
{
	do{
	P1 = 0;												//clear P1
	P1 = P2;											//set value from port 2 to Port 1
	}while(hexEncoder == 1);
	P2 = 0;												//RESET Port2
	while(hexEncoder != 1)
	{
		P1 = adc_val;								//display raw adc value
		delay1();										//wait 1 seconds then return 
	}
}


void delay1(void)							//1 second delay
{
	x = hundred;
	do
	{
		TH0 = 0XDB;						//9217 high byte 
		TL0 = 0XFF;						//9217 low byte
		TR0 = 1;							//start timer 0	
		while(TF0 != 1);			//wait for over flow
		TF0 = 0;							//reset flag
		TR0 = 0;							//stop timer
		x--;									//decrement x
	}while(x != 0);				  //wait for 100 overflows	
}

