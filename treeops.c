#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

#include "treeops.h"

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

//comparison function used for the binary tree
//CHANGE, COMPARISON STYLE
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
			
			//insert footpath into the existing array
			root->point->footpath_id_array[root->point->num_of_footpaths] = new->point
			->footpath_id_array[0];
			root->point->num_of_footpaths += 1;

			free(new->point);
			free(new);

			return root;
		}
	}

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

				root->SE->midpoint_has_value = 1;				
			}
		}
	}

	//if node has parent nodes with values, node values should be moved to new leaf node
	if (root->SW != NULL || root->SE != NULL || root->NW != NULL || root->NE != NULL) {
		if (root->point != NULL) {
			//update the parent pointer to NULL and find new leaf node for pointer 
			tree_node_t* another_node = create_new_node(root->point);
			root->point = NULL;
			root = recursive_insert(root, another_node, cmp, bot_left_x, bot_left_y, 
									top_right_x, top_right_y);
		}
	}
	return root;
}

/* Returns a pointer to an altered tree that now includes
   the object "value" in its correct location. */
tree_t*
insert_in_order(tree_t *tree, double long_value, double lat_value, long double bot_left_x, long double 
				bot_left_y, long double top_right_x, long double top_right_y, int footpath_id) {

	/*make new point*/
	point_t* newpoint;
	newpoint = malloc(sizeof(*newpoint));
	assert(newpoint!=NULL);

	newpoint->longitude = long_value;
	newpoint->latitude = lat_value;
	newpoint->footpath_id_array[0] = footpath_id;
	newpoint->num_of_footpaths = 1;

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

//function to travel through the tree and print the value of the longtitude and latitude
//PRINTS LONG AND LAT FOR TESTING
void
recursive_traverse(tree_node_t* root) {
	if (root) {
		recursive_traverse(root->NW);
		recursive_traverse(root->SW);
		if (root->point != NULL) {
			printf("%lf\n", root->point->longitude);
			printf("%lf\n", root->point->latitude);
		}
		recursive_traverse(root->NE);
		recursive_traverse(root->SE);
	}
}

/* Applies the "action" at every node in the tree, in
   the order determined by the cmp function. */
void
traverse_tree(tree_t* tree) {
	assert(tree!=NULL);
	recursive_traverse(tree->root);
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

//similar function to insert, but instead we do not create a new point_t, instead move
//an existing point_t to this new node
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

//function to travel through the tree, checking whether each rectangle is still overlapping,
//if not stop the recursion
void
recursive_overlapping_rect_traverse(tree_node_t* root, long double rect_bot_left_x, long double 	
									rect_bot_left_y, long double rect_top_right_x, 
									long double rect_top_right_y) {
	if (root) {
		
		//if it overlaps
		if (rectangleOverlap(rect_bot_left_x, rect_bot_left_y, rect_top_right_x, rect_top_right_y, 
							 root->rect_bot_left_x, root->rect_bot_left_y, root->rect_top_right_x, 
							 root->rect_top_right_y) == 1) {
			
			//if we have reached the leaf node
			if (root->NW == NULL && root->NE == NULL && root->SE == NULL && root->SW == NULL) {
				
				//print otu footpath
			}
			else {
			//travel in the order of SW, NW, NE, SE
				printf("SW ");
				recursive_overlapping_rect_traverse(root->SW, rect_bot_left_x, rect_bot_left_y, 
													rect_top_right_x, rect_top_right_y);
				printf("NW ");																
				recursive_overlapping_rect_traverse(root->NW, rect_bot_left_x, rect_bot_left_y, 
													rect_top_right_x, rect_top_right_y);
				printf("NE ");														
				recursive_overlapping_rect_traverse(root->NE, rect_bot_left_x, rect_bot_left_y, 
													rect_top_right_x, rect_top_right_y);
				printf("SE ");													
				recursive_overlapping_rect_traverse(root->SE, rect_bot_left_x, rect_bot_left_y, 
													rect_top_right_x, rect_top_right_y);				
			}
		}	
	}
}

//traverse the tree as long as the tree is overlapping with the given region query
void
traverse_overlapping_rect_tree(tree_t* tree, long double rect_bot_left_x, long double rect_bot_left_y, 
							   long double rect_top_right_x, long double rect_top_right_y) {
	assert(tree!=NULL);
	recursive_overlapping_rect_traverse(tree->root, rect_bot_left_x, rect_bot_left_y, rect_top_right_x, 
								   		rect_top_right_y);
}

/* tree operations
 * Alistair Moffat, PPSAA, Chapter 10, December 2012
 * (c) University of Melbourne */
 /*further adapted by Heng Yi Xuan for COMP20003 Assignment 2*/
