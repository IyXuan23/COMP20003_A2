#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "functions.h"

//function will replace all the commas that are within quotation marks with "*" char
//so that it will be easier to scanf into seperate variables
int
replace_commas(char target_str[], int str_len){

    int within_quotes = 0;
    int has_quotes = 0;

    for (int i = 0; i < str_len; i++) {
			
		//replace commas in quotation marks with asterik
		//for easier sscanf
		if (target_str[i] == ',' && within_quotes == 1) {

			target_str[i] = '*';
		} 
			
		//check to see if it is opening or closing quotation marks
		if (target_str[i] == '\"') {

			if (within_quotes == 0) {

				within_quotes = 1;
				has_quotes = 1;
			}
			else {

				within_quotes = 0;
			}
		}
	}
    return has_quotes;
}

//function will remove the quotation marks, and also change the "*" symbols inside the string
//back to ","
void
change_asterik_remove_quotes(char target_str[]) {

	//check if str is within quotations, if not, exit
	if (target_str[0] != '\"') {
		return;
	}

    int str_len = strlen(target_str);

    for (int i = 0; i < str_len; i++) {

        //move all the values forwards to remove quotation marks
         target_str[i] = target_str[i + 1];

         if (target_str[i] == '*') {
            
             target_str[i] = ',';
         }
    }
    //place null byte to end the string
    target_str[str_len - 2] = '\0';
}

//function to scan values from the input string, which includes checks for empty strings
void 
scan_values(char temp_str[], int* footpath_id, char* address, char* clue_sa, char* asset_type, double* 
			deltaz, double* distance, double* grade1in, double* mcc_id, double* mccid_int, double* 
			rlmax, double* rlmin, char* segside, double* statusid, double* streetid, double* 
			street_group, double* start_lat, double* start_lon, double* end_lat, double* end_lon, int 
			str_len) {

	char* temp_str_ptr;			
	int ptr_index = 0;
	int comma_num = 0;

	//scans footpath_id; first value
	sscanf(temp_str, "%d,", footpath_id);

	//check whether the strs are empty, if not scan them in, if they are, set the variable to empty
	//in order to remove previously saved string
	for (ptr_index = 0; ptr_index < str_len; ptr_index++) {

		if (temp_str[ptr_index] == ',') {
			temp_str_ptr = temp_str;
			comma_num++;

			//2 commas side by side means str is empty
			if (comma_num == 1) {
				if (temp_str[ptr_index + 1] != ',') {
					sscanf(temp_str_ptr + ptr_index + 1, "%128[^,]", address);
				}	
				else {
					address[0] = '\0';
				}	
			}

			if (comma_num == 2) {
				if (temp_str[ptr_index + 1] != ',') {
					sscanf(temp_str_ptr + ptr_index + 1, "%128[^,]", clue_sa);
				}
				else {
					clue_sa[0] = '\0';
				}	
			}

			if (comma_num == 3) {
				if (temp_str[ptr_index + 1] != ',') {
					sscanf(temp_str_ptr + ptr_index + 1, "%128[^,]", asset_type);
				}
				else {
					asset_type[0] = '\0';
				}	
			}

			//scan in the non-str values
			if (comma_num == 4) {
				sscanf(temp_str_ptr + ptr_index + 1, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,", 
						deltaz, distance, grade1in, mcc_id, mccid_int, rlmax, rlmin);
			}

			if (comma_num == 11) {
				if (temp_str[ptr_index + 1] != ',') {
					sscanf(temp_str_ptr + ptr_index + 1, "%128[^,]", segside);
				}
				else {
					segside[0] = '\0';
				}	
			}

			//scan remaining non-str values
			if (comma_num == 12) {
				sscanf(temp_str_ptr + ptr_index + 1, "%lf,%lf,%lf,%lf,%lf,%lf,%lf", 
					statusid, streetid, street_group, start_lat, start_lon, end_lat, 
					end_lon);
			}	
		}
	}	
}

