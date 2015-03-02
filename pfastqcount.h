#include <stdlib.h> 
#include <stdio.h> 
#include <fcntl.h> 
#include <sys/mman.h> 
#include <sys/stat.h> 
#include <time.h> 
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>



#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define SEQ_PREFIX_LENGTH 4
#define NUCLEOTIDE_BITS 4
// pow(NUCLEOTIDE_BITS,SEQ_PREFIX_LENGTH)
// #define SEQ_PERMUTATION_COUNT 256
#define SEQ_PERMUTATION_COUNT 65536
#define SEQ_USED_BITS 7
#define BITS_PER_QUAD SEQ_PREFIX_LENGTH * NUCLEOTIDE_BITS

#define SEQ_A 0
#define SEQ_C 1
#define SEQ_G 2
#define SEQ_T 3
#define SEQ_N 4

short encode_quad(char * map, unsigned long offset);
bool decode_quad(short sequence, char * ascii);
void print_csv(unsigned int counts[]);

