/*
 * Swarthmore College, CS 31
 * Copyright (c) 2019 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professor Tia Newhall
 */

/*
 * This file reads in float values from a given input file into a dynamically
 * allocated array. It prints mathematical information about this array such
 * as the number of values, the min and max value, the mean, median, and
 * standard deviation, and the unused capacity. 
 *
 * Anika Rajamani and Yael Borger
 * March 15, 2021 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "readfile.h"

/* 
 * N: strart size for first dynamically allocated array of values
 * MAXFILENAME: max length of input file name string + 1 (includes '\0')
 */
#define N             20
#define MAXFILENAME   256
// you can add your own constants (using #define) here if needed

/* function prototypes */
float *get_values(int *size, int *capacity, char *filename);
void print_array(float *arr, int size);
void bubbleSort(float *values, int s);
float findMean(float *values, int s);
float findStddev(float *values, int s, float mean);
  
/**********************************************************************/
int main(int argc, char *argv[]) {

  char filename[MAXFILENAME];
  float *vals=NULL;
  int size, capacity, medianIndex;
  float mean, stddev;

  // this code checks command line args to make sure program was run
  // with filename argument.  If not, calls exit to quit the program
  if(argc != 2) {
    printf ("usage: ./stats filename\n");
    exit(1);
  }
  // this copies the filename command line argument into the 
  // string variable filename
  strncpy(filename, argv[1], MAXFILENAME-1);

  // call get_values function:
  vals = get_values(&size, &capacity, filename);
  bubbleSort(vals, size);
  
  printf("Results: \n");
  printf("-------\n");
  printf("\tNum Values:\t %d \n",size);
  printf("\tMin:\t %.3f \n",vals[0]);
  printf("\tMax:\t %.3f \n", vals[size-1]); 
  
  mean = findMean(vals, size);
  printf("\tMean:\t %.3f \n", mean);
  medianIndex = size/2; 
  printf("\tMedian:\t %.3f \n", vals[medianIndex]);
  stddev = findStddev(vals, size, mean); 
  printf("\tStd dev:\t %.3f \n", stddev);
  printf("Unused Array Capacity:\t %d \n", capacity-size);
  return 0;
}
/**********************************************************************/
/* This function reads in values from the input file into a dynamically
 * allocated array of floats that it returns.  It uses a doubling 
 * algorithm to increase the size of the array as it needs more space
 * for the values read in from the file.
 * 
 *   size: set to the number of data values read in from the file
 *   capacity: set to the total number of buckets in the returned array
 *   filename: the name of the file to open
 *
 *   returns: the base address of the array of values read in 
 *            or NULL on error
 *
 *   TODO: notice this awesome function comment.  You should write 
 *         similarly detailed comments for every function you write.
 */
float *get_values(int *size, int *capacity, char *filename){

  float *values = NULL;
  float *new = NULL; 
  int ret; 

  ret = open_file(filename); 
  if(ret == -1) {
    printf("ERROR: can't open %s\n", filename); 
    exit(1);
  }
  
  values = (float*)malloc(20*sizeof(float)); 
  *size = 0;
  *capacity = 20;
  while(ret != -1) {
    if(*size == *capacity) {
      *capacity = *capacity *2; 
      new = (float*)malloc(*capacity*sizeof(float)); 
      for(int i = 0; i< *size; i++){
        new[i] = values[i];
      }
      free(values);
      values = new;
    }
    else {
      ret = read_float(&values[*size]);
      if(ret != -1){
        *size+=1;
      }
    }
  }

  close_file(); // close the file
  return values;   // return the values array
}
/*******************************************
 * Function: print_array 
 *
 * This function prints out the float values contained in a specific array. 
 *
 * arr: this is the array that is to be printed in the terminal 
 * (we deference the arr pointer using *)
 *
 * size: this is the size of the given array 
 *
 * returns: this is a void function that does not return anything - 
 * it just print the array in the terminal 
 *
 */
void print_array(float *arr, int size) {
  int i; 
  for(i = 0; i < size; i++) {
    printf("%.3f   ", arr[i]); 
  }
  printf("\n"); 
}
/*
 * Function: bubbleSort 
 *
 * This is the bubble sort algorithm that sorts through a given array. 
 *
 * values: this is the array of floats that will be sorted by this function 
 * s: the size of the given array which is an integer 
 */

void bubbleSort(float *values, int s) {
  int i, j; 
  float temp; 
  for(i = 0; i < s-1; i++){
    for(j = 0; j < s-i-1; j++) {
      if (values[j] > values[j+1]){
        temp = values[j]; 
        values[j] = values[j+1]; 
        values[j+1] = temp; 
      }
    }
  }
}

/*
 * Function: findMean
 *
 * This function calculates the mean of the array by adding up the elements
 * of the array and dividing the sum by the number of elements in the array.
 *
 * values: values is the array that we are calculating the mean of. 
 * s: this is the size of the array values.
 */
float findMean(float *values, int s) {
  int i;
  float average,sum=0; 
  for(i = 0; i < s; i++){
    sum += values[i];
  }
  average = sum/s;
  return average;
}
/*
 * Function: findStddev
 *
 * This function calculates the standard deviation of the array by taking the
 * values and subtracting the mean from each value, squaring that 
 * difference (each value in the array), taking the mean of those 
 * squared differences, and taking the square root of that average.
 * 
 * values: values is the array we are calculating the standard deviation of.
 * s: this is the size of the array values.
 * mean: this is the mean of values
 */

float findStddev(float *values, int s, float mean){
  int i;
  float stddev;
  for(i = 0; i < s; i++){
    values[i] = (values[i] - mean)*(values[i] - mean);
    stddev += values[i];
  }
  stddev = stddev/(s-1); //making sure to go step-by-step for the formula
  stddev = sqrt(stddev);
  return stddev;  
}
