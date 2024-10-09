#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAMES 100
#define MAX_REFERENCES 10000
#define MAX_FILENAME_LENGTH 100

// Function prototypes
void print_usage();
void run_simulation(const char *filename, int min_frames, int max_frames, int frame_increment);
double calculate_miss_rate(int *page_references, int num_references, const char *algorithm, int num_frames);

// Page replacement algorithms
double fifo(int *page_references, int num_references, int num_frames);
double lru(int *page_references, int num_references, int num_frames);
double opt(int *page_references, int num_references, int num_frames);

int main(int argc, char *argv[]) {
    if (argc != 5) {
        print_usage();
        return 1;
    }

    int min_frames = atoi(argv[1]);
    int max_frames = atoi(argv[2]);
    int frame_increment = atoi(argv[3]);
    const char *filename = argv[4];

    run_simulation(filename, min_frames, max_frames, frame_increment);

    return 0;
}

void print_usage() {
    printf("Usage: vmstats <min_frames> <max_frames> <frame_increment> <input_filename>\n");
    printf("  <min_frames>: Minimum number of frames (no less than 2)\n");
    printf("  <max_frames>: Maximum number of frames (no more than 100)\n");
    printf("  <frame_increment>: Frame number increment (positive)\n");
    printf("  <input_filename>: Input filename containing the references\n");
}

void run_simulation(const char *filename, int min_frames, int max_frames, int frame_increment) {
    printf("Running simulation...\n");

    // Open the input file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        return;
    }

    // Read page references from the input file
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

    // Open results file for writing
    FILE *results_file = fopen("vmrates.dat", "w");
    if (results_file == NULL) {
        printf("Error: Unable to create results file\n");
        return;
    }
	for (int num_frames = min_frames; num_frames <= max_frames; num_frames += frame_increment) {
	    fprintf(results_file,"%d ", num_frames);
	}
	fprintf(results_file,"\n");
    // Loop over the three page replacement methods
    const char *algorithms[3] = {"opt", "lru", "fifo"};
    for (int i = 0; i < 3; i++) {
        // Loop over the number of frames
        for (int num_frames = min_frames; num_frames <= max_frames; num_frames += frame_increment) {
            // Calculate miss rate for the current algorithm and number of frames
            double miss_rate = calculate_miss_rate(page_references, num_references, algorithms[i], num_frames);

            // Print the miss rate in the specified format
            printf("%s, %d frames: Miss rate = %.2f%%\n", algorithms[i], num_frames, miss_rate);

            // Write miss rate to results file
            fprintf(results_file, "%.2f ", miss_rate);
        }
        fprintf(results_file, "\n");
    }

    // Close results file
    fclose(results_file);

    printf("Simulation completed. Results written to vmrates.dat\n");
}

double calculate_miss_rate(int *page_references, int num_references, const char *algorithm, int num_frames) {
    if (strcmp(algorithm, "opt") == 0) {
        return opt(page_references, num_references, num_frames);
    } else if (strcmp(algorithm, "lru") == 0) {
        return lru(page_references, num_references, num_frames);
    } else if (strcmp(algorithm, "fifo") == 0) {
        return fifo(page_references, num_references, num_frames);
    } else {
        printf("Error: Unknown algorithm '%s'\n", algorithm);
        return -1.0;
    }
}

double fifo(int *page_references, int num_references, int num_frames) {
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
    }

    // Calculate and return miss rate
    return (double)page_faults / num_references * 100;
}

double lru(int *page_references, int num_references, int num_frames) {
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
    }

    // Calculate and return miss rate
    return (double)page_faults / num_references * 100;
}

double opt(int *page_references, int num_references, int num_frames) {
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
    }

    free(next_use);

    // Calculate and return miss rate
    return (double)page_faults / num_references * 100;
}
