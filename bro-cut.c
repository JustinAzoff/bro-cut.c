#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE 4096

int contains_string(char *haystack[], int haystack_size, char *needle) {
    for(int i=0; i < haystack_size ; ++i) {
        if(strcmp(haystack[i], needle) == 0) {
            return 1;
        }
    }
    return 0;
}

void output_indexes(char *line, int *indexes, int num_indexes) {
    int cur_field = 0;
    char * field;

    field = strsep(&line, "\t");
    while (cur_field != indexes[0]) {
        field = strsep(&line, "\t");
        ++cur_field;
    }
    printf("%s", field);
    ++cur_field;

    for(int i=1; i < num_indexes; ++i) {
        field = strsep(&line, "\t");
        while (cur_field != indexes[i]) {
            field = strsep(&line, "\t");
            ++cur_field;
        }
        printf("\t%s", field);
        ++cur_field;
    }
    printf("\n");
}

int find_output_indexes(int **output_indexes, int num_columns, char *columns[], int negate, char *line) {
    int *out_indexes;

    int num_fields = 0;
    char *fields = strdup(line+8);
    char *field;

    while((field = strsep(&fields, "\t")) != NULL) {
        num_fields++;
    }

    if(num_columns == 0){
        out_indexes=(int *) malloc(num_fields*sizeof(int));
        for(int i=0; i < num_fields ; ++i) {
            out_indexes[i] = i;
        }
        *output_indexes = out_indexes;
        return num_fields;
    }

    fields = strdup(line+8);
    if(!negate) {
        out_indexes=(int *) malloc(num_columns*sizeof(int));
        int idx = 0;
        int out_idx = 0;
        while((field = strsep(&fields, "\t")) != NULL) {
            if(contains_string(columns, num_columns, field)) {
                out_indexes[out_idx++] = idx;
            }
            idx++;
        }
        *output_indexes = out_indexes;
        return num_columns;
    }

    return 0;
}

int bro_cut(int num_columns, char *columns[], int negate) {
    char line[MAX_LINE];

    int *out_indexes;
    int num_out_indexes;

    while(fgets(line, MAX_LINE , stdin) != NULL) {
        line[strlen(line)-1]='\0';
        if(strlen(line) && line[0] == '#') {
            if(strncmp(line, "#fields", 7) == 0) {
                num_out_indexes = find_output_indexes(&out_indexes, num_columns, columns, negate, line);
            }
            continue;
        }

        output_indexes(line, out_indexes, num_out_indexes);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int negate = 0;
    int c;
    while((c = getopt(argc, argv, "n")) != -1){
        switch(c){
            case 'n':
                negate = 1;
                break;
        }
    }
    return bro_cut(argc-optind, &argv[optind], negate);
}
