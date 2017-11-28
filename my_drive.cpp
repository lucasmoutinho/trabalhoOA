#include <bits/stdc++.h>

using namespace std;

#if defined(_WIN32)						 		//
	#define CLEAR system("cls");	 	// pede ao preprocessador para
#else													 		// definir uma funcao de limpar a tela
	#define CLEAR system("clear"); 	// com base no S.O.
#endif												 		//

#define CLUSTER 4				 // usada para calcular quantidade de clusters necessarios em escreverArquivo()
#define TRILHA_SUPERF 10 // usada so aqui mesmo***
#define SEEK_T_MEDIO 4	 // usada para calculos de tempo em setorBruto() e em leituraArquivo()
#define SEEK_T_MINIMO 1	// usada so aqui mesmo***
#define T_MEDIO_LAT 6		 // usada para calculos de tempo em setorBruto(), leituraArquivo() e escreverArquivo()
#define TEMPO_TRANSF 12	// usada so aqui mesmo***
#define TRUE 1
#define FALSE 0
#define ESCRITA 1 // padroniza escolhas no menu, crucial em verificaArquivo()
#define LEITURA 2 // padroniza escolhas no menu, crucial em verificaArquivo()
#define REMOCAO 3 // padroniza escolhas no menu, crucial em verificaArquivo()

typedef struct block{															//
	unsigned char bytes_s[512]; // representa um bloco de memoria
} block;											//

typedef struct sector_array{										//
	block sector[60]; // representa um dos setores de um HD
} sector_array;			//

typedef struct track_array{												 //
	sector_array track[5]; // representa uma das trilhas/tracks de um HD
} track_array;					 //

typedef struct fatlist_s{														 //
	char file_name[100];			 // representa a apresentacao da tabela FAT
	unsigned int first_sector; // contem o nome do arquivo a ser inserido/procurado
	double tamanho_arquivo;		 // guarda o primeiro setor deste arquivo
} fatlist;									 // tambem guarda o tamanho do arquivo
fatlist *fat_list = NULL;		 //

typedef struct fatent_s{										 //
	unsigned int used; // representa uma ferramenta logica da tabela FAT
	unsigned int eof;	// guarda informacoes sobre cada cluster do HD
	unsigned int next; // a respeito de diponibilidade para guardar arquivos
} fatent;						 // e sobre a localizacao de outro pedaco de um arquivo armazenado

int numb_files = -1;		// registra a quantidade de arquivos guardados no HD
int tempo_gravacao = 0; // convencionada para ser utilizada em operacoes de tempo de gravacao
int tempo_leitura = 0;	// convencionada para ser utilizada em operacoes de tempo de leitura
fatent *fat_ent;				// criacao da referencia para a tabela FAT_ENT
FILE *fp;								// utilizado na entrada e saida de dados no programa


track_array *alocaCilindro(){
	track_array *array_cilindro = (track_array *)malloc(sizeof(track_array) * 10);
	return array_cilindro;
}

void alocarAFatList(char file_name[], int pos_inicial, int tamanho_arquivo){
	fat_list = (fatlist *)realloc(fat_list, (numb_files + 1) * sizeof(fatlist));

	strcpy(fat_list[numb_files].file_name, file_name);
	fat_list[numb_files].first_sector = pos_inicial;
	fat_list[numb_files].tamanho_arquivo = tamanho_arquivo;
}

void populaFatEnt(int used, int eof, int next, int sector){
	fat_ent[sector].used = used;
	fat_ent[sector].eof = eof;
	fat_ent[sector].next = next;
}

void vetorPosicao(int setor_bruto, int cyl_trk_sec[]){
	int cilindro, trilha, setor;

	cilindro = setor_bruto / 300;
	setor_bruto = setor_bruto % 300;
	trilha = setor_bruto / 60;
	setor = setor_bruto % 60;

	cyl_trk_sec[0] = cilindro;
	cyl_trk_sec[1] = trilha;
	cyl_trk_sec[2] = setor;
}

int setorBruto(){
	int pos_inicial = 0, i = 0, j = 0;
	tempo_gravacao += SEEK_T_MEDIO;

	while (fat_ent[pos_inicial].used == TRUE){
		pos_inicial += 4;
		if (pos_inicial % 60 == 0){
			i++;
			pos_inicial = i * 300 + j * 60;
			tempo_gravacao += T_MEDIO_LAT;
		}
		if (pos_inicial % 3000 == 0){
			j++;
			i = 0;
			pos_inicial = j * 60;
		}
	}

	return pos_inicial;
}

long int tamanhoDoArquivo(){
	long int size;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	return size;
}

void pressioneEnter(){
	cout << endl<< "Pressione Enter para continuar..." << endl;
	while (getchar() != '\n');
	while (getchar() != '\n');
}

void mostrarTabelaGorda(){
	int i = 0, setor, j = 0;
	cout << "-------------------------------" << endl;
	while (i <= numb_files){
		if (strcmp(fat_list[i].file_name, "erased") != 0){
			cout << " --- Nome do arquivo: " << fat_list[i].file_name << "\t";
			cout << " --- Tamanho em disco: " << fat_list[i].tamanho_arquivo << " Bytes"<< "\t";
			setor = fat_list[i].first_sector;
			cout << "--- Localizacao: ";
			while (fat_ent[setor].eof != TRUE){
				cout << setor << " ";
				setor = fat_ent[setor].next;
			}
			cout << endl
					 << endl;
		}
		else{
			j++;
		}
		i++;
	}
	cout << "-------------------------------" << endl;
	cout << "quantidade de arquivos: " << i - j << endl;
	pressioneEnter();
}

int verificaArquivo(char nome_arquivo[], int opcao_menu){
	int arquivo_encontrado = 0, i = 0;
	string confirmar;
	int res = 0;

	cout << endl<< "Informe o nome do arquivo" << endl;
	cin >> nome_arquivo;
	if (opcao_menu == ESCRITA){
		while (arquivo_encontrado == 0){
			if (fopen(nome_arquivo, "r+") == NULL){
				cout << endl
						 << "Arquivo nao encontrado" << endl;
				cout << "Informe outro nome de arquivo:  ";
				cin >> nome_arquivo;
				arquivo_encontrado = 0;
			}
			else{
				CLEAR
				cout << endl<< "O arquivo pode ser escrito" << endl;
				cout << endl<< "Deseja confirmar esta operacao? (s/n): ";
				cin >> confirmar;
				while ((confirmar != "n") && (confirmar != "s")){
					cout << "Opcao invalida, tente novamente: ";
					cin >> confirmar;
				}
				if (confirmar == "s"){
					CLEAR
					arquivo_encontrado = 1;
					res = 1;
				}
				else if (confirmar == "n"){
					CLEAR
					arquivo_encontrado = 1;
					res = 0;
					cout << "Retornando ao menu..." << endl
							 << endl;
				}
			}
		}
	}
	else if (opcao_menu == LEITURA){
		while (strcmp(fat_list[i].file_name, nome_arquivo) != 0 && (i <= numb_files)){
			i++;
		}
		if (i <= numb_files){
			CLEAR
			cout << endl<< "O arquivo pode ser lido do HD virtual" << endl;
			cout << endl<< "Deseja confirmar esta operacao? (s/n): ";
			cin >> confirmar;
			while ((confirmar != "n") && (confirmar != "s")){
				cout << "Opcao invalida, tente novamente: ";
				cin >> confirmar;
			}
			if (confirmar == "s"){
				CLEAR
				res = 1;
			}
			else if (confirmar == "n"){
				CLEAR
				res = 0;
				cout << "Retornando ao menu..." << endl<< endl;
			}
		}
		else{
			res = 0;
			cout << endl;
			cout << "----------------------------------" << endl;
			cout << "O arquivo nao esta presente no HD virtual" << endl;
			cout << "----------------------------------" << endl;
		}
	}
	else if (opcao_menu == REMOCAO){
		while (strcmp(fat_list[i].file_name, nome_arquivo) != 0 && (i <= numb_files)){
			i++;
		}
		if (i <= numb_files){
			CLEAR
			cout << endl<< "O arquivo pode ser removido do HD virtual" << endl;
			cout << endl<< "Deseja confirmar esta operacao? (s/n): ";
			cin >> confirmar;
			while ((confirmar != "n") && (confirmar != "s")){
				cout << "Opcao invalida, tente novamente: ";
				cin >> confirmar;
			}
			if (confirmar == "s"){
				CLEAR
				res = 1;
			}
			else if (confirmar == "n"){
				CLEAR
				res = 0;
				cout << "Retornando ao menu..." << endl<< endl;
			}
		}
		else{
			res = 0;
			cout << endl;
			cout << "----------------------------------" << endl;
			cout << "O arquivo nao esta presente no HD virtual" << endl;
			cout << "----------------------------------" << endl;
		}
	}
	return res;
}

void apagarArquivo(char file_name[]){
	int i = 0, setor;

	while (strcmp(fat_list[i].file_name, file_name) != 0){
		i++;
	}
	setor = fat_list[i].first_sector;
	strcpy(fat_list[i].file_name, "erased");
	while (fat_ent[setor].eof != TRUE){
		fat_ent[setor].used = FALSE;
		setor = fat_ent[setor].next;
	}
	fat_ent[setor].used = FALSE;
	fat_ent[setor].eof = FALSE;
}

void leituraArquivo(char file_name[], track_array *cylinder){
	int i = 0, setor, j = 0, bytes_lidos = 0, t;
	int cyl_trk_sec[] = {0, 0, 0};
	double tamanho_do_arquivo;

	tempo_leitura += SEEK_T_MEDIO;

	while (strcmp(fat_list[i].file_name, file_name) != 0){
		i++;
	}
	setor = fat_list[i].first_sector;

	tamanho_do_arquivo = fat_list[i].tamanho_arquivo;

	fp = fopen("saida.txt", "w+");
	t = setor;
	while (bytes_lidos < tamanho_do_arquivo){
		if (t + 4 < setor){
			tempo_leitura += T_MEDIO_LAT;
			t = setor;
		}
		vetorPosicao(setor, cyl_trk_sec);
		while (j < 512 && bytes_lidos < tamanho_do_arquivo){
			fprintf(fp, "%c", cylinder[cyl_trk_sec[0]].track[cyl_trk_sec[1]].sector[cyl_trk_sec[2]].bytes_s[j]);
			bytes_lidos++;
			j++;
		}

		j = 0;
		setor = fat_ent[setor].next;
	}
	fclose(fp);
}

void escreverArquivo(char file_name[], track_array *cylinder){
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

	pos_inicial = setorBruto();
	alocarAFatList(file_name, pos_inicial, tamanho_do_arquivo);
	vetorPosicao(pos_inicial, cyl_trk_sec);

	fp = fopen(file_name, "r+");
	cyl = cyl_trk_sec[0];
	trilha_inicial = cyl_trk_sec[1];
	setor_atual = pos_inicial;
	setor_anterior = pos_inicial;

	while (setores_escritos < (clusters_necessarios * 4)){
		for (i = 0; i < 512; i++){
			fscanf(fp, "%c", &cylinder[cyl_trk_sec[0]].track[cyl_trk_sec[1]].sector[cyl_trk_sec[2]].bytes_s[i]);
		}
		setores_escritos++;

		if (setores_escritos % 4 == 0){
			if (cyl_trk_sec[1] < 4){
				cyl_trk_sec[1]++;
				cyl_trk_sec[2] -= 3;
				proximo_setor = setor_atual + 57;
			}
			else if ((pos_inicial + 4) % 60 == 0){
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

			if (fat_ent[proximo_setor].used == TRUE){
				proximo_setor = setorBruto();
				vetorPosicao(proximo_setor, cyl_trk_sec);
				pos_inicial = proximo_setor;
				cyl = cyl_trk_sec[0];
				trilha_inicial = cyl_trk_sec[1];
			}
		}
		else{
			proximo_setor = setor_atual + 1;
			cyl_trk_sec[2]++;
		}

		if (cyl_trk_sec[0] != cyl){
			cyl = cyl_trk_sec[0];
			tempo_gravacao += T_MEDIO_LAT;
		}

		populaFatEnt(TRUE, FALSE, proximo_setor, setor_atual);
		setor_anterior = setor_atual;
		setor_atual = proximo_setor;
	}
	fat_ent[setor_anterior].eof = TRUE;
	fclose(fp);
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
	cout << endl<< endl;
	return opcao;
}

int main(){

	int opcao = 0;
	char nome_arquivo[100];
	track_array *cylinder = alocaCilindro();
	fat_ent = (fatent*)malloc(3000*sizeof(fatent));
	int res;
	CLEAR
	do{
		opcao = menu();
		tempo_gravacao = 0;
		tempo_leitura = 0;
		switch (opcao){
		case 1:
			CLEAR
			res = verificaArquivo(nome_arquivo, ESCRITA);
			if (res == 1){
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
			res = verificaArquivo(nome_arquivo, LEITURA);
			if (res == 1){
				cout << endl
						 << "Lido com sucesso do HD!" << endl;
				leituraArquivo(nome_arquivo, cylinder);
				cout << "Tempo total utilizado na leitura " << tempo_leitura << "(ms)" << endl;
			}
			break;
		case 3:
			CLEAR
			res = verificaArquivo(nome_arquivo, REMOCAO);
			if (res == 1){
				apagarArquivo(nome_arquivo);
			}
			break;
		case 4:
			CLEAR
			mostrarTabelaGorda();
			break;
		case 5:
			cout << "Saindo" << endl << endl;
			break;
		default:
			cout << "Valor invalido" << endl << endl;
			break;
		}

	} while (opcao != 5);

	free(cylinder);
	free(fat_ent);
	return 0;
}
