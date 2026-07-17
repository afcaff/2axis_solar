/*
 * File:   oled.c
 * Author: Adrian Franco
 */

#include <xc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "i2c.h"
#include "SSD1306.h"


#define OLED_SLAVE 0x78


void Draw_Arrow(unsigned char row, unsigned char col, unsigned char pointing_right);
void Draw_Divider(unsigned char row);
// 'row' is defined in 'ssd1306.h' line 362.




// Draws a simple > or < arrow character
void Draw_Arrow(unsigned char row, unsigned char col, unsigned char pointing_right) {
    if (pointing_right)
        SSD1306_Chr8(row, col, '>', 0);
    else
        SSD1306_Chr8(row, col, '<', 0);
}

/* Draws a dashed divider line across a row */
void Draw_Divider(unsigned char row) {
    SSD1306_Out8(row, 0, "--------------------", 0);
}

void OLED_Init()
{
    i2c_Init();    
    SSD1306_Init();
    SSD1306_Fill(0);
}

void OLED_Splash(){
    // -| splash screen |-
    SSD1306_Out16(0, 1, "*SOLAR TRACKER*", 0);   // Big title
    SSD1306_Out8(2, 2, ">> TEAM 13 <<", 0);
    SSD1306_Out8(3, 0, "-----------------", 1);
    SSD1306_Out8(4, 2, "Terry  Tyler", 1);
    SSD1306_Out8(5, 2, "Rogelio", 1);
    SSD1306_Out8(6, 2, "Garrett", 1);
    SSD1306_Out8(7, 2, "Adrian", 1);
    __delay_ms(2000);
    SSD1306_Fill(0);
    
    // display static items first
    SSD1306_Out8(0, 0, "[  SOLAR TRACKER]", 1);
    Draw_Divider(3);
    Draw_Divider(6);
    Draw_Arrow(4, 0, 0);
    Draw_Arrow(4, 13, 1);
    Draw_Arrow(5, 0, 0);
    Draw_Arrow(5, 14, 1);
    __delay_ms(1000);
}
/*  **************************************** */
void OLED_Update(uint8_t pan, uint8_t tilt) {
    char Sout[20];
    static unsigned char blink  = 0;    // Blink counter for status dot - static to make persistent

    
//    
//    //splash logo i think is fun not necessary but fun to have 
//    SSD1306_Out8(0, 0, "  |  |    |  |  ", 0);
//    SSD1306_Out8(1, 0, "  |  |    |  |  ", 0);
//    SSD1306_Out8(2, 0, " _|__|____|__|_ ", 0);
//    SSD1306_Out8(3, 0, "|              |", 0);
//    SSD1306_Out8(4, 0, "|   POWER  ON  |", 0);
//    SSD1306_Out8(5, 0, "|______________|", 0);
//    SSD1306_Out8(6, 0, "  ||||    ||||  ", 0);
//    SSD1306_Out8(7, 0, "                ", 0);
//    __delay_ms(1000);
//    SSD1306_Fill(0);
//    
    
        // blinking star(*) at row 0 col 18
        if (blink < 1)
            SSD1306_Chr8(0, 18, '*', 1);
        else
            SSD1306_Chr8(0, 18, ' ', 1);

        // pan printed at row 4. uses rogelio pan value
        sprintf(Sout, "PAN: %3d deg", pan);
        SSD1306_Out8(4, 1, Sout, 0);
    
        // tilt printed at row 5.. uses rogelio tilt value
        sprintf(Sout, "TILT: %3d deg", tilt);
        SSD1306_Out8(5, 1, Sout, 0);

        blink++;
        if (blink >= 2) blink = 0;
    }