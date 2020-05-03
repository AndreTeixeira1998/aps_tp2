# APS TP1

Trabalho prático 1 de APS 

## Compilacao

A compilacão e feita recorrendo ao cmake de forma standard.
Outros metodos de compilacão de c tambem podem ser utilizados

```
mkdir build && cd build
cmake ..
make
```

## Execucao

```
./aps_tp1 ../data/data_in.txt ../data/data_out.txt
```

Note que os ficheiros de entrada e saida podem ser alterados.

## Visualizacao

Após correr o programa, o ficheiro data_out.txt deve conter os dados necessários
para visualizar as formas de onda.

Estas podem ser visualizadas recorrendo a software como o excel, ou de forma mais
automatizada recorrendo ao script de python:

```
cd data
python3 visualization.py data_out.txt
```
