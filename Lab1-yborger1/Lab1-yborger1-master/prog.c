
/*
 * Swarthmore College, CS 31
 * Copyright (c) 2021 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professor Tia Newhall and Andrew Danner
 */

/*
 * TODO: add your top-level comment here
 */

/* C libraries are included here */
#include <stdio.h>      // the C standard I/O library
#include <stdlib.h>     // the C standard library
#include <time.h>       // time library, used to seed random num generator

/* define constant values here */
#define MAX  30        // the max size of the picture  

/* function prototypes go here */
int   read_int(char *msg);  /* these are defined for you */
float compute_average(int n, int m);
int get_in_range(int lo, int hi);
void draw_picture(int size);
/* TODO: add other function prototypes here: */


/***********************************************************/
int main (int argc, char *argv[]) {
  /* declare all local variables at the top of the function body */
  int num;  //pt 1
  int high;//pt 1
  int ans; //user-given number for picture pt2
  int decide= 1; //user choice, 0 or 1, to repeat pt2
  /* TODO: declare additional local variables here: */

  /* seed random number generator using the current time */
  srand(time(0));


  /* TODO: add main program control flow code here, starting
   * with editing this printf stmt to say what the program does
   * (you can add more printf stmts for this if the line gets too long)
   */

  printf("This program computes the average of a user specified number of random numbers from the range 0 to N, N specified by the user. \n");

  num = read_int("Enter the number of random values: ");
  if(num<=0){
    printf("Try again, %d is not a positive value.", num);
    num = read_int("Enter the number of random values: ");
  }
  high = read_int("\nEnter the largest value in the range: ");
  if(high <= 0){ //check max isn't negative either
    printf("Try again, %d has to be larger than 0.", high); //potential to simplify code: use part 2 get_in_range function; maybe next time
    high = read_int("\nEnter the largest value in the range: ");
   }
  printf("\n\nThe average of %d random values in the range [0,%d] is %f\n\n",num, high, compute_average(num, high)); //calculate the average directly where it is needed for printf
  
//PART 2
  printf("\nNow I'm going to draw a pretty picure for you!");
  while(decide){
    printf("\nEnter a value between 3 and %d", MAX);
    ans = get_in_range(3, MAX);
    draw_picture(ans);
    printf("\nDo you want to see another one? Enter 0 for no, 1 for yes");
    decide = get_in_range(0,1);
  }
  printf("\nBye bye\n\n");
  /* main returns an int value, 0 means it completed successfully */
  return 0;

}

/* TODO: ADD YOUR FUNCTION DEFINITONS HERE: */


/* THIS FUNCTION IS PROVIDED FOR YOU: DO NOT MODIFY! */
/******************************************
 * read_int: prompts the user to enter an integer value, 
 *           and returns to the entered value to the caller
 *  msg: the prompt msg to print
 *       (char * is the type for C strings, don't worry about understanding 
 *        the char * type now, we will talk about it later)
 *  returns: int value entered by user
 *
 * NOTE: this function is NOT robust to bad user input
 */
int read_int(char *msg) {

  int val;

  printf("%s", msg);
  scanf("%d", &val);
  return val;
}

float compute_average(int num, int high){
  float sum_rand=0;
  //note to self: make sure that one of the numbers is a float to do float math
  for(int i = 0; i <=num; i++){
    sum_rand += rand()%(high+1); //with %(high+1) we are guaranteed a maximum of high
  }
  float avg = sum_rand/num;
  return avg;
  return avg;
}
int get_in_range(int lo, int hi){
  int user_num = read_int("\nvalue: ");
  while(user_num < lo || user_num > hi){
    printf("Try again, %d is not between %d and %d", user_num, lo, hi);
    user_num = read_int("\nvalue: ");
  }
    return user_num; //return statement for int
}
void draw_picture(int size){ //no return needed for void
  //i is controlling the line, j is controlling the spaces, k is controlling the *
  int k_max;
  for(int i = 1; i <= size; i++){
    k_max = (2*i)-1; //this is too complex to be in the for-loop
    for(int j = size-i; j>0 ; j--){ //the negative of the drawing
      printf(" "); //spaces on left side is the only side needed
    }
    for(int k = 0; k < k_max; k++){
      printf("*");
    }
    //the pyramid rows do not go 1-2-3-4-5, it goes 1-3-5-7-9-11-13 etc.
    //to get that pattern in asterisks, I should have 2i-1 as the max for it
    printf("\n");
  }
}
