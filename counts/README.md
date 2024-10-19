<h1>counts</h1>
This C program works with basic I/O operations to emulate the standard Unix/Linux wc command. The C file is wc.c. The wc program counts how many lines, words, and characters are in a file, or series of files.

There are three optional command line parameters. If none of the three are given, then wc will print out all three counts (lines, words, and characters). If any of the optional parameters are given, then only those counts will be printed: -l is set to print the number of lines, -w is set to print the number of words, and -c is set to print the number of characters. Note that it's not an error for the same parameter to be given multiple times, and they can be given in any order.

After the optional parameters is a list of files. wc will print out the indicated counts for each file. If there are multiple files, then wc will additionally print out the total of each count across all given files. If no files are given, then wc will read from stdin instead of a file.
