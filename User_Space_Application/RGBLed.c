//################################################################################################
//
// Program     : RGB Led control
// Source file : RGBLed.c
// Authors     : Sarvesh Patil & Nagarjun Chinnari
// Date        : 12 October 2017
//
//################################################################################################

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "RGB_lib.h"

//===============================
// Main
//===============================
int main(int argc, char **argv)
{
 	
	//==================================================
	// Capturing inputs provided by user during runtime
	// duty_cycle  = duty cycle %
	// RGB_exp[0] = Red Led pin
	// RGB_exp[1] = Green Led pin
	// RGB_exp[2] = Blue Led pin
	//==================================================
 	duty_cycle = atoi(argv[1]);

 	for(i=2; i<argc; i++) 
    	RGB_exp[i-2] = atoi(argv[i]);
    
 	
    // Setup GPIO pins
    //=================
    IO_Setup();


    mouse_setup();


    printf("================================================================\n");
    printf("Entering program execution.\n");

    //=======================
    // Pattern driving logic
    //=======================
	while(1) 
	{
		// RGB_Out(Red_LED_pin, Green_LED_pin, Blue_LED_pin)
		// Send 1 to RGB_Out() function to make respective LEDs on
		// mouse_read==1 is a check for termination condition. On success, it breaks out of while loop
		//---------------------------------------------------------------------------------------------
     	RGB_Out(1,0,0);
	    if(mouse_read==1)
	    	break;

		RGB_Out(0,1,0);
		 if(mouse_read==1)
	    	break; 

		RGB_Out(0,0,1);
		if(mouse_read==1)
	    	break;

		RGB_Out(1,1,0);
		if(mouse_read==1)
	    	break;

		RGB_Out(1,0,1);
		if(mouse_read==1)
	    	break;

		RGB_Out(0,1,1);
		if(mouse_read==1)
	    	break;

		RGB_Out(1,1,1);
		if(mouse_read==1)
	    	break;
	}
	printf("================================================================\n");


	// Closing opened file descriptors
	//=================================
	printf("Closing opened file descriptors.\n");
	for(i=0;i<3;i++)
		close(fd[i]);
	close(fd_export);

	printf("================================================================\n");
	printf("Terninated.\n");
	printf("================================================================\n");


return 0;
}

//=================================================
// End of RGBLed source file
//=================================================