#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <omp.h>

//[int*, int*, int*, int*, ...]
//[int, int, int] [int, int] [int, int, int, int] [int] ...
#define MAX_EDGES 2048
#define MAX_ARR_LENGTH 2000000

struct vec {
    int size;
    int* arr;
};
struct count_pair {
    int index;
    int count;
};

void print_sparse_matrix(struct vec* sparse_matrix, int sparse_matrix_length) {
    for (int i = 0; i < sparse_matrix_length; i++) {
        printf("%d\n", i); //i is the node 
        for (int j = 0; j < sparse_matrix[i].size; j++) {
            printf("  %d\n", sparse_matrix[i].arr[j]);
        }
    }
}


//Coin toss between 0 and 1
double coin_toss(void) {
    return (1.0 / RAND_MAX) * rand();
}

//Determines if player picks next node or random node
int new_walk(double dampen) {
    if (coin_toss() <= dampen) {
        return 1;
    }
    return 0;
}

int split(char* line, int* first, int* second) {
    //outparameters first is first token, second is second token.
    int i = 0;
    while (!isspace(line[i])) {
        i++;
    }
    line[i] = 0;
    i++;
    *first = atoi(line); //first token
    while (isspace(line[i])) {
        i++;
    }
    *second = atoi(line + (char) i);
    return 0;
}

int make_adjacency_list(char* filename, struct vec* sparse_matrix) {
    int cur_node = 0;
    int temp_edge_buf[MAX_EDGES]; //for storing edges

    int sparse_matrix_length = 0;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;

    //open file
    fp = fopen(filename, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    // for line in file
    // count += 1;
    // if cur_node != this_line's node
    //      allocate an array of size count. Copy all the things into it. 
    int count = 0;
    int prev_node = -1;
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu :\n", read);
        // printf("%s", line);
        if (line[0] == '#') { //skip commented lines
            continue;
        }
        if (count >= MAX_EDGES) {
            printf("ALERT MORE THAN %d EDGES\n\n\n", MAX_EDGES);
            exit(-1);
        }
        int first;
        int second;
        split(line, &first, &second);
        // printf("first %d second %d\n", first, second);

        cur_node = first;
        if (prev_node == -1) { //for first run 
            prev_node = cur_node;
        }
        if (prev_node != cur_node) { //while we have the same node, keep adding edges to the edge buf. 
            //if we hit a new node, make an array out of all of the previous edges.
            //malloc up to count to make the array for all the things, then put the things in the array. 
            struct vec* vec = malloc(sizeof(vec));
            vec->size = count;
            vec->arr = malloc(sizeof(int) * count);
            sparse_matrix[prev_node] = *vec;
            sparse_matrix_length++;
            // sparse_matrix[prev_node].size = count;
            // sparse_matrix[prev_node].arr = malloc(sizeof(int) * count);
            for (int i = 0; i < count; i++) { //copy ints from buf into array 
                // printf("%d\n", temp_edge_buf[i]);
                sparse_matrix[prev_node].arr[i] = temp_edge_buf[i];
                // sparse_matrix[prev_node].arr[i] = temp_edge_buf[i];
            }
            // printf("\n");
            count = 0;
            //and now add the current thing to the temp buffer now that we've moved all the stuff out of it.

        } 
        temp_edge_buf[count] = second;
        count++;
        prev_node = cur_node;
    }
    free(line);
    return sparse_matrix_length;

}

//Accesses next node in graph
int nextnode(struct vec node) {
    int index = rand() % node.size;
    return node.arr[index];
}

//
int compare_function(const void* p1, const void* p2) {
    const struct count_pair* p11 = (const struct count_pair*) p1;
    const struct count_pair* p22 = (const struct count_pair*) p2;
    return p11->count < p22->count;
}

//PageRank algorithm
int pagerank(struct vec* sparse_matrix, int sparse_matrix_length, int K, double D) {
    struct count_pair* counts = malloc(sizeof(struct count_pair)*MAX_ARR_LENGTH);

    #pragma omp parallel for
    for (int i = 0; i < sparse_matrix_length; i++) {
        int current_node = i; //start at 0th node. 
        //follow path K times, incrementing count each time. 
        for (int j = 0; j < K; j++) {
            struct vec current_vec = sparse_matrix[current_node];
            counts[current_node].index = current_node;
            counts[current_node].count++;
            if (current_vec.size == 0) { //if we have no neighbors, exit. 
                break;
            }
            //Determine if we start from a new random node or
            //continue on the next node 
            if (new_walk(D) == 1) {
                int index = rand() % sparse_matrix_length;
                current_vec = sparse_matrix[index];
                current_node = nextnode(current_vec);
            }
            else {
                current_node = nextnode(current_vec);
            }
        }
    }

    qsort(counts, MAX_ARR_LENGTH, sizeof(*counts), compare_function);

    #pragma omp parallel for
    for (int i = 0; i < 5; i++) {
        printf("%d %d\n", counts[i].index, counts[i].count);
    }
}

int main(int argc, char* argv[]) {

    int k; //Length walk
	double d; //Dampening ratio
    int p = 8; //Number of processes

    //Checks command line input
	if(argc<2) {
		printf("Usage: n [number of threads]\n");
		exit(1);
	}
	
    //Set walk length
	k = atoll(argv[1]);
	printf("Walk Length = %d \n",k);

    //Check and set dampening ratio
	d = atof(argv[2]); //converts to float
	assert(d >= 0 && d <= 1);
	printf("Dampening Ratio = %f\n",d);
	
 
    //Set the number of threads to use
	omp_set_num_threads(p);

    // make_adjacency_list("facebook_combined.txt");
    struct vec* sparse_matrix = malloc(sizeof(struct vec*) * MAX_ARR_LENGTH); //make the sparse matrix 

    // int sparse_matrix_length = make_adjacency_list("facebook_combined.txt", sparse_matrix);
    int sparse_matrix_length = make_adjacency_list("web-Google_sorted.txt", sparse_matrix);

    // printf("%d\n", sparse_matrix_length);
    pagerank(sparse_matrix, sparse_matrix_length, k, d);

    // print_sparse_matrix(sparse_matrix, sparse_matrix_length);

    /*ISSUES:
        - Check that rand() works instead of rand_r()
        - Commandline input doesn't let us have float values for 0-1
    */
}
