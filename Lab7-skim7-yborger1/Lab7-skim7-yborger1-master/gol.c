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
#define SLEEP_USECS    100000

/* A global variable to keep track of the number of live cells in the
 * world (this is the ONLY global variable you may use in your program)
 */
static int total_live = 0;
static pthread_barrier_t mybarrier;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
   int id;  //ID of the particular thread
   int rowstart;
   int rowend;
   int colstart;
   int colsend;
   int how_parallelize;
   int partition_thread;
   int num_threads;
   int is_finished;

};

/****************** Function Prototypes **********************/

void *play_gol(void *args);
/* init gol data from the input file and run mode cmdline args */
void partition_function(struct gol_data*data, struct gol_data*thread_array, int partition_num);

int init_game_data_from_args(struct gol_data *data, char *argv[]);
void thread_initialize_play(struct gol_data *data, struct gol_data *thread_array, pthread_t *tids);

/* print board to the terminal (for OUTPUT_ASCII mode) */
void print_board(struct gol_data *data, int round);

void init_arrays_zero(struct gol_data *data);
int find_alive_neighbors(struct gol_data *data, int row, int column);
void edge_and_corners_case(struct gol_data *data, int * neighbor_row, int * neighbor_column);

/**********************************************************/
int main(int argc, char *argv[]) {

  int m, ret, num_threads, how_parallelize, whether_print_board;
  struct gol_data data; //initial big board
  struct gol_data *thread_array; //dynamically array of structs that have parts of the GOL board that the threads will execute
  double secs, ret1, starttime_double, endtime_double;
  struct timeval start_time, stop_time;
  pthread_t *tids; //book keeping data


  /* check number of command line arguments */
  if (argc < 6) {
    printf("usage: ./gol <infile> <0|1>\n");
    printf("(0: with no visi, 1: with ascii visi)\n");
    exit(1);
  }

  /* Initialize game state (all fields in data) from information
   * read from input file */
   num_threads = atoi(argv[3]);
   how_parallelize = atoi(argv[4]);
   whether_print_board = atoi(argv[5]);

  ret = init_game_data_from_args(&data, argv);

  data.num_threads = num_threads;
  data.how_parallelize = how_parallelize;
  data.output_mode = whether_print_board;

  if (how_parallelize == 0) {
    if (num_threads > data.rows) {
      printf("Too many threads for the amount of rows. Exiting.....\n");
      exit(1);
    }
  }

  if (how_parallelize == 1) {
    if (num_threads > data.cols) {
      printf("Too many threads for the amount of columns. Exiting.....\n");
      exit(1);
    }
  }

  if(ret != 0) {
    printf("Error init'ing with file %s, mode %s\n", argv[1], argv[2]);
    exit(1);
  }

  ret1 = gettimeofday(&start_time, NULL);
  if (ret1 <0 ){
    printf("Error getting time of day. \n");
  }

  tids = (pthread_t *)malloc(sizeof(pthread_t) * num_threads); //initialize array of bookkeeping data
  if (!tids) {
    perror("ERROR: malloc pthread_t array");
    exit(1);
  }

  thread_array = (struct gol_data *)malloc(sizeof(struct gol_data) * num_threads);

  if (!thread_array) {
    perror("malloc thread_array array");
    exit(1);
  }
  total_live = 0;
  starttime_double = start_time.tv_sec + (start_time.tv_usec/1000000.0);

  pthread_barrier_init(&mybarrier,NULL,num_threads);

  thread_initialize_play(&data, thread_array, tids);


  for (m = 0; m < num_threads; m++) {
    pthread_join(tids[m], 0);
  }

    // NOTE: DO NOT modify this call to print_board at the end
    //       (its for grading)
  //print_board(&data, data.iters);

  ret1 = gettimeofday(&stop_time, NULL);
  endtime_double = stop_time.tv_sec + (stop_time.tv_usec/1000000.0);

  secs = endtime_double - starttime_double;

  fprintf(stdout, "Total time: %0.3f seconds\n", secs);
  fprintf(stdout, "After %d rounds on a %d by %d board, the number of live cells after %d rounds: %d. \n",
      data.iters, data.cols, data.rows, data.iters, total_live);

  free(data.TDarray);
  free(data.new_array);
  free(thread_array);
  thread_array = NULL;
  free(tids);
  tids = NULL;
  pthread_barrier_destroy(&mybarrier);

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

  int rows, columns, iterations, ret,coordx, coordy, numoflive;

  FILE *infile;

  data->output_mode = atoi(argv[5]);

  infile = fopen(argv[1], "r");
  if (infile == NULL) {
    printf("Error: file open %s\n", argv[1]);
    return(1);
  }

  ret = fscanf(infile, "%d%d%d%d", &rows, &columns, &iterations, &numoflive);

  if (ret !=4 ){
    printf("Error: cannot read in four integers from file. \n");
  }

  data->rows = rows;
  data->cols = columns;
  data->iters = iterations;
  data->numoflivecells = numoflive;

  init_arrays_zero(data);

  while (ret != -1) {
      ret = fscanf(infile, "%d%d", &coordy, &coordx);
      data->TDarray[(coordy*data->cols) + coordx] = 1;
  }
  total_live = data->numoflivecells;
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

/* thread_initialize_play function:
 *  This function partitions the threads by rows and cols evenly and creates the
 *  threads ready to be played in the play_gol function.
 *  It first allocates the each thread's struct with its personalized ID, and
 *  what rows and columns to start and stop in play_gol, depending on the
 *  user's partitioning mode.
 *  parameter:
 *  data = pointer to a struct gol_data initialized with
 *  the whole GOL game board playing state
 *  thread_array = a pointer to an array of structs that holds personalized
 *  information for each thread
 *  tids = an array of pthread_t pointers that allocates IDs that the OS
 *  allocates for each thread created.
 */
void thread_initialize_play(struct gol_data *data, struct gol_data *thread_array, pthread_t *tids){

int i, moddif, partition_num, ret;


for (i = 0; i<data->num_threads; i++){
  thread_array[i].id = i;
  thread_array[i].how_parallelize = data->how_parallelize;

  thread_array[i].TDarray = data->TDarray;
  thread_array[i].new_array = data->new_array;

  thread_array[i].iters = data->iters;
  thread_array[i].output_mode = data->output_mode;


  if(!thread_array[i].TDarray) {
    printf("malloc failed\n");
    exit(1);
  }

  if(!thread_array[i].new_array) {
    printf("malloc failed\n");
    exit(1);
  }

  thread_array[i].TDarray = data->TDarray;
  thread_array[i].new_array = data->new_array;
  thread_array[i].rows = data->rows;
  thread_array[i].cols = data->cols;


  if (data->how_parallelize == 0){
    partition_num = data->rows / data->num_threads;
    thread_array[i].partition_thread = partition_num;

    if (data->rows % data->num_threads != 0){
        moddif = data->rows % data->num_threads;
        if (moddif > thread_array[i].id){
          thread_array[i].partition_thread ++ ;
        }
      }

    if (thread_array[i].id == 0) {
      thread_array[i].rowstart = 0;
    }

    else{
      thread_array[i].rowstart = thread_array[i-1].rowend;
    }
    thread_array[i].rowend = thread_array[i].rowstart + thread_array[i].partition_thread;
    thread_array[i].colstart = 0;
    thread_array[i].colsend = data->cols;

  }

    if (data->how_parallelize == 1){

      partition_num = data->cols / data->num_threads;
      thread_array[i].partition_thread = partition_num;


      if (data->cols % data->num_threads != 0){
          moddif = data->cols % data->num_threads;
          if (moddif > thread_array[i].id){
            thread_array[i].partition_thread ++ ;
          }
        }

      if (thread_array[i].id == 0) {
        thread_array[i].colstart = 0;
      }
      else{
        thread_array[i].colstart = thread_array[i-1].colsend;
      }

      thread_array[i].colsend = thread_array[i].colstart + thread_array[i].partition_thread;
      thread_array[i].rowstart = 0;
      thread_array[i].rowend = data->rows;

    }
  ret = pthread_create(&tids[i], 0, play_gol, &thread_array[i]);

  if (ret) {
    perror("Error: pthread_create\n");
    exit(1);
  }
}


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
 void *play_gol(void *args) {
  //  at the end of each round of GOL, determine if there is an
  //  animation step to take based on the ouput_mode,
  //   if ascii animation:
  //     (a) call system("clear") to clear previous world state from terminal
  //     (b) call print_board function to print current world state
  //     (c) call usleep(SLEEP_USECS) to slow down the animation

  struct gol_data *data;  // add local variable of the type we know is passed in
  data = (struct gol_data *)args;  // initialize it to the re-cast args value

  int i, j, alive_neighbors, iterations;
  int * temp_array;


  for(i=0; i < data->rows; i++) {
    for(j=0; j < data->cols; j++) {
      data->new_array[i*data->cols + j] = data->TDarray[i*data->cols + j];
    }
  }
  iterations = 0;

  while(iterations <= data->iters) {
    data->numoflivecells = 0;

    for(i=data->rowstart; i < data->rowend; i++) {
      for(j=data->colstart; j < data->colsend; j++) {
        alive_neighbors = find_alive_neighbors(data, i, j);
        if ((alive_neighbors == 0) | (alive_neighbors ==1) | (alive_neighbors >=4)) {
          data->new_array[i*data->cols +j] = 0;
        }

        if (alive_neighbors == 2){
          data->new_array[i*data->cols +j] = data->TDarray[i*data->cols +j];
          if (data->new_array[i*data->cols +j] == 1){
            data->numoflivecells ++;
          }
        }

        if (alive_neighbors == 3) {
            data->new_array[i*data->cols +j] = 1;
            data->numoflivecells ++;
        }
      }
    }

    pthread_barrier_wait(&mybarrier);

    pthread_mutex_lock(&mutex);
    total_live = total_live + data->numoflivecells;
    pthread_mutex_unlock(&mutex);

    pthread_barrier_wait(&mybarrier);

    temp_array = data->new_array;
    data->new_array = data->TDarray;
    data->TDarray = temp_array;

    if (data->output_mode == 1){
      if (data->id == 0) {
        usleep(SLEEP_USECS);
        system("clear");
        print_board(data, iterations);
      }
    }

    if ((data->id == 0) && (iterations != data->iters)){
      total_live = 0;
    }
    pthread_barrier_wait(&mybarrier);

    iterations ++;
  }
return NULL;
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

    int i, j;

    /* Print the round number. */
    fprintf(stderr, "Round: %d\n", round);

    for (i = 0; i < data->rows; ++i) {
        for (j = 0; j < data->cols; ++j) {
              if(data->TDarray[i*data->cols + j] == 1) {
                fprintf(stderr, " @");
              }
              else {
                fprintf(stderr, " .");
              }
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "Live cells: %d\n\n", total_live);
}
