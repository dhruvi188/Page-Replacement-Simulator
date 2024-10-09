#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void print_usage();
void generate_page_references(int range, int length, const char *filename);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_usage();
        return 1;
    }

    int range = atoi(argv[1]);
    int length = atoi(argv[2]);
    const char *filename = argv[3];

    generate_page_references(range, length, filename);

    return 0;
}

void print_usage() {
    printf("Usage: vmgen <range> <length> <output_filename>\n");
    printf("  <range>: Range of page references (1-100)\n");
    printf("  <length>: Length of the sequence\n");
    printf("  <output_filename>: Output filename to store the generated sequence\n");
}

void generate_page_references(int range, int length, const char *filename) {
    if (range <= 0 || range > 100) {
        printf("Error: Range must be between 1 and 100\n");
        return;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    srand(time(NULL));

    int prev_page = -1;
    for (int i = 0; i < length; i++) {
        int page = rand() % range;
        while (page == prev_page) {
            page = rand() % range;
        }
        fprintf(file, "%d ", page);
        prev_page = page;
    }

    fclose(file);

    printf("Page reference sequence generated and stored in %s\n", filename);
}
