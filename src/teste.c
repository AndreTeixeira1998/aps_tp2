#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <complex.h>
#include <tgmath.h>
#include <string.h>
#include "sintetizawfm.h"

//Tempo de amostragem
#define SAMPLING_TIME 100e-6 

//Ativar modo debug
#define DEBUG 0

//Macro para calculo de no de pontos a amostrar para um intervalo de tempo dt
#define N_PONTOS(dt) (uint32_t)(dt/SAMPLING_TIME) 

static double time = 0; 

Sig CriaSinal(Din din, double dt){
	Sig novo_sinal;
	novo_sinal.din = din;
	novo_sinal.duration = dt;
	novo_sinal.n_pontos = N_PONTOS(novo_sinal.duration);
	return novo_sinal;
}

int main(int argc, char **argv){
	FILE *data_in, *data_out;
	if (argc != 3){
		fprintf(stderr, "Chamada ao programa incorreta!\r\n");
		fprintf(stderr,"Exemplo:\r\n $ teste.c <ficheiro_de_entrada.txt> <ficheiro_de_saida.txt\r\n");
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

	
	//Parametros para onda Quadrada
	Din din_square;
	din_square.type = SQUARE;
	din_square.freq = 50;
	din_square.dc = 0;
	din_square.gain = 1;
	
	//Parametros para onda triangular
	Din din_triangle;
	din_triangle.type = TRIANGLE;
	din_triangle.freq = 50;
	din_triangle.dc = 0;
	din_triangle.gain = 1;

	//Parametros para dente de serra com degrau a esquerda
	Din din_sawL;
	din_sawL.type = SAWTOOTH_LEFT;
	din_sawL.freq = 50;
	din_sawL.dc = 0;
	din_sawL.gain = 1;

	//Parametros para dente de serra com degrau a direita
	Din din_sawR;
	din_sawR.type = SAWTOOTH_RIGHT;
	din_sawR.freq = 50;
	din_sawR.dc = 0;
	din_sawR.gain = 1;

	//Parametros para onda trapezoidal
	Din din_trap;
	din_trap.type = TRAPEZOIDAL;
	din_trap.freq = 50;
	din_trap.dc = 0;
	din_trap.gain = 1;

	//Parametros para onda personalizada
	Din din;
	din.type = CUSTOM;
	din.freq = 50;
	din.dc = 0;
	din.gain = 1;
	uint8_t temphrm[] = {1, 2, 3};
	uint8_t harm_len = sizeof(temphrm)/sizeof(uint8_t);
	float tempamp[] = {1, 0.5, 0.25};
	float temppha[] = {0, 0, 0};
	din = CustomizaWfm(din, temphrm, tempamp, temppha, harm_len);

	//Parametros para segunda onda personalizada (con inversao de fase)
	Din din2;
	din2.type = CUSTOM;
	din2.freq = 50;
	din2.dc = 0;
	din2.gain = 1;
	uint8_t temphrm2[] = {1, 2, 3};
	uint8_t harm_len2 = sizeof(temphrm)/sizeof(uint8_t);
	float tempamp2[] = {1, 0.5, 0.25};
	float temppha2[] = {M_PI, 0, 0};
	din2 = CustomizaWfm(din, temphrm2, tempamp2, temppha2, harm_len2);

	//Criacao dos sinais de entrada (parametros e duraçao)
	//Nota: e considerado o tempo de amostragem fixo (ver defines)
	Sig x1 = CriaSinal(din_square, 40e-3);
	Sig x2 = CriaSinal(din_triangle, 40e-3);
	Sig x3 = CriaSinal(din_sawL, 40e-3);
	Sig x4 = CriaSinal(din_sawR, 40e-3);
	Sig x5 = CriaSinal(din_trap, 40e-3);
	Sig x6 = CriaSinal(din, 40e-3);
	Sig x7 = CriaSinal(din2, 40e-3);
	Sig x8 = CriaSinal(din_square, 60e-3);

	//Vector conjunto de sinais a sintetizar
	Sig vect[] = {x1, x2, x3, x4, x5, x6, x7, x8};
	uint8_t vect_len = sizeof(vect)/sizeof(Sig);

	//Gera Pontos para o vetor conjunto
	float temp = 0;
	for(uint8_t waveidx = 0; waveidx < vect_len; waveidx++){
		for(int i = 0; i < vect[waveidx].n_pontos; i++){
			temp = SintetizaWfm(vect[waveidx].din, &time, SAMPLING_TIME);
			#if DEBUG
			printf("Onda no:%d \tTipo:%d \t ponto no:%d\t t:%f \ty:%f\r\n", waveidx, vect[waveidx].din.type, i, time, temp);
			#endif
			fprintf(data_out, "%f, %f\r\n", time, temp);
		}
	}

	fclose(data_in);
	fclose(data_out);

}