#ifndef TREEOPS_AND_LISTOPS_H
#define TREEOPS_AND_LISTOPS_H

//sufficient for 512 + 1 characters
#define TEMP_STRING_LEN 513
#define HOLDING_STR_LEN 128
#define MAX_DATA_POINTS 1936

typedef struct tree_node tree_node_t;
typedef struct node node_t;

//node structure for the linked list to store the data
struct node {

    //19 data types
   int footpath_id;
   char* address;
   char* clue_sa;
   char* asset_type;
   double deltaz, distance, grade1in;
   int mcc_id, mccid_int;
   double rlmax, rlmin;
   char* segside;
   int statusid, streetid, street_group;
   double start_lat, start_lon, end_lat, end_lon;

    //will be used in dict4, to ensure no duplicates are printed
   int already_printed;

    node_t *next;
};

//structure for the linked list
typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

//structure to hold the x and y values of the point along with its
//corresponding footpath_id
typedef struct {

    double longitude;
    double latitude;

    //array for the case of multiple footpaths
    int footpath_id_array[100];
    int num_of_footpaths;

} point_t;

//structure for each tree node;
struct tree_node {

    //used if it is a parent node, midpoint of its rectangle
    long double midpoint_x;
    long double midpoint_y;

    //used as a boolean to check whether midpoint has been initialised
    int midpoint_has_value;

    //values to outline the rectangle the point is in
    //will be used when checking rectangle overlaps
    long double rect_bot_left_x;
    long double rect_bot_left_y;
    long double rect_top_right_x;
    long double rect_top_right_y;

    point_t* point;

    tree_node_t* NW;
    tree_node_t* NE;
    tree_node_t* SW;
    tree_node_t* SE;

};

//quadtree structure
typedef struct {

	tree_node_t *root;
	int (*cmp)(long double, long double, long double, long double);
    
} tree_t;

//makes an empty tree, holding the function "func"
tree_t* make_empty_tree(int func(long double, long double, long double, long double));

//function to be used with insert_in_order function to build the tree
tree_node_t* recursive_insert(tree_node_t *root, tree_node_t *new, int cmp(long double, long double, 
                              long double, long double), long double bot_left_x, long double bot_left_y, 
                              long double top_right_x, long double top_right_y);

//function will create a node, by calling the create_node function, containing the rectangle coords
//and also the footpath_id, longitude & latitude values, then insert it into the quadtree
//recursively
tree_t* insert_in_order(tree_t *tree, double long_value, double lat_value, long double bot_left_x, 
                        long double bot_left_y, long double top_right_x, long double top_right_y, 
                        int footpath_id);

//function to calculate and return the midpoint of each node                        
long double create_midpoint(long double point1, long double point2);

//functions that traverses and recursively frees the tree
void free_tree(tree_t *tree);
void recursive_free_tree(tree_node_t *root);

//comparison function to be used while building the tree, returns a value for where a new node should go, 
//given the midpoint of the current rectangle, compared to the longitude and latitude of the node
int cmp(long double midpoint_x, long double midpoint_y, long double longitude, long double latitude);

//function to check whether the given point lies within the given rectangle
int check_if_in_rectangle(long double bot_left_x, long double bot_left_y, long double top_right_x, 
				 		  long double top_right_y, double longitude, double latitude);

//function that creates a new node, containing the given point
tree_node_t* create_new_node(point_t* point);                           
//function that creates a new point, given longitude, latitude and footpath_id values
point_t* create_new_point(double long_value, double lat_value, int footpath_id);

//recursively search the tree for the given point, and print its corresponding values into the output file
tree_node_t*
search_tree(double longitude, double latitude, tree_node_t* root, int cmp(long double, long double, 
			long double, long double), FILE* output_file);

//checks whether 2 given rectangles will overlap, returning 1 if yes and 0 if not
int
rectangleOverlap(long double rect1_bot_left_x, long double rect1_bot_left_y, long double rect1_top_right_x, 
				 long double rect1_top_right_y, long double rect2_bot_left_x, long double rect2_bot_left_y, 
				 long double rect2_top_right_x, long double rect2_top_right_y);

//checks whether a given point lies within a given rectangle, and returns 1 if yes, 0 if not
int
inRectangle(long double rect_bot_left_x, long double rect_bot_left_y, long double rect_top_right_x, 
			long double rect_top_right_y, double longitude, double latitude);

//function used together with traverse_overlapping_rect_tree
int
recursive_overlapping_rect_traverse(tree_node_t* root, long double rect_bot_left_x, long double 	
									rect_bot_left_y, long double rect_top_right_x, 
									long double rect_top_right_y, int print_array[], int counter);

//function will recursively traverse the tree as long as the nodes overlap the given rectangle query,
//and print any footpaths found within the query to the output file
int
traverse_overlapping_rect_tree(tree_t* tree, long double rect_bot_left_x, long double rect_bot_left_y, 
							   long double rect_top_right_x, long double rect_top_right_y, 
                               int print_array[]);




//LISTOPS BEGIN HERE

//makes an empty list
list_t *make_empty_list(void);

//checks if list is empty
int is_empty_list(list_t *list);

//recursively frees the list
void free_list(list_t *list);

//creates a node and insert it at the foot of the linked list
list_t
*insert_at_foot(list_t *list, int footpath_id1, char* address1, char* clue_sa1, char* asset_type1, double 
					deltaz1, double distance1, double grade1in1, int mcc_id1, int mccid_int1, double 
					rlmax1, double rlmin1, char* segside1, int statusid1, int streetid1, int 
					street_group1, double start_lat1, double start_lon1, double end_lat1, double 
					end_lon1);

void fill_arrays_with_nodes(node_t* node_ptr_array[], list_t* street_list);

//compare function used in qsort to compare the values within nodes
int compare_func(const void* node1_ptr, const void* node2_ptr);

//function will perform a binary search through the array, and locate the value closest to the given value
//of "find this", then print the corresponding values to the output file
int find_closest_value(node_t* node_ptr_array[], double find_this, int num_nodes, 
                      FILE* output_file);

//(used in dict3) locates the footpath using a linear search through the linked list, 
//and prints the corresponding values to output file                    
void find_footpath(FILE* output_file, int footpath_id, list_t* street_list);                               

//compare function for qsort, used in dict3 and dict4 to sort the footpath_id_array before printing
int compare_func2(const void* int1, const void* int2);

/* tree operations and listops
 * Alistair Moffat, PPSAA, Chapter 10, December 2012
 * (c) University of Melbourne */
 /*further adapted by Heng Yi Xuan for COMP20003 Assignment 2*/

#endif
