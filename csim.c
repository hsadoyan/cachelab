#include "cachelab.h"
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>


// Typedef a memory address //
typedef unsigned long long mem_64;


// declare a line struct //
typedef struct {

	// Line Parameters //
	mem_64 tag;
	char* block;
	int valid;

	// Cycles Since Used //
	//   (For Eviction)  //
	int age;

} Line;

// declare a set struct //
typedef struct {

	// Lines //
	Line* lines;

} Set;

// declare a cache struct //
typedef struct {
	
	// Cache Parameters //
	int E;
	mem_64 B;
	mem_64 S;
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

// Builds a line. Called by set_build //
Line line_build(int B)
{
	
	Line line;
	// Calloc space for the block //
	line.block = (char*) calloc(B, sizeof(char));
	line.valid = 0;

	return line;
}


// Builds a set. Called by cache_build//
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

// Builds a cache //
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
	cache->S = 2 << s;
	cache->B = 2 << b;
	cache->tag_size = sizeof(mem_64) - (s + b);

	int i;
	int S = cache->S;
	int B = cache->B;

	// Calloc space for a set
	cache->sets = (Set*) calloc(S, sizeof(Set));
	Set* sets = cache->sets;


	// Build an S number of sets //
	for(i = 0; i < S; i++)
	{
		sets[i] = set_build(E, B);
	}

	return cache;
}

// Frees the memory for a line. Called by set_free //
void line_free(Line line, int B)
{
	char* block = line.block;
	free(block);
	return;
}

// Frees the memory for a set. Called by cache_free //
void set_free(Set set, int E, int B)
{
	Line* lines = set.lines;
	
	int i;

	//Frees all the lines using line_free
	for(i = 0; i < E; i++)
	{
		line_free(lines[i], B);
	}

	free(lines);

	return;
}

// Frees the entire cache //
void cache_free(Cache* cache)
{
	Set* sets = cache->sets;
	int S = cache->S;
	int E = cache->E;
	int B = cache->B;

	int i;

	// Frees the individual sets 
	for (i = 0; i < S; i++)
	{
		set_free(sets[i], E, B);
	}

	// Frees the cache and sets
	free(sets);
	free(cache);
	return;
}

// Searches a specific set for the proper tag-valid combination //
Line* set_search(Set* set, mem_64 tag, int E)
{
	int i;
	Line line; 
	for (i = 0; i < E; i++)
	{
		line = set->lines[i];
		if(line.tag == tag && line.valid)
		{
			return &(set->lines[i]);
		}
	}

	return NULL;
}
void cache_hit(Cache* cache, Set* search_set, Line* found_line, int E)
{
	int i;
	for(i = 0; i < E; i++)
	{
		Line* current_line = &(search_set->lines[i]);
		if (current_line != found_line)
		{
			current_line->age++;
		}
		else
		{
			current_line->age = 0;
		}
	}
	cache->hits++;
	return;
}
void cache_miss(Cache* cache, Set* search_set, int E, mem_64 tag)
{
	cache->misses++;
	Line* oldest_line = NULL;
	int i;
	for (i = 0; i < E; i++)
	{
		Line* current_line = &(search_set->lines[i]);
		if (!current_line->valid)
		{
			current_line->tag = tag;
			current_line->age = 0;
			current_line->valid = 1;
			return;
		}
		else if(oldest_line == NULL || current_line->age > oldest_line->age)
		{
			oldest_line = current_line;
		}
	}

	oldest_line->tag = tag;
	oldest_line->age = 0;
	cache->evicts++;
	return;

}

void cache_search(Cache* cache, mem_64 address)
{
	int b = cache->b;
	int s = cache->s;
	int E = cache->E;
	int tag_size = cache->tag_size;

	int set_number = (address >> b) & ~(~0 << s);
	mem_64 tag = (address >> (s + b)) & ~(~0 << tag_size);

	Set* search_set = &(cache->sets[set_number]);
	Line* found_line = set_search(search_set, tag, E);

	// Call cache_hit
	if(found_line != NULL)
	{
		cache_hit(cache, search_set, found_line, E);
		return;
	}
	else //Call cache_miss
	{
		cache_miss(cache, search_set, E, tag);
		return;
	}


}

int v = 0;

int main(int argc, char* argv[])
{
	int s;
	int E;
	int b;
	char* t = "H";
	FILE* trace;


	//Read the flags from the console
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

	//Check that valid arguments were provided.
	if (s == 0 || E == 0 || b == 0 || t == NULL)
	{
		printf("Please Provide Valid Arguments for the Cache\n");
		return -1;
	}

	Cache* cache = cache_build(s,E,b);

	// Open and Read Trace File //
	trace = fopen(t, "r");
	if(trace != NULL)
	{
		char instruction;
		mem_64 address;
		int size; 
		while(fscanf(trace, " %c %llx,%d", &instruction, &address, &size) == 3)
		{
			switch(instruction)
			{
				case 'L':
				cache_search(cache, address);
				break;

				case 'S':
				cache_search(cache, address);
				break;

				case'M':
				cache_search(cache, address);
				cache_search(cache, address);
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


    printSummary(cache->hits, cache->misses, cache->evicts);

    //Wrap it up.
    fclose(trace);
    cache_free(cache);

    return 0;
}
