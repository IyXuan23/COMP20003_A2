#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "functions.h"
#include "treeops_and_listops.h"

int
main(int argc, char *argv[]) {

	FILE* input_file;
	FILE* output_file;
	
	//holding the str from input file for processing
	char temp_str[TEMP_STRING_LEN];
	//ptr for the strtold func to work
	char* ptr;

	list_t* street_list;
	tree_t* quadtree;

	//list data variables
	int footpath_id;
	char address[HOLDING_STR_LEN + 1];
	char clue_sa[HOLDING_STR_LEN + 1];
	char asset_type[HOLDING_STR_LEN + 1];
	double deltaz, distance, grade1in;
	double mcc_id, mccid_int; //will be int in struct, double for easier scanning
	double rlmax, rlmin;
	char segside[HOLDING_STR_LEN + 1];
	double statusid, streetid, street_group; //will be int in struct, double for easier scanning
	double start_lat, start_lon, end_lat, end_lon;

	//for storing the edges of the rectangle
	long double bot_left_x;
	long double bot_left_y;
	long double top_right_x;
	long double top_right_y;
	
	//get the file name to open
	input_file = fopen(argv[2], "r");
	assert(input_file);

	output_file = fopen(argv[3], "w");
	assert(output_file);

	bot_left_x = strtold(argv[4], &ptr);
	bot_left_y = strtold(argv[5], &ptr);
	top_right_x = strtold(argv[6], &ptr);
	top_right_y = strtold(argv[7], &ptr);

	//list created
	street_list = make_empty_list();

	//tree created
	quadtree = make_empty_tree(cmp);

	//begin scanning in data
	//skip header
	fgets(temp_str, TEMP_STRING_LEN, input_file);

	//scans row by row and save values
	while (fgets(temp_str, TEMP_STRING_LEN, input_file) != NULL) {

		int has_quotes = 0;
		int str_len = strlen(temp_str);

		//check whether the string contains <""> quotation marks
		//if so, replace commas within quotes for easier scanning
		has_quotes = replace_commas(temp_str, str_len);

		scan_values(temp_str, &footpath_id, address, clue_sa, asset_type, &deltaz, &distance, &grade1in, 
		&mcc_id, &mccid_int, &rlmax, &rlmin, segside, &statusid, &streetid, &street_group, &start_lat, 
		&start_lon, &end_lat, &end_lon, str_len);


		//revert the "changed commas" within quotes and remove quotations marks
		if (has_quotes != 0) {
			
			change_asterik_remove_quotes(address);
			change_asterik_remove_quotes(clue_sa);
			change_asterik_remove_quotes(asset_type);
			change_asterik_remove_quotes(segside);
		}

		//insert node into list
		street_list = insert_at_foot(street_list, footpath_id, address, clue_sa, asset_type, deltaz, 
		distance, grade1in, mcc_id, mccid_int, rlmax, rlmin, segside, statusid, streetid, street_group, 
		start_lat, start_lon, end_lat, end_lon);
	}

	
    //go through the linked list, and for each node, create a tree_node for the starting and ending
	//long and lat values
	node_t* curr = street_list->head;
	while (curr != NULL) {
		if (check_if_in_rectangle(bot_left_x, bot_left_y, top_right_x, top_right_y, curr->start_lon, 
								  curr->start_lat) == 1) {
			quadtree = insert_in_order(quadtree, curr->start_lon, curr->start_lat, bot_left_x, bot_left_y, 
								   	   top_right_x, top_right_y, curr->footpath_id);
		}
		if (check_if_in_rectangle(bot_left_x, bot_left_y, top_right_x, top_right_y, curr->end_lon, 
								  curr->end_lat) == 1) {								  
			quadtree = insert_in_order(quadtree, curr->end_lon, curr->end_lat, bot_left_x, bot_left_y, 		
								   	   top_right_x, top_right_y, curr->footpath_id);
		}								  
		curr = curr->next;						   
	}

	while (fgets(temp_str, HOLDING_STR_LEN, stdin) != NULL) {
		
		//update boolean value; used ensure no printing of duplicates
		curr = street_list->head;
		while (curr != NULL){
			curr->already_printed = 0;
			curr = curr->next;
		}

		//rect2 here refers to our query rectangle
		long double rect2_bot_left_x;
		long double rect2_bot_left_y;
		long double rect2_top_right_x;
		long double rect2_top_right_y;

		//counter will be used to track num of points in the array
		int counter;
		int print_array[MAX_DATA_POINTS];

		//save rectangle coords
		sscanf(temp_str, "%Lf %Lf %Lf %Lf", &rect2_bot_left_x, &rect2_bot_left_y, 
			   &rect2_top_right_x, &rect2_top_right_y);

		//remove \n from the string
		temp_str[strlen(temp_str) - 1] = '\0';

		printf("%s -->", temp_str);
		fprintf(output_file, "%s\n", temp_str);

		//check whether the rectangle overlaps our tree, if it does start the search
		if (rectangleOverlap(rect2_bot_left_x, rect2_bot_left_y, rect2_top_right_x, rect2_top_right_y, 
			quadtree->root->rect_bot_left_x, quadtree->root->rect_bot_left_y, 
			quadtree->root->rect_top_right_x, quadtree->root->rect_top_right_y) == 1) {
			
			counter = traverse_overlapping_rect_tree(quadtree, rect2_bot_left_x, rect2_bot_left_y, 
													 rect2_top_right_x, rect2_top_right_y, print_array);

			//sort the array, so that footpaths can be printed in order
			qsort(print_array, counter, sizeof(int), compare_func2);

			for (int i = 0; i < counter; i++) {
				find_footpath(output_file, print_array[i], street_list);
			}								 

			printf("\n");
		}
	}
	
	free_list(street_list);	
	free_tree(quadtree);

	fclose(input_file);
	fclose(output_file);
}	
