#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define RED   "\x1B[31m"

#define GRN   "\x1B[32m"

#define YEL   "\x1B[33m"

#define BLU   "\x1B[34m"

#define MAG   "\x1B[35m"

#define CYN   "\x1B[36m"

#define WHT   "\x1B[37m"

#define RESET "\x1B[0m"

char *colors[] = {
  "\x1B[31m",
  "\x1B[32m",
  "\x1B[33m",
  "\x1B[34m",
  "\x1B[35m",
  "\x1B[36m",
  "\x1B[37m",
  "\x1B[0m"
};

char *rcolor()
{
  srand(time(NULL));
  int i = (rand() % (7));
  return colors[i];
}
int main(int argc, char **argv)
{
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  for (int i = 0;; i++)
  {
    if ((i % 25000) == 0)
    {
      t = time(NULL);
      tm = *localtime(&t);
      printf("now: %s%d-%d-%d" RESET" %s%02d:%02d:%02d" RESET "\t\ti = %s%d" RESET "\n", rcolor(), tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, rcolor(), tm.tm_hour, tm.tm_min, tm.tm_sec, rcolor(), i);
      sleep(1);
    }
   }
  return 0;
}
