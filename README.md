# pfastqcount #

Preston's FASTQ Primer Counter

# About #

pfastqcount is an optimized C implementation of a FASTQ sequence prefix counter. The program counts the number of times every four-nucleotide prefix is encountered via the sequential memory mapping of each ASCII-encoded FASTQ file into the process space. This is useful as part of a genomic data sequencing pipeline to sanity check that barcodes used in multiplexed sample experiments occur as expected. An alternative Ruby implementation is provided (pfastqcount.rb) for maximum portability and validation. The C implementation has been written for 64-bit Unix-like systems only. The output of the program is valid comma-separated value (CSV) data that may be opened in your favorite spreadsheet software such as Excel, OpenOffice, R etc.

**Please cite and attribute this work properly when using it in your own.**

Input files are assumed to be ASCII-encoded and structured according to [the FASTQ file format specification](http://maq.sourceforge.net/fastq.shtml). The output may or may not be erroneous if the file is otherwise formatted.

# Installation #

You will need a C compiler (such as GCC) installed to compile the program. Most Linux systems ship with GCC. Mac OS X users should install Xcode before compiling. It is not officially supported on Windows, though patches will be accepted.

	./build.sh

This will produce the pfastqcount binary for your system. You may move it elsewhere, if you like.

# Usage #

To get syntax help:

	./pfastqcount
	
To run the program one one or more data files and print the results to the screen:

	./pfastqcount 42 some_data.fastq more_data.fastq

To run the program one one or more data files and create a .csv file:

	./pfastqcount 42 data1.fastq data2.fastq > results.csv

You may specify as many .fastq files as you'd like. The results will be merged and summed before output is generated.

# Author #

[Preston Lee](http://prestonlee.com)

# License & Copyright #

See LICENSE file for details.
