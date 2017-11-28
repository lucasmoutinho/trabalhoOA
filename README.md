##TRABALHO DE OA

#ALUNOS:

- Lucas da Silva Moutinho        : 15/0015747
- Tiago Rodrigues da Cunha Cabral: 15/0150296
- Pedro Henrique Rodrigues Gomes : 15/0144482


1. Objetivos
O objetivo deste trabalho é que o aluno aprenda a estrutura do disco magnético e como são
organizados os arquivos dentro dele.
Implementar, utilizando obrigatoriamente a linguagem C ou C++.
2. Especificação
2.1 Ambiente:
Crie uma estrutura de dados para emular um disco magnético rígido (HD) com as seguintes
características:
 Trilhas por cilindro: 5
 Setores por trilha: 60
 Trilhas por superfície: 10
 Tamanho do setor: 512 bytes
 Tamanho do cluster: 4 setores
 Tempo médio de seek: 4 ms
 Tempo mínimo de seek: 1ms
 Tempo media de latência: 6ms
 Tempo de transferência: 12ms/trilha
Para isso no programa principal deve ser utilizado um ponteiro para indicar os cilindros e alocar a quantidade de cilindros necessária.
track_array *cylinder;
Universidade de Brasília – UnB
Instituto de Ciências Exatas – IE
Departamento de Ciência da Computação – CIC
Devem ser definidas como tipos globais, as seguintes estruturas:
typedef struct block { unsigned char bytes_s[512]; } block;
typedef struct sector_array { block sector[60]; } sector_array;
typedef struct track_array { sector_array track[5]; } track_array;
Deve ser criada também uma tabela FAT. A tabela FAT deve ser feita a partir de dois arrays de
registros. O primeiro array deve conter uma lista com os nomes dos arquivos e o indicador do
primeiro setor onde o arquivo esta armazenado no HD.

typedef struct fatlist_s {
char file_name[100];
unsigned int first_sector;
} fatlist;

O segundo array deve ser uma lista com uma entrada por cada setor do HD.

typedef struct fatent_s {
unsigned int used;
unsigned int eof;
unsigned int next;
} fatent;

Sendo que o elemento 0 do array representa o primeiro setor da primeira trilha do primeiro cilindro.
O elemento 1 é o segundo setor da primeira trilha do primeiro cilindro. O elemento 60 é o primeiro
setor da segunda trilha do primeiro cilindro. O elemento 300 é primeiro setor da primeira trilha do
segundo cilindro. Conforme a figura abaixo.
O campo 'used' indica se o setor esta livre ou sendo utilizado. O campo 'eof' indica se é o último setor
do arquivo. O campo 'next' indica o próximo setor do arquivo. Um exemplo de como a FAT deve
funcionar está mostrada na Figura abaixo:
Universidade de Brasília – UnB
Instituto de Ciências Exatas – IE
Departamento de Ciência da Computação – CIC

2.2 Software:
O programa deve ter como nome: 'my_drive.cpp'. E assim que for executado deve mostrar o seguinte
menu:
1 - Escrever Arquivo
2 - Ler Arquivo
3 - Apagar Arquivo
4 - Mostrar Tabela FAT
5 - Sair