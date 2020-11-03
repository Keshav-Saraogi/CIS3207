#include "myshell.h"

#define INPUT_BUFF_LEN 1024
#define ARGV_LEN        100

/* global variables/flags */
int batch;              /* expect input from a text text file       */
int flag_io;            /* expect an I/O redirection                */
int redir_in;           /* redirect input                           */
int redir_out;          /* redirect output                          */
int redir_in_index;     /* index of argv for new input direction    */
int redir_out_index;    /* index of argv for direction of output    */
int append;             /* redirect output without truncation file  */
int pipe_enabled = 0;   /* indicates use of pipe                    */
int pipe_child_index;   /* index of argv of child-end of pipe       */
int bg_exec;            /* execute next line in the "background"    */

FILE *batchfp;                      /* pointer to text file if batch == 1   */
int num_of_arg;                     /* number of args in argvincluding NULL */
char *prompt;                       /* user prompt message                  */
char prompt_tail[] = "myshell> ";   /* tail end of user prompt message      */
struct environment myshell_env;     /* contains environment variables       */

const char *builtin_cmd_no_redir[] = { /* internal commands without I/O redirection */
    "cd",
    "clr",
    "pause",
    "quit"
};

const char *builtin_cmd[] = {   /* internal commands with redirection capabilities */
    "help",
    "dir",
    "environ",
    "echo"
};

int (*cmd_no_redir[])(char **) = {
    &my_cd,
    &my_clr,
    &my_pause,
    &my_quit
};

int (*cmd[])(char **) = {
    &my_help,
    &my_dir,
    &my_environ,
    &my_echo
};
/* end of global variables/flags */


int main(int argc, char *argv[]) {
    batch = (argc > 1) ? 1 : 0;   /* if batch == 1, get input from text file */

    var_init(argv[1]);

    shell_loop();

    free(myshell_env.PWD);
    free(myshell_env.SHELL);
    free(prompt);
    
    return EXIT_SUCCESS;
}

/* initialize environment variables and prompt msg */
void var_init(char *batchfile) {
    int len=0;
    char *shell_name = "myshell";
    char *slash = "/";
    if(batch) {
        batchfp = fopen(batchfile, "r");
    }

    myshell_env.NUM_BUILTIN_CMD_NO_REDIR = sizeof(builtin_cmd_no_redir) / sizeof(char *);
    myshell_env.NUM_BUILTIN_CMD = sizeof(builtin_cmd) / sizeof(char *);
    myshell_env.PWD = getcwd(NULL, 0);
    
    /* prepare SHELL and DIR environment variable */
    len = strlen(myshell_env.PWD);

    myshell_env.DIR = malloc(sizeof(char) * (len+1));
    strcpy(myshell_env.DIR, myshell_env.PWD);

    len += strlen(shell_name);

    myshell_env.SHELL = malloc(sizeof(char) * (len+2)); /* +2 is for slash and '\0' */
    strcpy(myshell_env.SHELL, myshell_env.PWD);
    strcat(myshell_env.SHELL, slash);
    strcat(myshell_env.SHELL, shell_name);

    /* prepare prompt variable */
    len=0;
    len = strlen(myshell_env.PWD);
    len += strlen(prompt_tail);

    prompt = malloc(sizeof(char) * (len+2));
    strcpy(prompt, myshell_env.PWD);
    strcat(prompt, slash);
    strcat(prompt, prompt_tail);
}

void shell_loop() {
    char user_in[INPUT_BUFF_LEN];
    char **argv;
    int loop = 1;

    /* variables if bg_exec == 1 */
    pid_t cpid;
    int status = 0;

    do {
        flag_io = 0;
        redir_in = 0;
        redir_out = 0;
        redir_in_index = 0;
        redir_out_index = 0;
        append = 0;
        pipe_child_index = 0;
        bg_exec = 0;

        /* variales if batch == 1 */
        
        if(batch) {
            fgets(user_in, INPUT_BUFF_LEN, batchfp);
            if(feof(batchfp))
                break;
        } else /* read from command line interpreter */ {
            fputs(prompt, stdout);
            fgets(user_in, INPUT_BUFF_LEN, stdin);

            if(feof(stdin)) {
                printf("\n");
                break;
            }
        }

        argv = parse_string(user_in);
        evaluate_flags(argv);

        /* finish shell in child process if bg_exec == 1 */
        if(bg_exec) {
            cpid = fork();
            if (cpid == -1) {
                perror("bg_exec fork");
                free_arg(argv);
            }
            if (cpid == 0) /* child */ {
                loop = shell_execute(argv);
                free_arg(argv);
                exit(1);
            }
        }

        else {
            loop = shell_execute(argv);
            free_arg(argv);
        }
    } while(loop);

}

/* Returns an array of strings from s split into substrings with whitespace
   as the delimiter */
char **parse_string(char *s) {
    char **arg, **argp;
    char *sp, *first_char;
    char *pre;
    int i;
    num_of_arg = 1;      /* account for NULL "argument" */

    sp = s;
    
    /* skip initial whitespace */
    while(*sp == ' ' || *sp == '\t')
        sp++;

    /* count number of arguments */
    if (*sp != '\n') {
        num_of_arg++;
        first_char = sp; /* first character location */
        pre = sp;
        sp++;
        while (*sp != '\n' && *sp != '\0') {
            if ((*sp != ' ' && *sp != '\t') && 
                 (*pre == ' ' || *pre == '\t')) 
                num_of_arg++;
            pre++;
            sp++;
        }
    }

    arg = (char **) malloc(num_of_arg * sizeof(char *));
    sp = first_char;
    argp = arg;

    for (i=0; i<num_of_arg-1; i++, argp++) {
        *argp = (char *) malloc(ARGV_LEN * sizeof(char));
        pre = *argp;
        while (*sp != ' ' && *sp != '\t' && *sp != '\n') {
            *pre = *sp;
            pre++;
            sp++;
        }
        *pre = '\0';

        while (*sp == ' ' || *sp == '\t')
            sp++;
    }

    arg[num_of_arg-1] = NULL;

    return arg;
}


/* sets appropriate flags so the shell can take notice */
void evaluate_flags(char **argv) {
    int index = 0;

    if(num_of_arg > 1 && strcmp(argv[num_of_arg-2],"&") == 0) {
        free(argv[num_of_arg-1]);
        free(argv[num_of_arg-2]);
        argv[num_of_arg-2] = malloc(sizeof(NULL) * 1);
        argv[num_of_arg-2] = NULL;
        bg_exec = 1;
        num_of_arg--;
    }
    while(argv[index] != NULL) {
        if(redir_out == 0 && (strcmp(argv[index],">") == 0 || strcmp(argv[index],">>") == 0)) {
            flag_io = 1;
            redir_out = 1;
            append = (strcmp(argv[index],">") != 0) ? 1 : 0;
            argv[index][1] = '\0';
            redir_out_index = index + 1;
        }
        else if(redir_in == 0 && strcmp(argv[index],"<") == 0) {
            flag_io = 1;
            redir_in = 1;
            redir_in_index = index + 1;
        }
        else if(pipe_enabled == 0 && strcmp(argv[index],"|") == 0) {
            free(argv[index]);
            argv[index] = malloc(sizeof(NULL) * 1);
            argv[index] = NULL;
            pipe_enabled = 1;
            pipe_child_index = index + 1;
        }
        index++;
    }
}


/* determines what to do with input, then continues with appropriate action */
int shell_execute(char **argv) {
    int i;
    pid_t childPID;
    int status = 0;
    
    if(pipe_enabled) {
        pipe_enabled = 0;
        return(invoke_with_pipe(argv));
    }
    if(argv[0] == NULL) /* if no command entered */
        return 1;

    /* if argv[0] is a builtin command that NEVER requires output redirection */
    for(i=0; i<myshell_env.NUM_BUILTIN_CMD_NO_REDIR; i++) {
        if(strcmp(argv[0],builtin_cmd_no_redir[i]) == 0)
            return (*cmd_no_redir[i])(argv);
    }

    /* if argv[0] is a builtin command that might require output redirection */
    for(i=0; i<myshell_env.NUM_BUILTIN_CMD; i++) {
        if(strcmp(argv[0],builtin_cmd[i]) == 0) {
            if(flag_io) { /* must run cmd in child process if I/O redirected */
                if(( childPID = fork() ) == -1) {
                    perror("redirect output builtin commands");
                }
                if(childPID == 0) {
                    (*cmd[i])(argv);
                    exit(1);
                }
                else
                    wait(&status);

                return 1;
            }
            return (*cmd[i])(argv);
        }
    }

    return invoke(argv); /* otherwise attempt to invoke an executible */
}


/* finds and executes argv[0] by using execvp command */
int invoke(char **argv) {
    pid_t childPID;
    int status;

    /* variables to use if flag_io == 1 */
    int newstdout;
    int newstdin;

    if(( childPID = fork() ) == -1) {
        perror("invoke");
    }

    if(childPID == 0) /* in child process */ {
        if(flag_io) {
            if(redir_out) {
                if (append)
                    newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_APPEND, S_IRWXU|S_IRWXG|S_IRWXO);
                else
                    newstdout = open(argv[redir_out_index], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG|S_IRWXO);
                
                free(argv[redir_out_index-1]);
                argv[redir_out_index-1] = malloc(sizeof(NULL)*1);
                argv[redir_out_index-1] = NULL;
                
                if (newstdout == -1)
                    perror("redirection of output");
                else 
                    dup2(newstdout,1);
            }
            if(redir_in) {
                free(argv[redir_in_index-1]);
                argv[redir_in_index-1] = malloc(sizeof(NULL)*1);
                argv[redir_in_index-1] = NULL;

                if (( newstdin = open(argv[redir_in_index], O_RDONLY) ) == -1) {
                    perror(argv[redir_in_index]);
                    return 1;
                } 
                else
                    dup2(newstdin,0);
            }
        } /* end if flag_io */
            
        if(( execvp(argv[0],argv) ) == -1) {
            perror(argv[0]);
            exit(EXIT_FAILURE);
        }
    } /* end child */

    else /* in parent */
        waitpid(childPID, &status, WUNTRACED);
    return 1;
}


/* this is an implimentation of piping output of a command as input of
   another command.
       output: arguments argv[0] through argv[pipe_child_index - 1]
       input:  arguments argv[pipe_child_index] through argv[NULL] */
int invoke_with_pipe(char **argv) {
    pid_t childpid1;
    pid_t childpid2;
    int pipefd[2];
    int status_pipe1;
    int status_pipe2;
    pipe(pipefd);

    childpid1 = fork();

    if(childpid1 == -1) {
        perror("fork1");
        return 1;
    }

    if(childpid1 == 0) {
        dup2(pipefd[1],1);
        close(pipefd[0]);
        shell_execute(argv);
        exit(1);
    }

    childpid2 = fork();

    if(childpid2 == -1) {
        perror("fork2");
        return 1;
    }

    if(childpid2 == 0) {
        dup2(pipefd[0], 0);
        close(pipefd[1]);
        shell_execute(argv+pipe_child_index);
        exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(childpid1, &status_pipe1, WUNTRACED);
    waitpid(childpid2, &status_pipe2, WUNTRACED);

    return 1;
}

/* frees allocated space from array returned from parse_string */
void free_arg(char **arg) {
    int i=0;
    while(i<num_of_arg){
        free(arg[i]);
        i++;
    }
    free(arg);
}