#include "cachelab.h"
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>

typedef unsigned long long mem_64;

typedef struct {

	// Line Parameters //
	mem_64 tag;
	char* block;
	int valid;

	// Cycles Since Used //
	//   (For Eviction)  //
	int age;

} Line;

typedef struct {

	// Lines //
	Line* lines;

} Set;

typedef struct {
	
	// Cache Parameters //
	int E;
	int B;
	int S;
	int b;
	int s;
	int tag_size;

	// Output Parameters //
	int hits;
	int misses;
	int evicts;

	// Sets //
	Set* sets;	

} Cache;

Line line_build(int B)
{
	// Calloc Line //
	Line line;

	line.block = (char*) calloc(B, sizeof(char));

	return line;
}

Set set_build(int E, int B)
{
	// Calloc Set //
	Set set;

	set.lines = (Line*) calloc(E, sizeof(Line));


	int i;
	Line* lines = set.lines;
	for (i = 0; i < E; i++)
	{
		lines[i] = line_build(B);
	}

	return set;
}

Cache* cache_build(int s, int E, int b)
{
	// Calloc Cache //
	Cache* cache;
	cache = (Cache*) calloc(1, sizeof(Cache));
	// Set Inputs //
	cache->E = E;
	cache->b = b;
	cache->s = s;

	// Initialize Outputs //
	cache->hits = 0;
	cache->misses = 0;
	cache->evicts = 0;

	// Calculate Parameters //
	cache->S = 2^s;
	cache->B = 2^b;
	cache->tag_size = sizeof(mem_64) - (s + b);

	int i;
	int S = cache->S;
	int B = cache->B;

	cache->sets = (Set*) calloc(S, sizeof(Set));
	Set* sets = cache->sets;

	for(i = 0; i < S; i++)
	{
		sets[i] = set_build(E, B);
	}

	return cache;
}

void line_free(Line line, int B)
{
	char* block = line.block;
	free(block);
	return;
}

void set_free(Set set, int E, int B)
{
	Line* lines = set.lines;
	
	int i;
	for(i = 0; i < E; i++)
	{
		line_free(lines[i], B);
	}

	free(lines);

	return;
}

void cache_free(Cache* cache)
{
	Set* sets = cache->sets;
	int S = cache->S;
	int E = cache->E;
	int B = cache->B;

	int i;
	for (i = 0; i < S; i++)
	{
		set_free(sets[i], E, B);
	}

	free(sets);
	free(cache);
	return;
}


int main(int argc, char* argv[])
{
	int v = 0;
	int s;
	int E;
	int b;
	char* t = "H";
	FILE* trace;

	char flag = getopt(argc, argv, "s:E:b:t:v");
	do
	{
		switch(flag)
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
				t = optarg;
				break;

			case 'v':
				v = 1;
				break;

			default:
				printf("Please Enter Arguments for the Cache\n");
				return -1;
		}
	} while ((flag = getopt(argc, argv, "s:E:b:t:v")) != -1);

	if (s == 0 || E == 0 || b == 0 || t == NULL)
	{
		printf("Please Provide All Arguments for the Cache\n");
		return -1;
	}

	// Open and Read Trace File //
	trace = fopen(t, "r");
	if(trace != NULL)
	{
		char instruction;
		mem_64 address;
		int size; 
		while(fscanf(trace,"%c %llx,%d", &instruction, &address, &size) == 3)
		{
			switch(instruction)
			{
				case 'L':
				break;

				case 'S':
				break;

				case'M':
				break;

				default:
				break;
			}
		}
	}
	else
	{
		printf("Please provide a trace file\n");
	}

	printf("%d\n", v);
	printf("%s\n", t);

	Cache* cache = cache_build(s,E,b);
    printSummary(0, 0, 0);
    fclose(trace);
    cache_free(cache);

    return 0;
}

