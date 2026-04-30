int DmaWaitNextC0();
void dmaStart(unsigned src_addr, unsigned dest_addr, unsigned size);
void dmaStartFill(unsigned src_addr, unsigned dest_addr, unsigned size);
void dmaStartFill2B(unsigned src_addr, unsigned dest_addr, unsigned size);
int dmaIsBussy();
int dmaError();