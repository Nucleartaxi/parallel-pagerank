#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

//[int*, int*, int*, int*, ...]
//[int, int, int] [int, int] [int, int, int, int] [int] ...

struct int_pair {
    int first;
    int second;
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
    struct int_pair tempbuf[128];

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
        printf("first %d second %d\n", first, second);

        cur_node = first;
        if (prev_node == cur_node) {
            tempbuf[count].first = first;
            tempbuf[count].second = second;
            count++;
        } else {
            //malloc up to count to make the array for all the things, then put the things in the array. 
            count = 0;
        }
    }
    free(line);

}

int main(int argc, char* argv[]) {
    // make_adjacency_list("facebook_combined.txt");
    make_adjacency_list("web-Google_sorted.txt");
}