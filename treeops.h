typedef struct tree_node tree_node_t;

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

typedef struct {

	tree_node_t *root;
	int (*cmp)(long double, long double, long double, long double);
    
} tree_t;

//makes an empty tree
tree_t* make_empty_tree(int func(long double, long double, long double, long double));

//function to be used with insert_in_order function to build the tree
tree_node_t* recursive_insert(tree_node_t *root, tree_node_t *new, int cmp(long double, long double, 
                              long double, long double), long double bot_left_x, long double bot_left_y, 
                              long double top_right_x, long double top_right_y);

//function that inserts the given node into the right position in the tree; will also shift the values
//in the parent node to a new leaf node to maintain the tree
tree_t* insert_in_order(tree_t *tree, double long_value, double lat_value, long double bot_left_x, 
                        long double bot_left_y, long double top_right_x, long double top_right_y, 
                        int footpath_id);

//function to calculate the midpoint of each node                        
long double create_midpoint(long double point1, long double point2);

//functions that traverses and frees the tree
void free_tree(tree_t *tree);
void recursive_free_tree(tree_node_t *root);

//comparison function to be used while building the tree, returns a value for where the new node should go
int cmp(long double midpoint_x, long double midpoint_y, long double longitude, long double latitude);

//USED FOR TESTING, PRINT ALL VALU
void recursive_traverse(tree_node_t* root);
void traverse_tree(tree_t* tree);

//function to check whether the given point lies within the rectangle
int check_if_in_rectangle(long double bot_left_x, long double bot_left_y, long double top_right_x, 
				 		  long double top_right_y, double longitude, double latitude);

tree_node_t* create_new_node(point_t* point);                           

tree_node_t*
search_tree(double longitude, double latitude, tree_node_t* root, int cmp(long double, long double, 
			long double, long double), FILE* output_file);


void traverse_trees_in_rectangle(tree_node_t* root);

int
rectangleOverlap(long double rect1_bot_left_x, long double rect1_bot_left_y, long double rect1_top_right_x, 
				 long double rect1_top_right_y, long double rect2_bot_left_x, long double rect2_bot_left_y, 
				 long double rect2_top_right_x, long double rect2_top_right_y);

void
recursive_overlapping_rect_traverse(tree_node_t* root, long double rect_bot_left_x, long double 	
									rect_bot_left_y, long double rect_top_right_x, 
									long double rect_top_right_y);

void
traverse_overlapping_rect_tree(tree_t* tree, long double rect_bot_left_x, long double rect_bot_left_y, 
							   long double rect_top_right_x, long double rect_top_right_y);     
/* tree operations
 * Alistair Moffat, PPSAA, Chapter 10, December 2012
 * (c) University of Melbourne */
 /*further adapted by Heng Yi Xuan for COMP20003 Assignment 2*/
