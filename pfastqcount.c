#include "pfastqcount.h"


int main(int argc, char ** argv) {
	int ret = EXIT_FAILURE;
	// The user must specify a minimum read lengeth as well as list of files to process.
	if(argc < 3) {
		printf("\nPreston's optimized FASTQ barcode Counter.\n");
		printf("Counts occurances of individual barcodes in the provided FASTQ files.\n");
		printf("This software is written in raw C for 64-bit systems.\n");
		printf("Copyright 2015 Preston Lee. All rights reserverd.\n");
		printf("The output of the program is valid comma-separated value (CSV) data.\n");
		printf("Simply redirect the output to a new file to open in your favorite\n");
		printf("spreadsheet software such as Excel, OpenOffice, R etc.)\n\n");
		printf("\tUsage:\n\n");
		printf("\t\t%s <min_read_length> <fastq_data>+\n\n", argv[0]);
		printf("\t..or write directly to a new file:\n\n");
		printf("\t\t%s <min_read_length> <fastq_data> ... > results.csv\n\n", argv[0]);
		printf("You should *not* include the leading barcode length in min_read_length.\n");
		printf("If you don't know (or it varies), use 1.\n\n");
		printf("\tExample:\n\n");
		printf("\t\t%s 36 1.fastq 2.fastq > output.csv\n\n", argv[0]);
		// printf("")
	} else {
		
		// Start by allocating structures for prefix counts.
		// The number of possible prefix permutations (of A, C, G and T) is 4 ** 4 == 256
		// Since we can shove a given prefix into a char it's not a ton of space.
		unsigned int counts[SEQ_PERMUTATION_COUNT];
		int i;
		for(i = 0; i < SEQ_PERMUTATION_COUNT; ++i) {
			counts[i] = 0;
		}
		int count = argc - 2;
		
		unsigned int skip = atoi(argv[1]); // We can save a lot of time by jumping over sections of the file we don't care about.
		if(skip < 1) {
			skip = 1;
		}
		// printf("Using a skip size of %d.\n", skip);
		
		// For each file...
		// printf("Processing %i files:\n", count);
		for(i = 1; i <= count; ++i) {
			// Files can take a while, so print the current one as a status message.
			char * file_name = argv[i + 1];
			// printf("\tFile: %s.", file_name);

			// Open the file normally.
			int fd = open(file_name, O_RDONLY);
			// exit(EXIT_FAILURE);
			off_t size_tmp = lseek(fd, 0, SEEK_END);
			unsigned long length = (unsigned long)size_tmp;
			// printf(" (%lu bytes)\n", length);
			
			// Make sure the file is ok.
			struct stat sb;
			if (fd == -1)
	        handle_error("open");
	    if (fstat(fd, &sb) == -1)           /* To obtain file size */
	        handle_error("fstat");
	
	
			// Memory map the file so the operating system manages paging.
				// printf("FD: %i", fd);
#ifdef __APPLE__
			// char * map = mmap(NULL, length, PROT_READ, MAP_PRIVATE | MAP_NOCACHE, fd, 0);
			char * map = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
#else
			char * map = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
#endif
			if (map == MAP_FAILED) {
					close(fd);
					perror("Error mmapping the file");
					exit(EXIT_FAILURE);
		  }
		
			// Read the sucker!
			unsigned long offset = 0;
			unsigned long cur = 0;
			const char newline = '\n';
			// We only care about the 2nd line of each 4-line block, so some bookkeeping is in order.
			// printf("NL: %i\n", (int)newline);
			char line_block_offset = 0;
			short line_offset = 0;
			short prefix;
			for(offset = 0; offset < length; ++offset) {
				cur = map[offset];
				if(cur == newline) {
					line_block_offset = (line_block_offset + 1) % 4;
					line_offset = -1;
				} else {
					line_offset++;
				}
				if(line_block_offset == 1 && line_offset == 0) { 
					// This is the block line and line position that we care about, so grab the first four characters and compress them into a single char.
					prefix = encode_quad(map, offset);
					// int tmp = 1 + counts[prefix];
					// printf("\t\tCounted %u + %u\n", prefix, tmp);
					counts[prefix]++;
					// We can skip the rest of the line now!
					// printf("SKIPPING %d bytes.\n", skip + SEQ_PREFIX_LENGTH - 1);
					offset += skip + SEQ_PREFIX_LENGTH - 1;
				} else if(line_block_offset == 3 && line_offset == 0) {
					// We can skip the entire line. (Or at least as much as the minimum read size.)
					offset += skip + SEQ_PREFIX_LENGTH - 1;
				} else {
					// We are either on a line in the current block that we don't care about or a position on the correct line that doesn't matter, so do nothing!
				}
			}
			close(fd);
			munmap(map, length);
		}
		ret = EXIT_SUCCESS;
		print_csv(counts);
		// printf("Done!\n");
	}
	return ret;
}


/* Reads the 32-bit encoded char of four seqence nucleotides and returns an ASCII-encoding string, effectively decompressing the 16-bit block into 32 bits of human-readable text. */
bool decode_quad(short sequence, char * ascii) {
	bool valid = true;
	char cur;
	short i;
	for(i = 0; i < SEQ_PREFIX_LENGTH; ++i)
	{
		int shift = ((SEQ_PREFIX_LENGTH * NUCLEOTIDE_BITS) - ((i+1) * NUCLEOTIDE_BITS));
		cur = (sequence >> shift) & SEQ_USED_BITS;
		switch(cur) {
			case SEQ_A:
			cur = 'A';
			break;
			case SEQ_C:
			cur = 'C';
			break;
			case SEQ_G:
			cur = 'G';
			break;
			case SEQ_T:
			cur = 'T';
			break;
			case SEQ_N:
			cur = 'N';
			break;
			default:
			valid = false;
			break;
		}
		ascii[i] = cur;
	}
	// if(valid){
	// 	printf(" DECODED TO %s\n", ascii);		
	// }
	return valid;
}

/* Reads four characters of ASCII-encoding text and returns the sequence as a binary-encoding char, effectively compressing the 32-bit block into 16 bits. */
short encode_quad(char * map, unsigned long offset) {
	char cur = 0;
	short prefix = 0;
	short shifted = 0;
	size_t i;
	char quad_bits = SEQ_PREFIX_LENGTH * NUCLEOTIDE_BITS;
	for(i = 0; i < SEQ_PREFIX_LENGTH; ++i)
	{
		cur = map[offset + i];
		switch(cur) {
			case 'A':
			cur = SEQ_A;
			break;
			case 'C':
			cur = SEQ_C;
			break;
			case 'G':
			cur = SEQ_G;
			break;
			case 'T':
			cur = SEQ_T;
			break;
			case 'N':
			cur = SEQ_N;
			break;
			default:
			printf("Unknown character: %c\n", cur);
			perror("Bailing out!!!");
			exit(1);
			break;
		}
		shifted = cur << (quad_bits - NUCLEOTIDE_BITS - (i * NUCLEOTIDE_BITS));
		// printf("%u ", (short)shifted);
		prefix |= shifted;
		// printf("%u ", (short)cur);
	}
	// printf("Prefix: %u\n", (int)prefix);
	return prefix;
}


void print_csv(unsigned int counts[]) {
	char text[SEQ_PREFIX_LENGTH + 1];
	// char text[SEQ_PREFIX_LENGTH + 1];
	int i;
	printf("Prefix, Count\n");
	for(i = 0; i < SEQ_PERMUTATION_COUNT; ++i)
	{
		decode_quad(i, text);
		if(counts[i] > 0) {
			printf("%s, %u\n", text, counts[i]);			
		}
	}
}
