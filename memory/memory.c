#include "replacement_algorithms.h"
#include "hash_table.h"
#include "memory.h"


void memory_init(struct memory* mem, char* algo, int frames, int nbuckets){
  mem->physicalMem=malloc(sizeof(struct physical_memory));
  mem->virtualMem=malloc(sizeof(struct virtual_memory));
  physical_memory_init(mem->physicalMem,frames);
  virtual_memory_init(mem->virtualMem,nbuckets);
  strcpy(mem->replacementAlgo,algo);
  mem->hdReads=0;
  mem->hdWrites=0;
  mem->pageFaults=0;
  mem->requests=0;
  printf(">>Memory Created\n");
}


void physical_memory_init(struct physical_memory* pm, int size) {
  pm->entries=malloc(sizeof(struct physical_memory_entry)*size);
  pm->size=size;
  pm->counter=0;
  pm->secChanceIndex=0;
  // initialize the entries/frames
  for (int i=0;i<pm->size;i++){
    pm->entries[i].set=0;
    pm->entries[i].dirty=0;
    pm->entries[i].reference=0;
  }
  printf(">>Physical Memory Created\n");
}


void virtual_memory_init(struct virtual_memory* vm,int buckets){
  // create and initialize the two hashed page tables for the two processes
  ht_initialize(&vm->ht[0],buckets);
  ht_initialize(&vm->ht[1],buckets);
  printf(">>Virtual Memory Created\n");
}


void memory_delete(struct memory* mem){
  virtual_memory_delete(mem->virtualMem);
  physical_memory_delete(mem->physicalMem);
  free(mem->virtualMem);
  free(mem->physicalMem);
  printf(">>Memory Deleted\n");
}


void physical_memory_delete(struct physical_memory* pm){
  free(pm->entries);
  printf(">>Physical Memory Deleted\n");
}


void virtual_memory_delete(struct virtual_memory* virtualMem){
  ht_delete(&(virtualMem->ht[0]));
  ht_delete(&(virtualMem->ht[1]));
  printf(">>Virtual Memory Deleted\n");
}


void memory_retrieve(struct memory *mem, unsigned int logical_address,char mode,int pid){
  // logical address is 32 bits
  // page size is equal to 4096 bytes -> log(4096)=12 bits -> offset is the last 12 bits of the logical address -> page number is the first 20 bits
  // logical address: [<---page#:20bits--->||<---offset:12bits--->]
  unsigned int page=logical_address >> 12; // "delete" offset -> take the page number
  // offset in this simulation doesn't need
  // unsigned int offset = (logical_address << 20) >> 20;
  struct timespec t;

  mem->requests++; // memory requests
  printf("\nPAGE: %d\n",page);
  ht_print(&(mem->virtualMem->ht[pid]));

  if (strcmp(mem->replacementAlgo,"lru")==0){
    clock_gettime(CLOCK_MONOTONIC, &t); // keep the time of the reference
  }

  // search the memory to check if page is already loaded
  if (memory_search(mem,page,mode,t,pid))
    return;

  // no, let's load the page from the hard disk
  // page not found-> page fault-> load the page from the hard disk
  mem->pageFaults++;
  mem->hdReads++;

  // if there is space available in memory, load the page
  if (memory_insert(mem,page,mode,t,pid))
    return;

  // if page doesn't fit, then an existing page must be replaced
  memory_replace_page(mem,page,mode,t,pid);
}


int memory_search(struct memory *mem, int page, char mode, struct timespec t,int pid){
  struct virtual_memory *vm=mem->virtualMem;
  struct physical_memory *pm=mem->physicalMem;

  // searching the page at the hashed page table
  int frame_num=ht_search(&vm->ht[pid],page,pid+1);
  if (frame_num==-1){ // not found
    printf(">Page doesn't exist in hashed page table.\n");
    return 0;
  }
  // found it
  // Then go to the corresponding frame
  if (mode=='W'){ // if mode is 'W' this means that the page modified -> page is now "dirty"
    pm->entries[frame_num].dirty=1;
  }

  if (strcmp(mem->replacementAlgo,"lru")==0){
    pm->entries[frame_num].latency=t; // renew the reference time of the page for the Least Recently Used (LRU) algorithm
  }
  else if(strcmp(mem->replacementAlgo,"second_chance")==0){
    pm->entries[frame_num].reference=1; // change the reference bit from 0 to 1 for the Second Chance algorithm, page now has a second chance
  }

  return 1;
}


int memory_insert(struct memory* mem, int page, char mode, struct timespec t, int pid) {
    struct virtual_memory *vm=mem->virtualMem;
    struct physical_memory *pm=mem->physicalMem;
    if (physical_memory_full(pm)) return 0; // there is no frame available to load the page (memory is full)

    int frame_num=physical_memory_set(mem,mode,t); // "allocate" a frame to load a page
    ht_insert(&vm->ht[pid],page,frame_num); // load page at the hashed page table on the frame that already allocated

    if (mode == 'W'){ // if mode is 'W' this means that the page modified -> page is now "dirty"
      pm->entries[frame_num].dirty=1;
    }
    printf(">Page:%d just loaded.\n",page);
    return 1;
}


void memory_replace_page(struct memory* mem, int page, char mode,struct timespec  t, int pid) {
  // replace a page based on the page replacement algorithm (which is chosen by the user)
  int index; // index is the "victim frame"
  if (strcmp(mem->replacementAlgo,"lru")==0){
    index=lru(mem);
  }
  else if(strcmp(mem->replacementAlgo,"second_chance")==0){
    index=secondChance(mem);
  }
  memory_delete_entry(mem,index); // write (delete) the victim frame back to hard disk (if this has been changed/if page is dirty)
  memory_insert(mem,page,mode,t,pid); // load the new page at the frame that previous freed
}


void memory_delete_entry(struct memory *mem, int index){
  // delete the corresponding page from the hashed page table which has been loaded
  // (index is the "victim frame")
  if(ht_delete_node(&mem->virtualMem->ht[0],index)){
    printf(">Page deleted from the hashed page table of the 1st process.\n");
  }

  if(ht_delete_node(&mem->virtualMem->ht[1],index)){
    printf(">Page deleted from the hashed page table of the 2nd process.\n");
  }
  // write (delete) the victim frame back to hard disk (if this has been changed/if page is dirty)
  if (mem->physicalMem->entries[index].dirty==1){
    mem->hdWrites++;
  }
  // and finally free this frame (physical memory entry)
  mem->physicalMem->entries[index].set=0;
  mem->physicalMem->entries[index].dirty=0;
  mem->physicalMem->entries[index].reference=0;
  mem->physicalMem->counter--;
}


int physical_memory_set(struct memory* mem, int mode, struct timespec t){
  // find an empty frame to the physical memory to save the page
  // then return the corresponding frame to load the page on the hashed page table
  struct physical_memory *pm=mem->physicalMem;
  for (int i=0; i<pm->size; i++){
    if (pm->entries[i].set==0){
      pm->entries[i].set=1;
      if (strcmp(mem->replacementAlgo,"lru")==0){
        pm->entries[i].latency=t; // save the reference time of the page for the Least Recently Used (LRU) algorithm
      }
      else if(strcmp(mem->replacementAlgo,"second_chance")==0){
        pm->entries[i].reference=0;
      }
      pm->counter++;
      return i;
    }
  }
  return -1;
}


int physical_memory_full(struct physical_memory* pm){
    return (pm->counter==pm->size);
}


void memory_stats(struct memory *mem){
  printf("\n*****************************************************\n");
  printf("\n-------------- Simulation Statistics --------------\n");
  printf("\n*****************************************************\n");

  printf(">HardDrive Reads:%d\n",mem->hdReads);
  printf(">HardDrive Writes:%d\n",mem->hdWrites);
  printf(">Page Faults:%d\n",mem->pageFaults);
  printf(">Reads from txt files/Memory Requests:%d\n",mem->requests);
  printf(">Frames number:%d\n",mem->physicalMem->size);
  printf("\n*****************************************************\n");
}
