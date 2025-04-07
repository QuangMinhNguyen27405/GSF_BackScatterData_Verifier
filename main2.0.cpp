#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsf.h"

void process_gsf_file(const char* file_path, const char* output_folder) {
    int handle;
    gsfDataID id;
    gsfRecords rec;

    // Open the GSF file
    if (gsfOpen(file_path, GSF_READONLY, &handle) == 0) {
        printf("File opened successfully: %s\n", file_path);
        
        // Prepare the output file path
        char output_file_path[512];
        snprintf(output_file_path, sizeof(output_file_path), "%s/%s.txt", output_folder, strrchr(file_path, '/') + 1);

        FILE *output_file = fopen(output_file_path, "w");
        if (output_file == NULL) {
            printf("Failed to open output file: %s\n", output_file_path);
            return;
        }

        while (gsfRead(handle, GSF_NEXT_RECORD, &id, &rec, NULL, 0) > 0) {
            if (id.recordID == GSF_RECORD_SWATH_BATHYMETRY_PING) {
                fprintf(output_file, "Found one record in file: %s\n", file_path);
                gsfSwathBathyPing *ping = &rec.mb_ping;

                if (ping->brb_inten) {
                    fprintf(output_file, "\n[BRB_INTENSITY]\n");
                    fprintf(output_file, "  Bits per sample     : %d\n", ping->brb_inten->bits_per_sample);
                    fprintf(output_file, "  Applied corrections : 0x%08X\n", ping->brb_inten->applied_corrections);

                    int beams = ping->number_beams;
                    for (int i = 0; i < beams; i++) {  // Limit to first 5 beams for demo
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

        fclose(output_file);
        gsfClose(handle);
    } else {
        printf("Failed to open GSF file: %s\n", file_path);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input_gsf_file> <output_folder>\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_folder = argv[2];

    // Process the GSF file
    process_gsf_file(input_file, output_folder);

    return 0;
}
