#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "linkedlist.h"
#include <signal.h>
#include "linkedListForHistory.h"
#include <pthread.h>


char* builtin_str[]={
	"cd",
    "help",
	"exit",
    "bg",
    "bglist",
    "kill",
    "history"
};


int sh_help(char **args)
{
  int i;
  printf("Rahul's SH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < 7; i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

//implementing cd
int sh_cd(char** args)
{
	if(args[1]==NULL)
	{
		fprintf(stderr,"sh: expected argument to \"cd\"\n");
	}
	else
	{
		if(chdir(args[1])!=0)
		{
			perror("Invalid Call");
		}
	}
	return 1;
}

//implementing exit
int sh_exit(char **args)
{
	return 0;
}

//backgroung jobs
process* headProcess=NULL;
int sh_bg(char **args)
{
//args -- bg echo "hello"
    ++args;
//args -- echo "hello"
    char *firstCmd= args[0];//echo
    int childpid=fork();
    if (childpid>=0)
    {
        if (childpid==0)
        {
            if (execvp(firstCmd,args)<0)
            {
                perror("Error on execvp\n");
                exit(0);
            }

        }
        else{
                if (headProcess==NULL)
                {
                    headProcess=create_list(childpid,firstCmd);
                }
                else{
                    add_to_list(childpid,firstCmd,true);
                }
        }
    }
    else{
        perror("fork() error");
    }
    return 1;
}

//showing all the jobs in background
int sh_bglist(char **args)
{
    print_list();
    return 1;
}

//killing process with specified pid
int sh_kill(char **args)
{
    // kill 1575
    char* pidCmd=args[1];
    if(!pidCmd)//kill 
    {
        printf("Please specify a pid\n");
    }
    else
    {
        int pid=atoi(pidCmd);
        process* target= search_in_list(pid, &headProcess);
        if (target!=NULL)
        {
            if (kill(target->pid,SIGTERM)>=0)
            {
                delete_from_list(pid);
            }
            else{
                perror("Could not kill pid specified\n");
                 }
        }
        else{
            printf("Specify a pid which is present in the list.\nType \"bglist\" to see active processes\n");
        }
    }
    return 1;
}

historyList *head_of_historylist = NULL;
int sh_history(char **args)
{
	print_list_of_history(head_of_historylist);
	return 1;
}

//bg echo "hello"
//bglist ->jobs
//kill 
int (*builtin_func[]) (char**)={
	&sh_cd,
    &sh_help,
	&sh_exit,
    &sh_bg,
    &sh_bglist,
    &sh_kill,
    &sh_history
};

//read whole line as a single string
char *sh_read_line()
{
    //variable named line will store the string
	char*line=NULL;
	ssize_t bufsize =0;
	if(getline(&line,&bufsize,stdin)==-1)//getline reads the line
	{
        //program comes here if it find any error while reading the line
		if(feof(stdin))
        	exit(EXIT_SUCCESS);
		else
		{
			perror("your-sh: getline\n");
			exit(EXIT_FAILURE);
		}
	}
	return line;
} 

#define SH_TOKEN_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n"

//this function will split the strin stored in line into different 
//strings as any one of the delimiters ooccurs
char** sh_split_line(char* line)
{
	int bufsize=SH_TOKEN_BUFSIZE,position=0;

	char ** tokens=malloc(bufsize * sizeof(char*));//array of strings
	char *token;

	if(!token)
	{
		fprintf(stderr,"your-sh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	//strtok :- Library Function 
	token=strtok(line,SH_TOK_DELIM);
	while(token!=NULL)
	{
		tokens[position]=token;
		token=strtok(NULL,SH_TOK_DELIM);

		position++;
	}
    	tokens[position]=NULL;
	return tokens;
}



int sh_launch(char **args, char *line)
{
	pid_t pid;
	int status;
	int fd[2];
	pipe(fd);
	pid = fork();//duplicating our current process
	if (pid == 0)
	{
		int val = 1;
        //command will run in child process
		if (execvp(args[0], args) == -1)
		{
			val = 0;
			// no need to use the read end of pipe here so close it
			close(fd[0]);
			write(fd[1], &val, sizeof(val));
			printf("Enter a valid command\n");
		}
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		perror("acm-sh");
	}
	else
	{
         //parent process wait until child process with process id pid terminates
		
		sleep(1);
		close(fd[1]);
		int val=1;
		read(fd[0], &val, sizeof(val));
		do
		{
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));

		bool *validate = (bool *)malloc(sizeof(bool));
		if (val == 1)
		{
			*validate = true;
		}
		else
		{
			*validate = false;
		}
		
		// add to list of history
		if (head_of_historylist == NULL)
		{

			head_of_historylist = create_list_of_history(line, *validate);
		}
		else
		{
			add_to_list_of_history(line, *validate);
		}
	}
	return 1;
}
//now after splitting the lines now
//our shell need to execute the command according to the arguements
int sh_execute(char **args, char *line)
{
	int i;
	if (args[0] == NULL)
	{
		return 1;
	}

	for (int i = 0; i < 7; i++)
    //if command matches with any of the builtin functions
    //shell with execute that here
	{
		if (strcmp(args[0], builtin_str[i]) == 0)
		{
			if (head_of_historylist == NULL)
			{
				head_of_historylist = create_list_of_history(line, true);
			}
			else
			{
				add_to_list_of_history(line, true);
			}

			return (*builtin_func[i])(args);
		}
	}

	return sh_launch(args, line);
}

void broadcastTermination(int pid,int status){
    if (WIFEXITED(status)){
        printf("exited, status=%d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)){
        printf("killed by signal %d\n", WTERMSIG(status));
    }
    else if(WIFSTOPPED(status))
    {
        printf("stopped by signal %d\n", WSTOPSIG(status));
    }
    else if(WIFCONTINUED(status))
    {
        printf("continued\n");
    }
    delete_from_list(pid);
}

static void signalHandler(int sig){
    int pid;
    int status;
    pid=waitpid(-1,&status,WNOHANG);
    broadcastTermination(pid,status);
}


int main(int argc,char **argv)
{
	char *line;
	char **args;
    char *test;
    char name[20];
    printf("Enter what you want your shell to be named(max size:15): (for eg. ACM-SH) " );
    scanf("%s",name);
    int count = 0;
    int status;
    signal(SIGCHLD,signalHandler);
	do{
		
		if(count != 0)printf("%s > ",name);
        count++;
		line=sh_read_line();
		args=sh_split_line(line);

        test = (char *)malloc(sizeof(char *) * (strlen(line)));

		strcpy(test, line);
		
		status=sh_execute(args,test);
        free(line);
		free(args);
	}
	while(status);

	return 0;
}