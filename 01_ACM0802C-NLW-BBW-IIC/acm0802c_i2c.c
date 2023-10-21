/*
 * acm0802c_i2c.c
 *
 *  Created on: Oct 21, 2023
 *      Author: AUDIY
 */

#include "acm0802c_i2c.h"
#include "stm32f4xx_hal.h"
#include <string.h>


/* Device Address Search */
uint16_t CheckACMDeviceAddress(I2C_HandleTypeDef *hi2c) {
    int i = 0;
    HAL_StatusTypeDef s;
    uint16_t AD[4] = {ACM0802C_AD0, ACM0802C_AD1, ACM0802C_AD2, ACM0802C_AD3}; // 0x78, 0x7A, 0x7C, 0x7E

    for (i = 0; i < 4; i++) {
        s = HAL_I2C_IsDeviceReady(hi2c, AD[i], 10, 1000);
        
        if (s == HAL_OK) {
            // if HAL_OK is returned, break this loop. 
            break;
        }
    }

    // Return the Address which meets s == HAL_OK.
    return (s == HAL_OK) ? AD[i] : 0x00;
}

/* Write Instruction */
uint8_t ACMWriteInstruction(I2C_HandleTypeDef *hi2c, uint16_t AD, uint8_t data) {
    uint8_t buf[2] = {INST_C0_0, data}; // 0x00, data
    uint8_t status = HAL_I2C_Master_Transmit(hi2c, AD, buf, 2, 1000);

    return status == HAL_OK;
}

/* Initialize ACM0802C-NLW-BBW-I2C */
void InitACM(I2C_HandleTypeDef *hi2c, uint16_t AD) {
    /* Wait for more than 40 ms after VDD rises to 4.5V */
    HAL_Delay(50);

    ACMWriteInstruction(hi2c, AD, FUNC_SET_8_2_8); // Function Set: 8-bit/2-line/5x8
    HAL_Delay(1); // wait for more than 100 us

    ACMWriteInstruction(hi2c, AD, DISP_DCB_OFF);   // Display ON/OFF:  B/C/D = OFF/OFF/OFF
    HAL_Delay(1); // wait for more than 100 us

    ACMWriteInstruction(hi2c, AD, CLR_DISP);       // Clear Display.
    HAL_Delay(20); // wait for more than 10 ms

    ACMWriteInstruction(hi2c, AD, ENTRY_CURSOR_RIGHT); // Entry Mode Set: Cursor/blink moves to right and DDRAM address is increased by 1.
    HAL_Delay(1); // wait for more than 100 us
}

/* Print Single Character to the ACM0802C-NLW-BLW-I2C. */
uint8_t ACMPrintChar(I2C_HandleTypeDef *hi2c, uint16_t AD, const char *cdata) {
    uint8_t buf[2] = {WRITE_C0_0, *cdata}; // 0x40, *cdata
    uint8_t status = HAL_I2C_Master_Transmit(hi2c, AD, buf, 2, 1000);
    HAL_Delay(10);

    return status == HAL_OK;
}

/* Print 1-Line String (under 40 characters) to the ACM0802C-NLW-BLW-I2C. */
void ACMPrintStr(I2C_HandleTypeDef *hi2c, uint16_t AD, uint8_t LN, const char *str) {
    uint8_t LNx = DDRAM_LN1;
    uint8_t str_len = 0;
    uint8_t i = 0;
    const char *startstr = str;

    if (LN == 1) {
        LNx = DDRAM_LN1; // DDRAM Address: 0x80
    } else {
        LNx = DDRAM_LN2; // DDRAM Address: 0xC0
    }

    str_len = strlen(str); // Count String Length.

    if (str_len > 40) {
        str_len = 40; // String Length must be under 40 per Line.
    }

    ACMWriteInstruction(hi2c, AD, ENTRY_CURSOR_RIGHT); // Cursor/blink moves to right and DDRAM address is increased by 1.
    ACMWriteInstruction(hi2c, AD, LNx); // Specify the DDRAM Address.

    /* Print Characters in the string */
    for (i = 0; i < str_len; i++) {
        ACMPrintChar(hi2c, AD, str); // Write the character the DDRAM.
        str++;
    }

    str = startstr; // Return *str pointer to start address.
}
