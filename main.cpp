#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <inttypes.h>
#define TWICE(x) x x

const int KB = 1024;
const int MB = 1024 * KB;
const int data_size = 32 * MB;
const int repeats = 64 * KB;
const int steps = 8 * MB;
const int times = 8;

long long clock_time() {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return (long long)(tp.tv_nsec + (long long)tp.tv_sec * 1000000000ll);
}

static inline uint64_t rdtsc(void)
{
	uint64_t result;
	asm volatile("rdtsc" : "=A" (result));
	return result;
}

int main() {
    int *data = (int *)malloc(data_size * sizeof(int));
    int *data1 = (int *)malloc(data_size * sizeof(int));
	if( data == NULL) {
		printf("Bad news\n");
		return 0;
	}
	srand(time(NULL));
	for (int i = 0;i < data_size; i++) {
		data[i] = rand();
	}
	// For cache miss
	for (int i = 0;i < data_size; i++) {
		data1[i] = rand();
	}
	int *p, d;
	FILE *f = fopen("data.txt", "w+");
    //int steps[] = { 1*KB, 4*KB, 8*KB, 16*KB, 32*KB, 64*KB, 128*KB, 256*KB, 512*KB, 1 * MB, 2 * MB, 4 * MB, 8 * MB, 16 * MB};
    int steps[] = { 1*KB, 4*KB, 8*KB, 16*KB, 24 * KB, 32*KB, 48 * KB,  64*KB, 128*KB, 
                    256*KB, 384 * KB, 512*KB, 1 * MB, 2 * MB, 3 * MB, 4 * MB, 
                    5 * MB, 6 * MB, 7 * MB, 8 * MB, 9 * MB};
	uint64_t taktsStart, taktsEnd;
	long long start, end;
    for (int i = 0; i <= sizeof(steps) / sizeof(int) - 1; i++) {
        double totalTime = 0;
		uint64_t totalTakts= 0;
        for (int k = 0; k < times; k++) {
            int size_mask = steps[i] / sizeof(int) - 1;
			srand(time(NULL));
            start = clock_time();
			taktsStart = rdtsc();
            for (int j = 0; j < repeats; j++) {
				TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE( d = ++*(data + ((j * 64) * rand()) % (steps[i]/sizeof(int))); 
												p = data + ((j * 64) * rand()) % (steps[i]/sizeof(int));)))))))
            }
			taktsEnd = rdtsc();
            end = clock_time();
			totalTakts += taktsEnd - taktsStart;
            totalTime += (end - start) / 1000000000.0;
        }
		fprintf(f, "%d %lf %lf %" PRIu64 "\n", steps[i] / KB, totalTime, totalTime / (repeats * times * 128) * 1e9, totalTakts);
        printf("%dKB time: %lf %lf ns %" PRIu64 "\n", steps[i] / KB, totalTime, totalTime / (repeats * times * 128) * 1e9, totalTakts);
    }
	fclose(f);
	system("gnuplot -persist cmd");
	system("gnuplot -persist ns");
    return 0;
}
