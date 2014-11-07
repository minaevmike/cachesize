#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <inttypes.h>
#define TWICE(x) x x

const int KB = 1024;
const int MB = 1024 * KB;
const int data_size = 10 * MB;
const int repeats = 512 * KB;
const int steps = 8 * MB;
const int times = 10;

long long clock_time() {
    struct timespec tp;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
    return (long long)(tp.tv_nsec + (long long)tp.tv_sec * 1000000000ll);
}

static inline uint64_t rdtsc(void)
{
	uint64_t result;
	asm volatile("rdtsc" : "=A" (result));
	return result;
}

void shuffle(int *array, size_t n)
{
	if (n > 1) 
	{
		size_t i;
		for (i = 0; i < n - 1; i++) 
		{
			size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
			int t = array[j];
			array[j] = array[i];
			array[i] = t;
		}
	}
}

int main() {
    int *data = (int *)malloc(data_size * sizeof(int));
	if( data == NULL) {
		printf("Bad news\n");
		return 0;
	}
	srand(time(NULL));
	for (int i = 0;i < data_size; i++) {
		data[i] = rand();
	}
	int *p, d;
	FILE *f = fopen("data.txt", "w+");
    int steps[] = { 1*KB, 4*KB, 8*KB, 16*KB, 20 * KB, 32*KB, 40 * KB, 64*KB, 128*KB, 192 * KB, 256*KB, 320 * KB, 512*KB, 1 * MB, 1 * MB + 512 * KB, 1 * MB + 3 * 256 * KB, 2 * MB, 4 * MB, 8 * MB};
    //int steps[] = { 1*KB, 4*KB, 8*KB, 16*KB, 24 * KB, 32*KB, 48 * KB,  64*KB, 128*KB,    256*KB, 384 * KB, 512*KB, 1 * MB, 2 * MB, 3 * MB, 4 * MB,  5 * MB, 6 * MB, 7 * MB, 8 * MB, 9 * MB};
	uint64_t taktsStart, taktsEnd;
	long long start, end;
	int last = 0;
    for (int i = 0; i <= sizeof(steps) / sizeof(int) - 1; i++) {
		int arr_byte_size = steps[i];
		int arr_size = steps[i] / sizeof(void*);
		void **d = (void**) malloc(arr_byte_size);
		int rands[arr_size];
		for(int l = 0; l < arr_size; l++)
			rands[l] = l + 1;
		shuffle(rands, arr_size);
		int next = rands[arr_size - 1];
		d[0] = d + next;
		for (int l = arr_size - 2; l > 0; l--){
			int val = rands[l];
			d[next] = d + val;
			next = val;
		}
		d[next] = d;
		void **c = d;
        double totalTime = 0, totalTimePosl = 0;
		uint64_t totalTakts= 0;
		shuffle(data, data_size);
		start = clock_time();
		for(int j = 0; j < repeats; j++) {
			TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(c = (void **)*c;)))))))));
		}
		end = clock_time();
		totalTime += (end - start) / 1e9;
		/*for(int k = 0; k < times; k++) {
			start = clock_time();
			for(int j = 0; j < arr_size; j++) {
				TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(TWICE(data[j]++;)))))))));
			}
			end = clock_time();
			totalTimePosl += (end - start) / 1e9;
		}*/
		fprintf(f, "%d %lf %lf \n", steps[i] / KB, totalTime, totalTime / (512*repeats) * 1e9);
        printf("%dKB time: %lf %lf ns \n", steps[i] / KB, totalTime, totalTime /(512* repeats) * 1e9 );
		free(d);
    }
	fclose(f);
	system("gnuplot -persist ns");
    return 0;
}
