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
} WfmParam;

typedef struct {
	uint8_t flag;
	uint8_t Nh;
	WfmType type;
	WfmParam F; 
	uint64_t ts;
	float freq;
	float dc;
	float gain;
} Din;

typedef struct{
	Din din;
	uint32_t n_pontos; 
	float duration; 
} Sig; 

Din CustomizaWfm(Din din, uint8_t *temphrm, float *tempamp, float *temppha, uint8_t vect_len);
float SintetizaWfm(Din din, double *time, double ts);

#endif