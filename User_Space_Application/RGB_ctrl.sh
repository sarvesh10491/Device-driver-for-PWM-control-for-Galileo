#!/bin/bash

flag=0

echo "============================================================="
echo "RGB Led control program"
echo "-------------------------"
echo "Select Duty cycle between [0 to 100]"
echo "Select 3 IO pins between [0 to 13]"
echo ""
echo "============================================================="
echo ""
read -p  'Enter duty cycle used for pattern : ' DUTY_CYCLE
read -p  'Enter IO pin used for RED Led : ' RED_IO_PIN
read -p  'Enter IO pin used for GREEN Led : ' GREEN_IO_PIN
read -p  'Enter IO pin used for BLUE Led : ' BLUE_IO_PIN
echo ""
echo "============================================================="
echo ""

#=============================================================
# Checks for any of the following invalid inputs from user
# 1] input value out of bounds
# 2] No input
# 3] Input not an integer
#
# If it finds invalid input in any case, sets flag to 1
#==============================================================

if ([ "$DUTY_CYCLE" -lt "0" ] || [ "$DUTY_CYCLE" -gt "100" ] || [ -z "$DUTY_CYCLE" ] || [ "`echo $DUTY_CYCLE |egrep ^[[:digit:]]+$`" = "" ]) 2> /dev/null
	then
	echo "Invalid Duty cycle input."
	export flag=1
fi

if ([ "$RED_IO_PIN" -lt 0 ] || [ "$RED_IO_PIN" -gt 13 ] || [ -z "$RED_IO_PIN" ] || [ "`echo $RED_IO_PIN |egrep ^[[:digit:]]+$`" = "" ]) 2> /dev/null
	then
	echo "Invalid IO pin input for RED Led."
	export flag=1
fi

if ([ "$GREEN_IO_PIN" -lt 0 ] || [ "$GREEN_IO_PIN" -gt 13 ] || [ -z "$GREEN_IO_PIN" ] || [ "`echo $GREEN_IO_PIN |egrep ^[[:digit:]]+$`" = "" ]) 2> /dev/null
	then
	echo "Invalid IO pin input for GREEN Led."
	export flag=1
fi

if ([ "$BLUE_IO_PIN" -lt 0 ] || [ "$BLUE_IO_PIN" -gt 13 ] || [ -z "$BLUE_IO_PIN" ] || [ "`echo $BLUE_IO_PIN |egrep ^[[:digit:]]+$`" = "" ]) 2> /dev/null
	then
	echo "Invalid IO pin input for BLUE Led."
	export flag=1
fi

echo ""
# flag == 1 indicates there was error in one or more inputs
# Execution will not start in that case
#===========================================================
if [ $flag -eq "1" ]
	then
	echo "Terminationg execution due to one or more invalid input(s)."
else
	echo "Initiating program execution."
	echo ""
	./RGBLed $DUTY_CYCLE $RED_IO_PIN $GREEN_IO_PIN $BLUE_IO_PIN
fi
echo ""
