// Compile with: gcc read_gsf.c -o read_gsf.exe -lgsf
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "gsf.h"

// Function to process a single GSF file
void process_gsf_file(const char *file_path) {
    int handle;
    gsfDataID id;
    gsfRecords rec;

    if (gsfOpen(file_path, GSF_READONLY, &handle) == 0) {
        printf("\nProcessing: %s\n", file_path);
        while (gsfRead(handle, GSF_NEXT_RECORD, &id, &rec, NULL, 0) > 0) {
            if (id.recordID == GSF_RECORD_SWATH_BATHYMETRY_PING) {
                gsfSwathBathyPing *ping = &rec.mb_ping;

                if (ping->brb_inten) {
                    printf("\n[BRB_INTENSITY] - File: %s\n", file_path);
                    printf("  Bits per sample     : %d\n", ping->brb_inten->bits_per_sample);
                    printf("  Applied corrections : 0x%08X\n", ping->brb_inten->applied_corrections);

                    int beams = ping->number_beams;
                    for (int i = 0; i < beams; i++) {  // Limit to first 5 beams for demo
                        gsfTimeSeriesIntensity *ts = &ping->brb_inten->time_series[i];
                        printf("  Beam %d:\n", i);
                        printf("    Sample count        : %d\n", ts->sample_count);
                        printf("    Detect sample index : %d\n", ts->detect_sample);
                        printf("    Start range sample  : %d\n", ts->start_range_samples);

                        for (int j = 0; j < ts->sample_count && j < 10; j++) {
                            printf("      Sample[%d] = %u\n", j, ts->samples[j]);
                        }
                    }
                    break;  // Stop after first ping with backscatter
                }
            }
        }
        gsfClose(handle);
    } else {
        printf("Failed to open GSF file: %s\n", file_path);
    }
}

// Function to iterate over GSF files in the folder
void process_gsf_folder(const char *folder_path) {
    struct dirent *entry;
    DIR *dp = opendir(folder_path);

    if (dp == NULL) {
        perror("Failed to open folder");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        // Skip "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "%s/%s", folder_path, entry->d_name);

        struct stat path_stat;
        stat(file_path, &path_stat);

        if (S_ISREG(path_stat.st_mode)) {  // Only process regular files
            // Check if the file has a .gsf extension
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
