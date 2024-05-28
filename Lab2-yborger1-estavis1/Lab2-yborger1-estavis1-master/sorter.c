/*
 * Swarthmore College, CS 31
 * Copyright (c) 2021 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professors Tia Newhall and Andy Danner
 */


/*
 * This program reads a file and makes an array from a certain number of floats
 * provided in the file. It then sorts these floats from smallest to largest
 * and prints sorted array.
 */

#include <stdio.h>      // the C standard I/O library
#include <stdlib.h>     // the C standard library
#include <string.h>     // the C string library
#include "readfile.h"   // my file reading routines

/* #define constant values here: */
#define MAXFILENAME  128
#define ARRAYSIZE    1000

/* function prototypes: */
void get_filename_from_commandline(char filename[], int argc, char *argv[]);
// our function prototypes
void print_array(float arr[], int len);
void sort_array(float arr[], int len);
int check(float arr[], int len);
void swap(int i1, int i2, float arr[]);
/********************************************************************/
int main (int argc, char *argv[]) {

  int ret;                    /* for storing function return values */
  char filename[MAXFILENAME]; /* the input filename  */
  float values[ARRAYSIZE];    /* stores values read in from file */
  /* additional variables: */
  float val, min, max; // each float value read-in
  int len; // length of read-in array

  /* initialize filename string from the command line arguments */
  get_filename_from_commandline(filename, argc, argv);

  /* try to open file */
  ret = open_file(filename);
  if(ret == -1) {
    printf("ERROR: can't open %s\n", filename);
    /* exit quits the program immediately. Some errors are not 
     * recoverable. Exit with an error message to handle these cases.
     */
    exit(1);
  }

  //read in file header to get number of floats needed
  
  ret = read_int(&len);
  if (ret == -1) {
    printf("ERROR: read_int failed\n");
    exit(1);
  }



  // tell the user:
  // (1) the name of the file being read
  // (2) how many float values there are
  // (3) the min and max values
  
  printf("Reading file named %s\n", filename);
  
  //read in minimum val
    ret = read_float(&min);
    if (ret == -1) {
      printf("ERROR: read_float failed\n");
      exit(1);
    }
  
  //read in maximum val
    ret = read_float(&max);
    if (ret == -1) {
      printf("ERROR: read_float failed\n");
      exit(1);
    }
  
  // prints min and max 
  printf("The file contains %d floating point numbers, ranging from "
      "%.2f to %.2f\n", len, min, max);
  

  //read in array vals
  for (int i = 0; i < len; i ++) {
    ret = read_float(&val);
    if (ret == -1) {
      printf("ERROR: read_float failed\n");
      exit(1);
    }
    values[i] = val;
  }

  
  // print out the unsorted values on a single line
  printf("The unsorted values are: ");
  print_array(values, len);
  // sort the array in place using a function
  sort_array(values, len);
  //double checks that it is sorted correctly, will re-try if not
  while (check(values, len) == -1) {
    sort_array(values, len);
  }
  // print out sorted values in the array
  printf("The sorted values are: ");
  print_array(values, len);

  close_file();  /* close file */
  return 0;
}
/***********************************************************/

/*
 * Function: print_array
 *
 * prints array
 *
 * arr[]: given array of floats to print
 * len: length of array 
 *
 * return: void
*/

void print_array(float arr[], int len) {
  for (int j = 0; j < len; j ++) {
    printf("%.2f ", arr[j]);
  }
  printf("\n");
}


/*
 * Function: sort_array
 *
 * sorts the array
 *
 * arr[]: array to be sorted
 * len: length of passed array
 *
 * return: void
*/

void sort_array(float arr[], int len) {
  for (int k = 0; k < len; k ++) {
    for (int l = 0; l < len - 1; l ++) {
      if (arr[l] > arr[l + 1]) {
        swap(l, l + 1, arr);
      }
    }
  }


}


/*
 * Function: check
 *
 * swaps array values
 *
 * arr[]: array to check sorting of
 * len: length of array
 *
 * return: 0 if all worked, -1 if not
*/

int check(float arr[], int len) {
  for(int m = 0; m < len - 1; m ++) {
    if (arr[m] > arr[m +1]) {
      return -1;
    }
  }
  return 0;
}

/*
 * Function: swap
 *
 * swaps array values
 *
 * i1 & i2: array positions to swap
 * arr[]: array that swap should happen in
 *
 * return: void
*/

void swap(int i1, int i2, float arr[]) {
  int x;
  x = arr[i1];
  arr[i1] = arr[i2];
  arr[i2] = x;
}

/***********************************************************/
/* THE FOLLOWING FUNCTION IS PROVIDED FOR YOU
 * DO NOT MODIFY, OR DO SO AT YOUR OWN RISK!
 *
 * This function gets the filename passed in as a command line option
 * and copies it into the filename parameter. It exits with an error
 * message if the command line is badly formed.
 *
 *   filename: the string to fill with the passed filename
 *   argc, argv: the command line parameters from main
 *               (number and strings array)
 */
void get_filename_from_commandline(char filename[], int argc, char *argv[]) {

  if(argc != 2) {
    printf("Usage: ./exp integer_file\n");
    exit(1);
  }
  if(strlen(argv[1]) >= MAXFILENAME) {
    printf("Filename, %s, is too long, cp to shorter name and try again\n",
        filename);
    exit(1);
  }
  strcpy(filename, argv[1]);
}
