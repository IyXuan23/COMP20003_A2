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

	node_t *next;
};

//structure for the list
typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

list_t *make_empty_list(void);

int is_empty_list(list_t *list);

void free_list(list_t *list);

list_t
*insert_at_foot(list_t *list, int footpath_id1, char* address1, char* clue_sa1, char* asset_type1, double 
					deltaz1, double distance1, double grade1in1, int mcc_id1, int mccid_int1, double 
					rlmax1, double rlmin1, char* segside1, int statusid1, int streetid1, int 
					street_group1, double start_lat1, double start_lon1, double end_lat1, double 
					end_lon1);

void fill_arrays_with_nodes(node_t* node_ptr_array[], list_t* street_list);

int compare_func(const void* node1_ptr, const void* node2_ptr);

int find_closest_value(node_t* node_ptr_array[], double find_this, int num_nodes, 
                      FILE* output_file);
                    
void find_footpath(FILE* output_file, int footpath_id, list_t* street_list);
/* =====================================================================
   Program written by Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.

   See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
   information.

   Prepared December 2012 for the Revised Edition.
   ================================================================== */

/*Code by Alistar Moffat, taken from COMP10002, adapted for Assignment 1
in COMP20003 by Heng Yi Xuan, who has further added a few more functions*/

/*Code has been copied from COMP20003 Assignment 1 and further developed for
Assignment 2*/
