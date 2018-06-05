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
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/input.h>

//===============================
// Main
//===============================

int RGB_exp[3],duty_cycle;


int mouse_read=0;
int fd_m=0;

typedef struct
{
    int R_pin;
    int G_pin;
    int B_pin;
    int duty_PWM;
} RGB_arg;

   
const char *mouse_device = "/dev/input/event2"; // Decive file path to read mice events
struct input_event ie;
int mouse_setup();


int main(int argc, char **argv)
{
 	int cmd,i,j;
 	int *a;
 	cmd=1;
 	char *file_name = "/dev/RGBLed";
    int fd;
    RGB_arg RGB;
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
    
 	RGB.duty_PWM=duty_cycle;
 	RGB.R_pin=RGB_exp[0];
 	RGB.G_pin=RGB_exp[1];
 	RGB.B_pin=RGB_exp[2];

	fd=open(file_name, O_RDWR);

  if(fd== -1) //Because open returns -1 on occurence of error
    {
      printf("ERROR Opening LED Driver\n");
      

    }
    

   j=ioctl(fd, cmd,&RGB);

   if(j<0)
   {
   	printf("error ioctl\n");
   }

    mouse_setup();


	a=malloc(sizeof(int));	

    printf("================================================================\n");
    printf("Entering program execution.\n");

    //=======================
    // Pattern driving logic
    //=======================
	while(1) 
	{
		
		// write(Red_LED_pin, Green_LED_pin, Blue_LED_pin)
		// Send 1 to write() function to make respective LEDs on
    //sleeps for 0.5 seconds
		// mouse_read==1 is a check for termination condition. On success, it breaks out of while loop
		//---------------------------------------------------------------------------------------------
		*a=4;
     write(fd,(char *) a,sizeof(int));	;
     	usleep(500000);
	    if(mouse_read==1)
	    	break;

		*a=2;
     	write(fd,(char *) a,sizeof(int));
     usleep(500000);
	    if(mouse_read==1)
	    	break;
	 *a=1;
     write(fd,(char *) a,sizeof(int));
     usleep(500000);
	    if(mouse_read==1)
	    	break;
	 *a=6;
     	write(fd,(char *) a,sizeof(int));
     usleep(500000);
	    if(mouse_read==1)
	    	break;
	 *a=5;
     	write(fd,(char *) a,sizeof(int));
     usleep(500000);
	    if(mouse_read==1)
	    	break;
	 *a=3;
     	write(fd,(char *) a,sizeof(int));
     usleep(500000);
	    if(mouse_read==1)
	    	break;
	  *a=7;
     	write(fd,(char *) a,sizeof(int));
     usleep(500000);
	    if(mouse_read==1)
	    	break;



		}
	printf("================================================================\n");


	// Closing opened file descriptors
	//=================================
	printf("Closing opened file descriptors.\n");

		close(fd);
	

	printf("================================================================\n");
	printf("Terninated.\n");
	printf("================================================================\n");


return 0;
}


// pthread function to read mouse event in parallel
//==================================================
void *m_thread_func(void *ptr)
{
  while(mouse_read!=1)
    {
        // Read Mouse     
        read(fd_m, &ie, sizeof(ie));

        if(ie.type==EV_KEY)
        {
           
            if(ie.code==BTN_RIGHT || ie.code==BTN_LEFT ) // Check for left or right click
            {
                mouse_read=1; // set global variable mouse_read to 1 to initiate termination
            }     
        } 
    } 
  pthread_exit(NULL);
  return NULL;
}



// Function for mouse setup
//===========================
int mouse_setup()
{   
	pthread_t m_tid;

    fd_m = open(mouse_device, O_RDONLY);

    if(fd_m == -1)
     {

      printf("ERROR Opening mouse device. %s\n", mouse_device);
      return -1;
     }
  
  	pthread_create(&m_tid,NULL,m_thread_func,NULL);
    return 0;
}

//=================================================
// End of RGBLed source file
//=================================================

