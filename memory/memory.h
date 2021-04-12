#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hash_table.h"

struct memory{
  struct physical_memory* physicalMem;
  struct virtual_memory* virtualMem;

  char replacementAlgo[21];
  int hdReads;
  int hdWrites;
  int pageFaults;
  int requests;
};


struct virtual_memory{
  hashtable ht[2];
};


struct physical_memory{
  struct physical_memory_entry* entries;
  int size; // size is equal with the frames number
  int counter; // counter for the seted entries
  int secChanceIndex;
};


struct physical_memory_entry{
  int set; // 0/1
  int dirty; // 0/1 (dirty bit)
  int reference; // 0/1
  struct timespec latency; // time of last reference
};


void memory_init(struct memory*,char*,int,int);
void memory_retrieve(struct memory*, unsigned int,char,int);
int memory_insert(struct memory*,int,char,struct timespec,int);
int memory_search(struct memory*,int,char,struct timespec,int);
void memory_replace_page(struct memory*,int,char,struct timespec,int);
void memory_delete_entry(struct memory*,int);
void memory_delete(struct memory*);
void memory_stats(struct memory*);

void physical_memory_init(struct physical_memory*,int);
int physical_memory_set(struct memory*,int,struct timespec);
int physical_memory_full(struct physical_memory*);
void physical_memory_delete(struct physical_memory*);

void virtual_memory_init(struct virtual_memory*,int);
void virtual_memory_delete(struct virtual_memory*);

#endif
