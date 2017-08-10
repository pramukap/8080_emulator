#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

FILE* raw, assembly;

void main(){
	char val; //stores char obtained from raw file
	
	//store_en -> determines what should happen to current char from raw file
	//0 -> reset, "0" goto 1, if store_en not set to store val then goto 0
	//1 -> "0", "x" goto 2, anything else goto 0
	//2 -> "0x" hex_temp <- a then goto 3
	//3 -> "0x"a hex_temp <-b then goto 4
	//4 -> "0x"ab, if val is "," then hex <- hex_temp and goto 0, if anything else goto 0
	int store_en = 0; 

	raw = fopen("SI_ROM.txt", "r"); //original file containing hex code + garbage
	
	int hex_size = 2; //size of hex code arraymeasured in bytes
	int hex_index = 0; //current index of hex array

	//hex array
	//stores each 2 hit (hex digit) code sequentially
	//increment index by 2
	//index of hex code is location in ROM
	char* hex = malloc(sizeof(char)*hex_size);
	char hex_temp[2] = {'0','0'}; //temporary storage array
	
	while(!feof(raw)){
		//Print entire file
		/*
		printf("%c", val);
		val = fgetc(hex);
		x++;
		*/

		//Process raw file, store hex in array
		val = fgetc(raw);
		//printf("%c %i\n", store_en);
		//printf("%c", val);

		if(store_en == 0){
			if(val == '0'){
				store_en = 1;
				continue;
			}
			else if(store_en != 2 || store_en != 3){
				store_en = 0;
				continue;
			}
		}
		else if(store_en == 1){
			if(val == 'x'){
				store_en = 2;
				continue;
			}
			else {
				store_en = 0;
				continue;
			}
		}
		else if(store_en == 2){
			hex_temp[0] = val;
			store_en = 3;
			continue;
		}
		else if(store_en == 3){
			hex_temp[1] = val;
			store_en = 4;
			continue;
		}
		else if(store_en == 4){
			if(val == ',' || val == ']'){
				hex_size += 2;
				hex = realloc(hex, sizeof(char)*hex_size);
				hex[hex_index + 0] = hex_temp[0];
				hex[hex_index + 1] = hex_temp[1];
				hex_index += 2;
				
				printf("0x%c%c \n", hex[hex_index-2], hex[hex_index-1]);

				store_en = 0;
				continue;
			}
			else {
				store_en = 0;
			}
		}

	
		//original code for processing raw
		/*
		if(val == '0'){
			if(store_en == 0){
				store_en = 1;
				continue;
			}
			else if(store_en != 2 && store_en != 3) {
				store_en = 0;
				continue;
			}
		}
		if(val == 'x'){
			if(store_en == 1){
				store_en = 2;
				continue;
			}
			else {
				store_en = 0;
				continue;
			}
		}
		if(val == ' '){
			//store to hex array because "0x"ab" " format was met
			if(store_en == 4){
				hex_temp = hex;
				hex = realloc(hex, sizeof(char)*(hex_size + 2));
				hex[hex_index][0] = hex_temp[0][0];
				hex[hex_index][1] = hex_temp[0][1];
				hex_index++;

				store_en = 0;
				continue;
			}
			else {
				store_en = 0;
				continue;
			}
		}

		//store val to hex_temp if store_en = 2 or 3
		if(store_en == 2){
			hex_temp[0][0] = val;
			store_en = 3;
		}
		if(store_en == 3){
			hex_temp[0][1] = val;
			store_en = 4;
		}
		*/			
	}

	printf("Raw file processed. Hex Code captured.\n");
	fclose(raw);

	//interpret hex code into assembly
		

	return;
}
