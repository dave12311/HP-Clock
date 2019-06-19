/*
 * bitop.h
 *
 * Created: 6/17/2019 8:03:05 PM
 *  Author: horva
 */ 


#ifndef BITOP_H_
#define BITOP_H_

#define clearBit(A,B) (A &= ~(1<<B))
#define setBit(A,B) (A |= (1<<B))
#define changeBit(A,B,C) (((C & 1) == 1) ? setBit(A,B) : clearBit(A,B))

#endif /* BITOP_H_ */