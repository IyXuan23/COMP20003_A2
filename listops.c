#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "listops.h"

list_t
*make_empty_list(void) {
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

int
is_empty_list(list_t *list) {
	assert(list!=NULL);
	return list->head==NULL;
}

void
free_list(list_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {

		//modified to free strs that were malloc from strdup
		free(curr->address);
		free(curr->clue_sa);
		free(curr->asset_type);
		free(curr->segside);

		prev = curr;
		curr = curr->next;
		free(prev);
	}
	
	free(list);
}

//assign values into the node, and then add the node into the linked list
list_t
*insert_at_foot(list_t *list, int footpath_id1, char* address1, char* clue_sa1, char* asset_type1, double 
					deltaz1, double distance1, double grade1in1, int mcc_id1, int mccid_int1, double 
					rlmax1, double rlmin1, char* segside1, int statusid1, int streetid1, int 
					street_group1, double start_lat1, double start_lon1, double end_lat1, double 
					end_lon1) {

	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);

	//modified for more values
	new->footpath_id = footpath_id1;

	new->address = strdup(address1);
    assert(new->address);
	new->clue_sa = strdup(clue_sa1);
    assert(new->clue_sa);
	new->asset_type = strdup(asset_type1);
    assert(new->asset_type);

	new->deltaz = deltaz1;
	new->distance = distance1;
	new->grade1in = grade1in1;
	new->mcc_id = (int)mcc_id1;
	new->mccid_int = (int)mccid_int1;
	new->rlmax = rlmax1;
	new->rlmin = rlmin1;

	new->segside = strdup(segside1);
    assert(new->segside);
    
	new->statusid = (int)statusid1;
	new->streetid = (int)streetid1;
	new->street_group = (int)street_group1;
	new->start_lat = start_lat1;
	new->start_lon = start_lon1;
	new->end_lat = end_lat1;
	new->end_lon = end_lon1;

	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}

//function for building the array with linked list node ptrs
void
fill_arrays_with_nodes(node_t* node_ptr_array[], list_t* street_list) {

	node_t* curr;
	curr = street_list->head;
	int i = 0;

	while (curr) {

		node_ptr_array[i] = curr;
		curr = curr->next;
		i++;
	}
}

//comparison function for qsort
int 
compare_func(const void* node1_ptr, const void* node2_ptr) {

	//arguments are ptrs to my array of node ptrs; dereference needed

	node_t* node1 = *(node_t**)node1_ptr;
	node_t* node2 = *(node_t**)node2_ptr;

	if (node1->grade1in > node2->grade1in) {
		return 1;
	}
	if (node1->grade1in < node2->grade1in) {
		return -1;
	}
	else {
		return 0;
	}
}

//uses a binary search, and comparing the grade1in values to find the closest one to the given number
int 
find_closest_value(node_t* node_ptr_array[], double find_this, int num_nodes, 
						FILE* output_file) {
	
	//uses a binary search algorithm
	int mid;
	int low = 0;
	int high = num_nodes - 1;

	int x;

	while (1){						
		mid = (low + high)/2;
		double curr_diff, higher_diff, lower_diff;

		// if both low and high have converged on the same value, can return it as the answer
		if (low == high) {

			fprintf(output_file, "%g\n", find_this);

			fprintf(output_file, "--> footpath_id: %d || address: %s || clue_sa: %s || asset_type: %s ||" 
			" deltaz: %.2lf || distance: %.2lf || grade1in: %.1lf || mcc_id: %d || mccid_int: %d || "
			"rlmax: %.2lf || rlmin: %.2lf || segside: %s || statusid: %d || streetid: %d "
			"|| street_group: %d || start_lat: %lf || start_lon: %lf || end_lat: %lf || end_lon: %lf ||\n", 
			node_ptr_array[mid]->footpath_id, node_ptr_array[mid]->address, node_ptr_array[mid]->clue_sa, 
			node_ptr_array[mid]->asset_type, node_ptr_array[mid]->deltaz, node_ptr_array[mid]->distance, 
			node_ptr_array[mid]->grade1in, node_ptr_array[mid]->mcc_id, node_ptr_array[mid]->mccid_int, 
			node_ptr_array[mid]->rlmax, node_ptr_array[mid]->rlmin, node_ptr_array[mid]->segside, 
			node_ptr_array[mid]->statusid, node_ptr_array[mid]->streetid, 
			node_ptr_array[mid]->street_group, node_ptr_array[mid]->start_lat, 
			node_ptr_array[mid]->start_lon, node_ptr_array[mid]->end_lat, node_ptr_array[mid]->end_lon);

			printf("%g --> %.1lf\n", find_this, node_ptr_array[mid]->grade1in);
			return mid;
		}

		else {
			
			//check that current value is the closest; smallest absolute difference
			//when compared to the previous and next item
			curr_diff = fabs(node_ptr_array[mid]->grade1in - find_this);

			int check_higher = 1;
			int check_lower = 1;

			//so as to not get a indexing error, if mid happens to be first/last element
			if (mid != num_nodes - 1) {

				higher_diff = fabs(node_ptr_array[mid + 1]->grade1in - find_this);
				x = 1;

				//for the case that there are similar grade1in values, check whether we need to continue
				//searching by checking with the next different grade1in value
				while (curr_diff == higher_diff) {
					printf("stuck here");
					higher_diff = fabs(node_ptr_array[mid + 1 + x]->grade1in - find_this);
					x++;
				}

				if (curr_diff > higher_diff) {
					check_higher = 0;
					low = mid + 1;
				}
			}

			if (mid != 0) {
				lower_diff = fabs(node_ptr_array[mid - 1]->grade1in - find_this);
				x = 1;

				while (curr_diff == lower_diff) {
					lower_diff = fabs(node_ptr_array[mid - 1 - x]->grade1in - find_this);
					x++;
				}

				if (curr_diff > lower_diff) {
					check_lower = 0;
					high = mid - 1;
				}
			}

			//if mid is the best match, print out values
			if (check_higher == 1 && check_lower == 1) {

				fprintf(output_file, "%g\n", find_this);

				fprintf(output_file, "--> footpath_id: %d || address: %s || clue_sa: %s || "
				"asset_type: %s || deltaz: %.2lf || distance: %.2lf || grade1in: %.1lf || mcc_id: %d || " 
				"mccid_int: %d || rlmax: %.2lf || rlmin: %.2lf || segside: %s || statusid: %d || streetid:"
				" %d || street_group: %d || start_lat: %lf || start_lon: %lf || end_lat: %lf || end_lon: "
				"%lf ||\n", 
				node_ptr_array[mid]->footpath_id, node_ptr_array[mid]->address, 
				node_ptr_array[mid]->clue_sa, node_ptr_array[mid]->asset_type, 
				node_ptr_array[mid]->deltaz, node_ptr_array[mid]->distance, 
				node_ptr_array[mid]->grade1in, node_ptr_array[mid]->mcc_id, 
				node_ptr_array[mid]->mccid_int, node_ptr_array[mid]->rlmax, 
				node_ptr_array[mid]->rlmin, node_ptr_array[mid]->segside, 
				node_ptr_array[mid]->statusid, node_ptr_array[mid]->streetid, 
				node_ptr_array[mid]->street_group, node_ptr_array[mid]->start_lat, 
				node_ptr_array[mid]->start_lon, node_ptr_array[mid]->end_lat, 
				node_ptr_array[mid]->end_lon);

				printf("%g --> %.1lf\n", find_this, node_ptr_array[mid]->grade1in);
				return mid;
			}
		}	
	}	
}

void
find_footpath(FILE* output_file, int footpath_id, list_t* street_list) {

	node_t* curr = street_list->head;

	while (curr != NULL) {
		if (curr->footpath_id == footpath_id) {
			fprintf(output_file, "--> footpath_id: %d || address: %s || clue_sa: %s || "
			"asset_type: %s || deltaz: %.2lf || distance: %.2lf || grade1in: %.1lf || mcc_id: %d || " 
			"mccid_int: %d || rlmax: %.2lf || rlmin: %.2lf || segside: %s || statusid: %d || streetid:"
			" %d || street_group: %d || start_lat: %lf || start_lon: %lf || end_lat: %lf || end_lon: "
			"%lf ||\n", 
			curr->footpath_id, curr->address, curr->clue_sa, curr->asset_type, curr->deltaz, 
			curr->distance, 
			curr->grade1in, curr->mcc_id, curr->mccid_int, curr->rlmax, curr->rlmin, curr->segside, 
			curr->statusid, curr->streetid, curr->street_group, curr->start_lat, curr->start_lon, 
			curr->end_lat, curr->end_lon);
		}
		curr = curr->next;
	}
}

/* =====================================================================
   Program written by Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.

   See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
   information.

   Prepared December 2012 for the Revised Edition.
   ================================================================== */
