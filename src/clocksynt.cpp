
#include <Arduino.h>
#include "board.h"
#include "Ticker.h"
#include "Cmd.h"
#include <Wire.h>


uint32_t fref = 25000000UL;                  // The reference frequency

void CalcRegisters(const uint32_t fout, uint8_t *regs)
{
    
    // Calc Output Multisynth Divider and R with e = 0 and f = 1 => msx_p2 = 0 and msx_p3 = 1
    uint32_t d = 4;
    uint32_t msx_p1 = 0;                         // If fout > 150 MHz then MSx_P1 = 0 and MSx_DIVBY4 = 0xC0, see datasheet 4.1.3
    int msx_divby4 = 0;
    int rx_div = 0;
    int r = 1;
 
    if (fout > 150000000UL)
        msx_divby4 = 0x0C;                       // MSx_DIVBY4[1:0] = 0b11, see datasheet 4.1.3
    else if (fout < 292969UL)                    // If fout < 500 kHz then use R divider, see datasheet 4.2.2. In reality this means > 292 968,75 Hz when d = 2048
    {
        int rd = 0;
        while ((r < 128) && (r * fout < 292969UL))
        {
            r <<= 1;
            rd++;
        }
        rx_div = rd << 4;
		
        d = 600000000UL / (r * fout);            // Use lowest VCO frequency but handle d minimum
        if (d % 2)                               // Make d even to reduce spurious and phase noise/jitter, see datasheet 4.1.2.1.
            d++;
 
        if (d * r * fout < 600000000UL)          // VCO frequency to low check and maintain an even d value
            d += 2;
    }	
    else                                         // 292968 Hz <= fout <= 150 MHz
    {
        d = 600000000UL / fout;                  // Use lowest VCO frequency but handle d minimum
        if (d < 6)
            d = 6;
        else if (d % 2)                          // Make d even to reduce phase noise/jitter, see datasheet 4.1.2.1.
           d++;
			
        if (d * fout < 600000000UL)              // VCO frequency to low check and maintain an even d value
            d += 2;
    }
    msx_p1 = 128 * d - 512;
 
    uint32_t fvco = (uint32_t) d * r * fout;
 
    // Calc Feedback Multisynth Divider
    double fmd = (double)fvco / fref;            // The FMD value has been found
    int a = fmd;                                 // a is the integer part of the FMD value
 
    double b_c = (double)fmd - a;                // Get b/c
    uint32_t c = 1048575UL;
    uint32_t b = (double)b_c * c;
    if (b > 0)
    {
        c = (double)b / b_c + 0.5;               // Improves frequency precision in some cases
        if (c > 1048575UL)
            c = 1048575UL;
    }
 
    uint32_t msnx_p1 = 128 * a + 128 * b / c - 512;   // See datasheet 3.2
    uint32_t msnx_p2 = 128 * b - c * (128 * b / c);
    uint32_t msnx_p3 = c;
 
    // Feedback Multisynth Divider register values
    regs[0] = (msnx_p3 >> 8) & 0xFF;
    regs[1] = msnx_p3 & 0xFF;
    regs[2] = (msnx_p1 >> 16) & 0x03;
    regs[3] = (msnx_p1 >> 8) & 0xFF;
    regs[4] = msnx_p1 & 0xFF;
    regs[5] = ((msnx_p3 >> 12) & 0xF0) + ((msnx_p2 >> 16) & 0x0F);
    regs[6] = (msnx_p2 >> 8) & 0xFF;
    regs[7] = msnx_p2 & 0xFF;
 
    // Output Multisynth Divider and R register values
    regs[8] = 0;                                  // (msx_p3 >> 8) & 0xFF
    regs[9] = 1;                                  // msx_p3 & 0xFF
    regs[10] = rx_div + msx_divby4 + ((msx_p1 >> 16) & 0x03);
    regs[11] = (msx_p1 >> 8) & 0xFF;
    regs[12] = msx_p1 & 0xFF;
    regs[13] = 0;                                 // ((msx_p3 >> 12) & 0xF0) + (msx_p2 >> 16) & 0x0F
    regs[14] = 0;                                 // (msx_p2 >> 8) & 0xFF
    regs[15] = 0;                                 // msx_p2 & 0xFF
 
    return;
}


uint8_t ReadRegister(uint8_t regAddr)
{
    int data = 0xFF;                  // Set value often not seen
 
    Wire.beginTransmission(0x60);     // The I2C address of the Si5351A
    Wire.write((uint8_t)regAddr);
    Wire.endTransmission();
    Wire.requestFrom(0x60, 1);
    if (Wire.available())
        data = Wire.read();
 
    return data;
}

void WriteRegister(uint8_t regAddr, uint8_t data)
{
    Wire.beginTransmission(0x60);     // The I2C address of the Si5351A
    Wire.write((uint8_t) regAddr);
    Wire.write((uint8_t) data);
    Wire.endTransmission();
}


void Initialize()
{
     
    // Initialize Si5351A
    while (ReadRegister(0) & 0x80);    // Wait for Si5351A to initialize
    WriteRegister(3, 0xFF);            // Output Enable Control, disable all
    	
    for (int i = 16; i < 24; i++)
        WriteRegister (i, 0x80);       // CLKi Control, power down CLKi
 
    WriteRegister(15, 0x00);           // PLL Input Source, select the XTAL input as the reference clock for PLLA and PLLB
    WriteRegister(24, 0x00);           // CLK3–0 Disable State, unused are low and never disable CLK0
    
    // Output Multisynth0, e = 0, f = 1, MS0_P2 and MSO_P3
    WriteRegister(42, 0x00);
    WriteRegister(43, 0x01);
    WriteRegister(47, 0x00);
    WriteRegister(48, 0x00);
    WriteRegister(49, 0x00);
 
    WriteRegister(16, 0x4F);           // Power up CLK0, PLLA, MS0 operates in integer mode, Output Clock 0 is not inverted, Select MultiSynth 0 as the source for CLK0 and 8 mA
 
    // Reference load configuration
    WriteRegister(183, 0x12);          // Set reference load C: 6 pF = 0x12, 8 pF = 0x92, 10 pF = 0xD2
 
    // Turn CLK0 output on
    WriteRegister(3, 0xFE);            // Output Enable Control. Active low
}

const uint32_t freq = 16000;    // The wanted output frequency

void set_frqu(uint32_t freq){
    uint8_t regs[16]; // Registers holding the FMD and OMD values
    CalcRegisters(freq, regs); 
       // Load PLLA Feedback Multisynth NA
    for (int i = 0; i < 8; i++)
        WriteRegister(26 + i, regs[i]);
 
    // Load Output Multisynth0 with d (e and f already set during init. and never changed)
    for (int i = 10; i < 13; i++)
        WriteRegister(34 + i, regs[i]);
 
    // Reset PLLA
    delayMicroseconds(500);            // Allow registers to settle before resetting the PLL
    WriteRegister(177, 0x20);

}

void setup_SI5351_clck_synt()
{                 
    Wire.begin(); 
    delayMicroseconds(500);            
 
    Initialize();
    set_frqu(freq);
 
}
 
void loop_clocksynt(){
 // int i =index("khz");
 // if(globals[i].updated){
 //     globals[i].updated = false;
 //     set_frqu((uint32_t) globals[i].value*1000);
 // }
}



