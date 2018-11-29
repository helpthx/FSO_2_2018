#include <stdio.h>
#include <stdint.h>

unsigned char num_paginas_tlb[16] = {0}; //numero de paginas na tlb
unsigned char num_quadros_tlb[16]= {0}; //numero de quadros da tlb
unsigned char tabela_paginas[256] = {0}; //tabela de paginas
unsigned char num_quadro = 0; //numero de frames usados
unsigned char ult_att_tlb = 0; //Utilima atualização na tlb
unsigned char memoria_fisica[65536] = {0}; //variavel para memoria fisica.

unsigned int erros_pagina  = 0; //Contador de erros de pagina
unsigned int sucesso_tlb = 0; //encontrado na tlb
unsigned int contador_acessos_memoria = 0; //contador de acesso na memoria

FILE *bfptr; 

//Funções de manipulação

unsigned char leitura_memoria_fisica(unsigned short end_memoria){ //leitura da posição da memoria fisica

	unsigned char valor_byte;
	valor_byte = memoria_fisica[end_memoria];
	return valor_byte; 
}

int init(void){ //Condição de inicialização da tradução init
	
	int true1 = (num_paginas_tlb[0] == 0) && (num_quadros_tlb[0] == 0);
	int true2 = (num_paginas_tlb[1] == 0) && (num_quadros_tlb[1] == 0);

	if(true1 == 1 && true2 == 1){
		return 0;
	}
	else{
		return 1;
	}
}

void atualizar_tlb(unsigned char valor_pagina, unsigned char valor_quadro){ //atualizar_tlb

	/*Atualizando a TLB*/
	if((valor_quadro == 0) && (ult_att_tlb == 0)){
		num_paginas_tlb[0] = valor_pagina;
		num_quadros_tlb[0] = valor_quadro;
		printf("Atualização da TLB: Posição: %u \nNumero da pagina: %u \nNumero do frame: %u\n", ult_att_tlb, num_paginas_tlb[0], num_quadros_tlb[0]);
		ult_att_tlb++;
	}
	else{
		num_paginas_tlb[ult_att_tlb] = valor_pagina;
		num_quadros_tlb[ult_att_tlb] = valor_quadro;
		printf("Atualização da TLB: Posição %u \nNumero da pagina: %u \nNumero do frame: %u\n", ult_att_tlb, num_paginas_tlb[ult_att_tlb], num_quadros_tlb[ult_att_tlb]);
		ult_att_tlb = (ult_att_tlb + 1) % 16;
	}

}

void att_memoria_fisica(unsigned char valor_pagina,unsigned char valor_quadro){ //atualizando a memoria fisica 

	fseek(bfptr, valor_pagina*256, SEEK_SET);

	unsigned short i = 0;
	while(i<256){
		fread(&memoria_fisica[valor_quadro*256 + i], 1, 1, bfptr);
		i++;
	}	
}

unsigned char verf_tabela_paginas(unsigned char valor_pagina){ //Caso não esteja na tlb procurar na pagina de tabelas verf_tabela_paginas

	unsigned char i=0;
	unsigned char j;
		
	while(i<255){
		
		/*Não existe um frame. Deve-se iniciar a tabela de pagina*/
		if((i == valor_pagina) && (num_quadro == 0)){
			printf("Inicializando a Tabela de Páginas. Gerador de erro de pagina.\n");
			
			printf("Atualizando a memoria fisica \n");

			att_memoria_fisica(valor_pagina, num_quadro);

			printf("Memória Física atualizada.\n");
			
			printf("Atualizando a TLB e a Tabela de Páginas\n");

			tabela_paginas[i] = num_quadro;
			num_quadro++;

			atualizar_tlb(valor_pagina,tabela_paginas[i]);
			
			printf("TLB e Tabela de páginas atualizada.\n");
			return tabela_paginas[i];	
		}
		
		/*Não existe o frame da pagina que foi procurada*/
		else if(i == valor_pagina && (tabela_paginas[i] == 0)){
			erros_pagina++;

			printf("Pagina não iniciada na memoria. Gerador de erro de pagina.\n");

			printf("Atualizando a memoria fisica \n");

			att_memoria_fisica(valor_pagina, num_quadro);

			printf("Memória fisica atualizada \n");

			printf("Atualizando a TLB e a Tabela de páginas.\n");

			tabela_paginas[i] = num_quadro;
			num_quadro++;
			
			atualizar_tlb(valor_pagina,tabela_paginas[i]);

			printf("TLB e Tabela de páginas atualizada.\n");
			return tabela_paginas[i];
		}
		/*Ja tem o frame para a pagina que foi procurada*/
		else if(i == valor_pagina){
			printf("Pagina encontrada na memoria.\n");
			printf("Atualizando a TLB com um novo numero de pagina...\n");
			atualizar_tlb(valor_pagina,tabela_paginas[i]);
			printf("TLB atualizada.\n");
			return tabela_paginas[i];
		}
		i++;
	}
}

unsigned char verificar_tlb(unsigned char valor_pagina){ //Verificar se esta na tlb 

	unsigned char i=0;

	printf("Verificando o numero da pagina na TLB...\n");

	if(init() == 1){
		while(i<16){ //16 entradas na TLB
			if(num_paginas_tlb[i] == valor_pagina){
				sucesso_tlb++;
				printf("Frame encontrado %u na TLB.\n",num_quadros_tlb[i]);
				return num_quadros_tlb[i];
			}
			i++;
		}
	}
	printf("Numero da pagina não encontrado na TLB.\n");
	printf("Abrindo a Tabela de Páginas...\n");

	return verf_tabela_paginas(valor_pagina);
}

//Aplicação main
int main(int argc, char *argv[]){
	

	if(argc < 2){
		return -1;
	}

	FILE *fp;
	
	fp = fopen(argv[1], "r");


	if(fp == NULL){
		printf("Error ao ler arquivo %s\n", argv[1]);
		return -1;
	}

	bfptr = fopen("BACKING_STORE.bin", "rb");
	
	if(bfptr == NULL){
		printf("Erro ao ler BACKING_STORE.bin\n");
		return -1;
	}

	int valor_lido; //Valor lido no arquivo addresses que será separado por mascara
	unsigned short end_logico; //1. O endereço lógico que está sendo traduzido (o valor inteiro que está sendo lido em addresses.txt)
	unsigned char valor_pagina; // 2. Valor da pagina a partir do endereço virtual;
	unsigned char valor_quadro; // 2. Os valores de quadro 
	unsigned char offset; //3. O valor do offset traduzido a partir do endereço virtual;
	unsigned short i;
	unsigned short end_memoria; //4. O endereço físico correspondente (aquele para o qual seu programa traduziu o endereço lógico); 
	unsigned char valor_byte; // 5. O valor do byte sinalizado armazenado no endereço físico resultante.
	double taxa_pagina_erro; //1. Taxa de erros de página – o percentual de referências de endereços que resultaram em erros de página; 
	double taxa_tlb_sucesso; //2. Taxa de sucesso do TLB – o percentual de referências de endereços que foram resolvidas no TLB.  
	do{
		while(fscanf(fp,"%d",&valor_lido) != EOF){
			contador_acessos_memoria++;
			printf("valor lido do address.txt: %d\n",valor_lido);

			/*Aplicando as mascaras para separar os bits de offset e data*/
			end_logico = 0xFFFF & valor_lido; // Mascara 16 bits da extrema direita e pega os 16 bits do endereço lógico
			valor_pagina = (0xFF00 & end_logico) >> 8; //Pega os 8 bits do número da página
			offset = 0x00FF & end_logico; //Pega os 8 bits do deslocamento(offset)
			printf("\n 1 - Endereço lógico: %u \n 2 - Numero da pagina: %u \n 3 - Offset: %u\n",end_logico,valor_pagina,offset);
			valor_quadro = verificar_tlb(valor_pagina);
			printf(" 2 - Valor do quadro encontrado: %u\n", valor_quadro);
			end_memoria = valor_quadro*256 + offset;
			printf(" 5 - Endereço Físico: %u\n", end_memoria);
			valor_byte = leitura_memoria_fisica(end_memoria);
			printf(" 6 - Valor do Byte: %u\n", valor_byte);
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
