/*
 * Swarthmore College, CS 31
 * Copyright (c) 2019 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professor Tia Newhall
 */

//
// Yael Borger, Rezhwan Kamal
// CPSC 31 Lab 6.2
// April 26, 2021
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parsecmd.h"

/***************************************************/
/* a static global variable means private to code in this file
 * it is used by parse_cmd to store a copy of the command
 * line that it can modify and make argv parm point to
 */
static char cmdline_copy[MAXLINE];

int next_space(char *array);
int next_alnum(char *array);
int find_num_tokens(const char *array, int *bg);

/*
 * parse_cmd: separates up to MAXLINE characters of the command line
 * into up to MAXARGS tokens, with each bucket of argv pointing to a
 * token. Function also sets *bg depending on if there is an '&'
 *
 * Parameters:
 *   - const char *cmdline: array of char for user command line arguments
 *   - char *argv[]: array of MAXLINE chars with MAXARGS buckets
 *   - int *bg: determines whether program runs in foreground/background
 *
 * Returns:
 *   - 0 if parsing is successful
 *   - -1 for an empty cmdline
 *   - -2 for a NULL cmdline
 */
int parse_cmd(const char *cmdline, char *argv[], int *bg) {
  int tokens = 0;
  *bg = 0;
  if (cmdline == NULL) {
    return -2;
  }
  if (!strcmp(cmdline, "")) {
    argv[0] = NULL;
    return -1;
  }
  strncpy(cmdline_copy, cmdline, MAXLINE);
  for (int i = 0; i < MAXLINE; i++) {
    if (cmdline_copy[i] == '\0') {
      break;
    }
    if (cmdline_copy[i] == '&') {
      *bg = 1;
      break;
    }
    if (!isspace(cmdline[i])) {
      if (tokens == MAXARGS-1) {
        break;
      }
      argv[tokens] = &cmdline_copy[i];
      tokens++;
      i += next_space(&cmdline_copy[i]);
      cmdline_copy[i] = '\0';
    }
  }
  argv[tokens] = NULL;
  return 0;
}

/***************************************************/
/*
 * parse_cmd_dynamic: allocates a 2D array that holds buckets for
 * each token of the user's command line. Also sets *bg.
 *
 * Parameters:
 *  - const char *cmdline: array of char for user command line arguments
 *  - int *bg: determines whether program runs in foreground/background
 *
 * Returns:
 *  - **argv: 2D array containing parsed command that the user needs
 *            to free()
 */
char **parse_cmd_dynamic(const char *cmdline, int *bg) {
  char **argv, *copy;
  int start = 0, index = 0, token_size = 0;
  *bg = 0;
  int tokens = find_num_tokens(cmdline, bg);

  if (cmdline == NULL) {
    return NULL;
  }
  if (!strcmp(cmdline, "")) {
    argv = malloc(sizeof(char)*2);
    argv[0] = malloc(sizeof(char));
    argv[0] = "";
    argv[1] = NULL;
    return argv;
  }
  
  copy = malloc(sizeof(char)*(strlen(cmdline)+1));
  strncpy(copy, cmdline, strlen(cmdline));
  copy[strlen(cmdline)] = '\0';
  argv = malloc(sizeof(char *)*(tokens+1));
  
  for (int i = 0; i < tokens; i++) { // for each token
    index += next_alnum(&copy[index]);   // find the first char in token   
    start = index;                   // save the index of that char
    index += next_space(&copy[index]);   // index = end of token
    token_size = index - start;      // size = end - start of token
    argv[i] = malloc(sizeof(char)*(token_size+1)); // allocate a bucket
    strncpy(argv[i], &copy[start], token_size);
    argv[i][token_size] = '\0';
  }
  argv[tokens] = NULL;
  free(copy);
  return argv;
}


/***************************************************/
/*
 * next_alnum: Finds the index of the next alphanumeric character
 * for a given char array, given a pointer to it.
 *
 * Parameters:
 *  - *array: pointer to the char array
 * 
 * Returns:
 *  - int value for displacement from base address before the next
 *    alphanumeric character
 */

int next_alnum(char *array) {
  int i = 0;
  while (isspace(array[i])) {
    i++;
  }
  return i;
}

/***************************************************/
/*
 * next_space: Finds the index of the next space for a given char array,
 * given a pointer to it.
 * 
 * Parameters: 
 *  - *array: pointer to the char array
 * Returns:
 *  - int value for displacement from base address before the next
 *          whitespace or null-terminating character ('\0')
 */
int next_space(char *array) {
  int i = 0;
  while (!isspace(array[i]) && array[i] != '\0') {
    i++;
  }
  return i;
}


/***************************************************/
/*
 * find_num_tokens: finds the number of tokens separated by
 * whitespaces for a given array of 'const char'. Also sets *bg
 * depending on whether there is a '&'
 *
 * Parameters:
 *  - const char *array: pointer to the char array
 *  - int *bg: determines whether process runs in foreground/background
 *
 * Returns:
 *  - tokens: total number of tokens in the char array 
 */
int find_num_tokens(const char *array, int *bg) {
  int i = 0, tokens = 0;
  while (array[i] != '\0') {
    if (array[i] == '&') {
      *bg = 1;
      break;
    }
    if (!isspace(array[i])) {
      tokens++;
      while (!isspace(array[i]) && array[i] != '\0') {
        i++;
      }
    } else {
      i++;
    }
  }
  return tokens;  
}
