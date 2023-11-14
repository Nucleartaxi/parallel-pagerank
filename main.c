#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

//[int*, int*, int*, int*, ...]
//[int, int, int] [int, int] [int, int, int, int] [int] ...
#define MAX_EDGES 2048

struct vec {
    int size;
    int* arr;
};

void print_sparse_matrix(struct vec* sparse_matrix, int sparse_matrix_length) {
    for (int i = 0; i < sparse_matrix_length; i++) {
        printf("%d\n", i); //i is the node 
        for (int j = 0; j < sparse_matrix[i].size; j++) {
            printf("  %d\n", sparse_matrix[i].arr[j]);
        }
    }
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

int nextnode(struct vec node) {
    int index = rand() % node.size;
    return node.arr[index];
}

int pagerank(struct vec* sparse_matrix, int sparse_matrix_length, int K) {
    int* counts = malloc(sizeof(int)*sparse_matrix_length);
    for (int i = 0; i < sparse_matrix_length; i++) {
        int current_node = i; //start at 0th node. 

        //follow path K times, incrementing count each time. 
        for (int j = 0; j < K; j++) {
            struct vec current_vec = sparse_matrix[current_node];
            counts[current_node]++;
            if (current_vec.size != 0) { //otherwise stay on current node.
                current_node = nextnode(current_vec);
            }
        }
    }
    for (int i = 0; i < sparse_matrix_length; i++) {
        printf("%d %d\n", i, counts[i]);
    }
}

int main(int argc, char* argv[]) {
    // make_adjacency_list("facebook_combined.txt");
    struct vec* sparse_matrix = malloc(sizeof(struct vec*) * 2000000); //make the sparse matrix 
    // int sparse_matrix_length = make_adjacency_list("facebook_combined.txt", sparse_matrix);
    int sparse_matrix_length = make_adjacency_list("web-Google_sorted.txt", sparse_matrix);
    printf("%d\n", sparse_matrix_length);
    pagerank(sparse_matrix, sparse_matrix_length, 100);
    // print_sparse_matrix(sparse_matrix, sparse_matrix_length);

}