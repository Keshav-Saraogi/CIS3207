#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std; 

#define LENGTH  100

int main(int argc, char *argv[])
{
    char lineBuffer[LENGTH+1]; // Creating an array to hold the length of the provided statement. 
    FILE *fp = NULL;
    int count = 0;

    if(argc <= 1)
    {
        fprintf(stderr, "wgrep search-term [file ...]\n");
        exit(1);
    }
    fp = fopen(argv[2], "r" );
    if(!fp)
    {
        fprintf(stderr, "wgrep: cannot open file\n");
        exit(1);
    }
    while(fgets(lineBuffer, LENGTH, fp)) // Inputs upto 511 characters from the command line
    {
        if(strstr(lineBuffer, argv[1])) // Searches the substring provided as the second argument from the command line
        {
            printf("%s", lineBuffer); // Prints the entire line
            ++count;                  // As an addition, it prints the number of occurrences, making it easier to count.
            num++;
        }
    }
    fclose(fp);
    printf("found %d occurrences\n", count);
    return 0;
}