#define YRAM (volatile unsigned*)0xE5017000 

void DoDMAlcdNonblock();
void DmaWaitNext();

void DoDMAlcdNonblockStripXram(unsigned y1,unsigned y2);
void DoDMAlcdNonblockStripXramStart(unsigned y1,unsigned y2);