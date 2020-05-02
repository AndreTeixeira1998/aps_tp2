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
	
	static float r_square[N_HARM_FOURIER] = {0};
	static float beta_square[N_HARM_FOURIER] = {0};
	static uint8_t k_square[N_HARM_FOURIER] = {0}; 
	
	static float r_tri[N_HARM_FOURIER] = {0};
	static float beta_tri[N_HARM_FOURIER] = {0};
	static uint8_t k_tri[N_HARM_FOURIER] = {0}; 
	
	static float r_saw_l[N_HARM_FOURIER] = {0};
	static float beta_saw_l[N_HARM_FOURIER] = {0};
	static uint8_t k_saw_l[N_HARM_FOURIER] = {0}; 
	
	static float r_saw_r[N_HARM_FOURIER] = {0};
	static float beta_saw_r[N_HARM_FOURIER] = {0};
	static uint8_t k_saw_r[N_HARM_FOURIER] = {0}; 
	
	static float r_trap[N_HARM_FOURIER] = {0};
	static float beta_trap[N_HARM_FOURIER] = {0};
	static uint8_t k_trap[N_HARM_FOURIER] = {0}; 
	
	
	omega = 2*M_PI*din.freq;
	wavetype = din.type;

	float y = 0;

	if (din.flag == 1){
		float ak[N_HARM_FOURIER], bk[N_HARM_FOURIER];

		//	Square wave harmonic calculation
		for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
			k_square[i] = i*2-1; 
			ak[i] = 0;
			bk[i] = 4/(M_PI*k_square[i]);
		}
		for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
			r_square[i] = sqrt(pow(ak[i], 2) + pow(bk[i], 2));	
			beta_square[i] = cargf(ak[i] + I*bk[i]);
		}

		//	Triangular wave harmonic calculation
		for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
			k_tri[i]=2*i-1;
           	ak[i]=0;
           	bk[i]=8/(pow(M_PI,2)*pow(i,2))*pow(-1, (i-1));	
		}
		for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
			r_tri[i] = sqrt(pow(ak[i], 2) + pow(bk[i], 2));	
			beta_tri[i] = cargf(ak[i] + I*bk[i]);
		}

		//	Sawtooth left wave harmonic calculation
		for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
			k_saw_l[i]=i;
	       		ak[i]=0;
        		bk[i]=-2*pow(-1, i)/(M_PI*i);
		}
		for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
			r_saw_l[i] = sqrt(pow(ak[i], 2) + pow(bk[i], 2));	
			beta_saw_l[i] = cargf(ak[i] + I*bk[i]);
		}

		//	Sawtooth right wave harmonic calculation
		for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
			k_saw_r[i]=i;
       		ak[i]=0;
       		bk[i]=2*pow(-1, i)/(M_PI*i);
		}
		for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
			r_saw_r[i] = sqrt(pow(ak[i], 2) + pow(bk[i], 2));	
			beta_saw_r[i] = cargf(ak[i] + I*bk[i]);
		}


		//	Trapzoidal wave harmonic calculation
		for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
			k_trap[i]=i;
       			ak[i]=0;
       			bk[i]= -((256*pow(sin(M_PI*i/8),3))*(2*pow(sin(M_PI*i/8),4)-3*pow(sin(M_PI*i/8),2)+1))/(pow(M_PI,2)*pow(i,2));
		}
		for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
			r_trap[i] = sqrt(pow(ak[i], 2) + pow(bk[i], 2));	
			beta_trap[i] = cargf(ak[i] + I*bk[i]);
		}
		return 0;
	}
	else if(din.flag==0){
		switch(wavetype){
			case CUSTOM:
				for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
					y += din.F.amp[i] * cos(din.F.hrm[i]*omega*(*time)+din.F.pha[i]);
				}	
				break;
			case SQUARE:
				for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
					y += r_square[i] * cos(k_square[i]*omega*(*time)+beta_square[i]);
				}	
				break;
			case TRIANGLE:
				for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
					y += r_tri[i] * cos(k_tri[i]*omega*(*time)+beta_tri[i]);
				}	
				break;
			case SAWTOOTH_LEFT:
				for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
					y += r_saw_l[i] * cos(k_saw_l[i]*omega*(*time)+beta_saw_l[i]);
				}	
				break;
			case SAWTOOTH_RIGHT:
				for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
					y += r_saw_r[i] * cos(k_saw_r[i]*omega*(*time)+beta_saw_r[i]);
				}	
				break;
			case TRAPEZOIDAL:
				for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
					y += r_trap[i] * cos(k_trap[i]*omega*(*time)+beta_trap[i]);
				}	
				break;
			default:
				fprintf(stderr, "Tipo de forma de onda inválido");
				exit(0);
		}
		*time += ts;
		y *= din.gain;
		y += din.dc;
	
		return y;
	}
	else {
		printf("Flag não especificada! Operacao cancelada\n");
		exit(0);
	}
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

