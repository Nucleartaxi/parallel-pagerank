#include <stdlib.h>
#include <stdio.h>

//[int*, int*, int*, int*, ...]
//[int, int, int] [int, int] [int, int, int, int] [int] ...

int make_adjacency_list(char* filename) {
    int cur_node = 0;
    int count = 0;

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    size_t read;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
    }
    free(line);
    // for line in file
    // count += 1;
    // if cur_node != this_line's node
    //      allocate an array of size count. Copy all the things into it. 

}

int main(int argc, char* argv[]) {
    make_adjacency_list("facebook_combined.txt");
}