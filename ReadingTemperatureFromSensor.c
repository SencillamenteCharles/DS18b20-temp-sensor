/*
 * DS18B20_TEMP_SENSOR.c
 *
 * Created: 19/07/2018 11:37:11 p. m.
 *  Author: D.
 */ 
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#define skipRom 0xCC
#define convertT 0x44
#define readScratchPad 0xBE
#define UP     1
#define DOWN   0
#define RX 2
typedef enum{FALSE,TRUE}bool;
	
void pinMode(uint8_t MODE);
bool reset_DS18B20();
void sendCommand_DS18B20(uint8_t command);	
bool receiveData_DS18B20(int** arr,uint8_t arr_lenght);
int data[9]={};
int main(void)
{
	 
		
    while(1)
    {
    if(reset_DS18B20())
	   {_delay_us(200);
	   sendCommand_DS18B20(skipRom);
	   _delay_us(300);
	   sendCommand_DS18B20(convertT);
	   _delay_ms(750);
	  
	   }
	  
	   
	   if(reset_DS18B20())
	   {
		  _delay_us(200);
		 sendCommand_DS18B20(skipRom); 
		 _delay_us(200);
		 sendCommand_DS18B20(readScratchPad);
		 _delay_us(200);
		 int arr[9];
		 receiveData_DS18B20(&arr,9);
		 DDRB=255;
		 PORTB=arr[1];
		 
	   }
	   _delay_ms(800);
	   
    }
}
void pinMode(uint8_t MODE)
{
	if(MODE==UP)
	{
	DDRD|=1<<PIND0;
	PORTD|=1<<PIND0;
	}	
	else if(MODE==DOWN)
	{
	DDRD|=1<<PIND0;
	PORTD&=~(1<<PIND0);//pin 
	}
	else
	{
	DDRD&=~(1<<PIND0);//pin again as input
	//PORTD|=1<<PIND0;//enable pull up
	}
	
}
 bool reset_DS18B20()
{
	pinMode(DOWN);
	_delay_us(500);
	pinMode(UP);
	pinMode(RX);//pin set as input with pull up enabled
	_delay_us(60);
	if ((PIND & 0b00000001)==DOWN)
	{
		
		return TRUE;
		
	}		
	else
	    return FALSE;
}
void sendCommand_DS18B20(uint8_t command)
{
	/*
	All data and commands are transmitted least significant
    bit first over the 1-Wire bus.
	-each time slot last minimum 60us
	-there are 2 slots for write :write 0 and write 1
	-In write 1 master pulls 1 wire com line low within 15 us, on the
	other hand for write 0 master must hold line low about 60us
	-Pull up resistor (5k) must be  used
	
	*/
	pinMode(UP);
	bool writeSlot;
	for (uint8_t i=0;i<8;i++)
	{
		
		writeSlot=(command>>i ) & 0b00000001;
		if ( writeSlot==1)//write 1 slot?
		{
			PORTD&=~(1<<PIND0);
			_delay_us(15);
			PORTD|=1<<PIND0;
			_delay_us(45);
		}
		else
		{
			PORTD&=~(1<<PIND0);
			_delay_us(60);
			PORTD|=1<<PIND0;
			
		}
		_delay_us(1);//rest time
		
	}
	//pinMode(RX);
	
}
bool receiveData_DS18B20(int** arr,uint8_t arr_lenght)
{
	
	int* data=(int*)arr;
	
	for (uint8_t i=0;i<arr_lenght;i++)
	{
		_delay_us(200);
		data[i]=0;
		for (uint8_t b=0;b<8;b++)
		{
			pinMode(DOWN);
			pinMode(UP);
			//slave sends bit
			pinMode(RX);
			PORTD|=1<<PIND0;
			_delay_us(2);
			data[i]=data[i]>>1;
			if ((PIND & 0b00000001)==0)//if after 15us the bus is high, we have a one
				{	
				}
			else
			    data[i]|=0b10000000;						
			_delay_us(58);	
		}
		
		
	}
	return TRUE;
}
