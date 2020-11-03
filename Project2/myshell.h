#ifndef MYSHELL_H_
#define MYSHELL_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <sys/stat.h>           /* dup2 function */
#include <sys/types.h>
#include <sys/wait.h>

/* internal shell commands */
int my_cd(char **);
int my_clr();
int my_dir(char **);
int my_environ(char **);
int my_echo(char **);
int my_help(char **);
int my_pause();
int my_quit();

/* shell functions */
void var_init(char *);
void shell_loop();
char **parse_string(char *);
void evaluate_flags(char **);
int shell_execute(char **);
int invoke(char **);
int invoke_with_pipe(char **);
void free_arg(char **);

/* global variables */
extern FILE *batchfp;
extern int num_of_arg;
extern char *prompt;
extern char prompt_tail[];
extern const char *builtin_cmd_no_output[];
extern const char *builtin_cmd[];
extern int (*cmd_no_output[])(char **);
extern int (*cmd[])(char **);

struct environment 
{   /* environment strings */
    char *PWD;
    char *DIR;
    char *SHELL;
    int NUM_BUILTIN_CMD_NO_REDIR;
    int NUM_BUILTIN_CMD;
};
extern struct environment myshell_env;

/* global flags */
extern int batch;
extern int flag_io;
extern int redir_in;
extern int redir_out;
extern int redir_in_index;
extern int redir_out_index;
extern int append;
extern int pipe_enabled;
extern int pipe_child_index;
extern int bg_exec;

#endif