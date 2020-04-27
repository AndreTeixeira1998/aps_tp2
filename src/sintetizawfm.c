#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <complex.h>
#include <tgmath.h>
#include <string.h>
#include "sintetizawfm.h"


float SintetizaWfm(Din din, double *time, double ts){
	static float omega = 0;
	static WfmType wavetype = UNDEFINED;
	static float r[N_HARM_FOURIER] = {0};
	static float beta[N_HARM_FOURIER] = {0};
	static uint8_t k[N_HARM_FOURIER] = {0}; 

	float ak[N_HARM_FOURIER], bk[N_HARM_FOURIER];
	float y = din.dc;

	omega = 2*M_PI*din.freq;
	wavetype = din.type;

	switch(wavetype){
		case CUSTOM:
			for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
				k[i] = din.F.hrm[i];
				r[i] = din.F.amp[i];
				beta[i] = din.F.pha[i];
			}
		break;

		case SQUARE:
			for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
				k[i] = i*2-1; 
				ak[i] = 0;
				bk[i] = 4/(M_PI*k[i]);
			}
		break;

		case TRIANGLE:
			for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
				k[i]=2*i-1;
        	    ak[i]=0;
        	    bk[i]=8/(pow(M_PI,2)*pow(i,2))*pow(-1, (i-1));	
			}
		break;

		case SAWTOOTH_LEFT:
		    for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
				k[i]=i;
            	ak[i]=0;
            	bk[i]=-2*pow(-1, i)/(M_PI*i);
			}
		break;

		case SAWTOOTH_RIGHT:
		    for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
				k[i]=i;
            	ak[i]=0;
            	bk[i]=2*pow(-1, i)/(M_PI*i);
			}
		break;

		case TRAPEZOIDAL:
			for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
				k[i]=i;
            	ak[i]=0;
            	bk[i]= -((256*pow(sin(M_PI*i/8),3))*(2*pow(sin(M_PI*i/8),4)-3*pow(sin(M_PI*i/8),2)+1))/(pow(M_PI,2)*pow(i,2));
			}
		break;

		default:
			fprintf(stderr, "Tipo de forma de onda inválido");
			exit(0);
	}

	if (wavetype != CUSTOM){
		for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
			r[i] = sqrt(pow(ak[i], 2) + pow(bk[i], 2));	
			beta[i] = cargf(ak[i] + I*bk[i]);
		}
	} 

	for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
		y += r[i] * cos(k[i]*omega*(*time)+beta[i]);
	}

	*time += ts;
	y = y*din.gain;

	return y;
}

Din CustomizaWfm(Din din, uint8_t *temphrm, float *tempamp, float *temppha, uint8_t vect_len){
	for(int i = 0; i < N_HARM_FOURIER; i++){
		if (i < vect_len){
			din.F.hrm[i] = temphrm[i];
			din.F.amp[i] = tempamp[i];
			din.F.pha[i] = temppha[i];
		}
		else {
			din.F.hrm[i] = 0;
			din.F.amp[i] = 0;
			din.F.pha[i] = 0;
		}
	}
	return din;
}

