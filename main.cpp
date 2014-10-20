#include <cstdio>
#include <time.h>
#include <sys/mman.h>
#define TWICE(x) x x

const int KB = 1024;
const int MB = 1024 * KB;
const int data_size = 32 * MB;
const int repeats = 64 * MB;
const int steps = 8 * MB;
const int times = 8;

long long clock_time() {
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return (long long)(tp.tv_nsec + (long long)tp.tv_sec * 1000000000ll);
}

int main() {
    // allocate memory and lock
    void* map = mmap(NULL, (size_t)data_size, PROT_READ | PROT_WRITE, 
                     MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    if (map == MAP_FAILED) {
        return 0;
    }
    int* data = (int*)map;
	int *p, d;
    // write all to avoid paging on demand
    for (int i = 0;i< data_size / sizeof(int);i++) {
        data[i]++;
    }

    int steps[] = { 1*KB, 4*KB, 8*KB, 16*KB, 24 * KB, 32*KB, 64*KB, 128*KB, 
                    128*KB*2, 128*KB*3, 512*KB, 1 * MB, 2 * MB, 3 * MB, 4 * MB, 
                    5 * MB, 6 * MB, 7 * MB, 8 * MB, 9 * MB};
    for (int i = 0; i <= sizeof(steps) / sizeof(int) - 1; i++) {
        double totalTime = 0;    
        for (int k = 0; k < times; k++) {
            int size_mask = steps[i] / sizeof(int) - 1;
            long long start = clock_time();
            for (int j = 0; j < repeats; j++) {
				data[ (j * 64) % (steps[i]/sizeof(int))]++;
            }
            long long end = clock_time();
            totalTime += (end - start) / 1000000000.0;
        }
        printf("%dKB time: %lf\n", steps[i] / KB, totalTime);
    }
    munmap(map, (size_t)data_size);
    return 0;
}
