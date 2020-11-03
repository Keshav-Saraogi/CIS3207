/* myshell_cmds.c
 *
 * This file contains the internal commands of myshell.
 */
 
#include "myshell.h"

#define BUFF 512

/* changes directory to argv[1]
   returns 1 on success, 0 on failure */
int my_cd(char **argv) {
    int len; /* for allocation of space for prompt*/
    char *slash = "/";

    if (argv[1] != NULL) {
        if(chdir(argv[1]) == 0) 
        {
            free(myshell_env.PWD);
            free(prompt);
            myshell_env.PWD = getcwd(NULL, 0);
            len = strlen(myshell_env.PWD);
            len += strlen(prompt_tail);
            prompt = malloc(sizeof(char) * (len+2));
            strcpy(prompt, myshell_env.PWD);
            strcat(prompt, slash);
            strcat(prompt, prompt_tail);
        }
        else 
        {
            perror(argv[1]);
        }
    }
    else 
    {
        puts(myshell_env.PWD);
    }
    return 1;
}

/* clears screen */
int my_clr() 
{
    system("clear");
    return 1;
}

/* lists files and directores.
   Contents of current directory are listed if there is no argument,
   otherwise lists contents of directory specified in argv[1].
   Returns 1 so shell can continue to execute */
int my_dir(char **argv) 
{
    DIR *dp;
    struct dirent *dir;

    /* variables to use if redir_out == 1 */
    int newstdout;

    if(redir_out) 
    {
        if (append)
            newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
        else
            newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);

        if (newstdout == -1)
            perror("my_help/preparing for redirection of output");
        else
            dup2(newstdout,1);

        /* if no directory is listed, open current directory, else open directory provided */
        if(argv[1][0] == '>') 
        {
            if( (dp = opendir(myshell_env.PWD)) == NULL) 
            {
                perror("dir");
                return 1;
            }
        }
        else 
        {
            if( (dp = opendir(argv[1])) == NULL) 
            {
                perror(argv[1]);
                return 1;
            }
        }
    } /* end if flag_io */

    else /* no flags */{
        if(argv[1] == NULL) 
        {
            if( (dp = opendir(myshell_env.PWD)) == NULL) 
            {
                perror("dir");
                return 1;
            }
        }
        else 
        {
            if( (dp = opendir(argv[1])) == NULL) 
            {
                perror(argv[1]);
                return 1;
            }
        }
    }

    while(dir = readdir(dp))
        if(dir->d_name[0] != '.')
            puts(dir->d_name);

    closedir(dp);
    return 1;
}

/* prints environment variables to stdout */
int my_environ(char **argv) 
{
    pid_t childPID;
    int status;

    /* variables to use if redir_out == 1 */
    int newstdout;

    /* if output is to be redirected, change file descriptor for stdout */
    if(redir_out) 
    {
        if (append)
            newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
        else
            newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);

        if (newstdout == -1)
            perror("my_help/preparing for redirection of output");
        else 
        {
            dup2(newstdout,1);
        }
    }

    /* use fork/wait to ensure system("echo $PATH") executes immediately 
       after printf("PATH=") */
    if (( childPID = fork() ) == 0) 
    {
        printf("PATH=");
        exit(1);
    }
    else if(childPID > 0) 
    {
        wait(&status);
        system("echo $PATH");
        printf("PWD=%s\n", myshell_env.PWD);
        printf("DIR=%s\n", myshell_env.DIR);
        printf("SHELL=%s\n", myshell_env.SHELL);
        printf("NUM_BUILTIN_CMD_NO_REDIR=%d\n", myshell_env.NUM_BUILTIN_CMD_NO_REDIR);
        printf("NUM_BUILTIN_CMD=%d\n", myshell_env.NUM_BUILTIN_CMD);
    }
    else
        perror("my_environ");

    return 1;
}

/* prints argv[i], i = {1,...,N}, N >= 1 through stdout
   
   If argv[1] is NULL, */
int my_echo(char **argv) 
{
    char **p1 = argv+1;
    
    /* variables to use if redir_out == 1 */
    FILE *fp;
    int newstdout;

    /* if output is redirected, echo command will print to stdout after we change 
       the file descriptor */
    if(redir_out) 
    {
        if (append)
            newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
        else
            newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);

        if (newstdout == -1)
            perror("my_help/preparing for redirection of output");
        else 
        {
            dup2(newstdout,1);
        }

        while(strcmp(*p1, ">") != 0) 
        {
            printf("%s ", *p1);
            p1++;
        }
        printf("\n");
    }

    else 
    {
        while(*p1 != NULL) 
        {
            printf("%s ", *p1);
            p1++;
        }
        printf("\n");
    }
    return 1;
}

/* must be reconfigured if readme is not in same directory as myshell executable */
int my_help(char **argv) 
{
    char buffer[BUFF];
    char *more = "more -d ";
    char *echo = "echo ";
    char *slash = "/";
    char *n = "readme";

    /* variables to use if redir_out == 1 */
    FILE *fp;
    int c;
    int newstdout;

    /* if output is redirected, readme file will print to stdout
       after we change the file descriptor */
    if(redir_out) 
    {
        strcpy(buffer, myshell_env.DIR);
        strcat(buffer, slash);
        strcat(buffer, n);

        if (append)
            newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
        else
            newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);

        if (newstdout == -1)
        {
        	perror("my_help/preparing for redirection of output");
        }
        else 
        {
            dup2(newstdout,1);
        }

        if (( fp = fopen(buffer, "r") ) == NULL) 
        {
            perror("opening file, my_help");
        }
        while(( c = fgetc(fp) ) != EOF) 
        {
            fputc(c, stdout);
        }
    }

    /* else output is not redirected and so by default will display readme
       file through more filter. The command to do this is concatenated into
       buffer, the executed system() */
    else  
    { 
        strcpy(buffer, more);
        strcat(buffer, myshell_env.DIR);
        strcat(buffer, slash);
        strcat(buffer, n);
        system(buffer);
    }
    return 1;
}

/* pauses operation of shell until newline character is detected */
int my_pause() 
{
    printf("press 'Enter' to continue");
    while(getchar() != '\n');
    return 1;
}

/* flags the shell to terminate */
int my_quit() 
{
    return 0;
}