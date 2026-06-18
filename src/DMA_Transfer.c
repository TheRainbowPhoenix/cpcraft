#include <sdk/os/lcd.h>
#define DMA0_SAR_0 (volatile unsigned*)0xFE008020
#define DMA0_DAR_0 (volatile unsigned*)0xFE008024
#define DMA0_TCR_0 (volatile unsigned*)0xFE008028
#define DMA0_CHCR_0 (volatile unsigned*)0xFE00802C
#define DMA0_DMAOR (volatile unsigned short*)0xFE008060
#define MSTPCR0 (volatile unsigned*)0xA4150030
#define SYNCO() __asm__ volatile("SYNCO\n\t":::"memory");
int DmaWaitNextC0() {
    while(1) { if((*DMA0_DMAOR)&4) break; if((*DMA0_CHCR_0)&2) break; }
    SYNCO(); *DMA0_CHCR_0 &= ~1; return 0;
}
void dmaStart(unsigned src_addr, unsigned dest_addr, unsigned size) {
    *MSTPCR0&=~(1<<21); *DMA0_CHCR_0&=~1; *DMA0_DMAOR=0;
    *DMA0_SAR_0=src_addr; *DMA0_DAR_0=dest_addr; *DMA0_TCR_0=(size/32);
    *DMA0_CHCR_0=0x00105400; *DMA0_DMAOR|=1; *DMA0_DMAOR&=~6; *DMA0_CHCR_0|=1;
}
int dmaIsBussy() { return ((*DMA0_CHCR_0) & 0x01); }
int dmaError() { return ((*DMA0_DMAOR) & 4) == 4; }
void dmaStartFill(unsigned src_addr, unsigned dest_addr, unsigned size) { dmaStart(src_addr, dest_addr, size); }
