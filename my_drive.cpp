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
int tempo_leitura = 0;
fatent *fat_ent;
FILE *fp;

void pressioneEnter(){
	cout << endl << "Pressione Enter para continuar..." << endl;
	while (getchar() != '\n');
	while(getchar() != '\n'); 
}

void alocarAFatList(char file_name[], int pos_inicial){
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

void alocaFatEnt(int used ,int eof ,int next, int sector){
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

void vetorPosicao(int setor_bruto, int cyl_trk_sec[]){
	/*
		Converte o valor de setor bruto para sua posicao
		cilindro/trilha/setor
		Param:
		-setor_bruto: int de setor bruto
		-cyl_trk_sec: array de inteiros para o cilindro/trilha/setor
	*/
	int cilindro, trilha, setor;

	cilindro = setor_bruto/300;
	setor_bruto = setor_bruto%300;
	trilha = setor_bruto/60;
	setor = setor_bruto%60;


	cyl_trk_sec[0] = cilindro;
	cyl_trk_sec[1] = trilha;
	cyl_trk_sec[2] = setor;
}


int procuraNaFatEnt(){
	/*
		Procura o valor de setor bruto usado para gravar o arquivo
	*/
	int pos_inicial = 0, i = 0, j = 0;
	tempo_gravacao = SEEK_T_MEDIO;

	while(fat_ent[pos_inicial].used == TRUE){
		pos_inicial+=4;
		if(pos_inicial % 60 == 0){
			i++;
			pos_inicial = i*300 + j*60;
			tempo_gravacao += T_MEDIO_LAT;
		}
		if(pos_inicial % 3000 == 0){
			j++;
			i = 0;
			pos_inicial = j*60;
		}
	}

 	return pos_inicial;
}


long int tamanhoDoArquivo(){
	/*
	Retorna o tamanho do arquivo em bytes
	Observação: o 'size' sempre
	tera alguns bytes a mais devido ao
	'\n' ao final do arquivo e das linhas.
	*/
	long int size;

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
      cout << endl << "O arquivo pode ser escrito/lido" << endl;
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

void leituraArquivo(char file_name[], track_array *cylinder) {
	/*
		Funcao para Leitura do arquivo
	*/
	int i = 0, setor, j = 0, numb_sectors = 1, read_sector = 0, tamanho_do_arquivo, l = 0;
	int t;
	char file_name_2[100];
	int cyl_trk_sec[] = {0, 0, 0};

	tempo_leitura = SEEK_T_MEDIO;

	/* Enquanto nao acha o nome no arquvio na tabela fat ...*/
	while(strcmp(fat_list[i].file_name, file_name) != 0 && (i <= numb_files)){
		i++;
	}
	setor = fat_list[i].first_sector;

	/* Conta a quantidade de setores*/
	while(fat_ent[setor].eof == FALSE){
		numb_sectors++;
		setor = fat_ent[setor].next;
	}


	if(i <= numb_files){
		strcpy(file_name_2, fat_list[i].file_name);
		fp = fopen(file_name_2, "r+");
		tamanho_do_arquivo = tamanhoDoArquivo();
	}

	/* Se o arquivo existe no Hd Salva no saida.txt */
	if(strcmp(fat_list[i].file_name, file_name) != 0){
		cout << "O arquivo nao esta presente o HD virtual" << endl;
	} else {
		fp = fopen("saida.txt", "w+");
		setor = fat_list[i].first_sector;
		t = setor;

		while(l < tamanho_do_arquivo){
			if(t+4 < setor){
				tempo_leitura += T_MEDIO_LAT;
				t = setor;
			}
			vetorPosicao(setor, cyl_trk_sec);
			while(j < 512 && l < tamanho_do_arquivo){
				fprintf(fp, "%c", cylinder[cyl_trk_sec[0]]
						.track[cyl_trk_sec[1]].sector[cyl_trk_sec[2]].bytes_s[j]);
				j++;
				l++;
			}

			j = 0;
			setor = fat_ent[setor].next;
			read_sector++;
		}
		fclose(fp);
	}
}

void escreverArquivo(char file_name[], track_array *cylinder){
	/*
	Funcao para escrita do arquivo no HD
	*/
	int pos_inicial, i, setores_escritos = 0, proximo_setor, setor_atual, setor_anterior, trilha_inicial, cyl;
	int cyl_trk_sec[] = {0, 0, 0};
	double clusters_necessarios;
	double tamanho_do_arquivo;

	fp = fopen(file_name, "r+");
	tamanho_do_arquivo = tamanhoDoArquivo();
	clusters_necessarios = ceil(tamanho_do_arquivo / (CLUSTER * 512));
	fclose(fp);

	cout << "Tamanho do arquivo a ser gravado: " << tamanho_do_arquivo << " bytes" << endl;
	cout << "O arquivo necessitará de " << clusters_necessarios << " cluster(s)" << endl;
	pressioneEnter();

	pos_inicial = procuraNaFatEnt();
	alocarAFatList(file_name, pos_inicial);
	vetorPosicao(pos_inicial, cyl_trk_sec);

	fp = fopen(file_name, "r+");

	tempo_gravacao = 0;

	cyl = cyl_trk_sec[0];
	trilha_inicial = cyl_trk_sec[1];
	setor_atual = pos_inicial;
	setor_anterior = pos_inicial;

	while(setores_escritos < (clusters_necessarios*4)){
		/* Conta até o setores_escritos chegar ao número de setores necessários
		multiplo de 4 */

		/* Guarda o setor atual pois pos_incial se altera
		no decorrer do algoritmo*/

		for(i = 0; i < 512; i++){
			/* Loop para leitura do arquivo */
			fscanf(fp, "%c", &cylinder[cyl_trk_sec[0]].track[cyl_trk_sec[1]].sector[cyl_trk_sec[2]].bytes_s[i]);
		}
		setores_escritos++;
		/* soma 1 nos setores ja escritos(tanto faz o local) */

		if(setores_escritos % 4 == 0){
			/*
			Aqui eh checado se o algoritmo escreveu 1 cluster, se sim ele
			soma 57.(57 = 60 - 4 + 1) Em que somar 60 pula para o cluster
			de baixo e se reduz 4 para voltar ao inicio do cluster para
			assim somar 1.
			*/
			if(cyl_trk_sec[1] < 4){
				cyl_trk_sec[1]++;
				cyl_trk_sec[2] -= 3;
				proximo_setor = setor_atual + 57;	
			}
			else if((pos_inicial+4) % 60 == 0 ){
				cyl_trk_sec[0]++;
				cyl_trk_sec[1] = trilha_inicial;
				cyl_trk_sec[2] = 0;
				pos_inicial += 244;
				proximo_setor = setor_atual + 1;
			}
			else{
				cyl_trk_sec[1] = trilha_inicial;
				cyl_trk_sec[2]++;
				proximo_setor = setor_atual - 239;
				pos_inicial += 4;
			}
		} 
		else {
			/* Caso nao tenha escrito um cluster pos_inicial++ normalmente */
			proximo_setor = setor_atual + 1;
			cyl_trk_sec[2]++;
		}

		if(cyl_trk_sec[0] != cyl){
			cyl = cyl_trk_sec[0];
			tempo_gravacao += T_MEDIO_LAT;
		}

		alocaFatEnt(TRUE, FALSE, proximo_setor, setor_atual);
		setor_anterior = setor_atual;
		setor_atual = proximo_setor;
	}
	fat_ent[setor_anterior].eof = TRUE;
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
	fat_ent = (fatent*)malloc(3000*sizeof(fatent));
	int res;
	CLEAR
	do{
		opcao = menu();
		tempo_gravacao = 0;
		tempo_leitura = 0;
		switch(opcao){
			case 1:
				CLEAR
				res = verificaArquivo(nome_arquivo);
				if (res == 1) {
					numb_files++;
					escreverArquivo(nome_arquivo, cylinder);
					cout << "Gravado com sucesso no HD!" << endl << endl;
					cout << "Tempo total utlizado na gravacao: " << tempo_gravacao << "m(s)" << endl;

				}
				break;
			case 2:
				CLEAR
				cout << "Leitura de arquivo do HD virtual" << endl;
				cout << "Irá gerar um arquivo saida.txt" << endl;
				res = verificaArquivo(nome_arquivo);
				leituraArquivo(nome_arquivo, cylinder);
				cout << "Tempo total utilizado na leitura " << tempo_leitura << "(ms)" << endl;
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

	free(cylinder);
	free(fat_ent);
	return 0;
}
