# UNIX SHELL

A Shell provides you with an interface to the Unix system. It gathers input from you and executes programs based on that input. When a program finishes executing, it displays that program's output.

This is a custom shell developed in C.

## Running this shell in your local System

Follow these steps to run the shell in your pc.

* Clone the repository.
* Change the directory to the directory where this is cloned.
* Run following commands in your terminal.
 
     ```
     gcc yoursh.c linkedlist.c linkedlist.h utilities.h utilities.c linkedListForHistory.c linkedListForHistory.h
     ```

    ```
    ./a.out
    ```
    This will open the shell in your terminal.

**Note** - The commands given in step 2 assumes that you have gcc installed on your system and you are working in an ubuntu operating system.

## Discovering My Shell

#### 1. ```help```
 >To know about different builtin command inside terminal,type help and hit enter.

![Image of help command](/static/shell_help.png)

 #### 2. ```cd```
 >To change the current working directory, type cd directoryName and hit enter.
 >To know about current working directory type pwd and hit enter.

![Image of cd command](/static/shell_cd.png)

 #### 3. ```kill```
 >To kill a process with a specific _pid_,type kill _pid_ and hit enter.

![Image of kill command](/static/shell_kill.png)

 #### 4. ```exit```
 >To exit shell, type exit and hit enter.

![Image of exit command](/static/shell_exit.png)

 #### 5. ```history```
 >To know about previous command entered,type history and hit enter.

![Image of history command](/static/shell_history.png)

 #### 6. ```bg```
 >To run a command in background,so that you can run different commands in parallel,type bg and then the command and hit enter.

![Image of bg command](/static/shell_bg.png)

 #### 7. ```bglist```
 >print all the background jobs and details about it,type bglist and hit enter.

![Image of bglist command](/static/shell_bglist.png)

 


