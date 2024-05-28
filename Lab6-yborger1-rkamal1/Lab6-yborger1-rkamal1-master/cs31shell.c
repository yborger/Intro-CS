/*
 * Swarthmore College, CS 31
 * Copyright (c) 2019 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professor Tia Newhall
 */

/* 
  * Yael Borger, Rezhwan Kamal
  * CS31 Lab 6 Part 1
  * April 19, 2021
  * Implemented a shell program
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include "parsecmd.h"

#define MAXHIST 10 // max number of commands in the history

// used to store history information about past commands
struct history_t {
  char command[MAXLINE];  // the command line from a past command
  int process;            // process number associated with cmd  
};

/* global variables: add only globals for history list state
 *                   all other variables should be allocated on the stack
 * static: means these variables are only in scope in this .c file 
 *
 * history: a circular list of the most recent command input strings.  
 *          Note: there are MAXHIST elements, each element is a
 *          history_t struct, whose definition you can change (by
 *          adding more fields).  It currently has one filed to store
 *          the command line string (a statically declared char array 
 *          of size MAXLINE)
 *          Remember: the strcpy function to copy one string to another
 */
static struct history_t history[MAXHIST]; 
static int history_next = 0; //indicates index
static int history_size = 0; //when it reaches 10 we are at max

void sigchld_handler(int signum);
void add_queue(char value[]);
void print_queue();

/******************************************************************/

int main( ){

  char cmdline[MAXLINE];
  char *argv[MAXARGS];
  int bg, num, ret, status = 0;
  pid_t pid;

  signal(SIGCHLD, sigchld_handler);
  if ( signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
    printf("ERROR signal failed\n");
  }
  while(1) {
    // (1) print the shell prompt (in a cool color!)
    printf("\e[1;36mcs31shell> \e[0m");
    fflush(stdout);

    // (2) read in the next command entered by the user
    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      perror("fgets error");
    }
    if (feof(stdin)) { /* End of file (ctrl-d) */
      fflush(stdout);
      exit(0);
    }
    // (3) make a call to parse_cmd function to parse it into its
    //     argv format
    //     if the command is '!num' change the command line accordingly
    if (cmdline[0] == '!') {
      num = atoi(&cmdline[1]);
      if (num == history[num % MAXHIST].process) {
        strcpy(cmdline, history[num % MAXHIST].command);
      } else {
        printf("!%d: not found\n", num);
        continue;
      }
    }
    ret = parse_cmd(cmdline, 
        argv, &bg);
    // error detection
    if (ret == -2) {
      printf("Error: cmdline is NULL");
      exit(1);
    }
    if (ret == -1) {
      printf("Error: cmdline is empty");
      exit(1);
    }
    add_queue(cmdline); // add cmdline to history[]
    // exit shell if user inputs "exit"
    if (strcmp(argv[0], "exit")== 0) {
      printf("bye bye\n");
      exit(1);
    }
    // print history[] contents in order
    else if (strcmp(argv[0], "history") == 0) {
      print_queue();
    }
    // fork off a child to execute a process
    else {
      pid = fork();
      if (bg == 1) {      // execute in the background
        if (pid == 0) {
          if (execvp(argv[0], argv) < 0) {
            printf("%s: Command not found.\n", argv[0]);
          }
          exit(0);
        }
      }
      else {           // execute in the foreground
        if (pid == 0) {
          if (execvp(argv[0], argv) < 0) {
            printf("%s: Command not found.\n", argv[0]);
          }
          exit(0);
        }
        else {
          // wait for the child to finish
          waitpid(pid, &status, 0);
        }
      }
    }
  }
  return 0;
}

/******************************************************************/
/* 
 * Signal handler to reap zombie children after they have exited
 * signum is the name of the SIGCHLD signal being passed in
 * Return is void, and the function reaps exited child processes
 */
void sigchld_handler(int signum) {
      int status;
      pid_t pid;

              /*
               *      * reap any and all exited child processes
               *           * (loop because there could be more than one)
               *                */
      while( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
                 //   uncomment debug print stmt to see what is being handled
                // printf("signal %d me:%d child: %d\n", signum, getpid(), pid);
                                           
      }
}

/*********************************/
/* add_queue(char value[]) function is meant to increment the size of
 * the overall history of the shell and make sure history is a circular 
 * queue through the variable history_next. It also adds the latest 
 * value(argument in commandline) to the history struct and numbers it 
 * within the struct.
 *
 * char value[] is a char array parameter holding the latest argument in
 * the commandline and is copied into a struct.
 * 
 * return is void
 */
void add_queue(char value[]){
  strcpy(history[history_next].command, value); 
  history[history_next].process = history_size;
  history_size++;
  history_next = history_size % MAXHIST; // make sure it's never > MAXHIST
 }

/*************************************************************/
/*
 * print_queue() prints up to the last MAXHIST most recent commands
 * Function has no inputs, uses history_t struct values & static variables
 * Return is void but prints out a series of commands and process numbers
 */
void print_queue(){
  if (history_size >= MAXHIST) {
    for(int i = history_next; i < MAXHIST; i++){
        printf("%d: %s", history[i].process, (history[i].command));
    }
  }
  for(int j = 0; j < history_next; j++){
     printf("%d: %s", history[j].process, (history[j].command));
  }
}
