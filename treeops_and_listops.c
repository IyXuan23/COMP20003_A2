#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>

#include "treeops_and_listops.h"

tree_t*
make_empty_tree(int func(long double, long double, long double, long double)) {
	tree_t *tree;
	tree = malloc(sizeof(*tree));
	assert(tree!=NULL);
	/* initialize tree to empty */
	tree->root = NULL;
	/* and save the supplied function pointer */
	tree->cmp = func;        
	return tree;
}

//comparison function used for the quad tree, used to decide which of the 4 child nodes
//should the new node be inserted in
int
cmp(long double midpoint_x, long double midpoint_y, long double longitude, long double latitude) {

	//for SW
	if ((longitude <= midpoint_x - LDBL_EPSILON) && (latitude < midpoint_y - LDBL_EPSILON)) {
		return 0;
	}
	//for NW
	if ((longitude <= midpoint_x - LDBL_EPSILON) && (latitude >= midpoint_y + LDBL_EPSILON)) {
		return 1;
	}
	//for NE
	if ((longitude > midpoint_x + LDBL_EPSILON) && (latitude >= midpoint_y + LDBL_EPSILON)) {
		return 2;
	}
	//for SE
	if ((longitude > midpoint_x + LDBL_EPSILON) && (latitude < midpoint_y - LDBL_EPSILON)) {
		return 3;
	}

	//error value
	return 4;
}

//recursive search to find where the node should be inserted into
tree_node_t*
recursive_insert(tree_node_t *root, tree_node_t *new, int cmp(long double, long double, long double, 
				 long double), long double bot_left_x, long double bot_left_y, long double top_right_x, 
				 long double top_right_y) {
	
	//base case
	if (root==NULL) {
		return new;    
	}

	//special case where the given value is equivalent to existing node
	if (root->SW == NULL && root->NW == NULL && root->NE == NULL && root->SE == NULL) {

		double diff1 = fabs(new->point->longitude - root->point->longitude);
		double diff2 = fabs(new->point->latitude - root->point->latitude);

		if ((diff1 < LDBL_EPSILON) && (diff2 < LDBL_EPSILON)) {
			
			//insert new node footpath into the existing node array
			root->point->footpath_id_array[root->point->num_of_footpaths] = new->point
			->footpath_id_array[0];

			root->point->num_of_footpaths += 1;

			//and then remove the node
			free(new->point);
			free(new);

			return root;
		}
	}

	//if leaf node has not been reached,
	//decide which of the 4 quadrants the new node goes into
	if (root!=NULL) {

		int path = cmp(root->midpoint_x, root->midpoint_y, new->point->longitude, new->point->latitude);

		if (path == 0) {
			root->SW = recursive_insert(root->SW, new, cmp, bot_left_x, bot_left_y, root->midpoint_x, 
										root->midpoint_y);
			//create the new midpoint for the new node
			if (root->SW->midpoint_has_value == 0) {
				root->SW->midpoint_x = create_midpoint(bot_left_x, root->midpoint_x);
				root->SW->midpoint_y = create_midpoint(bot_left_y, root->midpoint_y);

				//add rectangle boundaries for the new node
				root->SW->rect_bot_left_x = bot_left_x;
				root->SW->rect_bot_left_y = bot_left_y;
				root->SW->rect_top_right_x = root->midpoint_x;
				root->SW->rect_top_right_y = root->midpoint_y;

				//update boolean value for midpoint
				root->SW->midpoint_has_value = 1;
			}	
		}
		else if (path == 1) {
			root->NW = recursive_insert(root->NW, new, cmp, bot_left_x, root->midpoint_y, root->midpoint_x, 
										top_right_y);
			//create the new midpoint for the new node
			if (root->NW->midpoint_has_value == 0) {
				root->NW->midpoint_x = create_midpoint(bot_left_x, root->midpoint_x);
				root->NW->midpoint_y = create_midpoint(top_right_y, root->midpoint_y);

				//add rectangle boundaries for the new node
				root->NW->rect_bot_left_x = bot_left_x;
				root->NW->rect_bot_left_y = root->midpoint_y;
				root->NW->rect_top_right_x = root->midpoint_x;
				root->NW->rect_top_right_y = top_right_y;

				//update boolean value for midpoint
				root->NW->midpoint_has_value = 1;				
			}
		}
		else if (path == 2) {
			root->NE = recursive_insert(root->NE, new, cmp, root->midpoint_x, root->midpoint_y, 
										top_right_x, top_right_y);
			//create the new midpoint for the new node
			if (root->NE->midpoint_has_value == 0) {
				root->NE->midpoint_x = create_midpoint(top_right_x, root->midpoint_x);
				root->NE->midpoint_y = create_midpoint(top_right_y, root->midpoint_y);

				//add rectangle boundaries for the new node
				root->NE->rect_bot_left_x = root->midpoint_x;
				root->NE->rect_bot_left_y = root->midpoint_y;
				root->NE->rect_top_right_x = top_right_x;
				root->NE->rect_top_right_y = top_right_y;

				//update boolean value for midpoint
				root->NE->midpoint_has_value = 1;
			}
		}
		else if (path == 3) {
			root->SE = recursive_insert(root->SE, new, cmp, root->midpoint_x, bot_left_y, 
										top_right_x, root->midpoint_y);
			//create the new midpoint for the new node
			if (root->SE->midpoint_has_value == 0) {
				root->SE->midpoint_x = create_midpoint(top_right_x, root->midpoint_x);
				root->SE->midpoint_y = create_midpoint(bot_left_y, root->midpoint_y);

				//add rectangle boundaries for the new node
				root->SE->rect_bot_left_x = root->midpoint_x;
				root->SE->rect_bot_left_y = bot_left_y;
				root->SE->rect_top_right_x = top_right_x;
				root->SE->rect_top_right_y = root->midpoint_y;

				//update boolean value for midpoint
				root->SE->midpoint_has_value = 1;				
			}
		}
	}

	//if node has parent nodes with values, parent node values should be moved to new leaf node
	if (root->SW != NULL || root->SE != NULL || root->NW != NULL || root->NE != NULL) {
		if (root->point != NULL) {

			//create a new node containing the point, update the parent node point to NULL
			//parent node will only contain midpoint and rectangle coords
			//then recursively insert the new node into the tree
			tree_node_t* another_node = create_new_node(root->point);
			root->point = NULL;
			root = recursive_insert(root, another_node, cmp, bot_left_x, bot_left_y, 
									top_right_x, top_right_y);
		}
	}
	return root;
}

//create a new node, with a point, and recursively insert it into the quadtree
tree_t*
insert_in_order(tree_t *tree, double long_value, double lat_value, long double bot_left_x, long double 
				bot_left_y, long double top_right_x, long double top_right_y, int footpath_id) {

	/*make new point*/
	point_t* newpoint = create_new_point(long_value, lat_value, footpath_id);

	assert(tree!=NULL);

	tree_node_t* new = create_new_node(newpoint);

	/* and insert it into the tree */
	tree->root = recursive_insert(tree->root, new, tree->cmp, bot_left_x, bot_left_y, 
								  top_right_x, top_right_y);

	//code for the first node, where midpoint is not generated due to lack of recursion
	if (tree->root->midpoint_has_value == 0) {
		tree->root->midpoint_x = create_midpoint(top_right_x, bot_left_x);
		tree->root->midpoint_y = create_midpoint(bot_left_y, top_right_y);
		tree->root->midpoint_has_value = 1;	

		//also set the boundaries of the rectangle the node is in
		tree->root->rect_bot_left_x = bot_left_x;
		tree->root->rect_bot_left_y = bot_left_y;
		tree->root->rect_top_right_x = top_right_x;
		tree->root->rect_top_right_y = top_right_y;			
	}
	return tree;
}

//function calculates and return midpoint of 2 points
long double
create_midpoint(long double point1, long double point2) {
	return (long double)(point1 + point2)/2;
}

void
free_tree(tree_t *tree) {
	assert(tree!=NULL);
	recursive_free_tree(tree->root);
	free(tree);
}

void
recursive_free_tree(tree_node_t *root) {
	if (root) {
		recursive_free_tree(root->SW);
		recursive_free_tree(root->NW);
		recursive_free_tree(root->NE);
		recursive_free_tree(root->SE);
		if (root->point) {
			free(root->point);
		}
		free(root);
	}
}

//check whether the point lies in the given boundary
int check_if_in_rectangle(long double bot_left_x, long double bot_left_y, long double top_right_x, 
				 		  long double top_right_y, double longitude, double latitude) {

	if ((longitude <= top_right_x - LDBL_EPSILON) && (longitude > bot_left_x + LDBL_EPSILON)) {
		if ((latitude < top_right_y - LDBL_EPSILON) && (latitude >= bot_left_y + LDBL_EPSILON)) {
			return 1;
		}
	}							   
	return 0;
}

//function that creates a new node, containin the given point
tree_node_t* 
create_new_node(point_t* point) {

	tree_node_t* new;
	/* make the new node */
	new = malloc(sizeof(*new));
	assert(new!=NULL);

	new->NW = new->NE = new->SE = new->SW = NULL;
	new->midpoint_has_value = 0;
	new->point = point;

	return new;
}

//function that creates a new point, given longitude, latitude and footpath_id
point_t*
create_new_point(double long_value, double lat_value, int footpath_id) {

	point_t* newpoint;
	newpoint = malloc(sizeof(*newpoint));
	assert(newpoint!=NULL);

	newpoint->longitude = long_value;
	newpoint->latitude = lat_value;
	newpoint->footpath_id_array[0] = footpath_id;
	newpoint->num_of_footpaths = 1;

	return newpoint;
}

//search the quadtree for a given point, prints the path taken, returning the leaf node
tree_node_t*
search_tree(double longitude, double latitude, tree_node_t* root, int cmp(long double, long double, 
			   long double, long double), FILE* output_file) {
	
	//leaf node with a value
	if (root->SW == NULL && root->NW == NULL && root->NE == NULL && root->SE == NULL) {
		return root;
	}

	//if its not a leaf node
	int path = cmp(root->midpoint_x, root->midpoint_y, longitude, latitude);

	//if value is in SW
	if (path == 0) {
		printf(" SW");
		if (root->SW != NULL) {
			return search_tree(longitude, latitude, root->SW, cmp, output_file);
		}
	}
	//if value is in NW
	else if (path == 1) {
		printf(" NW");
		if (root->NW != NULL) {
			return search_tree(longitude, latitude, root->NW, cmp, output_file);
		}
	}
	//if value is in NE
	else if (path == 2) {
		printf(" NE");
		if (root->NE != NULL) {
			return search_tree(longitude, latitude, root->NE, cmp, output_file);
		}
	}
	//if value is in SE
	else if (path == 3) {
		printf(" SE");
		if (root->SE != NULL) {
			return search_tree(longitude, latitude, root->SE, cmp,  output_file);
		}
	}
	//error case
	return NULL;
}

//rect1 will be our tree rectangle, and rect2 will be the query rectangle
int
rectangleOverlap(long double rect1_bot_left_x, long double rect1_bot_left_y, long double rect1_top_right_x, 
				 long double rect1_top_right_y, long double rect2_bot_left_x, long double rect2_bot_left_y, 
				 long double rect2_top_right_x, long double rect2_top_right_y) {

	if (rect2_bot_left_y > rect1_top_right_y + LDBL_EPSILON || 
		rect1_bot_left_y > rect2_top_right_y + LDBL_EPSILON || 
		rect1_bot_left_x > rect2_top_right_x + LDBL_EPSILON || 
		rect2_bot_left_x > rect1_top_right_x + LDBL_EPSILON) {

			return 0;
	}
	else {
		return 1;
	}
}

//function checks whether a given point sits within a rectangle and returns 1 if true
int
inRectangle(long double rect_bot_left_x, long double rect_bot_left_y, long double rect_top_right_x, 
			long double rect_top_right_y, double longitude, double latitude) {
			
	if ((longitude >= rect_bot_left_x + LDBL_EPSILON) && (longitude <= rect_top_right_x - LDBL_EPSILON) && 
		(latitude >= rect_bot_left_y + LDBL_EPSILON) && (latitude <= rect_top_right_y - LDBL_EPSILON)) {
			return 1;
		}
	else {
		return 0;
	}	
}

//function to travel through the tree, checking whether each rectangle is still overlapping,
//if not stop the recursion
int
recursive_overlapping_rect_traverse(tree_node_t* root, long double rect_bot_left_x, long double 	
									rect_bot_left_y, long double rect_top_right_x, 
									long double rect_top_right_y, int print_array[], int counter) {
	if (root != NULL) {
		
		//if we have reached the leaf node
		if (root->NW == NULL && root->NE == NULL && root->SE == NULL && root->SW == NULL) {
			
			//check whether point lies within the query
			if (inRectangle(rect_bot_left_x, rect_bot_left_y, rect_top_right_x, rect_top_right_y, 
				root->point->longitude, root->point->latitude) == 1) {	

				//store the footpath_id's in an array, which we will use for printing
				for (int i = 0; i < root->point->num_of_footpaths; i++) {
					
					print_array[counter] = root->point->footpath_id_array[i];
					counter++;
				}

				return counter;
			}
		}	

		else {
		//travel in the order of SW, NW, NE, SE
			if (root->SW != NULL && rectangleOverlap(rect_bot_left_x, rect_bot_left_y, 
				rect_top_right_x, rect_top_right_y, root->SW->rect_bot_left_x, 
				root->SW->rect_bot_left_y, root->SW->rect_top_right_x, 
				root->SW->rect_top_right_y) == 1) {

				printf(" SW");
				counter = recursive_overlapping_rect_traverse(root->SW, rect_bot_left_x, rect_bot_left_y, 
															  rect_top_right_x, rect_top_right_y, 
															  print_array, counter);
			}
			if (root->NW != NULL && rectangleOverlap(rect_bot_left_x, rect_bot_left_y, 
				rect_top_right_x, rect_top_right_y, root->NW->rect_bot_left_x, 
				root->NW->rect_bot_left_y, root->NW->rect_top_right_x, 
				root->NW->rect_top_right_y) == 1) {	

				printf(" NW");																
				counter = recursive_overlapping_rect_traverse(root->NW, rect_bot_left_x, rect_bot_left_y, 
															  rect_top_right_x, rect_top_right_y, 	
															  print_array, counter);
			}
			if (root->NE != NULL && rectangleOverlap(rect_bot_left_x, rect_bot_left_y, 
				rect_top_right_x, rect_top_right_y, root->NE->rect_bot_left_x, 
				root->NE->rect_bot_left_y, root->NE->rect_top_right_x, 
				root->NE->rect_top_right_y) == 1) {	

				printf(" NE");														
				counter = recursive_overlapping_rect_traverse(root->NE, rect_bot_left_x, rect_bot_left_y, 
															  rect_top_right_x, rect_top_right_y, 		
															  print_array, counter);
			}
			if (root->SE != NULL && rectangleOverlap(rect_bot_left_x, rect_bot_left_y, 
				rect_top_right_x, rect_top_right_y, root->SE->rect_bot_left_x, 
				root->SE->rect_bot_left_y, root->SE->rect_top_right_x, 
				root->SE->rect_top_right_y) == 1) {		

				printf(" SE");													
				counter = recursive_overlapping_rect_traverse(root->SE, rect_bot_left_x, rect_bot_left_y, 
															rect_top_right_x, rect_top_right_y, 			
															print_array, counter);	
			}
		}	
	}
	return counter;
}

//traverse the tree as long as the tree is overlapping with the given region query
int
traverse_overlapping_rect_tree(tree_t* tree, long double rect_bot_left_x, long double rect_bot_left_y, 
							   long double rect_top_right_x, long double rect_top_right_y, 
							   int print_array[]) {

	int counter = 0;

	assert(tree!=NULL);
	counter = recursive_overlapping_rect_traverse(tree->root, rect_bot_left_x, rect_bot_left_y, 
												  rect_top_right_x, rect_top_right_y, print_array, 
												  counter);
	return counter;
}


//LISTOPS BEGIN HERE-----------------------------------------------------------------------------------

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
		if (curr->footpath_id == footpath_id && curr->already_printed == 0) {
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

			curr->already_printed = 1;
		}
		curr = curr->next;
	}
}


//compare function for qsort, used in dict3 and dict4 to sort the footpath_id_array before printing
int 
compare_func2(const void* int1, const void* int2) {

	//arguments are ptrs to my array of node ptrs; dereference needed

	int footpath1 = *(int*)int1;

	int footpath2 = *(int*)int2;

	if (footpath1 > footpath2) {
		return 1;
	}
	if (footpath1 < footpath2) {
		return -1;
	}
	else {
		return 0;
	}
}

/* tree operations and list operations
 * Alistair Moffat, PPSAA, Chapter 10, December 2012
 * (c) University of Melbourne */
 /*further adapted by Heng Yi Xuan for COMP20003 Assignment 2*/
