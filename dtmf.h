#ifndef DTMF_H_
#define DTMF_H_

#include "goertzel.h"

void     DTMFAddSample(unsigned x);
void     DTMFReset();
unsigned DTMFPower();
void     DTMFDisplay();

#endif /* DTMF_H_ */
