/**
 * @file   MainFunction.c
 * @brief  データの送信を行う<br>
 *         10msec毎に割り込み関数で処理を実行
 *
 * @author RTCorp. Ryota Takahashi
 */

#include "LPC13xx.h"
#include "core_cm3.h"
#include "type.h"

#include "timer.h"
#include "pwm.h"
#include "MainFunction.h"
#include "UserInterface.h"
#include "uart.h"
#include "SystemTickTimer.h"

#include "mpu9250.h"
#include "debug.h"
#include "uart.h"
#include "tinyMathFunction.h"
#include "usbTransmission.h"
#include "EEPROM_24LC16BT_I.h"

volatile static uint8_t flag_send_data_enable = 0;  //タイマ割り込み関数の中でデータを送信するかのフラグ
volatile static uint8_t output_mode = 1;            //データを出力する形式
volatile static uint32_t time;                      //起動してからの時間[msec]を格納するための変数

/**
 * MainFunction.c内の関数を使用できるようにする<br>
 * MainFunction.c内に記述されている割り込み関数
 * TIMER32_1_IRQHandler()を有効にする.
 *
 * @param void
 * @return void
 */
void startTimerInterruptMainFunction(void)
{
	startTimer32_1();
}

/**
 * MainFunction.c内の関数を使用できるようにする<br>
 * MainFunction.c内に記述されている割り込み関数
 * TIMER32_1_IRQHandler()を停止させる.
 *
 * @param void
 * @return void
 */
void stopTimerInterruptMainFunction(void)
{
	stopTimer32_1();
}

/**
 * 機体データの送信をするかどうかを選択
 *
 * @param en 1:送信 0:送信じ禁止
 * @return void
 */
void setSendDataEnable(uint8_t en)
{
	flag_send_data_enable = en;
}

/**
 * EEPROMに出力モードを書き込む<br>
 *
 * @param mode 書き込む動作モード
 */
void writeEEPROMOutputMode(uint8_t mode){
	write1byteEEPROM(1,0,mode);
}

/**
 * EEPROMから出力モードを読み出す<br>
 *
 * @return EEPROMから読み出したモードの値
 */
uint8_t readEEPROMOutputMode(void){
	return read1byteEEPROM(1,0);
}

/**
 * アウトプットモードをセットする.<br>
 * - OUTPUT_MODE_57600_BINARY_9AXIS USBとUARTから9軸センサのデータをbinary形式で出力
 * - OUTPUT_MODE_USB_ASCII_9AXIS USBから9軸センサのデータをascii文字列,カンマ区切りで出力
 *
 */
void setOutputMode(uint8_t mode){
	output_mode = mode;

	switch(mode){
		case OUTPUT_MODE_57600_BINARY_9AXIS:
			UARTInit(57600);
			writeEEPROMOutputMode(mode);
			break;
		case OUTPUT_MODE_USB_ASCII_9AXIS:
			writeEEPROMOutputMode(mode);
			break;
		default:
			output_mode = 0;
			writeEEPROMOutputMode(OUTPUT_MODE_USB_ASCII_9AXIS);
			break;
	}
};

/**
 * 10msec毎に行われる処理 <br>
 *          <li>各種情報をUART,USBで送信 </li>
 *         </ul>
 * @param  void
 * @return void
 */
void TIMER32_1_IRQHandler(void)
{
	GETDAT_T send_data;
	static uint8_t timestamp;
	float omega[3];
	float acc[3];
	float mag[3];
	float temp;
	//static uint32_t time;

	LPC_TMR32B1->IR=0x08; //clear interrupt flag


	//myPrintfUSB("time:%d\n",time);

	time_count_start();

	//送信データ格納用変数のヘッダ
	send_data.num  = 28;
	send_data.d[0] = 0xff;
	send_data.d[1] = 0xff;
	send_data.d[2] = 0x52;
	send_data.d[3] = 0x54;
	send_data.d[4] = 0x39;
	send_data.d[5] = 0x41;
	send_data.d[6] = 0x10;  // 2016　version
	send_data.d[7] = timestamp;

	if(timestamp == 0xff) timestamp = 0;
	else timestamp++;

	//9軸センサのデータの更新
	updateMPU9250Data();
	updateAK8963Data();

	//9軸センサの観測値を送信用配列に格納
	getDataFromMPU9250( (uint8_t *)(&send_data.d[8]) );
	getDataFromAK8963( (uint8_t *)(&send_data.d[22]) );

	//100Hzの周期でデータを送る
	getOmega(&omega[0]);
	getMag(&mag[0]);
	getAcc(&acc[0]);
	getTemp(&temp);


	if(flag_send_data_enable == 1 )
	{

		//UART経由でデータを送信
		switch(output_mode){
			case OUTPUT_MODE_57600_BINARY_9AXIS:
				LPC_UART->IER = IER_THRE | IER_RLS;			    // Disable RBR
				UARTSend(&send_data.d[0], send_data.num);
				LPC_UART->IER = IER_THRE | IER_RLS | IER_RBR;	// Re-enable RBR
				break;
			case OUTPUT_MODE_USB_ASCII_9AXIS:

				break;
		}

	}

	if(isUSBConfig() == 1 && flag_send_data_enable == 1 ) //USB接続確立時
	{
		switch(output_mode){
			case OUTPUT_MODE_57600_BINARY_9AXIS:
				//USB経由でデータを送信
				VCOM_SendData(send_data);
				break;
			case OUTPUT_MODE_USB_ASCII_9AXIS:

				myPrintfUSB("%d,%f,%f,%f,",timestamp,omega[0],omega[1],omega[2]);
				myPrintfUSB("%f,%f,%f,", acc[0],acc[1],acc[2]);
				myPrintfUSB("%f,%f,%f,%f\n",mag[0],mag[1],mag[2],temp);


				break;
		}
	}
	//time = time_count_end();

}

/******************************************************************************
**                            End Of File
******************************************************************************/
