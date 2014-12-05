Homework 1: String Replacement

Through this programming assignment, the students will learn to do the following:

Get familiar with developing C programs in Unix environment.
Use Makefile to automate the build process.
Know how to read from a file.
Know how to use pointers and strings.
Use malloc() and free() to dynamically allocate and reclaim memory.
Your program must take three arguments: string_a, string_b, and a file name. It will replace ALL occurrences of string_a in the given file with string_b and then print out the file (with replaced text) to the standard output. More specifically, your program needs to do the following:

The program needs to provide sanity-check for command line arguments. For example, there should be exactly three arguments; string_a can't be empty; the file must exist and can be accessed without problem, etc.
The program shall read and process one line of the file at a time (using standard libc function fgets(), or the function getline() provided in the book). You can assume that each line in the given file is no longer than 1024 characters (including the terminating null character). However, you CANNOT assume that the new line (after the replacement) will not be longer than 1024 characters. You need to calculate the maximum possible length of the new line and dynamically allocate its memory. Hint: the new line won't be shorter than the original line, but it also won't be longer than the length of the original line multiplied by the ratio of strlen(string_b) over strlen(string_a). After the string replacement, you can print out the new line and reclaim its memory.
You can use the standard libc function strstr(), which can locate the first occurrence of a string in another string. Note that there can be more than one instances of string_a in a line. You need to replace them all. That is, you may need to call strstr() multiple times during the processing of each line.
Here is an example show what your program should be able to do (which also shows a perfect way to destroy the beauty of a Shakespeare's poem):

% cat test.txt
Shall I compare thee to a summer's day?
Thou art more lovely and more temperate.
Rough winds do shake the darling buds of May,
And summer's lease hath all too short a date.
% ./replace d DDD mytest.txt
Shall I compare thee to a summer's DDDay?
Thou art more lovely anDDD more temperate.
Rough winDDDs DDDo shake the DDDarling buDDDs of May,
AnDDD summer's lease hath all too short a DDDate.

Please follow the directions below carefully, when you work on the assignment:

You should create a separate directory where you put all your files. Assuming your name is John Smith, you shall name your directory john-smith-hw1. Of course, you need to replace that with your true name. 
IMPORTANT: You must include in the directory a file named DISCLAIMER, which certifies that you have been worked on the homework by yourself and only by yourself. A template of this disclaimer can be downloaded from moodle. You must fix your name and date it. WITHOUT PROPER DISCLAIMER, YOUR SUBMISSION WILL BE TREATED AS INVALID AND YOU WON'T BE ABLE TO RECEIVE ANY GRADE FOR THIS ASSIGNMENT.
You must have a Makefile with at least two targets. The default 'make' will build the executable file and 'make clean' will remove all generated files (including the object files, the executable, and all other temporary files); your submission should be a directory that contains only the source code, the Makefile, and the DISCLAIMER file. You need to archive and compress the directory by running the following command in the parent directory:
 % tar czvf firstname-lastname-hw1.tar.gz firstname-lastname-hw1
This will create a file named firstname-lastname-hw1.tar.gz, which you should upload to moodle.
You must use dynamic memory to store the content of the new line. That is, you need to use malloc() to allocate the memory, create the new line after replacing all the instances of the given strings, print out the new line, and then reclaim it afterwards before moving on to the next line. This is a bit cumbersome but the purpose here is for you to learn to use dynamic memory.
We will use the following criteria for grading. The TA will run your program using different test input, and will inspect your source code to see if your program properly implements the expected functions.

If the submission is not a .tar.gz file or one cannot use 'tar xzvf firstname-lastname-hw1.tar.gz' to recreate the directory, the grade is zero.
If the DISCLAIMER file is missing, or the student did not put his/her name and date in the file, there will be no grade.
Your source code must be able to be compiled to produce the final executable; if we can't compile your code, there will be no grade.
Makefile: 10%. We'll check Makefile whether it can build the executable using 'make' and can clean up all unnecessary files using 'make clean'.
Checking command-line: 10%. We'll check whether the program provide all necessary checks for the expected command line arguments.
Reading file: 10%. We'll check whether the program correctly opens, reads the file line-by-line, and eventually closes the file.
Allocate and deallocate memories for the new lines: 20%. We'll check whether you allocate memory for the new lines with appropriate size, and whether you free the memory properly after use.
Find all instances of the string to be replaced for each line: 20%. 
Generate the new line with the correct replacement: 20%.
Other miscellaneous functions not listed above: 10%.
The deadline for this assignment is at 23:55 on September 24th. Late submission will incur penalties as described in the syllabus.
