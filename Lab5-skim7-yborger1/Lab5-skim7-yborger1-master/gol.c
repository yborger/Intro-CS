/*
 * Swarthmore College, CS 31
 * Copyright (c) 2021 Swarthmore College Computer Science Department,
 * Swarthmore PA, Professors Tia Newhall, Andrew Danner, and Kevin Webb
 */

// Alison Kim and Yael Borger
// Lab #5 4.12.2021
// Spring 2021
// Newhall and Danner

/*
 * To run:
 * ./gol file1.txt  0  # run with config file file1.txt, do not print board
 * ./gol file1.txt  1  # run with config file file1.txt, ascii animation
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

/****************** Definitions **********************/
/* Two possible modes in which the GOL simulation can run */
#define OUTPUT_NONE   0   // with no animation
#define OUTPUT_ASCII  1   // with ascii animation

/* Used to slow down animation run modes: usleep(SLEEP_USECS);
 * Change this value to make theh animation run faster or slower
 */
//#define SLEEP_USECS  1000000
#define SLEEP_USECS    1000000

/* A global variable to keep track of the number of live cells in the
 * world (this is the ONLY global variable you may use in your program)
 */
static int total_live = 0;

/* This struct represents all the data you need to keep track of your GOL
 * simulation.  Rather than passing individual arguments into each function,
 * we'll pass in everything in just one of these structs.
 * this is passed to play_gol, the main gol playing loop
 *
 * NOTE: You will need to use the provided fields here, but you'll also
 *       need to add additional fields. (note the nice field comments!)
 * NOTE: DO NOT CHANGE THE NAME OF THIS STRUCT!!!!
 */
struct gol_data {

   // NOTE: DO NOT CHANGE the names of these 4 fields (but USE them)
   int rows;  // the row dimension
   int cols;  // the column dimension
   int iters; // number of iterations to run the gol simulation
   int numoflivecells; //number of live cells
   int output_mode;
   int * TDarray;
   int * new_array;

};

/****************** Function Prototypes **********************/

void play_gol(struct gol_data *data);
/* init gol data from the input file and run mode cmdline args */
int init_game_data_from_args(struct gol_data *data, char *argv[]);

/* print board to the terminal (for OUTPUT_ASCII mode) */
void print_board(struct gol_data *data, int round);

void init_arrays_zero(struct gol_data *data);
int find_alive_neighbors(struct gol_data *data, int row, int column);
void edge_and_corners_case(struct gol_data *data, int * neighbor_row, int * neighbor_column);

/**********************************************************/
int main(int argc, char *argv[]) {

  int ret;
  struct gol_data data;
  double secs, ret1, starttime_double, endtime_double;
  struct timeval start_time, stop_time;

  /* check number of command line arguments */
  if (argc < 3) {
    printf("usage: ./gol <infile> <0|1>\n");
    printf("(0: with no visi, 1: with ascii visi)\n");
    exit(1);
  }

  /* Initialize game state (all fields in data) from information
   * read from input file */


  ret = init_game_data_from_args(&data, argv);

  if(ret != 0) {
    printf("Error init'ing with file %s, mode %s\n", argv[1], argv[2]);
    exit(1);
  }

  ret1 = gettimeofday(&start_time, NULL);
  if (ret1 <0 ){
    printf("Error getting time of day. \n");
  }

  starttime_double = start_time.tv_sec + (start_time.tv_usec/1000000.0);

  /* Invoke play_gol in different ways based on the run mode */
  if(data.output_mode == OUTPUT_NONE) {  // run with no animation
    play_gol(&data);
  }
  else { // run with ascii animation
    play_gol(&data);

    // clear the previous print_board output from the terminal:
    // (NOTE: you can comment out this line while debugging)
  //  if(system("clear")) { perror("clear"); exit(1); }

    // NOTE: DO NOT modify this call to print_board at the end
    //       (its for grading)
    print_board(&data, data.iters);
  }

  ret1 = gettimeofday(&stop_time, NULL);
  endtime_double = stop_time.tv_sec + (stop_time.tv_usec/1000000.0);

  secs = endtime_double - starttime_double;


  /* Print the total runtime, in seconds. */
  // NOTE: do not modify these calls to fprintf
  fprintf(stdout, "Total time: %0.3f seconds\n", secs);
  fprintf(stdout, "Number of live cells after %d rounds: %d\n\n",
      data.iters, total_live);

  free(data.TDarray);
  free(data.new_array);


  return 0;
}
/**********************************************************/
/* initialize the gol game state from command line arguments
 *       argv[1]: name of file to read game config state from
 *       argv[2]: run mode value
 * data: pointer to gol_data struct to initialize
 * argv: command line args
 *       argv[1]: name of file to read game config state from
 *       argv[2]: run mode
 * returns: 0 on success, 1 on error
 */
int init_game_data_from_args(struct gol_data *data, char *argv[]) {

// dynamically allocating runmode from commandline into the struct
  int runmode, rows, columns, iterations, ret,coordx, coordy, numoflive;

  FILE *infile;  // a file pointer is not a pointer like pointers to other

  runmode = atoi(argv[2]);
  data->output_mode = runmode;

// open the file

  infile = fopen(argv[1], "r");
  if (infile == NULL) {
    printf("Error: file open %s\n", argv[1]);
    return(1);
  }

//read in the values within the file onto struct
  ret = fscanf(infile, "%d%d%d%d", &rows, &columns, &iterations, &numoflive);

  if (ret !=4 ){
    printf("Error: cannot read in four integers from file. \n");
  }

  data->rows = rows;
  data->cols = columns;
  data->iters = iterations;
  data->numoflivecells = numoflive;
  data->TDarray = malloc(sizeof(int)*(data->rows)*(data->cols));

  init_arrays_zero(data);

  while (ret != -1) {
      ret = fscanf(infile, "%d%d", &coordx, &coordy);
      data->TDarray[(coordy*data->cols) + coordx] = 1; //this should initialize all the live cells at the beginning/
  }

  printf("runmode: %d rows: %d columns: %d iteratons: %d number of live cells: %d. \n", data->output_mode, data->rows, data->cols, data->iters, data->numoflivecells);

  // (3) close the file when done with it
  fclose(infile);
  return 0;
}

/* init_array_zero function:
 *  initializes every value within the Two D array to zero.
 *    * goes through a double nested for loop to call every coordinate

 *    * sets the value to -
 *   parameter: data: pointer to a struct gol_data  initialized with
 *         all GOL game playing state
 */
void init_arrays_zero(struct gol_data *data) {

  int * twodarray;
  int * new_twodarray;
  int i, j;

  twodarray = malloc(sizeof(int)*(data->rows)*(data->cols));  // NxM int values
  new_twodarray = malloc(sizeof(int)*(data->rows)*(data->cols));  // NxM int values

  if(!twodarray) {
    printf("malloc failed\n");
    exit(1);
  }

  if(!new_twodarray) {
    printf("malloc failed\n");
    exit(1);
  }

  for(i=0; i < data->rows; i++) {
    for(j=0; j < data->cols; j++) {
      twodarray[i*data->cols + j] = 0;
    }
  }

  for(i=0; i < data->rows; i++) {
    for(j=0; j < data->cols; j++) {
      new_twodarray[i*data->cols + j] = 0;
    }
  }

  data->TDarray = twodarray;
  data->new_array = new_twodarray;


}
/**********************************************************/
/* the gol application main loop function:
 *  runs rounds of GOL,
 *    * updates program state for next round (world and total_live)
 *    * performs any animation step based on the output/run mode
 *
 *   data: pointer to a struct gol_data  initialized with
 *         all GOL game playing state
 */
void play_gol(struct gol_data *data) {
  //  at the end of each round of GOL, determine if there is an
  //  animation step to take based on the ouput_mode,
  //   if ascii animation:
  //     (a) call system("clear") to clear previous world state from terminal
  //     (b) call print_board function to print current world state
  //     (c) call usleep(SLEEP_USECS) to slow down the animation

  int i, j, alive_neighbors, iterations;
  int * temp_array;
  temp_array = malloc(sizeof(int)*(data->rows)*(data->cols));  // NxM int values

  for(i=0; i < data->rows; i++) {
    for(j=0; j < data->cols; j++) {
      data->new_array[i*data->cols + j] = data->TDarray[i*data->cols + j];
    }
  }

  iterations = 0;

  while(iterations < data->iters) {

    for(i=0; i < data->rows; i++) {
      for(j=0; j < data->cols; j++) {
        alive_neighbors = find_alive_neighbors(data, i, j);
        if ((alive_neighbors == 0) | (alive_neighbors ==1) | (alive_neighbors >=4)) {
          data->new_array[i*data->cols +j] = 0;
        }
        if (alive_neighbors == 3) {
            data->new_array[i*data->cols +j] = 1;
        }
      }
    }
    if (data->output_mode == 1){
      usleep(SLEEP_USECS);
      system("clear");
      print_board(data, iterations);

    }

    for(i=0; i < data->rows; i++) {
      for(j=0; j < data->cols; j++) {
        temp_array[i*data->cols + j] = data->new_array[i*data->cols + j];
      }
    }

    for(i=0; i < data->rows; i++) {
      for(j=0; j < data->cols; j++) {
        data->new_array[i*data->cols + j] = data->TDarray[i*data->cols + j];
      }
    }

    for(i=0; i < data->rows; i++) {
      for(j=0; j < data->cols; j++) {
        data->TDarray[i*data->cols + j] = temp_array[i*data->cols + j];
      }
    }
    iterations ++;
}
free(temp_array);

}


/* find_alive_neighbors function:
 *  finds the alive neighbors of a particular cell in the two_D array
 *    * parameters:
 *    data -- the pointer to the gol_data struct that contains the two D array
 *    row -- the current row of the cell in question
 *    column -- the current column of the cell in question
      * returns: an integer depicting the number of alive neighbors a cell has.
 */

int find_alive_neighbors(struct gol_data *data, int row, int column) {
  int neighbor_row;
  int neighbor_column;
  int count;
  count = 0;

  neighbor_row = row + 1;
  neighbor_column = column;

  edge_and_corners_case(data, &neighbor_row, &neighbor_column);

  if (data->TDarray[neighbor_row*data->cols + neighbor_column] == 1) {
    count ++;
  }

  neighbor_row = row;
  neighbor_column = column + 1;

  edge_and_corners_case(data, &neighbor_row, &neighbor_column);

  if (data->TDarray[neighbor_row*data->cols + neighbor_column] == 1) {
    count ++;
  }

  neighbor_row = row + 1;
  neighbor_column = column + 1;

  edge_and_corners_case(data, &neighbor_row, &neighbor_column);

  if (data->TDarray[neighbor_row*data->cols + neighbor_column] == 1) {
    count ++;
  }

  neighbor_row = row - 1;
  neighbor_column = column;

  edge_and_corners_case(data, &neighbor_row, &neighbor_column);

  if (data->TDarray[neighbor_row*data->cols + neighbor_column] == 1) {
    count ++;
  }

  neighbor_row = row;
  neighbor_column = column - 1;

  edge_and_corners_case(data, &neighbor_row, &neighbor_column);

  if (data->TDarray[neighbor_row*data->cols + neighbor_column] == 1) {
    count ++;
  }

  neighbor_row = row - 1;
  neighbor_column = column - 1;

  edge_and_corners_case(data, &neighbor_row, &neighbor_column);

  if (data->TDarray[neighbor_row*data->cols + neighbor_column] == 1) {
    count ++;
  }

  neighbor_row = row - 1;
  neighbor_column = column + 1;

  edge_and_corners_case(data, &neighbor_row, &neighbor_column);

  if (data->TDarray[neighbor_row*data->cols + neighbor_column] == 1) {
    count ++;
  }

  neighbor_row = row + 1;
  neighbor_column = column - 1;

  edge_and_corners_case(data, &neighbor_row, &neighbor_column);

  if (data->TDarray[neighbor_row*data->cols + neighbor_column] == 1) {
    count ++;
  }

  return count;

}


/* find_alive_neighbors function:
* This function is to specifically cater towards edge and corner cells where their neighbors
*    may be "out of the grid". We fix this by having the neighbor cell be the cell on the other side of the
*    2D array. (The two d array wraps around)
 *    * parameters:
 *    data -- the pointer to a gol_data struct that contains the two D array
 *    neighbor_row -- the pointer to the current row of the neighbor cell in question
 *    neighbor_column -- the pointer to the current column of the neighbor cell in question
      * returns: void. Updates the neighbor's rows and columns because it is a pointer.
 */

void edge_and_corners_case(struct gol_data *data, int * neighbor_row, int * neighbor_column) {

if (*neighbor_row <0) {
  *neighbor_row += data->rows;

}

if (*neighbor_row >= data->rows) {
  *neighbor_row = data->rows - *neighbor_row;

}

if (*neighbor_column < 0) {
  *neighbor_column += data->cols;

}

if (*neighbor_column >= data->cols) {
  *neighbor_column = data->cols - *neighbor_column;
}

}


/**********************************************************/
/* Print the board to the terminal.
 *   data: gol game specific data
 *   round: the current round number

 * NOTE: You may add extra printfs if you'd like, but please
 *       leave these fprintf calls exactly as they are to make
 *       grading easier!
 */
void print_board(struct gol_data *data, int round) {

    int i, j, counter;

    /* Print the round number. */
    fprintf(stderr, "Round: %d\n", round);
    counter = 0;

    for (i = 0; i < data->rows; ++i) {
        for (j = 0; j < data->cols; ++j) {
              if(data->TDarray[i*data->cols + j] == 1) {
                fprintf(stderr, " @");
                counter++;
              }
              else {
                fprintf(stderr, " .");
              }
        }
        fprintf(stderr, "\n");
    }
    total_live = counter;
    /* Print the total number of live cells. */
    fprintf(stderr, "Live cells: %d\n\n", total_live);
}
