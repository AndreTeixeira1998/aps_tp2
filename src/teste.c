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

#define BUFFER_SIZE 256
#define N_SIGNALS 10

//Ativar/Desativar modo debug
#define DEBUG 0


#define INVALID_PARAMETER 1

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

	char buffer[BUFFER_SIZE];
	char *parametro, *atributo;
	uint8_t idx_sinal = UNDEFINED;
	Din din_vect[N_SIGNALS];
	Sig sig_vect[N_SIGNALS];
	uint8_t harm_len = 0;
	uint8_t temphrm[N_HARM_FOURIER] = {1, 2, 3};
	float tempamp[N_HARM_FOURIER] = {1, 0.5, 0.25};
	float temppha[N_HARM_FOURIER] = {0, 0, 0};

	while(fgets(buffer, BUFFER_SIZE, data_in)) {
		if ((buffer[0] == '#') || (buffer[0] == '\n') || (buffer[0] == '\r')) continue;
		parametro = strtok(buffer, ":");
		atributo = strtok(NULL, "\n");

		if (!strcmp(parametro, "tipo")){
			idx_sinal == UNDEFINED ? idx_sinal = 0 : idx_sinal++;
			din_vect[idx_sinal].type = atoi(atributo);
		}
		else if (!strcmp(parametro, "ganho")){
			atributo != NULL ? din_vect[idx_sinal].gain = atof(atributo) : exit(INVALID_PARAMETER);
		}
		else if (!strcmp(parametro, "dc")){
			atributo != NULL ? din_vect[idx_sinal].dc = atof(atributo) : exit(INVALID_PARAMETER);
		}
		else if (!strcmp(parametro, "flag")){
			atributo != NULL ? din_vect[idx_sinal].flag = atoi(atributo) : exit(INVALID_PARAMETER);
		}
		else if (!strcmp(parametro, "freq")){
			atributo != NULL ? din_vect[idx_sinal].freq = atof(atributo) : exit(INVALID_PARAMETER);
		}
		else if (!strcmp(parametro, "harm")){
			if(atributo != NULL){
				atributo = strtok(atributo, ",");
				uint8_t idx = 0; 
				while (atributo != NULL){
					temphrm[idx] = atoi(atributo);
					atributo = strtok(NULL, ",\n");
					idx++;
				}
				harm_len = idx;
			}
			else{
				 exit(INVALID_PARAMETER);
			}
		}
		else if (!strcmp(parametro, "amp")){
			if(atributo != NULL){
				atributo = strtok(atributo, ",");
				for (uint8_t idx = 0; atributo != NULL; idx++){
					tempamp[idx] = atof(atributo);
					atributo = strtok(NULL, ",\n");
				}
			}
			else{
				 exit(INVALID_PARAMETER);
			}
		}
		else if (!strcmp(parametro, "pha")){
			if(atributo != NULL){
				atributo = strtok(atributo, ",");
				for (uint8_t idx = 0; atributo != NULL; idx++){
					temppha[idx] = atof(atributo);
					atributo = strtok(NULL, ",\n");
				}
			}
			else{
				 exit(INVALID_PARAMETER);
			}
		}
		else if (!strcmp(parametro, "dur")){
			if (din_vect[idx_sinal].type == 0){
				din_vect[idx_sinal] = CustomizaWfm(din_vect[idx_sinal], temphrm, tempamp, temppha, harm_len);
			}
			double duration_temp = 0;
			sscanf(atributo, "%lf", &duration_temp);
			sig_vect[idx_sinal] = CriaSinal(din_vect[idx_sinal], duration_temp);
		}
	}

	//Inicializacao
	SintetizaWfm(sig_vect[0].din, &time, SAMPLING_TIME);
	float temp = 0;

	for(uint8_t waveidx = 0; waveidx <= idx_sinal; waveidx++){
		for(int i = 0; i < sig_vect[waveidx].n_pontos; i++){
			temp = SintetizaWfm(sig_vect[waveidx].din, &time, SAMPLING_TIME);
			#if DEBUG 
			printf("Onda no:%d \tTipo:%d \t ponto no:%d\t t:%f \ty:%f\r\n", waveidx, sig_vect[waveidx].din.type, i, time, temp);
			#endif
			fprintf(data_out, "%f, %f\r\n", time, temp);
		}
	}

	fclose(data_in);
	fclose(data_out);

}