#include <bits/stdc++.h>

using namespace std;

#if defined(_WIN32)
	#define CLEAR system("cls");
#else
	#define CLEAR system("clear");
#endif

#define CLUSTER 4
#define TRILHA_SUPERF  10
#define SEEK_T_MEDIO 4
#define SEEK_T_MINIMO 1
#define T_MEDIO_LAT 6
#define TEMPO_TRANSF 12
#define TRUE 1
#define FALSE 0


typedef struct block{
     unsigned char bytes_s[512];
} block;

typedef struct sector_array{
    block sector[60];
} sector_array;

typedef struct track_array{
    sector_array track[5];
} track_array;

typedef struct fatlist_s {
    char file_name[100];
    unsigned int first_sector;
} fatlist;
fatlist *fat_list = NULL;

typedef struct fatent_s {
    unsigned int used;
    unsigned int eof;
    unsigned int next;
} fatent;


int numb_files = -1;
int tempo_gravacao = 0;
fatent *fat_ent;
FILE *fp;

void allocFatList(char file_name[], int pos_inicial){
	/*
		Re-aloca memoria para a estrutura de FatList de acordo com o necessario
		Param:
		- file_name[]: nome do arquivo respectivo a fatList
		- pos_inicial: setor de inicio do arquivo
	*/
	fat_list = (fatlist*)realloc(fat_list, (numb_files+1)*sizeof(fatlist));

	strcpy(fat_list[numb_files].file_name, file_name);
	fat_list[numb_files].first_sector = pos_inicial;
}

void allocFatEnt(int used ,int eof ,int next, int sector){
	/*
		Popula a FatEnt.
		Param:
		- used: Se a area esta sendo usada ou nao
		- eof: fim de arquivo TRUE ou FALSE
		- next: proximo setor
		- sector: setor respectivo da fatent
	*/
	fat_ent[sector].used = used;
	fat_ent[sector].eof = eof;
	fat_ent[sector].next = next;
}

void oneToThree(int index, int cyl_trk_sec[]){
	/*
		Converte o valor de setor bruto para sua posicao
		cilindro/trilha/setor
		Param:
		-index: int de setor bruto
		-cyl_trk_sec: array de inteiros para o cilindro/trilha/setor
	*/
	int t;
	int s;
    int c = index/300;

    index -= 300*c;
    if((index/60) >= 5){
        t = ((index/60)/5)-1;
    }else{
        t = (index/60);
    }
    s = index%60;


    cyl_trk_sec[0] = c;
    cyl_trk_sec[1] = t;
    cyl_trk_sec[2] = s;
}


int searchFatList(int cyl_trk_sec[]){
	/*
		Procura o valor de setor bruto usado para gravar o arquivo
	*/
	int i, j, k;
	i = j = k = 0;
	tempo_gravacao = SEEK_T_MEDIO;

	while(fat_ent[i].used == TRUE){
		i++;
		if(i % 60 == 0){
			j++;
			i = j*300;
			tempo_gravacao += T_MEDIO_LAT;
		}
		if(i % 3000 == 0){
			k++;
			i = k*60;
		}
	}

 	return i;

}


int sizeOfFile(){
	/*
	Retorna o tamanho do arquivo em bytes
	Observação: o 'size' sempre
	tera alguns bytes a mais devido ao
	'\n' ao final do arquivo e das linhas.
	*/
	int size;

	fseek(fp, 0, SEEK_END); /* Leva o ponteiro para o final do arquivo */
	size = ftell(fp); /* Retorna a posição do ponteiro dentro do arquivo */

	return size;
}


track_array *allocCylinder(){
	/*
		Aloca 10 cilindros porque:
		Qtd de cilindro = numeros de trila por superfice
	*/

	track_array *new_track_array =
		(track_array*)malloc(sizeof(track_array)*10);

	return new_track_array;
}


// Funcao utilizada para verificar se o arquivo a ser aberto existe ou nao.
int verificaArquivo(char nome_arquivo[]){

  int arquivo_encontrado = 0;
	string confirmar;
	int res;

  cout << endl << "Informe o nome do arquivo" << endl;
  cin >> nome_arquivo;
  while(arquivo_encontrado == 0) {
    if(fopen(nome_arquivo, "r+") == NULL){
      cout << endl << "Arquivo nao encontrado" << endl;
      cout << "Informe outro nome de arquivo:  ";
      cin >> nome_arquivo;
      arquivo_encontrado = 0;
    }else{
      CLEAR
      cout << endl << "O arquivo pode ser escrito" << endl;
			cout << endl << "Deseja confirmar esta operacao? (s/n): ";
			cin >> confirmar;
			while((confirmar != "n") && (confirmar != "s")) {
				cout << "Opcao invalida, tente novamente: ";
				cin >> confirmar;
			}
			if (confirmar == "s") {
				CLEAR
				arquivo_encontrado = 1;
				res = 1;
			}else if (confirmar == "n") {
				CLEAR
				arquivo_encontrado = 1;
				res = 0;
				cout << "Retornando ao menu..." << endl << endl;
			}
    }
  }
	return res;
}

void escreverArquivo(char file_name[], track_array *cylinder){
	/*
		Funcao para escrita do arquivo no HD
	*/
	double cluster_needed;
	double fp_size;
	int cyl_trk_sec[] = {0, 0, 0};
	int cyl;
	int pos_inicial, i, written_sector = 0, j, next_sector, actual_sector;

	fp = fopen(file_name, "r+");
	fp_size = sizeOfFile();

	cluster_needed = ceil(fp_size / (CLUSTER * 512));
	cout << "Tamanho do arquivo a ser gravado: " << fp_size << " bytes" << endl;

	printf(" - O arquivo necessitará de" " %.0lf cluster(s)\n", cluster_needed);

	fclose(fp);
	pos_inicial = searchFatList(cyl_trk_sec);
	allocFatList(file_name, pos_inicial);
	oneToThree(pos_inicial, cyl_trk_sec);

	fp = fopen(file_name, "r+");

	tempo_gravacao = 0;

	cyl = cyl_trk_sec[0];
	while(written_sector < (cluster_needed*4)){
		/* Conta até o written_sector chegar ao número de setores necessários
		   multiplo de 4 */

		actual_sector = pos_inicial;
		/* Guarda o setor atual pois pos_incial se altera
		   no decorrer do algoritmo*/

		for(i = 0; i < 512; i++){
		/* Loop para leitura do arquivo */
			fscanf(fp, "%c", &cylinder[cyl_trk_sec[0]].track[cyl_trk_sec[1]]
				.sector[cyl_trk_sec[2]].bytes_s[i]);
		}
		written_sector++;
		/* soma 1 nos setores ja escritos(tanto faz o local) */

		if(written_sector % 4 == 0){
			/*
			Aqui eh checado se o algoritmo escreveu 1 cluster, se sim ele
			soma 57.(57 = 60 - 4 + 1) Em que somar 60 pula para o cluster
			de baixo e se reduz 4 para voltar ao inicio do cluster para
			assim somar 1.
			*/
			tempo_gravacao += T_MEDIO_LAT;
			j = 0;
			cyl_trk_sec[2] += 57;
			while(fat_ent[cyl_trk_sec[2]].used != FALSE){
				/* Checa se o cluster logo abaixo esta vago */
				j++;
				cyl_trk_sec[2]++;
				if(j == 4){
				/* Caso todo o cluster abaixo esteja ocupado,
				   ele pula para o de baixo */
					cyl_trk_sec[2] += 57;
				}
			}
			pos_inicial = cyl_trk_sec[2];
		} else {
			/* Caso nao tenha escrito um cluster pos_inicial++ normalmente */
			pos_inicial++;
			cyl_trk_sec[2] = pos_inicial;
		}

		if(cyl_trk_sec[0] != cyl){
			tempo_gravacao += T_MEDIO_LAT;
		}

		next_sector = pos_inicial;
		allocFatEnt(TRUE, FALSE, next_sector, actual_sector);
	}
	fat_ent[actual_sector].eof = TRUE;
	/* Esta linha atribui eof no ultimo setor escrito.
	   Melhor aqui do que com um if dentro do loop. */
}

int menu(){
    int opcao;

    cout << endl;
    cout << "----------------------------------" << endl;
    cout << "1 - Escrever arquivo" << endl;
    cout << "2 - Ler arquivo" << endl;
    cout << "3 - Apagar arquivo" << endl;
    cout << "4 - Mostrar tabela FAT" << endl;
    cout << "5 - Sair" << endl;
    cout << "----------------------------------" << endl;
    cout << "Digite a sua opcao entre as acima: ";

    cin >> opcao;
    cout << endl << endl;
    return opcao;
}

int main(){

    int opcao = 0;
    char nome_arquivo[100];
		track_array *cylinder = allocCylinder();
		fat_ent = (fatent*)malloc(30000*sizeof(fatent));
		int res;
    CLEAR
    do{
        opcao = menu();

        switch(opcao){
            case 1:
                CLEAR
                res = verificaArquivo(nome_arquivo);
								if (res == 1) {
									numb_files++;
									escreverArquivo(nome_arquivo, cylinder);
									cout << "Gravado com sucesso no HD!" << endl << endl;
									cout << "Tempo total utlizado na gravacao: " << tempo_gravacao << "m(s)" << endl;
									tempo_gravacao = 0;
								}
                break;
            case 2:

                break;
            case 3:

                break;
            case 4:

                break;
            case 5:
                cout << "Saindo" << endl << endl;
                break;
            default:
                cout << "Valor invalido" << endl << endl;
                break;
        }


    } while(opcao != 5);

    return 0;
}
