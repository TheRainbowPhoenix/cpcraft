#include <fxcg/display.h>

// DMA Base Addresses
#define DMA0_SAR_0  (volatile unsigned*)0xFE008020
#define DMA0_DAR_0  (volatile unsigned*)0xFE008024
#define DMA0_TCR_0  (volatile unsigned*)0xFE008028
#define DMA0_CHCR_0 (volatile unsigned*)0xFE00802C
#define DMA0_DMAOR  (volatile unsigned short*)0xFE008060

// Module Stop Register 0
#define MSTPCR0     (volatile unsigned*)0xA4150030

// LCD-related definitions
#define LCD_GRAM    0x202
#define LCD_BASE    0xB4000000

// Sync operation
#define SYNCO() __asm__ volatile("SYNCO\n\t":::"memory");

// DMA Modes
#define DMA_MODE_1_BYTE 0
#define DMA_MODE_2_BYTE 1
#define DMA_MODE_4_BYTE 2

int DmaWaitNextC0() {
    while(1){
        if((*DMA0_DMAOR)&4)//Address error has occurred stop looping
            break;
        if((*DMA0_CHCR_0)&2)//Transfer is done
            break;
    }
    SYNCO();
    *DMA0_CHCR_0 &= ~1; // Disable DMA on channel 0
    //*DMA0_DMAOR = 0; // Disable all DMA
}

void dmaStart(unsigned src_addr, unsigned dest_addr, unsigned size)
{
    //Bdisp_WriteDDRegister3_bit7(1);
    //Bdisp_DefineDMARange(6,389,y1,y2);
    //Bdisp_DDRegisterSelect(LCD_GRAM);

    //*MSTPCR0 = 0xFD7FF7F6;
    *MSTPCR0&=~(1<<21);//Clear bit 21
    *DMA0_CHCR_0&=~1;//Disable DMA on channel 0
    *DMA0_DMAOR=0;//Disable all DMA
    *DMA0_SAR_0=(((int)src_addr));//Source address is VRAM
    *DMA0_DAR_0=(((int)dest_addr));//Destination is LCD
    *DMA0_TCR_0=(int)(size/32);//Transfer count bytes/32
    //*DMA0_CHCR_0=0x00105400; //0x00101400
    *DMA0_CHCR_0=0x00105400; //0x00101400
    *DMA0_DMAOR|=1;//Enable DMA on all channels
    *DMA0_DMAOR&=~6;//Clear flags
    *DMA0_CHCR_0|=1;//Enable channel0 DMA
}

void dmaStartFill_old(unsigned src_addr, unsigned dest_addr, unsigned size)
{
    //*MSTPCR0 = 0xFD7FF7F6;
    *MSTPCR0&=~(1<<21);//Clear bit 21
    *DMA0_CHCR_0&=~1;//Disable DMA on channel 0
    *DMA0_DMAOR=0;//Disable all DMA
    *DMA0_SAR_0=(((int)src_addr));//Source address is VRAM
    *DMA0_DAR_0=(((int)dest_addr));//Destination is LCD
    *DMA0_TCR_0=(int)(size/32);//Transfer count bytes/32
    *DMA0_CHCR_0=0b00000000000100000100010000100000;//0x00104400
    *DMA0_DMAOR|=1;//Enable DMA on all channels
    *DMA0_DMAOR&=~6;//Clear flags
    *DMA0_CHCR_0|=1;//Enable channel0 DMA
}

void dmaStartFill_old2B(unsigned src_addr, unsigned dest_addr, unsigned size)
{
    //*MSTPCR0 = 0xFD7FF7F6;
    *MSTPCR0&=~(1<<21);//Clear bit 21
    *DMA0_CHCR_0&=~1;//Disable DMA on channel 0
    *DMA0_DMAOR=0;//Disable all DMA
    *DMA0_SAR_0=(((int)src_addr));//Source address is VRAM
    *DMA0_DAR_0=(((int)dest_addr));//Destination is LCD
    *DMA0_TCR_0=(int)(size/2);//Transfer count bytes/32
    *DMA0_CHCR_0=0x00004408; //0x00104400
    *DMA0_DMAOR|=1;//Enable DMA on all channels
    *DMA0_DMAOR&=~6;//Clear flags
    *DMA0_CHCR_0|=1;//Enable channel0 DMA
}

int dmaIsBussy()
{
    return ((*DMA0_CHCR_0) & 0b01);
}

int dmaError()
{
    return ((*DMA0_DMAOR)&4 == 4);
}