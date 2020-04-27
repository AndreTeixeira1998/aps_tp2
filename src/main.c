#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <complex.h>
#include <tgmath.h>
#include <string.h>
#include "sintetizawfm.h"

#define N_PONTOS 2e3
#define TIMESTEP 40e-3

static double time = 0; 

float SintetizaWfm(Din din){
	/*
		Ainda falta flags e verificaçoes
		also tempo e n_pontos ainda ta hardcoded
	*/

	static float omega = 0;
	static uint64_t ts = 0;
	static uint8_t nh = 0;
	static WfmType wavetype = UNDEFINED;
	static float r[N_HARM_FOURIER] = {0};
	static float beta[N_HARM_FOURIER] = {0};
	static uint8_t k[N_HARM_FOURIER] = {0}; 

	float ak[N_HARM_FOURIER], bk[N_HARM_FOURIER];
	float y = din.dc;

	if (omega == 0){
		omega = 2*M_PI*din.freq;
	}
	else{
		while (omega > 2*M_PI){
			omega -= 2*M_PI;
		}
	}

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
			puts("TRAPEZOIDAL nao implementado :(");
		break;

		default:
			fprintf(stderr, "Tipo de forma de onda inválido");
			return -1;
	}

	if (wavetype != CUSTOM){
		for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
			r[i] = sqrt(pow(ak[i], 2) + pow(bk[i], 2));	
			beta[i] = cargf(ak[i] + I*bk[i]);
		}
	} 

	for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
		y += r[i] * cos(k[i]*omega*time+beta[i]);
	}

	time += TIMESTEP;
	y = y*din.gain;

	return y;
}

int main(int argc, char **argv){
	FILE *data_in, *data_out;
	if (argc != 3){
		fprintf(stderr, "Chamada ao programa incorreta!\r\n");
		fprintf(stderr,"Exemplo:\r\n $ main.c <ficheiro_de_entrada.txt> <ficheiro_de_saida.txt\r\n");
		return -1;
	}

	data_in = fopen(argv[1], "r");
	if (data_in == NULL){
		fprintf(stderr, "Erro ao ler o ficheiro de entrada \"%s\".\r\n", argv[1]);
		return -1;
	}

	data_out = fopen(argv[2], "w");
	if (data_out == NULL){
		fprintf(stderr, "Erro ao ler o ficheiro de saida \"%s\".\r\n", argv[2]);
		return -1;
	}

	//Custom1
	uint8_t temphrm[3] = {1, 2, 3};
	float tempamp[3] = {1, 0.5, 0.25};
	float temppha[3] = {0, 0, 0};

	//Custom2
	//uint8_t temphrm[3] = {1, 2, 3};
	//float tempamp[3] = {1, 0.5, 0.25};
	//float temppha[3] = {M_PI, 0, 0};

	//Definição dos sinais

	Din din_square;
	din_square.type = TRAPEZOIDAL;
	din_square.freq = 50e-3;
	din_square.dc = 0;
	din_square.gain = 1;

	Din din_triangle;
	din_triangle.type = TRIANGLE;
	din_triangle.freq = 50e-3;
	din_triangle.dc = 0;
	din_triangle.gain = 1;

	Din din;
	din.type = CUSTOM;
	din.freq = 50e-3;
	din.dc = 0;
	din.gain = 1;

	//Atribuicao e Padding dos vetores dos harmonicos
	uint8_t harm_len = sizeof(temphrm)/sizeof(uint8_t);
	for(int i = 0; i < N_HARM_FOURIER; i++){
		if (i < harm_len){
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

	//Vetor conjunto (HARDCODED) para os sinais conjuntos
	Din vect[] = {din_square};

	//Gera Pontos para o vetor conjunto
	for(uint8_t waveidx = 0; waveidx < 1; waveidx++){
		for(int i = 0; i < N_PONTOS; i++){
			float temp = 0;
			temp = SintetizaWfm(vect[waveidx]);
			printf("%f\r\n", temp);
			fprintf(data_out, "%f,%f\r\n", time, temp);
		}
	}

	fclose(data_in);
	fclose(data_out);

}