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
	uint8_t amp[N_HARM_FOURIER];
	uint8_t pha[N_HARM_FOURIER];
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

float SintetizaWfm(Din din){
	static float omega = 0;
	static uint64_t ts = 0;
	static double time = 0;
	static uint8_t nh = 0;
	static WfmType wavetype = UNDEFINED;
	static float r[N_HARM_FOURIER] = {0};
	static float beta[N_HARM_FOURIER] = {0};
	static float k[N_HARM_FOURIER] = {0}; //Not sure if float

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
			puts("Custom");

		break;

		case SQUARE:
			for (uint8_t i = 1; i < N_HARM_FOURIER; i++){
				k[i] = i*2-1; //O -1 era por causa do matlab?
				ak[i] = 0;
				bk[i] = 4/(M_PI*k[i]);
			}
			puts("SQUARE");
		break;

		case TRIANGLE:
			puts("TRIANGLE");
		break;

		case SAWTOOTH_LEFT:
			puts("SAWTOOTH LEFT");
		break;

		case SAWTOOTH_RIGHT:
			puts("SAWTOOTH RIGHT");
		break;

		case TRAPEZOIDAL:
			puts("TRAPEZOIDAL");
		break;

		default:
			puts("Tipo de forma de onda inválido");
			return -1;
	}

	if (wavetype != CUSTOM){
		for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
			r[i] = sqrt(pow(ak[i], 2) + pow(bk[i], 2));	
			beta[i] = cargf(ak[i] + I*bk[i]);
		}
	} 
	else{
		puts("Nao implementado :O");
	}

	for (uint8_t i = 0; i < N_HARM_FOURIER; i++){
		y += r[i] * cos(k[i]*omega*time+beta[i]);
	}
	time += 40e-3; // Wut?
	y = y*din.gain;

	return y;
}

int main(int argc, char **argv){
	FILE *data_in, *data_out;
	if (argc != 3){
		printf("Chamada ao programa incorreta!\r\n");
		printf("Exemplo:\r\n $ main.c <ficheiro_de_entrada.txt> <ficheiro_de_saida.txt\r\n");
		return -1;
	}

	data_in = fopen(argv[1], "r");
	if (data_in == NULL){
		printf("Erro ao ler o ficheiro de entrada \"%s\".\r\n", argv[1]);
		return -1;
	}

	data_out = fopen(argv[2], "w");
	if (data_out == NULL){
		printf("Erro ao ler o ficheiro de saida \"%s\".\r\n", argv[2]);
		return -1;
	}


	uint8_t temphrm[3] = {1, 2, 3};
	float tempamp[3] = {1, 0.5, 0.25};
	float temppha[3] = {0, 0, 0};

	Din Din;
	Din.type = SQUARE;
	memcpy(Din.F.hrm, temphrm, sizeof(temphrm));
	memcpy(Din.F.amp, tempamp, sizeof(tempamp));
	memcpy(Din.F.pha, temppha, sizeof(temppha));
	Din.freq = 50;
	Din.dc = 0;
	Din.gain = 1;

	for (int i = 0; i < 1e6; i++){
		float temp = 0;
		temp = SintetizaWfm(Din);
		printf("%f\r\n", temp);
		fprintf(data_out, "%d,%f\r\n", i, temp);
	}

	fclose(data_in);
	fclose(data_out);

}