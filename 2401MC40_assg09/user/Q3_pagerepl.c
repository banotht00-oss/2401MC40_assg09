#include "types.h"
#include "user.h"

#define MAXFRAMES 16
#define MAXREFS   128

static int defaultrefs[] = {
  7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
  0, 3, 2, 1, 2, 0, 1, 7, 0, 1
};

static void
usage(void)
{
  printf(2, "usage: pagerepl [frames page[,page...]]\n");
  printf(2, "       with no arguments, runs 3, 4, and 5 frames on the sample string\n");
}

static void
printframes(int *frames, int nframes)
{
  int i;

  printf(1, "[");
  for(i = 0; i < nframes; i++){
    if(i)
      printf(1, " ");
    if(frames[i] < 0)
      printf(1, "-");
    else
      printf(1, "%d", frames[i]);
  }
  printf(1, "]");
}

static void
runalgorithm(char *name, int lru, int nframes, int *refs, int nrefs,
             int *faults, int *hits)
{
  int frames[MAXFRAMES];
  int lastused[MAXFRAMES];
  int i, j, found, victim, nextfifo;

  for(i = 0; i < nframes; i++){
    frames[i] = -1;
    lastused[i] = -1;
  }
  nextfifo = 0;
  *faults = 0;
  *hits = 0;

  printf(1, "%s with %d frames:\n", name, nframes);
  for(i = 0; i < nrefs; i++){
    found = -1;
    for(j = 0; j < nframes; j++)
      if(frames[j] == refs[i]){
        found = j;
        break;
      }

    if(found >= 0){
      (*hits)++;
      lastused[found] = i;
      printf(1, "%d ", refs[i]);
      printframes(frames, nframes);
      printf(1, " Hit\n");
      continue;
    }

    (*faults)++;
    victim = -1;
    for(j = 0; j < nframes; j++)
      if(frames[j] < 0){
        victim = j;
        break;
      }
    if(victim < 0){
      if(lru){
        victim = 0;
        for(j = 1; j < nframes; j++)
          if(lastused[j] < lastused[victim])
            victim = j;
      } else {
        victim = nextfifo;
        nextfifo = (nextfifo + 1) % nframes;
      }
    }
    frames[victim] = refs[i];
    lastused[victim] = i;
    printf(1, "%d ", refs[i]);
    printframes(frames, nframes);
    printf(1, " Fault\n");
  }
  printf(1, "%s: faults=%d hits=%d hit-ratio=%d%%\n",
         name, *faults, *hits, (*hits * 100) / nrefs);
}

static void
runcomparison(int nframes, int *refs, int nrefs, int *fifo_faults,
              int *lru_faults, int *fifo_hits, int *lru_hits)
{
  runalgorithm("FIFO", 0, nframes, refs, nrefs, fifo_faults, fifo_hits);
  runalgorithm("LRU", 1, nframes, refs, nrefs, lru_faults, lru_hits);
}

int
main(int argc, char **argv)
{
  int refs[MAXREFS];
  int nrefs, nframes, i, j, value, fifo_faults, lru_faults;
  int fifo_hits, lru_hits;
  char *s;

  if(argc == 1){
    nrefs = sizeof(defaultrefs) / sizeof(defaultrefs[0]);
    for(i = 0; i < nrefs; i++)
      refs[i] = defaultrefs[i];

    printf(1, "Reference string: 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1\n");
    printf(1, "frames algorithm faults hit-ratio\n");
    for(nframes = 3; nframes <= 5; nframes++){
      runcomparison(nframes, refs, nrefs, &fifo_faults, &lru_faults,
                    &fifo_hits, &lru_hits);
      printf(1, "%d FIFO %d %d%%\n", nframes, fifo_faults,
             (fifo_hits * 100) / nrefs);
      printf(1, "%d LRU  %d %d%%\n", nframes, lru_faults,
             (lru_hits * 100) / nrefs);
    }
    exit();
  }

  if(argc < 3){
    usage();
    exit();
  }
  nframes = atoi(argv[1]);
  if(nframes < 1 || nframes > MAXFRAMES){
    usage();
    exit();
  }

  nrefs = 0;
  for(i = 2; i < argc; i++){
    s = argv[i];
    while(*s){
      while(*s == ' ' || *s == '\t' || *s == ',')
        s++;
      if(*s == 0)
        break;
      if(*s < '0' || *s > '9' || nrefs == MAXREFS){
        usage();
        exit();
      }
      value = 0;
      while(*s >= '0' && *s <= '9'){
        j = *s - '0';
        if(value > (0x7fffffff - j) / 10){
          usage();
          exit();
        }
        value = value * 10 + j;
        s++;
      }
      refs[nrefs++] = value;
      if(*s && *s != ' ' && *s != '\t' && *s != ','){
        usage();
        exit();
      }
    }
  }
  if(nrefs == 0){
    usage();
    exit();
  }

  runcomparison(nframes, refs, nrefs, &fifo_faults, &lru_faults,
                &fifo_hits, &lru_hits);
  printf(1, "summary: frames algorithm faults hit-ratio\n");
  printf(1, "%d FIFO %d %d%%\n", nframes, fifo_faults,
         (fifo_hits * 100) / nrefs);
  printf(1, "%d LRU  %d %d%%\n", nframes, lru_faults,
         (lru_hits * 100) / nrefs);
  exit();
}
