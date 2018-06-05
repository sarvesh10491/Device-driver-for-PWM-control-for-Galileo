//################################################################################################
//
// Program     : RGB Led control header
// Header file : RGB_lib.h
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
#include <linux/input.h>

#define BASE 1000

// GPIO pin multiplexing table.
// Each column represents corresponding IO0 to IO13 shield pins.
// Each row represents specific function select GPIO pins.
// If value is "-1", no gpio setting required for that pin.
//=====================================================================================
//                   IO : 0  1  2  3  4  5  6  7  8  9 10 11 12 13       Functions :
int gpio_ctrl[4][14] = {{32,28,34,16,36,18,20,-1,-1,22,26,24,42,30},  // Level shifter
						{-1,45,77,76,-1,66,68,-1,-1,70,74,44,-1,46},  // MUX select 1
						{-1,-1,-1,64,-1,-1,-1,-1,-1,-1,-1,72,-1,-1},  // MUX select 2
						{11,12,13,14, 6, 0, 1,38,40, 4,10, 5,15, 7}}; // Linux gpio pins

// Variable declarations area
//============================
int duty_cycle;
int RGB_exp[3];

int fd_export;
int fd[3];
	
char buf[256];
int len,i,j;

int mouse_read=0;
int fd_m=0;
   
const char *mouse_device = "/dev/input/event2"; // Decive file path to read mice events
struct input_event ie;


// Timer setting
//===============
struct timespec cur_time,pre_time;

float duty_per;
int PWM_On;
int PWM_Off;

// Function to setup GPIO pins
//=============================
void IO_Setup()
{
	// PWM setting
	//=============	
	duty_per=(float) duty_cycle/100;      // Fraction of cycle period for which LED will be on
	PWM_On = (duty_per) * 20 * BASE;      // Time for which PWM cycle will be on
	PWM_Off = (1-(duty_per)) * 20 * BASE; // Time for which PWM cycle will be off

	// Opening gpio/export
    //======================
    fd_export = open("/sys/class/gpio/export", O_WRONLY);
	if(fd_export < 0) 
	{
		printf("gpio export open failed \n");
		return;
	}

	for(i=0; i < 3; i++)   // To loop over 3 IO pins as given by user (column in gpio_ctrl table)
	{
		for(j=0; j<4; j++) // To loop over all required gpio pins for selected IO (row in gpio_ctrl table)
		{
			printf("Setting up GPIO pin %d for IO%d \n", gpio_ctrl[j][RGB_exp[i]], RGB_exp[i]);

			if(gpio_ctrl[j][RGB_exp[i]] != -1)
			{
				// Writing to /gpio/export
				//=========================
				len = snprintf(buf, sizeof(buf), "%d",gpio_ctrl[j][RGB_exp[i]]);
				if(len < 0)
				{
					printf("sprintf error\n");
				}
				else
					write(fd_export,buf,2); 
			 

				// Writing to /gpio/direction
				//=============================
				if(gpio_ctrl[j][RGB_exp[i]] < 64)  // GPIO 64-79 can be output only. Hence direction setting not required.
				{

					
					len = snprintf(buf, sizeof(buf),"/sys/class/gpio/gpio%d/direction",gpio_ctrl[j][RGB_exp[i]]);
					if(len < 0)
					{
						printf("sprintf error\n");
					}
					else
			        	fd[i] = open(buf, O_WRONLY);

						
					if (fd[i] < 0)
					{
						perror("gpio/direction");
					}
					else
						write(fd[i], "out", 3);
					//close(fd[i]);
				}

				
				// Writing to /gpio/value
				//========================
				len = snprintf(buf, sizeof(buf),"/sys/class/gpio/gpio%d/value",gpio_ctrl[j][RGB_exp[i]]);
				if(len < 0)
				{
				  	printf("sprintf error\n");
				}
				else
					fd[i] = open(buf, O_WRONLY);

				if (fd[i] < 0)
				{
					perror("gpio/value");
				}
				else
					write(fd[i], "0", 1);
	        }					
		}
		printf("\n");
    }
printf("\n");
}

// Function to print out RGB pattern
//=====================================
void RGB_Out(int r,int g, int b)
{
	clock_gettime(CLOCK_MONOTONIC, &pre_time);
	clock_gettime(CLOCK_MONOTONIC, &cur_time);

	while(cur_time.tv_nsec - pre_time.tv_nsec < 500000000ul)  // Check for timer overflow for next pattern
	{
			// Switch on respective RGB leds
			if(r == 1)
				write(fd[0],"1",1);

			if(g == 1)
				write(fd[1],"1",1);

			if(b == 1)
				write(fd[2],"1",1);

			usleep(PWM_On);

			if(r == 1)
				write(fd[0],"0",1);

			if(g == 1)
				write(fd[1],"0",1);

			if(b == 1)
				write(fd[2],"0",1);

			usleep(PWM_Off);

			clock_gettime(CLOCK_MONOTONIC, &cur_time);
	}
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

//========================================================
// End of RGBLed library
//========================================================