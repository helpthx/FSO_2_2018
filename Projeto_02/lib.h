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

FILE *bfptr; //backing store memory file pointer

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

	/*Initializing the tlb*/
	if((frame_number == 0) && (last_updated_in_tlb == 0)){
		tlb_page_number[0] = page_number;
		tlb_frame_number[0] = frame_number;
		printf("TLB updated: Position %u \nPage Number: %u \nFrame Number: %u\n", last_updated_in_tlb, tlb_page_number[0], tlb_frame_number[0]);
		last_updated_in_tlb++;
	}
	else{
		tlb_page_number[last_updated_in_tlb] = page_number;
		tlb_frame_number[last_updated_in_tlb] = frame_number;
		printf("TLB updated: Position %u \nPage Number: %u \nFrame Number: %u\n", last_updated_in_tlb, tlb_page_number[last_updated_in_tlb], tlb_frame_number[last_updated_in_tlb]);
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
		
		/*There's no frame used. We have to initialize the page table*/
		if((i == page_number) && (number_frames_used == 0)){
			printf("Page not initialized in memory yet. Page error generated.\n");
			
			printf("Updating physical memory with the new frame...\n");
			update_physical_memory(page_number, number_frames_used);
			printf("Updated.\n");
			//printf("The page table was not initialized and will be initialized.\n");
			page_table[i] = number_frames_used;
			number_frames_used++;

			/*update the tlb*/
			printf("Updating the tlb with the new page number...\n");
			
			update_tlb(page_number,page_table[i]);
			
			printf("Done.\n");
			return page_table[i];	
		}
		/*There's no frame for the page we're searching now*/
		else if(i == page_number && (page_table[i] == 0)){
			page_error++;
			printf("Page not initialized in memory yet. Page error generated.\n");
			//printf("There's a frame free and the page table will be updated.\n");
			printf("Updating physical memory with the new frame...\n");
			update_physical_memory(page_number, number_frames_used);
			printf("Updated.\n");
			page_table[i] = number_frames_used;
			number_frames_used++;
			
			printf("Updating the tlb with the new page number...\n");
			update_tlb(page_number,page_table[i]);
			printf("Done.\n");
			return page_table[i];
		}
		/*There's already a frame for the page we're searching*/
		else if(i == page_number){
			printf("Page found in memory.\n");
			printf("Updating the tlb with the new page number...\n");
			update_tlb(page_number,page_table[i]);
			printf("Done.\n");
			return page_table[i];
		}
	}
}

uint8_t verify_tlb(uint8_t page_number){

	uint8_t i;

	printf("Verifying for page number in TLB...\n");

	if(is_initialized() == 1){
		for(i = 0; i < 16; i++){

			if(tlb_page_number[i] == page_number){
				tlb_success++;
				printf("Found frame %u in TLB.\n",tlb_frame_number[i]);
				return tlb_frame_number[i];
			}
		}
	}
	printf("Page number not found in TLB.\n");
	printf("Opening the page table...\n");
	return verify_page_table(page_number);
}
