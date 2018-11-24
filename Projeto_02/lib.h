#include <stdio.h>
#include <stdint.h>

uint8_t tlb_page_number[16] = {0};
uint8_t tlb_frame_number[16]= {0};
uint8_t page_table[256] = {0};
uint8_t number_frames_used = 0;
uint8_t last_updated_in_tlb = 0;
uint8_t physical_memory[65536] = {0};
unsigned int page_error = 0;
unsigned int tlb_success = 0;
unsigned int memory_access_counter = 0;

FILE *bfptr; //Criando o ponteiro para o arquivo do Problema_01.c

uint8_t read_physical_memory(uint16_t memory_position){

	uint8_t data_read;
	data_read = physical_memory[memory_position];
	return data_read;
}

int is_initialized(void){
	
	int true1 = (tlb_page_number[0] == 0) && (tlb_frame_number[0] == 0);
	int true2 = (tlb_page_number[1] == 0) && (tlb_frame_number[1] == 0);

	if(true1 == 1 && true2 == 1){
		return 0;
	}
	else{
		return 1;
	}
}

void update_tlb(uint8_t page_number, uint8_t frame_number){

	/*Iniciando a TLB*/
	if((frame_number == 0) && (last_updated_in_tlb == 0)){
		tlb_page_number[0] = page_number;
		tlb_frame_number[0] = frame_number;
		printf("Atualização da TLB: Posição %u \nNumero da pagina: %u \nNumero do frame: %u\n", last_updated_in_tlb, tlb_page_number[0], tlb_frame_number[0]);
		last_updated_in_tlb++;
	}
	else{
		tlb_page_number[last_updated_in_tlb] = page_number;
		tlb_frame_number[last_updated_in_tlb] = frame_number;
		printf("Atualização da TLB: Posição %u \nNumero da pagina: %u \nNumero do frame: %u\n", last_updated_in_tlb, tlb_page_number[last_updated_in_tlb], tlb_frame_number[last_updated_in_tlb]);
		last_updated_in_tlb = (last_updated_in_tlb + 1) % 16;
	}

}

void update_physical_memory(uint8_t page_number,uint8_t frame_number){

	uint16_t i = 0;
	fseek(bfptr, page_number*256, SEEK_SET);

    for(i = 0; i < 256; i++){
        fread(&physical_memory[frame_number*256 + i], 1, 1, bfptr);
    }
}

uint8_t verify_page_table(uint8_t page_number){

	uint8_t i;
	uint8_t j;

	for(i = 0; i < 256; i++){
		
		/*Não existe um frame. Deve-se inicar a tabela de pagina*/
		if((i == page_number) && (number_frames_used == 0)){
			printf("Pagina não iniciada na memoria. Gerador de erro de pagina.\n");
			
			printf("Atualizando a memoria fisica com um novo frame...\n");
			update_physical_memory(page_number, number_frames_used);
			printf("Updated.\n");
			
			page_table[i] = number_frames_used;
			number_frames_used++;

			/*Atualizando a TLB*/
			printf("Atualizando a TLB com um novo numero de pagina...\n");
			
			update_tlb(page_number,page_table[i]);
			
			printf("Feito.\n");
			return page_table[i];	
		}
		
		/*Não existe o frame da pagina que foi procurada*/
		else if(i == page_number && (page_table[i] == 0)){
			page_error++;
			printf("Pagina não iniciada na memoria. Gerador de erro de pagina.\n");
			//printf("There's a frame free and the page table will be updated.\n");
			printf("Atualizando a memoria fisica com um novo frame...\n");
			update_physical_memory(page_number, number_frames_used);
			printf("Atualizado.\n");
			page_table[i] = number_frames_used;
			number_frames_used++;
			
			printf("Atualizando a TLB com um novo numero de pagina...\n");
			update_tlb(page_number,page_table[i]);
			printf("Feito.\n");
			return page_table[i];
		}
		/*Ja tem o frame para a pagina que foi procurada*/
		else if(i == page_number){
			printf("Pagina encontrada na memoria.\n");
			printf("Atualizando a TLB com um novo numero de pagina...\n");
			update_tlb(page_number,page_table[i]);
			printf("Feito.\n");
			return page_table[i];
		}
	}
}

uint8_t verify_tlb(uint8_t page_number){

	uint8_t i;

	printf("Verificando o numero da pagina na TLB...\n");

	if(is_initialized() == 1){
		for(i = 0; i < 16; i++){

			if(tlb_page_number[i] == page_number){
				tlb_success++;
				printf("Frame encontrado %u na TLB.\n",tlb_frame_number[i]);
				return tlb_frame_number[i];
			}
		}
	}
	printf("Numero da pagina não encontrado na TLB.\n");
	printf("Abrindo a tabela de pagina...\n");
	return verify_page_table(page_number);
}
