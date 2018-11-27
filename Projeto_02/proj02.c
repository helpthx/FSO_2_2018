#include "lib.h"

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
			memory_access_counter++;
			printf("valor lido do address.txt: %d\n",valor_lido);
			/*Aplicando as mascaras para separar os bits de offset e data*/
			end_logico = 0xFFFF & valor_lido;
			valor_pagina = (0xFF00 & end_logico) >> 8;
			offset = 0x00FF & end_logico;
			printf("\n 1 - Endereço lógico: %u \n 2 - Numero da pagina: %u \n 3 - Offset: %u\n",end_logico,valor_pagina,offset);
			valor_quadro = verify_tlb(valor_pagina);
			printf(" 2 - Numero do quadros encontrado: %u\n", valor_quadro);
			end_memoria = valor_quadro*256 + offset;
			printf(" 5 - Posição da memoria encontrada: %u\n", end_memoria);
			dados_lidos = read_physical_memory(end_memoria);
			printf(" 6 - Dados lido: %u\n", dados_lidos);
			printf("\n");
		}

		taxa_pagina_erro = page_error/(memory_access_counter*1.0);
		taxa_tlb_sucesso = tlb_success/(memory_access_counter*1.0);
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
