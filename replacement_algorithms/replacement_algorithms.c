#include "replacement_algorithms.h"

int lru(struct memory *mem){
  // LRU algorithm chooses to replace the page that was not referenced for the longest time.
  printf("\n~~~~~~~~~LRU~~~~~~~~~~\n");

  struct physical_memory *pm=mem->physicalMem;
  int index=0; // position of the oldest entry (victim frame)

  struct timespec min_t;
  min_t.tv_sec=pm->entries[0].latency.tv_sec;
  min_t.tv_nsec=pm->entries[0].latency.tv_nsec;

  // compare all entries time to find the oldest one
  for (int i=1; i < pm->size; ++i){
    time_t sec=pm->entries[i].latency.tv_sec;
    long nsec=pm->entries[i].latency.tv_nsec;

    if (sec<min_t.tv_sec || (sec==min_t.tv_sec && nsec<min_t.tv_nsec)){
      min_t.tv_sec=sec;
      min_t.tv_nsec=nsec;
      index=i;
    }
  }
  return index;
}


int secondChance(struct memory* mem) {
  // The Second Chance algorithm starts each time from the oldest page and continues until it finds the first page with a reference bit equal to 0.
  // If the reference bit of the page is 1 is given a second chance at this page.
  printf("\n~~~~~~~~~SECOND CHANCE~~~~~~~~~~\n");

  struct physical_memory* pm=mem->physicalMem;
  while(1){
    if (pm->secChanceIndex==pm->size) {
      pm->secChanceIndex=0;
    }
    if (pm->entries[pm->secChanceIndex].reference==1){
      pm->entries[pm->secChanceIndex].reference=0; // give at the page a second change, set the bit to 0
    }
    else{ // reference bit equal to 0
      return pm->secChanceIndex++; // found a page for removal
    }
    pm->secChanceIndex++;
  }
}
