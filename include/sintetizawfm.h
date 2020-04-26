#ifndef SINTESE_H
#define SINTESE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <complex.h>
#include <tgmath.h>
#include <string.h>

#define N_HARM_FOURIER 20


typedef enum {
	CUSTOM = 0,
	SQUARE = 1,
	TRIANGLE = 2,
	SAWTOOTH_LEFT = 3,
	SAWTOOTH_RIGHT = 4,
	TRAPEZOIDAL = 5,
	UNDEFINED = 99
} WfmType;

typedef struct {
	uint8_t hrm[N_HARM_FOURIER];
	float amp[N_HARM_FOURIER];
	float pha[N_HARM_FOURIER];
} WfmParam; //epah é o q ele chama Din.F dunno wtf é isso

typedef struct {
	uint8_t flag;
	uint8_t Nh;
	WfmType type;
	WfmParam F; //nome lmao do prof :/
	uint64_t ts;
	float freq;
	float dc;
	float gain;
} Din;


float SintetizaWfm(Din din);

#endif