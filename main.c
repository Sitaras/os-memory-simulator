#include "hash_table.h"
#include "memory.h"

int reading(FILE *file, unsigned int *address, char *mode){
  if (feof(file)){
    return 0;
  }
  fscanf(file, "%x ", address); // the address is in hexadecimal form
  fscanf(file, "%c ", mode); // mode is 'R' (read) of 'W' (write)
  return 1;
}

int main(int argc, char const *argv[]){

  const int buckets=10; // hash table buckets

  if (argc<5){
    printf("Program execution template:\n./simulator <lru/second_chance> <frames_number> <references_per_process> <maximum_references>\n");
    printf("\n>>Needed Arguments:\n1)The paging replacement algorithm (lru or second_chance)\n");
    printf("2)The number of frames (positive number)\n3)The number of references (q) per process (positive number)\n4)The max number of references\n");
    exit(1);
  }

  if(strcmp(argv[1],"lru")!=0 && strcmp(argv[1],"second_chance")!=0){
    printf("\n>>Wrong input!\n Please, type: 'lru' or 'second_chance'.\n");
    exit(1);
  }

  int frames=atoi(argv[2]);  // physical memory frames
  int q=atoi(argv[3]);       // references per process
  int maxRefs=atoi(argv[4]); // maximum number of references

  if (frames<=0){
    printf("\n>>Wrong input!\n The number of frames should be positive.\n");
    exit(1);
  }

  if (q<=0){
    printf("\n>>Wrong input!\n The number of references (q) should be positive.\n");
    exit(1);
  }

  if (maxRefs<-1){
    printf("\n>>Wrong input!\n The max number of references should be positive or -1.\n");
    exit(1);
  }

  char algorithm[21];
  strcpy(algorithm,argv[1]);
  printf("Page Replacement Algorithm: %s\n",algorithm);

  // open trace files
  FILE *tr1=fopen("./traces/gcc.trace", "r");
  if (tr1==NULL){
    perror("fopen");
    exit(1);
  }

  FILE *tr2=fopen("./traces/bzip.trace","r");
  if (tr2==NULL){
    perror("fopen");
    exit(1);
  }

  // initialize the memory (physical and virtual memory)
  struct memory* mem=malloc(sizeof(struct memory));
  memory_init(mem,algorithm,frames,buckets);


  unsigned int addr;
  char mode;            // 'R'/'W'
  int flag=0;           // flag for the EOF check
  int counter=0;
  // simulation continues while the total number of process reports (counter) haven't reached the maximum references yet
  // or until reached the end of a trace file then the maxRefs is undefined (maxRefs==-1)
  while (counter<maxRefs || maxRefs==-1){

    // process 1
    for (int i=1; i<=q; i++){
      if (reading(tr1,&addr,&mode)==0){
        flag=1;
        break;
      }
      counter++;
      printf("\n\n~~~~~~~Process 1~~~~~~~\n\n");
      memory_retrieve(mem,addr,mode,0);   // retrieve address from memory
    }
    ///////////////////////////////////////////////////////////////////////////////////

    // process 2
    for (int i=1; i<=q; i++){
      if (reading(tr2,&addr,&mode)==0){
        flag=1;
        break;
      }
      counter++;
      printf("\n\n~~~~~~~Process 2~~~~~~~\n\n");
      memory_retrieve(mem,addr,mode,1);   // retrieve address from memory
    }
    ///////////////////////////////////////////////////////////////////////////////////

    if (flag==1){ // check for EOF in at least 1 file
      break;
    }
  }
  printf("\n*****************************************************\n");
  printf("\n-------------- Simulation Ended --------------\n");
  printf("\n*****************************************************\n");
  memory_stats(mem);

  memory_delete(mem);
  free(mem);

  fclose(tr1);
  fclose(tr2);


  return 0;
}
