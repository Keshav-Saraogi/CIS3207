#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *reverse(char *line)
{
	char *final = (char *)malloc(sizeof(char *)); 
    if (final == NULL)
    {
        fprintf(stderr, "malloc failed \n");
        exit(1);
    }
    int last = strlen(line) - 1; // last refers to the last index of the last word
    int reverse_string_index = 0; //
    for (int i = strlen(line) - 1; i >= 0; i--)
    {
        if (line[i] == ' ')
        {
            for (int j = i + 1; j <= last; j++)
            {
                final[reverse_string_index] = line[j];
                reverse_string_index++;
            }
            final[reverse_string_index++] = ' ';
            last = i - 1;
        }
    }
    for (int i = 0; i <= last; i++)
    {
        final[reverse_string_index] = line[i];
        reverse_string_index++;
    }

	final[reverse_string_index] = '\0';
    return final;
}

int main(int argv, char **argc)
{
    if (argv > 3)
    {
        fprintf(stdout, "usage: reverse <input> <output>");
        exit(1);
    }
    else if (argv == 3)
    {
        if (strcmp(argc[1], argc[2]) == 0)
        {
            fprintf(stderr, "%s", "Input and output file must differ\n");
            exit(1);
        }
        FILE *input;
        input = fopen(argc[1], "r");
        FILE *output;
        output = fopen(argc[2], "w");
        if (input == NULL)
        {
            fprintf(stderr, "error: cannot open file \'%s\' \n", argc[1]);
            exit(1);
        }
        if (output == NULL)
        {
            fprintf(stderr, "error: cannot open file \'%s\' \n", argc[2]);
            exit(1);
        }
        char *line = NULL;
        size_t len = 0;
        ssize_t line_size = 0;
        while ((line_size = getline(&line, &len, input)) != -1)
        {
            line[line_size - 1] = '\0';
            char *reverse_str = reverse(line);
            fprintf(output, "%s\n", reverse_str);
        }
    }
    else if (argv == 2)
    {
        FILE *input;
        input = fopen(argc[1], "r");
        if (input == NULL)
        {
            fprintf(stderr, "error: cannot open file \'%s\' \n", argc[1]);
            exit(1);
        }
        char *line = NULL;
        size_t len = 0;
        ssize_t line_size = 0;
        while ((line_size = getline(&line, &len, input)) != -1)
        {
            line[line_size - 1] = '\0';
            fprintf(stdout, "%s\n", reverse(line));
        }
    }
    else if (argv == 1)
    {
        fprintf(stdout, "Enter a string:");
        char *str = (char *)malloc(sizeof(char *));
        if (str == NULL)
        {
            fprintf(stderr, "malloc failed \n");
            exit(1);
        }
        gets(str);
        fprintf(stdout, "%s\n", reverse(str));
    }
    return 0;
}