#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "gsf/gsf.h"

#define MAX_LINES 1000000


void add_to_list(char ***list, int *count, int *capacity, const char *path) {
    if (*count >= *capacity) {
        *capacity = (*capacity == 0) ? 100 : (*capacity * 2); // Start with 100, then double
        *list = realloc(*list, (*capacity) * sizeof(char *));
        if (*list == NULL) {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
    }
    (*list)[*count] = strdup(path);
    if ((*list)[*count] == NULL) {
        perror("strdup failed");
        exit(EXIT_FAILURE);
    }
    (*count)++;
}

void check_for_backscatter(
    const char* file_path,
    FILE *report_file,
    int* files_with_backscatter,
    int* files_missing_backscatter,
    char ***files_with_backscatter_list,
    char ***files_missing_backscatter_list,
    int *with_capacity,
    int *missing_capacity
) {
    int handle;
    gsfDataID id;
    gsfRecords rec;
    int has_backscatter = 0;

    // Open the GSF file
    if (gsfOpen(file_path, GSF_READONLY, &handle) == 0) {
        while (gsfRead(handle, GSF_NEXT_RECORD, &id, &rec, NULL, 0) > 0) {
            if (id.recordID == GSF_RECORD_SWATH_BATHYMETRY_PING) {
                gsfSwathBathyPing *ping = &rec.mb_ping;

                if (ping->brb_inten) {
                    has_backscatter = 1;
                    break;  // Stop once backscatter is found
                }
            }
        }

        // Add file to appropriate list
        if (has_backscatter) {
            add_to_list(files_with_backscatter_list, files_with_backscatter, with_capacity, file_path);
        } else {
            add_to_list(files_missing_backscatter_list, files_missing_backscatter, missing_capacity, file_path);
        }


        gsfClose(handle);
    } else {
        printf("Failed to open GSF file: %s\n", file_path);
    }
}

void process_files_in_directory(
    const char* directory_path,
    FILE *report_file,
    int* files_with_backscatter,
    int* files_missing_backscatter,
    char ***files_with_backscatter_list,
    char ***files_missing_backscatter_list,
    int *with_capacity,
    int *missing_capacity
) {
    DIR *dir = opendir(directory_path);
    if (dir == NULL) {
        printf("Failed to open directory: %s\n", directory_path);
        return;
    }

    struct dirent *entry;
    struct stat file_stat;

    while ((entry = readdir(dir)) != NULL) {
        // Skip current and parent directory
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Build full path
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", directory_path, entry->d_name);

        // Get file status
        if (stat(path, &file_stat) != 0) {
            continue; // Failed to get status, skip
        }

        if (S_ISDIR(file_stat.st_mode)) {
            // Recurse into subdirectory
            process_files_in_directory(
                path,
                report_file,
                files_with_backscatter,
                files_missing_backscatter,
                files_with_backscatter_list,
                files_missing_backscatter_list,
                with_capacity,
                missing_capacity
            );
        } else if (S_ISREG(file_stat.st_mode)) {
            // Check if file has .gsf extension
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".gsf") == 0) {
                check_for_backscatter(path, report_file, files_with_backscatter, files_missing_backscatter, files_with_backscatter_list, files_missing_backscatter_list, with_capacity, missing_capacity);            
            }
        }
    }

    closedir(dir);
}


void generate_report(const char* input_directory, const char* output_directory) {
    char report_file_path[512];
    
    // Safely concatenate the output directory path with the report filename
    snprintf(report_file_path, sizeof(report_file_path), "%s/gsf_backscatter_report.txt", output_directory);

    FILE *report_file = fopen(report_file_path, "w");
    if (report_file == NULL) {
        printf("Failed to open report file for writing.\n");
        return;
    }

    char **files_with_backscatter_list = NULL;
    char **files_missing_backscatter_list = NULL;
    int files_with_backscatter = 0;
    int files_missing_backscatter = 0;
    int with_capacity = 0;
    int missing_capacity = 0;

    fprintf(report_file, "GSF Backscatter Data Report\n\n");
    fprintf(report_file, "Checking GSF files in directory: %s\n\n", input_directory);

    // Process all files in the directory
    process_files_in_directory(input_directory, report_file, &files_with_backscatter, &files_missing_backscatter, &files_with_backscatter_list, &files_missing_backscatter_list, &with_capacity, &missing_capacity);

    // Write summary
    fprintf(report_file, "\nSummary:\n");
    fprintf(report_file, "Total GSF files checked: %d\n", files_with_backscatter + files_missing_backscatter);
    fprintf(report_file, "Files with backscatter: %d\n", files_with_backscatter);
    fprintf(report_file, "Files missing backscatter: %d\n", files_missing_backscatter);

    // Write list of files with backscatter data
    fprintf(report_file, "Files with backscatter data:\n");
    for (int i = 0; i < files_with_backscatter; i++) {
        fprintf(report_file, "  %s\n", files_with_backscatter_list[i]);
    }

    // Write list of files missing backscatter data
    fprintf(report_file, "\nFiles missing backscatter data:\n");
    for (int i = 0; i < files_missing_backscatter; i++) {
        fprintf(report_file, "  %s\n", files_missing_backscatter_list[i]);
    }

    fclose(report_file);

    for (int i = 0; i < files_with_backscatter; i++) free(files_with_backscatter_list[i]);
    free(files_with_backscatter_list);

    for (int i = 0; i < files_missing_backscatter; i++) free(files_missing_backscatter_list[i]);
    free(files_missing_backscatter_list);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input_directory> <output_folder>\n", argv[0]);
        return 1;
    }

    const char* input_directory = argv[1];
    const char* output_folder = argv[2];

    // Process all GSF files in the directory
    generate_report(input_directory, output_folder);

    return 0;
}
