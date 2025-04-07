// Compile with: gcc read_gsf.c -o read_gsf.exe -lgsf
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "gsf.h"

#define OUTPUT_FOLDER "text"

// Function to create the output folder if it doesn't exist
void create_output_folder() {
    struct stat st = {0};
    if (stat(OUTPUT_FOLDER, &st) == -1) {
        mkdir(OUTPUT_FOLDER, 0700);
    }
}

// Function to process a single GSF file and save output to text folder
void process_gsf_file(const char *file_path) {
    int handle;
    gsfDataID id;
    gsfRecords rec;
    
    char output_file_path[1024];
    snprintf(output_file_path, sizeof(output_file_path), "%s/%s.txt", OUTPUT_FOLDER, strrchr(file_path, '/') + 1);
    
    FILE *output_file = fopen(output_file_path, "w");
    if (!output_file) {
        printf("Failed to create output file: %s\n", output_file_path);
        return;
    }

    if (gsfOpen(file_path, GSF_READONLY, &handle) == 0) {
        fprintf(output_file, "Processing: %s\n", file_path);
        while (gsfRead(handle, GSF_NEXT_RECORD, &id, &rec, NULL, 0) > 0) {
            if (id.recordID == GSF_RECORD_SWATH_BATHYMETRY_PING) {
                gsfSwathBathyPing *ping = &rec.mb_ping;

                if (ping->brb_inten) {
                    fprintf(output_file, "\n[BRB_INTENSITY] - File: %s\n", file_path);
                    fprintf(output_file, "  Bits per sample     : %d\n", ping->brb_inten->bits_per_sample);
                    fprintf(output_file, "  Applied corrections : 0x%08X\n", ping->brb_inten->applied_corrections);

                    int beams = ping->number_beams;
                    for (int i = 0; i < beams; i++) {
                        gsfTimeSeriesIntensity *ts = &ping->brb_inten->time_series[i];
                        fprintf(output_file, "  Beam %d:\n", i);
                        fprintf(output_file, "    Sample count        : %d\n", ts->sample_count);
                        fprintf(output_file, "    Detect sample index : %d\n", ts->detect_sample);
                        fprintf(output_file, "    Start range sample  : %d\n", ts->start_range_samples);

                        for (int j = 0; j < ts->sample_count && j < 10; j++) {
                            fprintf(output_file, "      Sample[%d] = %u\n", j, ts->samples[j]);
                        }
                    }
                    break;  // Stop after first ping with backscatter
                }
            }
        }
        gsfClose(handle);
    } else {
        fprintf(output_file, "Failed to open GSF file: %s\n", file_path);
    }
    fclose(output_file);
}

// Function to iterate over GSF files in the folder
void process_gsf_folder(const char *folder_path) {
    struct dirent *entry;
    DIR *dp = opendir(folder_path);

    if (dp == NULL) {
        perror("Failed to open folder");
        return;
    }

    create_output_folder();

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "%s/%s", folder_path, entry->d_name);

        struct stat path_stat;
        stat(file_path, &path_stat);

        if (S_ISREG(path_stat.st_mode)) {
            if (strstr(entry->d_name, ".gsf") != NULL) {
                process_gsf_file(file_path);
            }
        }
    }
    closedir(dp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <folder_path>\n", argv[0]);
        return 1;
    }

    const char *folder_path = argv[1];
    process_gsf_folder(folder_path);

    return 0;
}
