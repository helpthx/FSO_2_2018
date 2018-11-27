#include <stdio.h>
#include <stdint.h>

uint8_t numb_paginas_tlb[16] = {0}; //numero de paginas na tlb
uint8_t numb_quadros_tlb[16]= {0}; //numero de quadros da tlb
uint8_t tabela_paginas[256] = {0}; //tabela de paginas
uint8_t numb_quadros_usados = 0; //numero de frames usados
uint8_t ult_att_tlb = 0; //Utilima atualização na tlb
uint8_t memoria_fisica[65536] = {0}; //variavel para memoria fisica.
unsigned int erros_pagina  = 0; //Contador de erros de pagina
unsigned int sucesso_tlb = 0; //encontrado na tlb
unsigned int contador_acessos_memoria = 0; //contador de acesso na memoria

FILE *bfptr; 

//Funções
int init(void);
void atualizar_tlb(uint8_t valor_pagina, uint8_t valor_quadro);
void att_memoria_fisica(uint8_t valor_pagina,uint8_t valor_quadro);
uint8_t leitura_memoria_fisica(uint16_t end_memoria);
uint8_t verf_tabela_paginas(uint8_t valor_pagina);
uint8_t verificar_tlb(uint8_t valor_pagina);

//Aplicação main
int main(int argc, char *argv[]){


	if(argc < 2){
		printf("Deve ser usado: ./proj02.out addresses.txt\n");
		return -1;
	}

	int valor_lido; //Valor lido no arquivo addresses que será separado por mascara
	uint16_t end_logico; //1. O endereço lógico que está sendo traduzido (o valor inteiro que está sendo lido em addresses.txt)
	uint8_t valor_pagina; // 2. Valor da pagina a partir do endereço virtual;
	uint8_t valor_quadro; // 2. Os valores de quadro 
	uint8_t offset; //3. O valor do offset traduzido a partir do endereço virtual;
	uint16_t i;
	uint16_t end_memoria; //4. O endereço físico correspondente (aquele para o qual seu programa traduziu o endereço lógico); 
	uint8_t dados_lidos; // 5. O valor do byte sinalizado armazenado no endereço físico resultante.
	double taxa_pagina_erro; //1. Taxa de erros de página – o percentual de referências de endereços que resultaram em erros de página; 
	double taxa_tlb_sucesso; //2. Taxa de sucesso do TLB – o percentual de referências de endereços que foram resolvidas no TLB.  
	FILE *fp;
	
	fp = fopen(argv[1], "r");


	if(fp == NULL){
		printf("Error ao abrir %s\n", argv[1]);
		return -1;
	}

	bfptr = fopen("BACKING_STORE.bin", "rb");
	
	if(bfptr == NULL){
		printf("Erro ao abrir BACKING_STORE.bin\n");
		return -1;
	}

	do{
		while(fscanf(fp,"%d",&valor_lido) != EOF){
			contador_acessos_memoria++;
			printf("valor lido do address.txt: %d\n",valor_lido);

			/*Aplicando as mascaras para separar os bits de offset e data*/
			end_logico = 0xFFFF & valor_lido;
			valor_pagina = (0xFF00 & end_logico) >> 8;
			offset = 0x00FF & end_logico;
			printf("\n 1 - Endereço lógico: %u \n 2 - Numero da pagina: %u \n 3 - Offset: %u\n",end_logico,valor_pagina,offset);
			valor_quadro = verificar_tlb(valor_pagina);
			printf(" 2 - Numero do quadros encontrado: %u\n", valor_quadro);
			end_memoria = valor_quadro*256 + offset;
			printf(" 5 - Posição da memoria encontrada: %u\n", end_memoria);
			dados_lidos = leitura_memoria_fisica(end_memoria);
			printf(" 6 - Dados lido: %u\n", dados_lidos);
			printf("\n");
		}

		taxa_pagina_erro = erros_pagina/(contador_acessos_memoria*1.0);
		taxa_tlb_sucesso = sucesso_tlb/(contador_acessos_memoria*1.0);

		/*Estatisticas de miss table*/
		printf("\n--------------- \n");
		printf("Estatísticas\n");
		printf("--------------- \n");
		printf("1 - Taxa de erro de pagina: %.2lf\n", taxa_pagina_erro*100);
		printf("2 - Taxa de sucesso do TLB: %.2lf\n", taxa_tlb_sucesso*100);
		fclose(fp);
		fclose(bfptr);
		return 0;
			
	}while(fp != NULL && bfptr != NULL);
}

//Funções de manipulação

uint8_t leitura_memoria_fisica(uint16_t end_memoria){ //leitura da posição da memoria fisica

	uint8_t dados_lidos;
	dados_lidos = memoria_fisica[end_memoria];
	return dados_lidos; 
}

int init(void){ //Condição de inicialização da tradução init
	
	int true1 = (numb_paginas_tlb[0] == 0) && (numb_quadros_tlb[0] == 0);
	int true2 = (numb_paginas_tlb[1] == 0) && (numb_quadros_tlb[1] == 0);

	if(true1 == 1 && true2 == 1){
		return 0;
	}
	else{
		return 1;
	}
}

void atualizar_tlb(uint8_t valor_pagina, uint8_t valor_quadro){ //atualizar_tlb

	/*Iniciando a TLB*/
	if((valor_quadro == 0) && (ult_att_tlb == 0)){
		numb_paginas_tlb[0] = valor_pagina;
		numb_quadros_tlb[0] = valor_quadro;
		printf("Atualização da TLB: Posição %u \nNumero da pagina: %u \nNumero do frame: %u\n", ult_att_tlb, numb_paginas_tlb[0], numb_quadros_tlb[0]);
		ult_att_tlb++;
	}
	else{
		numb_paginas_tlb[ult_att_tlb] = valor_pagina;
		numb_quadros_tlb[ult_att_tlb] = valor_quadro;
		printf("Atualização da TLB: Posição %u \nNumero da pagina: %u \nNumero do frame: %u\n", ult_att_tlb, numb_paginas_tlb[ult_att_tlb], numb_quadros_tlb[ult_att_tlb]);
		ult_att_tlb = (ult_att_tlb + 1) % 16;
	}

}

void att_memoria_fisica(uint8_t valor_pagina,uint8_t valor_quadro){ //atualizando a memoria fisica att_memoria_fisica

	uint16_t i = 0;
	fseek(bfptr, valor_pagina*256, SEEK_SET);

    for(i = 0; i < 256; i++){
        fread(&memoria_fisica[valor_quadro*256 + i], 1, 1, bfptr);
    }
}

uint8_t verf_tabela_paginas(uint8_t valor_pagina){ //Caso não esteja na tlb procurar na pagina de tabelas verf_tabela_paginas

	uint8_t i;
	uint8_t j;

	for(i = 0; i < 256; i++){
		
		/*Não existe um frame. Deve-se inicar a tabela de pagina*/
		if((i == valor_pagina) && (numb_quadros_usados == 0)){
			printf("Pagina não iniciada na memoria. Gerador de erro de pagina.\n");
			
			printf("Atualizando a memoria fisica com um novo frame...\n");
			att_memoria_fisica(valor_pagina, numb_quadros_usados);
			printf("Updated.\n");
			
			tabela_paginas[i] = numb_quadros_usados;
			numb_quadros_usados++;

			/*Atualizando a TLB*/
			printf("Atualizando a TLB com um novo numero de pagina...\n");
			
			atualizar_tlb(valor_pagina,tabela_paginas[i]);
			
			printf("Feito.\n");
			return tabela_paginas[i];	
		}
		
		/*Não existe o frame da pagina que foi procurada*/
		else if(i == valor_pagina && (tabela_paginas[i] == 0)){
			erros_pagina++;
			printf("Pagina não iniciada na memoria. Gerador de erro de pagina.\n");
			printf("Atualizando a memoria fisica com um novo frame...\n");
			att_memoria_fisica(valor_pagina, numb_quadros_usados);
			printf("Atualizado.\n");
			tabela_paginas[i] = numb_quadros_usados;
			numb_quadros_usados++;
			
			printf("Atualizando a TLB com um novo numero de pagina...\n");
			atualizar_tlb(valor_pagina,tabela_paginas[i]);
			printf("Feito.\n");
			return tabela_paginas[i];
		}
		/*Ja tem o frame para a pagina que foi procurada*/
		else if(i == valor_pagina){
			printf("Pagina encontrada na memoria.\n");
			printf("Atualizando a TLB com um novo numero de pagina...\n");
			atualizar_tlb(valor_pagina,tabela_paginas[i]);
			printf("Feito.\n");
			return tabela_paginas[i];
		}
	}
}

uint8_t verificar_tlb(uint8_t valor_pagina){ //Verificar se esta na tlb 

	uint8_t i;

	printf("Verificando o numero da pagina na TLB...\n");

	if(init() == 1){
		for(i = 0; i < 16; i++){

			if(numb_paginas_tlb[i] == valor_pagina){
				sucesso_tlb++;
				printf("Frame encontrado %u na TLB.\n",numb_quadros_tlb[i]);
				return numb_quadros_tlb[i];
			}
		}
	}
	printf("Numero da pagina não encontrado na TLB.\n");
	printf("Abrindo a tabela de pagina...\n");
	return verf_tabela_paginas(valor_pagina);
}
