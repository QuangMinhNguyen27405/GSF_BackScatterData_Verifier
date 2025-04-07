// Run command: 
#include <stdio.h>
#include "gsf.h"

int main() {
    int handle;
    gsfDataID id;
    gsfRecords rec;

    // Replace to your gsf file path
    if (gsfOpen("1-PAN-09-13905-ANA-20241204025604(Head1)_MBE.gsf", GSF_READONLY, &handle) == 0) {
        printf("File opened successfully!\n");

        while (gsfRead(handle, GSF_NEXT_RECORD, &id, &rec, NULL, 0) > 0) {
            if (id.recordID == GSF_RECORD_SWATH_BATHYMETRY_PING) {
                printf("Found one record!\n");
                gsfSwathBathyPing *ping = &rec.mb_ping;

                if (ping->brb_inten) {
                    printf("\n[BRB_INTENSITY]\n");
                    printf("  Bits per sample     : %d\n", ping->brb_inten->bits_per_sample);
                    printf("  Applied corrections : 0x%08X\n", ping->brb_inten->applied_corrections);

                    int beams = ping->number_beams;
                    for (int i = 0; i < beams && i < 5; i++) {  // Limit to first 5 beams for demo
                        gsfTimeSeriesIntensity *ts = &ping->brb_inten->time_series[i];
                        printf("  Beam %d:\n", i);
                        printf("    Sample count        : %d\n", ts->sample_count);
                        printf("    Detect sample index : %d\n", ts->detect_sample);
                        printf("    Start range sample  : %d\n", ts->start_range_samples);

                        for (int j = 0; j < ts->sample_count && j < 10; j++) {
                            printf("      Sample[%d] = %u\n", j, ts->samples[j]);
                        }
                    }
                    break;  // stop after first ping with backscatter
                }
            }
        }

        gsfClose(handle);
    } else {
        printf("Failed to open GSF file.\n");
    }

    return 0;
}

