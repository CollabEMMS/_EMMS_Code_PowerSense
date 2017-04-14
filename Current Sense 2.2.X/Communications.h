/* 
 * File:   Communications.h
 * Author: austin
 *
 * Created on February 21, 2016, 8:37 AM
 */

/*******************************
 ASSUMPTION
    the 'master' will never queue up messages to send to various modules. It will only react to what modules send to it.
    this will make it that we do not need to keep separate buffers for each possible port
 */

#include "stdbool.h"


#ifndef COMMUNICATIONS_H
#define	COMMUNICATIONS_H
#endif

void communications(bool);
void SPISlaveInit(void);
