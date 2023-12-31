#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <omp.h>
#include <time.h>

//[int*, int*, int*, int*, ...]
//[int, int, int] [int, int] [int, int, int, int] [int] ...
#define MAX_EDGES 4096
#define MAX_ARR_LENGTH 2000000
#define NODES_IN_PARTITION 5000

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
double coin_toss(unsigned int* myseed) {
    return (1.0 / RAND_MAX) * rand_r(myseed);
}

//Determines if player picks next node or random node
int new_walk(double dampen, unsigned int* myseed) {
    return coin_toss(myseed) <= dampen;
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

            for (int i = 0; i < count; i++) { //copy ints from buf into array 
                sparse_matrix[prev_node].arr[i] = temp_edge_buf[i];;
            }
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
int nextnode(struct vec node, unsigned int* myseed) {
    int index = rand_r(myseed) % node.size;
    return node.arr[index];
}

//Used for compare in qsort function
int compare_function(const void* p1, const void* p2) {
    const struct count_pair* p11 = (const struct count_pair*) p1;
    const struct count_pair* p22 = (const struct count_pair*) p2;
    return p11->count < p22->count;
}

//PageRank algorithm
int pagerank(struct vec* sparse_matrix, int sparse_matrix_length, int K, double D, int p) {
    struct count_pair* final_counts = malloc(sizeof(struct count_pair)*MAX_ARR_LENGTH);

    //set up lock data structure
    int num_locks = MAX_ARR_LENGTH / NODES_IN_PARTITION;
    //printf("num locks = %d\n", num_locks);

    omp_lock_t my_lock[num_locks];
    for (int i = 0; i < num_locks; i++) {
        omp_init_lock(&my_lock[i]);
    }

    unsigned int myseed = rand(); //get an initial random seed value 

    double mytime = omp_get_wtime();

    //parallel for loop
    #pragma omp parallel for shared(sparse_matrix, my_lock, final_counts) firstprivate(sparse_matrix_length, D, K, myseed) default(none)
    for (int i = 0; i < sparse_matrix_length; i++) {
        myseed += omp_get_thread_num(); //vary seed based on thread
        int current_node = i; //start at 0th node. 
        //follow path K times, incrementing count each time. 
        for (int j = 0; j < K; j++) {
            struct vec current_vec = sparse_matrix[current_node];

            //acquire lock for the block of nodes of length NODES_IN_PARTITION before performing critical operation.
            int lock_index_pls = current_node / NODES_IN_PARTITION;
            omp_set_lock(&(my_lock[lock_index_pls]));
            final_counts[current_node].count++;
            final_counts[current_node].index = current_node;
            omp_unset_lock(&(my_lock[lock_index_pls]));

            if (current_vec.size == 0) { //if we have no neighbors, exit. 
                break;
            }
            //Determine if we start from a new random node or
            //continue on the next node 
            if (new_walk(D, &myseed)) {
                int index = rand_r(&myseed) % sparse_matrix_length;
                current_node = index;
            } else {
                current_node = nextnode(current_vec, &myseed);
            }
        }
    }

    mytime = omp_get_wtime()-mytime;
    printf("Time: %f\n\n", mytime);

    qsort(final_counts, MAX_ARR_LENGTH, sizeof(*final_counts), compare_function);

    int total_iterations = sparse_matrix_length * K;  //we know how many times we iterate becuase we defined the for loop, so we can get our total counts this way

    for (int i = 0; i < 5; i++) {
        printf("%d %f\n", final_counts[i].index, final_counts[i].count / (double) total_iterations);
    }
    printf("\n");
}

int main(int argc, char* argv[]) {

    int k; //Length walk
	double d; //Dampening ratio
    int p = 8; //Number of processes

    //Checks command line input
	if(argc<3) {
		printf("Usage: n [number of threads] [walk length] [damping ratio]\n");
		exit(1);
	}
    //set num threads
	p = atoll(argv[1]);
	printf("\nNumber of Threads = %d \n",p);

    //Set walk length
	k = atoll(argv[2]);
	printf("Walk Length = %d \n",k);

    //Check and set dampening ratio
	d = atof(argv[3]); //converts to float
	assert(d >= 0 && d <= 1);
	printf("Dampening Ratio = %f\n\n",d);
 
    //Set the number of threads to use
	omp_set_num_threads(p);

    struct vec* sparse_matrix = malloc(sizeof(struct vec*) * MAX_ARR_LENGTH); //make the sparse matrix 

    int sparse_matrix_length = make_adjacency_list("web-Google_sorted.txt", sparse_matrix);

    pagerank(sparse_matrix, sparse_matrix_length, k, d, p);

}
