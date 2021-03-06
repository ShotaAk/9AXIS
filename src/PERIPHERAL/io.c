/* ------------------------------------------------------------ *
File io.c

IOポートの設定
* ------------------------------------------------------------ */
#include "io.h"
#include "LPC13xx.h"
#include "core_cm3.h"
#include "type.h"

/* ------------------------------------------------------------ *
  Input  void
  Output void

  IOポートの設定.
  SW1,LED1に繋がっているIOピンを設定
* ------------------------------------------------------------ */
void initIO (void)
{

	//SW1
	LPC_IOCON->PIO0_1 = 0x0010;   //IOConfig PIO0_1
	LPC_GPIO0->DIR &= 0xfffd;     //Data Direction(1 output , 0 input)   [1bit] SW1

	//LED0
	LPC_IOCON->R_PIO1_1 = 0x0001;   //IOConfig PIO1_1
	LPC_GPIO1->DIR |= 0x0002;     //Data Direction(1 output , 0 input)   [1bit] LED0

	//LED1
	LPC_IOCON->R_PIO1_2 = 0x0001;   //IOConfig PIO1_2
	LPC_GPIO1->DIR |= 0x0004;     //Data Direction(1 output , 0 input)   [2bit] LED1



};
/******************************************************************************
**                            End Of File
******************************************************************************/
