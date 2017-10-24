#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>

using namespace std;

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

typedef struct fatent_s {
    unsigned int used;
    unsigned int eof;
    unsigned int next;
} fatent;


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

    do{
        opcao = menu();

        switch(opcao){
            case 1:

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