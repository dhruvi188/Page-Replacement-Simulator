#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAMES 100
#define MAX_REFERENCES 10000

// Function prototypes
void print_usage();
void simulate(const char *filename, int num_frames, const char *algorithm);

// Page replacement algorithms
void fifo(int *page_references, int num_references, int num_frames);
void lru(int *page_references, int num_references, int num_frames);
void opt(int *page_references, int num_references, int num_frames);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_usage();
        return 1;
    }

    int num_frames = atoi(argv[1]);
    if (num_frames <= 0 || num_frames > MAX_FRAMES) {
        printf("Error: Number of frames must be between 1 and %d\n", MAX_FRAMES);
        return 1;
    }

    const char *filename = argv[2];
    const char *algorithm = argv[3];

    simulate(filename, num_frames, algorithm);

    return 0;
}

void print_usage() {
    printf("Usage: vmsim <num_frames> <input_filename> <algorithm>\n");
    printf("  <num_frames>: Number of physical memory frames (1-100)\n");
    printf("  <input_filename>: File containing page reference sequence\n");
    printf("  <algorithm>: Page replacement algorithm (opt, lru, fifo)\n");
}

void simulate(const char *filename, int num_frames, const char *algorithm) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    int page_references[MAX_REFERENCES];
    int num_references = 0;
    while (fscanf(file, "%d", &page_references[num_references]) == 1) {
        num_references++;
        if (num_references >= MAX_REFERENCES) {
            printf("Error: Maximum number of page references exceeded\n");
            fclose(file);
            return;
        }
    }

    fclose(file);

    printf("Page references read from %s\n", filename);
    printf("Number of frames: %d\n", num_frames);
    printf("Algorithm: %s\n", algorithm);

    // Run the selected page replacement algorithm
    if (strcmp(algorithm, "fifo") == 0) {
        fifo(page_references, num_references, num_frames);
    } else if (strcmp(algorithm, "lru") == 0) {
        lru(page_references, num_references, num_frames);
    } else if (strcmp(algorithm, "opt") == 0) {
        opt(page_references, num_references, num_frames);
    } else {
        printf("Error: Unknown algorithm '%s'\n", algorithm);
    }
}

// Page replacement algorithm: First-In-First-Out (FIFO)
void fifo(int *page_references, int num_references, int num_frames) {
    int frames[MAX_FRAMES];
    int page_faults = 0;
    int next_frame = 0; // Index of the next frame to replace
    int cnt = 0;
    // Initialize frames to -1 (indicating empty)
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }

    // Iterate through the page references
    for (int i = 0; i < num_references; i++) {
        int page = page_references[i];

        // Check if page is already in memory
        int in_memory = 0;
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == page) {
                in_memory = 1;
                break;
            }
        }

        if (!in_memory) {
            cnt++;
            // Page fault: Replace the oldest page in memory
            frames[next_frame] = page;
            next_frame = (next_frame + 1) % num_frames;
		if(cnt>num_frames)
            page_faults++;
        }

        // Print current allocation state of physical memory frames
        printf("%d: [", page);
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == -1) {
                printf(" | ");
            } else {
                printf("%2d|", frames[j]);
            }
        }
        printf("] %c\n", (!in_memory ? 'F' : ' '));
    }

    // Calculate and print miss rate
    double miss_rate = (double)page_faults / num_references * 100;
    printf("\nMiss rate = %d / %d = %.2f%%\n", page_faults, num_references, miss_rate);
}

// Page replacement algorithm: Least Recently Used (LRU)
void lru(int *page_references, int num_references, int num_frames) {
    int frames[MAX_FRAMES];
    int page_faults = 0;
    int time[MAX_FRAMES];
    int current_time = 0;
    int cnt = 0;
    // Initialize frames to -1 (indicating empty) and time to 0
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
        time[i] = 0;
    }

    // Iterate through the page references
    for (int i = 0; i < num_references; i++) {
        int page = page_references[i];

        // Check if page is already in memory
        int in_memory = 0;
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == page) {
                in_memory = 1;
                time[j] = current_time++;
                break;
            }
        }

        if (!in_memory) {
            cnt++;
            // Page fault: Find the least recently used page in memory
            int min_time = time[0];
            int min_index = 0;
            for (int j = 1; j < num_frames; j++) {
                if (time[j] < min_time) {
                    min_time = time[j];
                    min_index = j;
                }
            }

            // Replace the least recently used page with the new page
            frames[min_index] = page;
            time[min_index] = current_time++;
            if(cnt>num_frames)
            page_faults++;
        }

        // Print current allocation state of physical memory frames
        printf("%d: [", page);
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == -1) {
                printf(" | ");
            } else {
                printf("%2d|", frames[j]);
            }
        }
        printf("] %c\n", (!in_memory ? 'F' : ' '));
    }

    // Calculate and print miss rate
    double miss_rate = (double)page_faults / num_references * 100;
    printf("\nMiss rate = %d / %d = %.2f%%\n", page_faults, num_references, miss_rate);
}

// Page replacement algorithm: Optimal (OPT)
void opt(int *page_references, int num_references, int num_frames) {
    int frames[MAX_FRAMES];
    int page_faults = 0;
    int *next_use = (int *)malloc(num_frames * sizeof(int));
    int cnt = 0;
    // Initialize frames to -1 (indicating empty)
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }

    // Iterate through the page references
    for (int i = 0; i < num_references; i++) {
        int page = page_references[i];

        // Check if page is already in memory
        int in_memory = 0;
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == page) {
                in_memory = 1;
                break;
            }
        }

        if (!in_memory) {
             cnt++;
            // Page fault: Find the page that will not be used for the longest time in the future
            int furthest_next_use = -1;
            int replace_index = -1;
            for (int j = 0; j < num_frames; j++) {
                int found = 0;
                for (int k = i + 1; k < num_references; k++) {
                    if (page_references[k] == frames[j]) {
                        next_use[j] = k;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    next_use[j] = num_references;
                }
                if (next_use[j] > furthest_next_use) {
                    furthest_next_use = next_use[j];
                    replace_index = j;
                }
            }

            // Replace the page with the longest time until next use
            frames[replace_index] = page;
            if(cnt>num_frames)
            page_faults++;
        }

        // Print current allocation state of physical memory frames
        printf("%d: [", page);
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == -1) {
                printf(" | ");
            } else {
                printf("%2d|", frames[j]);
            }
        }
        printf("] %c\n", (!in_memory ? 'F' : ' '));
    }

    // Calculate and print miss rate
    double miss_rate = (double)page_faults / num_references * 100;
    printf("\nMiss rate = %d / %d = %.2f%%\n", page_faults, num_references, miss_rate);

    free(next_use);
}
