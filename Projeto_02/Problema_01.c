#include "lib.h"

int main(int argc, char *argv[]){


	if(argc < 2){
		printf("Use: ./... addresses.txt\n");
		return -1;
	}

	int read_value;
	uint16_t logical_addr;
	uint8_t page_number;
	uint8_t offset;
	uint8_t frame_number;
	uint16_t i;
	uint16_t memory_position;
	uint8_t data_read;
	double error_page_rate;
	double tlb_success_rate;
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
		while(fscanf(fp,"%d",&read_value) != EOF){
			memory_access_counter++;
			printf("read value: %d\n",read_value);
			logical_addr = 0xFFFF & read_value;
			page_number = (0xFF00 & logical_addr) >> 8;
			offset = 0x00FF & logical_addr;
			printf("\nlogical addr: %u \npage number: %u \noffset: %u\n",logical_addr,page_number,offset);
			frame_number = verify_tlb(page_number);
			printf("Frame number found: %u\n", frame_number);
			memory_position = frame_number*256 + offset;
			printf("Memory position found: %u\n", memory_position);
			data_read = read_physical_memory(memory_position);
			printf("Data read: %u\n", data_read);
			printf("\n");
		}

		error_page_rate = page_error/(memory_access_counter*1.0);
		tlb_success_rate = tlb_success/(memory_access_counter*1.0);

		printf("Error page rate: %.2lf\n", error_page_rate*100);
		printf("Tlb success rate: %.2lf\n", tlb_success_rate*100);
		fclose(fp);
		fclose(bfptr);
		return 0;
			
	}while(fp != NULL && bfptr != NULL);
}