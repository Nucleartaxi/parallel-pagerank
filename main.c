#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

//[int*, int*, int*, int*, ...]
//[int, int, int] [int, int] [int, int, int, int] [int] ...

struct vec {
    int size;
    int* arr;
};

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

int make_adjacency_list(char* filename) {
    int cur_node = 0;
    int temp_edge_buf[128]; //for storing edges

    printf("%d\n", sizeof(struct vec));
    printf("%d\n", sizeof(int));
    printf("%d\n", sizeof(int*));
    struct vec* sparse_matrix[1000000];
    // int sparse_matrix_length = 0;

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
        if (count >= 128) {
            printf("ALERT MORE THAN 128 EDGES\n\n\n");
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
            sparse_matrix[prev_node] = vec;
            // sparse_matrix[prev_node].size = count;
            // sparse_matrix[prev_node].arr = malloc(sizeof(int) * count);
            for (int i = 0; i < count; i++) { //copy ints from buf into array 
                // printf("%d\n", temp_edge_buf[i]);
                sparse_matrix[prev_node]->arr[i] = temp_edge_buf[i];
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

}

int main(int argc, char* argv[]) {
    // make_adjacency_list("facebook_combined.txt");
    make_adjacency_list("web-Google_sorted.txt");
}