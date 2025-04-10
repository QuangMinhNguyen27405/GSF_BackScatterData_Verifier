#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "gsf.h"

void check_for_backscatter(const char* file_path, FILE *report_file, int* files_with_backscatter, int* files_missing_backscatter) {
    int handle;
    gsfDataID id;
    gsfRecords rec;
    int has_backscatter = 0;

    // Open the GSF file
    if (gsfOpen(file_path, GSF_READONLY, &handle) == 0) {
        printf("Checking GSF file: %s\n", file_path);

        while (gsfRead(handle, GSF_NEXT_RECORD, &id, &rec, NULL, 0) > 0) {
            if (id.recordID == GSF_RECORD_SWATH_BATHYMETRY_PING) {
                gsfSwathBathyPing *ping = &rec.mb_ping;

                if (ping->brb_inten) {
                    has_backscatter = 1;
                    (*files_with_backscatter)++;
                    break;  // Stop once backscatter is found
                }
            }
        }

        if (!has_backscatter) {
            (*files_missing_backscatter)++;
            fprintf(report_file, "Missing backscatter data: %s\n", file_path);
        }

        gsfClose(handle);
    } else {
        printf("Failed to open GSF file: %s\n", file_path);
    }
}

void process_files_in_directory(const char* directory_path, FILE *report_file, int* files_with_backscatter, int* files_missing_backscatter) {
    DIR *dir = opendir(directory_path);
    if (dir == NULL) {
        printf("Failed to open directory: %s\n", directory_path);
        return;
    }

    struct dirent *entry;
    struct stat file_stat;

    while ((entry = readdir(dir)) != NULL) {
        // Skip current and parent directory
        if (entry->d_name[0] == '.' && (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))) {
            continue;
        }

        // Build full file path
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, entry->d_name);

        // Check if it's a regular file using stat()
        if (stat(file_path, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
            // Check if file has .gsf extension
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".gsf") == 0) {
                // Check if backscatter data is present in the GSF file
                check_for_backscatter(file_path, report_file, files_with_backscatter, files_missing_backscatter);
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

    int files_with_backscatter = 0;
    int files_missing_backscatter = 0;

    fprintf(report_file, "GSF Backscatter Data Report\n\n");
    fprintf(report_file, "Checking GSF files in directory: %s\n\n", input_directory);

    // Process all files in the directory
    process_files_in_directory(input_directory, report_file, &files_with_backscatter, &files_missing_backscatter);

    printf(input_directory);

    fprintf(report_file, "\nSummary:\n\n");
    fprintf(report_file, "Total GSF files checked: %d\n", files_with_backscatter + files_missing_backscatter);
    fprintf(report_file, "Files with backscatter: %d\n", files_with_backscatter);
    fprintf(report_file, "Files missing backscatter: %d\n", files_missing_backscatter);

    fclose(report_file);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_directory>\n", argv[0]);
        return 1;
    }

    const char* input_directory = argv[1];
    const char* output_directory = argv[2];

    // Generate the report
    generate_report(input_directory, output_directory);

    return 0;
}
