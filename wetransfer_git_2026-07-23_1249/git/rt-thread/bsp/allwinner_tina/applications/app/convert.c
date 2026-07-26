#include "rtthread.h"
#include "convert.h"

//#define DEMO

uint16_t ex_data = 0xFFFF;
uint8_t btn_count_code = 0;
uint8_t btn_count_flags = 0;

uint32_t State = 0, Option;
int8_t Floor = 0, Code = LC_BINARY, FirstFloor = 0, LastFloor = 9;

signed char gray_to_binary(unsigned char gdata,unsigned char bit);

void convert( uint16_t data, int8_t* floor, uint32_t* state)
{

uint16_t change;
	
	if( Option & LO_FLAGS_COM)
	{
		//FLAGS_PULL_UP;
		data ^= FLAGS_MASK;
	}
	else
	{
		//FLAGS_PULL_DOWN;
	}
	
	if( Code == LC_7SEG )
		data ^= FLOOR_MASK;	
	
	change = data ^ ex_data  ;
	ex_data = data;
	
	if(Code == LC_7SEG)
	{
		unsigned char Okey=1;
		switch(data & SSEGMENT_MASK)
		{
			case SEVEN_SEG_0: Floor = 0;
				break;
			case SEVEN_SEG_1: Floor = 1;
				break;
			case SEVEN_SEG_2: Floor = 2;
				break;
			case SEVEN_SEG_3: Floor = 3;
				break;
			case SEVEN_SEG_4: Floor = 4;
				break;
			case SEVEN_SEG_5: Floor = 5;
				break;
			case SEVEN_SEG_6: Floor = 6;
				break;
			case SEVEN_SEG_7: Floor = 7;
				break;
			case SEVEN_SEG_8: Floor = 8;
				break;
			case SEVEN_SEG_9: Floor = 9;
				break;
			default: Okey = 0;
				break;
		}
		
		if(Okey)
		{
		
			if(data & SEGMENT_1)
			{	
				if(Floor < 10)
					Floor = Floor + 10;
			}
			else if(data & SEGMENT_2)
			{
				if(Floor < 10)
					Floor = Floor + 20;
			}
			else if(data & SEGMENT_N)
			{
				if(Floor < 10)
					Floor = 0-Floor;
			}
			
			Floor = Floor - FirstFloor;
		}
	}
	
	else if(Code == LC_TTUP)
	{
		if(change & TT_M1 )
		{
			if(data & ARROW_UP )
			{
				if(data & TT_M1 )
					Floor++;
			}
			
			if(data & ARROW_DOWN )
			{
				if(data & TT_M1 )
					Floor--;
			}
		}
		
		if((data & TT_817) || (data & TT_818))
		{
			if(!(data & TT_817))
			{	
				Floor = 0;
			}
			
			if(!(data & TT_818))
			{
				Floor = LastFloor - FirstFloor;
			}
		}

	}
	else if(Code == LC_BINARY)
	{
		Floor = data & BINARY_MASK ;
	}
	else
	{
		Floor = gray_to_binary(data & BINARY_MASK,5);
	}
	
	int8_t stopc = LastFloor - FirstFloor;
	if(stopc < 0 ) 
		stopc = -stopc;
	
	if(Floor > stopc)
		Floor = stopc;
	
	if((Floor) < 0)
		Floor = 0;
	
	State = LS_STOP;
	
	if((data & (ARROW_UP|ARROW_DOWN)) == ARROW_UP)
		State = LS_MOVING_UP;
		
	else if((data & (ARROW_UP|ARROW_DOWN)) == ARROW_DOWN)
		State = LS_MOVING_DOWN;

	if(data & OVERLOADED)
	{
		State = LS_OVERLOAD;
	}

	
	if(data & SERVICE)
		State = LS_OUT_OF_SERVICE;
	
	//Buttons 
	/*
	
	if(!(BTN_FLAGS & data))
	{
		btn_count_flags++;
	}
	else
	{
		btn_count_flags=0;
	}
	
	if(btn_count_flags == 15)
	{
		Option ^= LO_FLAGS_COM;
	}
	
	if(!(BTN_CODE & data))
	{
		btn_count_code++;
	}
	else
	{
		btn_count_code = 0;
	}
	
	if(btn_count_code ==15)
	{
		Code++;
		
		if(Code> LC_7SEG)
			Code = LC_GRAY;

	}
	
	if(Code == LC_GRAY)
	{

	}
	else if(Code == LC_BINARY)
	{

	}
	else if(Code == LC_TTUP)
	{

	}
	else if(Code == LC_7SEG)
	{

	}
	*/

	*floor = Floor;
	*state = State;

}

signed char gray_to_binary(unsigned char gdata,unsigned char bit)
{
	unsigned char bdata=0;
	for(unsigned char i = 0;i<bit;i++)
	{
		bdata ^= gdata;
		gdata >>=1;
	}
	
	return bdata;
}
