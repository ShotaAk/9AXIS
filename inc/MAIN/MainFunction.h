/* ------------------------------------------------------------ *
File MainFunction.h

* ------------------------------------------------------------ */
#ifndef MAINFUNCTION_H
#define MAINFUNCTION_H

void startTimerInterruptMainFunction(void);
void stopTimerInterruptMainFunction(void);
void setSendDataEnable(uint8_t en);

void writeEEPROMOutputMode(uint8_t mode);
uint8_t readEEPROMOutputMode(void);
void setOutputMode(uint8_t mode);

//出力データの形式
#define OUTPUT_MODE_USB_ASCII_9AXIS (0)
#define OUTPUT_MODE_57600_BINARY_9AXIS (1)


#endif

/******************************************************************************
**                            End Of File
******************************************************************************/
