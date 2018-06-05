//################################################################################################
//
// Program     : RGB Led control Kernel module
// Header file : RGBLed_mod.c
// Authors     : Sarvesh Patil & Nagarjun Chinnari
// Date        : 25 October 2017
//
//################################################################################################


#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/version.h>
#include <linux/gpio.h>
#include <linux/jiffies.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

// Driver information
//======================
#define  MODULE_NAME   "RGBLed"
#define  DEVICE_NAME   "RGBLed"
#define  CLASS_NAME    "RGBLedClass"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sarvesh Patil & Nagarjun Chinnari");
MODULE_DESCRIPTION("Linux RGB Led control Driver");
MODULE_VERSION("0.1");


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






// Character device initializations
//===================================
static dev_t RGB_dev_number;              // Allotted device number
static struct cdev RGB_cdev;              // cdev structure

static struct class*  RGBLedClass  = NULL;
static struct device* RGBLedDevice = NULL;


static int     RGB_open(struct inode *, struct file *);
static int     RGB_release(struct inode *, struct file *);
static ssize_t RGB_write(struct file *, const char *, size_t, loff_t *);

static long    RGB_ioctl(struct file *, unsigned int, unsigned long);


// Variable declaration area
//============================
int duty_PWM;
int ret;

	
char buf[256];


int RGB_exp[3],flag;
int RGB[3]={0,0,0};



ktime_t on_interval,off_interval,off;

static struct hrtimer PWM_timer;

//static struct timer_list my_timer;
typedef struct
{
    int R_pin;
    int G_pin;
    int B_pin;
    int duty_PWM;
} RGB_arg;




// File operations definition
//=============================
static struct file_operations fops =
{
   .owner = THIS_MODULE,
   .open = RGB_open,
   .write = RGB_write,
   .release = RGB_release,
   .unlocked_ioctl = RGB_ioctl,
   
};

//callback function
//===============
 enum hrtimer_restart PWM_callback(struct hrtimer *timer_for_restart)
{   
	ktime_t advance,lcurrent = ktime_get(); 
   
	  if(flag == 1)
	 	{
	 		
	 		//setting the values of R,G,B leds to 1 correspondingly
	 		if(RGB[2]==1)
			gpio_set_value(gpio_ctrl[3][RGB_exp[0]], 1);

			if(RGB[1] == 1)
			gpio_set_value(gpio_ctrl[3][RGB_exp[1]], 1);				

			if(RGB[0] == 1)
			gpio_set_value(gpio_ctrl[3][RGB_exp[2]], 1);

	 		flag = 0;

	 		advance = on_interval;
	 		hrtimer_forward(timer_for_restart,lcurrent,advance);//calling the hrtimer for on_interval time and then goes to callback function
		 return HRTIMER_RESTART;
	
	 	}
	 

	else{
		  //setting the values of R,G,B leds to 0 correspondingly
		
			gpio_set_value(gpio_ctrl[3][RGB_exp[0]], 0);


			gpio_set_value(gpio_ctrl[3][RGB_exp[1]], 0);				


			gpio_set_value(gpio_ctrl[3][RGB_exp[2]], 0);

	 		flag = 1;

	 		advance = off_interval;

	 		hrtimer_forward(timer_for_restart,lcurrent,advance);//calling the hrtimer for off_interval time and then goes to callback function
		 
	
	 	return HRTIMER_RESTART;
	}
return 0;
	
}



//##########################
// Driver function area
//##########################

// ioctl function
static long RGB_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	RGB_arg RGB;
	int i,j;
	printk("%d\n",cmd);
	

	if(cmd == 1)
	{
		//copy the values of R pin, G pin , B pin , PWM  from user space to kernel space and storing the values 
		copy_from_user(&RGB, (RGB_arg *)arg, sizeof(RGB_arg));
        RGB_exp[0] =RGB.R_pin;
        RGB_exp[1] =RGB.G_pin;
        RGB_exp[2]=RGB.B_pin;
        duty_PWM = RGB.duty_PWM;
        printk("%d %d %d %d",RGB_exp[0],RGB_exp[1],RGB_exp[2],duty_PWM);

	}
	else
		return -EINVAL;

	// PWM setting
	//=============	
	    // Fraction of cycle period for which LED will be on
		on_interval = ktime_set(0,((unsigned long)duty_PWM * 20000000ul )/100);
		// Fraction of cycle period for which LED will be off
	 	off_interval = ktime_set(0,((100 - (unsigned long)duty_PWM) * 20000000ul)/100);

		//intializing hrtimer and setting its parameters
		hrtimer_init(&PWM_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		PWM_timer.function = &PWM_callback;
		

	for(i=0; i < 3; i++)   // To loop over 3 IO pins as given by user (column in gpio_ctrl table)
	{
		for(j=0; j<4; j++) // To loop over all required gpio pins for selected IO (row in gpio_ctrl table)
		{

			printk("Setting up GPIO pin %d for IO%d \n", gpio_ctrl[j][RGB_exp[i]], RGB_exp[i]);

			
			if(gpio_ctrl[j][RGB_exp[i]] != -1)
			{
				// Writing to /gpio/export
				//=========================
					gpio_request(gpio_ctrl[j][RGB_exp[i]], "sysfs");

					gpio_export(gpio_ctrl[j][RGB_exp[i]], false);            

				// Writing to /gpio/direction
				//=============================
				if(gpio_ctrl[j][RGB_exp[i]] < 64)  // GPIO 64-79 can be output only. Hence direction setting not required.
				{
					
						gpio_direction_output(gpio_ctrl[j][RGB_exp[i]], 0);   
								
				}

	        }					
		}
		printk("\n");
    }
printk("\n");


	return 0;
}

// Open function
int RGB_open(struct inode *inode, struct file *file)
{
  return 0;
}


// Release function
int RGB_release(struct inode *inode, struct file *file)
{
	// Deleting the Hrtimer
	hrtimer_cancel(&PWM_timer);
  return 0;
}

// Write function
ssize_t RGB_write(struct file *file, const char *buf,
           size_t count, loff_t *ppos)
{
	
	int *a = kmalloc(sizeof(int),GFP_KERNEL); 
	int on_flag=1;
	
	copy_from_user(a,(int *)buf,count);
	
	flag=1;	  
	//converting the integer to bits and storing them in a array
	RGB[2] = ((*a & 0x4) > 0);
  	RGB[1] = ((*a & 0x2) > 0);
  	RGB[0] = ((*a & 0x1) > 0);
  	kfree(a);

    	if(on_flag==1)	
		{
		on_flag=0;
		off=ktime_set(0,0);
		//starting the hrtimer 
    	hrtimer_start(&PWM_timer, off, HRTIMER_MODE_REL);
    	}
  return 0;
}




//########################
// Driver module init
//########################
static int __init RGBLed_init(void)
{
  printk(KERN_INFO "Initializing the RGBLed LKM\n");  
 
  // Request dynamic allocation of a device major number
  if (alloc_chrdev_region(&RGB_dev_number, 0, 1, DEVICE_NAME) < 0) {
      printk(KERN_DEBUG "Can't register device\n"); return -1;
  }

  // Populate sysfs entries 
  RGBLedClass = class_create(THIS_MODULE, DEVICE_NAME);

  
  // Connect the file operations with the cdev 
  cdev_init(&RGB_cdev, &fops);

  // Connect the major/minor number to the cdev 
  ret = cdev_add(&RGB_cdev, (RGB_dev_number), 1);

  if (ret) {
    printk("Bad cdev\n");
    return ret;
  }

  // Send uevents to udev, so it'll create /dev nodes
  RGBLedDevice = device_create(RGBLedClass, NULL, MKDEV(MAJOR(RGB_dev_number), 0), NULL, DEVICE_NAME);   


 	

  return 0;
}

//########################
// Driver module exit
//########################
static void __exit RGBLed_exit(void)
{
  /* Release the major number */
  unregister_chrdev_region((RGB_dev_number), 1);

  /* Destroy device */
  device_destroy (RGBLedClass, MKDEV(MAJOR(RGB_dev_number), 0));
  cdev_del(&RGB_cdev);  
  /* Destroy driver_class */
  class_destroy(RGBLedClass);

  printk("RGBLed driver removed.\n");
//delete the timer
hrtimer_cancel(&PWM_timer);
  
	}



module_init(RGBLed_init);
module_exit(RGBLed_exit);