//===============================================================
// Dagmawi Zerihun
// April 2023
// csim.c
// Cachelab
//===============================================================
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "cachelab.h"

/*** TYPES ***/
typedef unsigned long long int ADDR_T;

// c style structs for your cache construction
typedef struct Line_T
{
    char valid;
    int tag;
    int timestamp;
}Line_T;

typedef struct Set_T
{
    Line_T *lines;
    int curr_time;
}Set_T;

typedef struct Cache_T
{
    Set_T *sets;
}Cache_T;

/*** Global Arguments ***/
int s = 0;           /* number of set bits */
int S;               /* number of Sets = 2^s */
int E = 0;           /* number of lines per set */
int b = 0;           /* number of block bits */
int B;               /* number of blocks = 2^b */
int verbose = 0;     /* toggle for verbose mode */
char *tracefile = 0; /* filename of trace input file */

int num_hits = 0;
int num_misses = 0;
int num_evictions = 0;
int LRU_counter = 0; // counter for keeping track of timestamps

int t; /* number of bits for tag */

/*** function declarations ***/
void printUsage(char *argv[]);
Cache_T buildCache();
void initializeCache(Cache_T cache);
void freeCache(Cache_T cache, int S, int E);
void simulateCache(Cache_T cache);
void updateCache(Cache_T cache, ADDR_T address);
int findLRU(Cache_T cache, int set);

//===============================================================
//===============================================================
int main(int argc, char *argv[])
{

    int c;

    while ((c = getopt(argc, argv, "s:E:b:t:v")) != -1)
    {
        switch (c)
        {
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            tracefile = optarg;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        };
    }
    S = 1 << s; // number of sets
    B = 1 << b; // number of blocks per line
    t = 64 - (s + b);
    // comment this out later once you are sure you have
    // command line options working.
    // printf("S[%d,%d] E[%d] B[%d,%d] v[%d] t[%s]\n",
    //  S,s,E,B,b,verbose,tracefile);

    Cache_T cache = buildCache();
    //printf("Cache built\n");
    initializeCache(cache);
    //printf("Cache initialized\n");
    simulateCache(cache);
    //printf("Simulation Done\n");

    printSummary(num_hits, num_misses, num_evictions);

    freeCache(cache, S, E);

    return 0;
}

//===============================================================
//===============================================================

void printUsage(char *argv[])
{
    printf("Usage: \n");
    printf("%s -s #setbits -E #lines -b #blockbits -t tracefilename -v -h\n", argv[0]);
    printf("where s specifies number of sets S = 2^s\n");
    printf("where E specifies number of lines per set\n");
    printf("where b specifies size of blocks B = 2^b\n");
    printf("where t specifies name of tracefile\n");
    printf("where v turns no verbose mode (default is off)\n");
    printf("where h prints this help message\n");
}

/* returns a cache struct by allocating memory for its sets and respective lines using global variables S and E*/
Cache_T buildCache()
{
    Cache_T cache;
    cache.sets = (Set_T *)malloc(sizeof(Set_T) * S); // allocate space for sets
    // Allocates space for lines in each set`
    for (int i = 0; i < S; i++)
    {
        cache.sets[i].lines = (Line_T *)malloc(sizeof(Line_T) * E);
    }

    return cache;
}

/*sets initial values for valid bit, tag and timestamp of each line in each set of the Cache*/
void initializeCache(Cache_T cache) {
    for (int set_index = 0; set_index < S; set_index++) {
        for (int line_index = 0; line_index < E; line_index++) {
            cache.sets[set_index].lines[line_index].valid = 0;
            cache.sets[set_index].lines[line_index].tag = 0;
            cache.sets[set_index].lines[line_index].timestamp = 0;
        }
    }
}


/* Frees up memory occupied by cache */
void freeCache(Cache_T cache, int S, int E)
{
    for (int set_index = 0; set_index < S; set_index++)
    {
        if (cache.sets[set_index].lines != NULL)
        {
            free(cache.sets[set_index].lines);
        }
    }

    if (cache.sets != NULL)
    {
        free(cache.sets);
    }

    return;
}

/* Reads the trace file, accesses the cache , and processes access according to L,S and M */
void simulateCache(Cache_T cache)
{
    printf("Cache built\n");
    FILE *trace_fp = fopen(tracefile, "r");
    char trace_cmd;
    ADDR_T address;
    int size;

    //check if trace file exists
    if(trace_fp != NULL){
        while (fscanf(trace_fp, " %c %llx,%d", &trace_cmd, &address, &size) == 3)
        {
	    switch (trace_cmd)
	    {
	    case 'L':
	        updateCache(cache, address);
	        break;
	    case 'S':
	        updateCache(cache, address);
	        break;
	    case 'M':
	        updateCache(cache, address);
	        updateCache(cache, address);
	        break;
	    default:
	        break;
	    }
        }

        fclose(trace_fp);
     }
    }

/* Handles cache access, and updates hits, misses and evictions based on LRU policy */

void updateCache(Cache_T cache, ADDR_T address)
{ 

    //calculate tag and set using the input address
    ADDR_T set = (address >> b) & ((1 << s) - 1);
    ADDR_T tag = address >> (s + b);
    int stamp = LRU_counter;
    int stamp_index = 0;
    int found_empty_line = 0;

    for (int line = 0; line < E; line++)
    {
        Line_T *line_obj = &cache.sets[set].lines[line];

        // If is valid and tag is a match(hit), we hit the cache line
        if (line_obj->valid == 1)
        {
            if (line_obj->tag == tag)
            {
                num_hits++;
                line_obj->timestamp = LRU_counter++;
                return;
            }

            // Update stamp
            if (line_obj->timestamp < stamp)
            {
                stamp = line_obj->timestamp;
                stamp_index = line;
            }
        }
        // If valid is 0 and we haven't found an empty line yet(miss), then we insert this block in our cache line
        else if (!found_empty_line)
        {
            found_empty_line = 1;
            line_obj->valid = 1;
            line_obj->tag = tag;
            line_obj->timestamp = LRU_counter++;
            num_misses++;
            return;
        }
    }

    // If all lines in set are valid & no empty line is found, then we evict the LRU block
    num_evictions++;
    cache.sets[set].lines[stamp_index].tag = tag;
    cache.sets[set].lines[stamp_index].timestamp = LRU_counter++;
}




/*  returns the Least Recently Used line by iterating through all the lines in a set and finding the minimum timestamp */
int findLRU(Cache_T cache, int set)
{
    int LRU_timestamp = cache.sets[set].lines[0].timestamp;

    for (int line = 0; line < E; line++)
    {
        Line_T line_obj = cache.sets[set].lines[line];
        if (line_obj.timestamp <= LRU_timestamp)
        {
            LRU_timestamp = line_obj.timestamp;
        }
    }

    return LRU_timestamp;
}

