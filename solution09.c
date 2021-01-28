/*###############################_Pier_Luigi_Manfrini_#############################
 *
 *	This program evaluates the following Taylor expansion:
 *
 *				 ___N
 *			         \      (x^i)
 *			e^x = 1 + .     _____
 *				 /__i=1	  i!
 *
 * 	The values of x and N are passed as CLI arguments, where x is a double
 *	and N is a positive (>0) integer. The main process forks creating N+1
 *	children each calculating the single ratio (x^i)/i! and passing the
 *	result back to the father before dying. The latter sums up all the results
 *	received from all its children and display the total on STDOUT.
 *
 *	Usage:
 *		<./solution09> <x> <N>
 *	
 *	
 *	Compile:
 *		gcc solution09.c -o solution09 -Wall -Werror -fsanitize=leak
 *
 *#################################################################################
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

double evaluate(double, int);
double factorial(int i);
double exponential(double, int);

int main(int argc, char **argv) {

  if (argc != 3) {
    dprintf(2, "Wrong Usage: <./solution09> <x> <N>\n");
    exit(EXIT_FAILURE);
  }
  double x = (double)strtod(argv[1], NULL);
  int N = (int)atoi(argv[2]);
  if (!(N > 0)) {
    dprintf(2, "Wrong Usage: N must be positive and greater than zero!\n");
    exit(EXIT_FAILURE);
  }

  int c2par[2];
  pipe(c2par);
  pid_t pid[N];

  for (int i = 0; i <= N; i++) {
    switch (pid[i] = fork()) {
    case -1:
      perror("");
      exit(EXIT_FAILURE);
      break;
    case 0: // Child
      close(c2par[READ]);
      double ret = evaluate(x, i);
      if (write(c2par[WRITE], &ret, sizeof(double)) != sizeof(double))
        perror("");
      close(c2par[WRITE]);
      exit(EXIT_SUCCESS);
      break;
    default: // Father
      break;
    }
  }
  // Also father
  close(c2par[WRITE]);
  ssize_t n_read;
  double sum = 0;
  double child_res;
  while ((n_read = read(c2par[READ], &child_res, sizeof(double))) ==
         sizeof(double)) {
    sum += child_res;
  }
  dprintf(STDOUT_FILENO, "The result of the Taylor Espansion is:\n\t> %10f\n\n",
          sum);
  pid_t collect;
  while ((collect = wait(NULL)) > 0) {
  }
  return 0;
}

double evaluate(double x, int i) {
  return (double)(exponential(x, i) / factorial(i));
}

double exponential(double x, int i) {
  /* Evaluates x^i */
if(x==0) return 0;
  double res = 1;
  for (int j = 1; j <= i; j++) {
    res = (double)(res * x);
  }
  return res;
}

double factorial(int i) {
  /* Evaluates i! */
  if (i == 2)
    return 2;
  else if (i == 1)
    return 1;
  else if (i == 0)
    return 1;
  else
    return i * factorial(i - 1);
}
