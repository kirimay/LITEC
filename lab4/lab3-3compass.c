/*  Lab3-3
Michael J. Gardner II && Chrstine Marini
Section 03
Side B
Date: 04/03/15

The goal of this code is to read values from the compass via i2c
and steer the wheels toward a desired direction.
 */
#include <stdio.h>
#include <c8051_SDCC.h>
#include <i2c.h>
#define PW_CENTER_STR 2685
#define PW_MIN_STR    2265
#define PW_MAX_STR    3195
//-----------------------------------------------------------------------------
// 8051 Initialization Functions
//-----------------------------------------------------------------------------
void Port_Init(void);
void PCA_Init (void);
void XBR0_Init(void);
void SMB_Init(void);
unsigned int ReadCompass(void);
void Steering_Servo(unsigned int direction);
unsigned int tmp0_lo_to_hi;
unsigned int STR_PW   = 0;
unsigned int count = 0;
char input;
 int actual_heading;
 int desired_heading = 900; //East
unsigned int error;
unsigned int offset;
//-----------------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------------

void main(void)
{
    // initialize board
    Sys_Init();
    putchar(' '); //the quotes in this line may not format correctly
    Port_Init();
    XBR0_Init();
    PCA_Init();
	SMB_Init();
	
    //print beginning message
    printf("Embedded Control Steering Calibration\n");
    // set the PCA output to a neutral setting
    //__________________________________________
    //__________________________________________
    STR_PW = PW_CENTER_STR;
    tmp0_lo_to_hi= 0xFFFF - STR_PW;
    PCA0CP0 = tmp0_lo_to_hi;
    count=0;
	while (count < 50);
	while (1)
	{
		count =0;
		while (count < 2);
		actual_heading = ReadCompass();
        offset = (unsigned int)((actual_heading +3600- desired_heading ) % 3600);
        printf("\r\n%d||%d",actual_heading,offset);
		Steering_Servo(offset);

    }
	

        
}

void Port_Init()
{
    P1MDOUT = 0x0F;  //set output pin for CEX0 and CEX2 in push-pull mode
	P0MDOUT &= ~0x32;
}

unsigned int ReadCompass(void)
{
	unsigned char Data[2];
	unsigned int range =0;
	unsigned char addr=0xC0;
	i2c_read_data(addr, 2,Data,2);
	range = ((unsigned int) Data[0] << 8 | Data[1]);
	return range;

}

//-----------------------------------------------------------------------------
// XBR0_Init
//-----------------------------------------------------------------------------
//
// Set up the crossbar
//
void XBR0_Init()
{
    
    XBR0  = 0x27;  //configure crossbar as directed in the laboratory

}

void SMB_Init(void)
{
	SMB0CR =0x93;
	ENSMB =1;
}

//-----------------------------------------------------------------------------
// PCA_Init
//-----------------------------------------------------------------------------
//
// Set up Programmable Counter Array
//
void PCA_Init(void)
{
    // reference to the sample code in Example 4.5 -Pulse Width Modulation 
    // implemented using the PCA (Programmable Counter Array), p. 50 in Lab Manual.
    PCA0MD = 0x81;
    PCA0CPM0 = 0xC2;    //CCM0 in 16-bit compare mode
    PCA0CN = 0x40;      //Enable PCA counter
    EIE1 |= 0x08;       //Enable PCA interrupt
    EA = 1;             //Enable global interrupts
}

//-----------------------------------------------------------------------------
// PCA_ISR
//-----------------------------------------------------------------------------
//
// Interrupt Service Routine for Programmable Counter Array Overflow Interrupt
//
void PCA_ISR ( void ) __interrupt 9
{

    if (CF)
    {
        CF =0;
        PCA0 = 0x7000;
        count++;
    }

    PCA0CN &= 0xC0;
    // reference to the sample code in Example 4.5 -Pulse Width Modulation 
    // implemented using the PCA (Programmable Counter Array), p. 50 in Lab Manual.
}

void Steering_Servo(unsigned int direction)
{
    
    //wait for a key to be pressed
	if (direction < 1800)
	{
		if (STR_PW <= PW_CENTER_STR - (float)(direction)/4.2)
		{
			STR_PW = PW_CENTER_STR - (float)(direction)/4.2;
		}
		else
		{
			STR_PW -= 10;
		}
	}
	else if ( direction == 0 || direction ==3600)
	{
		STR_PW=PW_CENTER_STR;
	}
	else
	{
		if (STR_PW >= PW_CENTER_STR + (float)(3600-direction)/1.9)
		{
			STR_PW = PW_CENTER_STR + (float)(3600-direction)/1.9;
		}
		else
		{
			STR_PW += 10;
		}			
	}		


    printf("\r\nSTR_PW: %u", STR_PW);
    tmp0_lo_to_hi= 0xFFFF - STR_PW;
    PCA0CP0 = tmp0_lo_to_hi;


}




