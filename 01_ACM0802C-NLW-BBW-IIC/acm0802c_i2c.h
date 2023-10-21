/*
 * acm0802c_i2c.h
 *
 *  Created on: Oct 21, 2023
 *      Author: AUDIY
 */

#ifndef INC_ACM0802C_I2C_H_
#define INC_ACM0802C_I2C_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"


/* Begin Constant Definition */
/*-----------------------------------------------------------------------------
* ACM0802C-NLW-BBW-I2C Device Address
*
* Device Address (bin): 0b01111[SA1][SA0]0
* Device Address (hex): 0x7[8|A|C|E]
* 
* SA1/SA0 depends on the connection.
* When No.4/No.5 pin is pulled-up by Vdd: SA1(No.4)/SA0(No.5) is 0b1.
* When No.4/No.5 pin is pulled_down     : SA1(No.4)/SA0(No.5) is 0b0.
-----------------------------------------------------------------------------*/
#define ACM0802C_AD0 0x78 // No.4(SA1) = pulled-down, No.5(SA0) = pulled-down: 0b01111000
#define ACM0802C_AD1 0x7A // No.4(SA1) = pulled-down, No.5(SA0) = pulled-up  : 0b01111010
#define ACM0802C_AD2 0x7C // No.4(SA1) = pulled-up  , No.5(SA0) = pulled-down: 0b01111100
#define ACM0802C_AD3 0x7E // No.4(SA1) = pulled-up  , No.5(SA0) = pulled-up  : 0b01111110


/*-----------------------------------------------------------------------------
* ACM0802C-NLW-BBW-I2C Register Address
*
* 1. Instruction
* Instruction Address (bin): 0b[C0]0000000
* Instruction Address (hex): 0x[0|8]0
*
* 2. Write characters
* Write Operation Address (bin): 0b[C0]1000000
* Write Operation Address (hex): 0x[4|C]0
*
* C0 bit Setting.
* C0 = 0: Last control byte to be sent. Only a stream of data bytes is allowed to follow.
*         This stream may only be terminated by s STOP or RE-START condition.
* C0 = 1: Another control byte will follow the data byte unless a STOP or RE-START condition is received.
-----------------------------------------------------------------------------*/
#define INST_C0_0  0x00 // 0b00000000
#define INST_C0_1  0x80 // 0b10000000
#define WRITE_C0_0 0x40 // 0b01000000
#define WRITE_C0_1 0xC0 // 0b11000000


/*-----------------------------------------------------------------------------
* ACM0802C-NLW-BLW-I2C Instruction Sets
*
* ACM0802C has following instructions
*  1. Read Display Data
*  2. Write Display Data
*  3. Clear Display
*  4. Return Home
*  5. Entry Mode Set
*  6. Display ON/OFF
*  7. Cursor or Display Shift
*  8. Function Set
*  9. Set CGRAM Address
* 10. Set DDRAM Address
* 11. Read Busy Flag & Address
-----------------------------------------------------------------------------*/
/* 1. Read Display Data */
// Undefined

/* 2. Write Display Data */
// Undefined

/* 3. Clear Display */
#define CLR_DISP 0x01 // 0b00000001

/* 4. Return Home */
#define RTN_HOME 0x20 // 0b00000010

/* 5. Entry Mode Set
*  Data (bin): 0b000001[I/D][S]
*  Data (hex): 0x0[4|5|6|7]
*
*  I/D: Increment/Decrement Bit
*  0b0: cursor/blink moves to left and DDRAM address is decreased by 1.
*  0b1: cursor/blink moves to right and DDRAM address is increased by 1.
*
*  S: Shift of Entire Display
*  0b0: Shift of entire display is not performed.
*  0b1: Shift of entire display is performed according to I/D value (I/D = "1": shift left, I/D = "0": shift right).
*/
#define ENTRY_CURSOR_LEFT  0x04 // Cursor/blink moves to left and DDRAM address is decreased by 1 : 0b00000100
#define ENTRY_DISP_RIGHT   0x05 // Right shift of entire display                                  : 0b00000101
#define ENTRY_CURSOR_RIGHT 0x06 // Cursor/blink moves to right and DDRAM address is increased by 1: 0b00000110
#define ENTRY_DISP_LEFT    0x07 // Left shift of entire display                                   : 0b00000111

/* 6. Display ON/OFF
*  Data (bin): 0b00001[D][C][B]
*  Data (hex): 0b0[8-F]
* 
*  D:  Display ON/OFF control bit.
*  0b0: display is turned off, but display data is remained in DDRAM.
*  0b1: entire display is turned on.
*
*  C: Cursor ON/OFF control bit.
*  0b0: cursor is disappeared in current display, but I/D register remains its data.
*  0b1: cursor is turned on.
*
*  B: Cursor Blink ON/OFF control bit.
*  0b0: blink is off.
*  0b1: cursor blink is on, that performs alternate between all the high data and display character at the cursor position.
*       If fosc has 540 kHz frequency, blinking has 185 ms interval. 
*/
#define DISP_DCB_OFF 0x08 // D/C/B = OFF/OFF/OFF: 0b00001000
#define DISP_B_ON    0x09 // D/C/B = OFF/OFF/ON : 0b00001001
#define DISP_C_ON    0x0A // D/C/B = OFF/ON/OFF : 0b00001010
#define DISP_CB_ON   0x0B // D/C/B = OFF/ON/ON  : 0b00001011
#define DISP_D_ON    0x0C // D/C/B = ON/OFF/OFF : 0b00001100
#define DISP_DB_ON   0x0D // D/C/B = ON/OFF/ON  : 0b00001101
#define DISP_DC_ON   0x0E // D/C/B = ON/ON/OFF  : 0b00001110
#define DISP_DCB_ON  0x0E // D/C/B = ON/ON/ON   : 0b00001111

/* 7. Cursor or Display Shift
*  Data (bin): 0b0001[S/C][R/L]00
*  Data (hex): 0x1[0|4|8|C]
*
*  S/C: Cursor or Display Shift.
*  0b0: Cursor shift.
*  0b1: Display shift.
*
*  R/L: Right/Left Shift.
*  0b0: Shift to Left.
*  0b1: Shift to Right.
*/
#define CURSOR_SHIFT_LEFT  0x10 // S/C = Cursor , R/L = Left : 0b00010000
#define CURSOR_SHIFT_RIGHT 0x14 // S/C = Cursor , R/L = Right: 0b00010100
#define DISP_SHIFT_LEFT    0x18 // S/C = Display, R/L = Left : 0b00011000
#define DISP_SHIFT_RIGHT   0x1C // S/C = Display, R/L = Right: 0b00011100

/* 8. Function Set
*  Data (bin): 0b001[DL][N][F]00
*  Data (hex): 0x[2|3][4|8|C]
*
*  DL: Interface data length control bit.
*  0b0: 4-bit interface.
*  0b1: 8-bit/I2C/SPI interface.
*
*  N: Display line number control bit.
*  0b0: 1-line display mode.
*  0b1: 2-line display mode.
*
*  F: Display font type control bit.
*  0b0: 5x8  dot format.
*  0b1: 5x11 dot format.
*/
#define FUNC_SET_4_1_8  0x20 // DL/N/F = 4-bit/1-line/5x8 : 0b00100000
#define FUNC_SET_4_1_11 0x24 // DL/N/F = 4-bit/1-line/5x11: 0b00100100
#define FUNC_SET_4_2_8  0x28 // DL/N/F = 4-bit/2-line/5x8 : 0b00101000
#define FUNC_SET_4_2_11 0x2C // DL/N/F = 4-bit/2-line/5x11: 0b00101100
#define FUNC_SET_8_1_8  0x30 // DL/N/F = 8-bit/1-line/5x8 : 0b00110000
#define FUNC_SET_8_1_11 0x34 // DL/N/F = 8-bit/1-line/5x11: 0b00110100
#define FUNC_SET_8_2_8  0x38 // DL/N/F = 8-bit/2-line/5x8 : 0b00111000
#define FUNC_SET_8_2_11 0x3C // DL/N/F = 8-bit/2-line/5x11: 0b00111100

/* 9. Set CGRAM Address */
// Undefined

/* 10. Set DDRAM Address 
*  Data (bin): 0b1[AC6][AC5][AC4][AC3][AC2][AC1][AC0]
*  Data (hex): 0x[80 - A7|C0 - E7]
*
*  How to define address.
*  Address = DDRAM_LNx | [00 - 27]
*  Ex. 0x00 = DDRAM_LN1 | 0x00
*      0x27 = DDRAM_LN1 | 0x27
*      0x42 = DDRAM_LN2 | 0x02
*      0x67 = DDRAM_LN2 | 0x27
*
*  (1). Display Position
*  |00|01|02|03|04|05|06|07|
*  |40|41|42|43|44|45|46|47|
*
*  (2). For Shift Left
*  |01|02|03|04|05|06|07|08|
*  |41|42|43|44|45|46|47|48|
*
*  (3). For Shift Right
*  |27|00|01|02|03|04|05|06|
*  |67|40|41|42|43|44|45|46|
*/
#define DDRAM_LN1 0x80 // 0b10[AC5][AC4][AC3][AC2][AC1][AC0]
#define DDRAM_LN2 0xC0 // 0b11[AC5][AC4][AC3][AC2][AC1][AC0]


/* 11. Read Busy Flag & Address */
// Undefined

/*End Constant Definition*/


/* Begin Prototype Declaration*/
/*-----------------------------------------------------------------------------
* CheckACMDeviceAddress
* Search & Return the Active I2C Device Address.
-----------------------------------------------------------------------------*/
uint16_t CheckACMDeviceAddress(I2C_HandleTypeDef *hi2c);

/*-----------------------------------------------------------------------------
* ACMWriteInstruction
* Transmit an instruction set to the ACM0802C-NLW-BLW-I2C.
-----------------------------------------------------------------------------*/
uint8_t ACMWriteInstruction(I2C_HandleTypeDef *hi2c, uint16_t AD, uint8_t data);

/*-----------------------------------------------------------------------------
* InitACM
* Initialize ACM0802C-NLW-BBW-I2C.
-----------------------------------------------------------------------------*/
void InitACM(I2C_HandleTypeDef *hi2c, uint16_t AD);

/*-----------------------------------------------------------------------------
* ACMPrintChar
* Print Single Character to the ACM0802C-NLW-BLW-I2C.
-----------------------------------------------------------------------------*/
uint8_t ACMPrintChar(I2C_HandleTypeDef *hi2c, uint16_t AD, const char *data);

/*-----------------------------------------------------------------------------
* ACMPrintStr
* Print 1-Line String (under 40 characters) to the ACM0802C-NLW-BLW-I2C.
-----------------------------------------------------------------------------*/
void ACMPrintStr(I2C_HandleTypeDef *hi2c, uint16_t AD, uint8_t LN, const char *str);

/* End Prototype Declaration */

#endif /* INC_ACM0802C_I2C_H_ */
