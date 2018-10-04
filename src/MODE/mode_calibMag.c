#include "mode_calibMag.h"
#include "type.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "SystemTickTimer.h"
#include "debug.h"
#include "mpu9250.h"

void mode_calibMag(void)
{
	turnGreenLED(0);
	myPrintfUSB("############  calib Mag mode ################\n");
	myPrintfUSB(" rotate the sensor various direction !! \n");
	stopTimerInterruptMainFunction();
	calibMagOffset();
}
