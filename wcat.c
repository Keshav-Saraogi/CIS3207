#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
int main(int argc, char *argv[]) 
{
    FILE *file;
    int chr;
    int count;
    if(argc <= 1)
    {
    	exit(1);
    }
    else
    {
    	for(count = 1; count < argc; count++) 
    	{
        	if((file = fopen(argv[count], "r")) == NULL) 
        	{
            	printf("wcat: cannot open file\n");
            	exit(1);
        	}
        	while((chr = getc(file)) != EOF)
        	{
            	fprintf(stdout, "%c", chr);
        	}
    	fclose(file);
    	}
    exit(0);
    }
}