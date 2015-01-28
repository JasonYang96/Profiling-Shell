// UCLA CS 111 Lab 1 main program

// Copyright 2012-2014 Paul Eggert.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/resource.h>
#include "command.h"

static char const *program_name;
static char const *script_name;

static void
usage (void)
{
  error (1, 0, "usage: %s [-p PROF-FILE | -t] SCRIPT-FILE", program_name);
}

static int
get_next_byte (void *stream)
{
  return getc (stream);
}

int
main (int argc, char **argv)
{
  struct timespec begin_time;
  clock_gettime(CLOCK_MONOTONIC, &begin_time);
  char buffer[1023] = "";
  struct timespec end_time;
  struct timespec real_time;
  struct timespec UTC_time;
  struct rusage self;
  struct rusage child;
  struct timeval user_time;
  struct timeval CPU_time;
  double Real_time;
  double UTC_Time;
  double CPU_Time;
  double User_time;
  int command_number = 1;
  pid_t self_pid;
  bool print_tree = false;
  char const *profile_name = 0;
  program_name = argv[0];

  for (;;)
    switch (getopt (argc, argv, "p:t"))
      {
      case 'p': profile_name = optarg; break;
      case 't': print_tree = true; break;
      default: usage (); break;
      case -1: goto options_exhausted;
      }
 options_exhausted:;

  // There must be exactly one file argument.
  if (optind != argc - 1)
    usage ();

  script_name = argv[optind];
  FILE *script_stream = fopen (script_name, "r");
  if (! script_stream)
    error (1, errno, "%s: cannot open", script_name);
  command_stream_t command_stream =
    make_command_stream (get_next_byte, script_stream);
  int profiling = -1;
  if (profile_name)
    {
      profiling = prepare_profiling (profile_name);
      if (profiling < 0)
	error (1, errno, "%s: cannot open", profile_name);
    }

  command_t last_command = NULL;
  command_t command;
  while ((command = read_command_stream (command_stream)))
    {
      if (print_tree)
      	{
      	  printf ("# %d\n", command_number++);
      	  print_command (command);
      	}
      else
      	{
      	  last_command = command;
      	  execute_command (command, profiling);
      	}
    }
  if (profiling != -1)
  {
    //get user and CPU time
    getrusage(RUSAGE_SELF, &self);
    getrusage(RUSAGE_CHILDREN, &child);
    timeradd(&self.ru_utime, &child.ru_utime, &user_time);
    timeradd(&self.ru_stime, &child.ru_stime, &CPU_time);
    User_time = user_time.tv_sec + (user_time.tv_usec/1000000.0);
    CPU_Time = CPU_time.tv_sec + (CPU_time.tv_usec/1000000.0);

    //get real time
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    real_time.tv_sec = end_time.tv_sec - begin_time.tv_sec;
    real_time.tv_nsec = end_time.tv_nsec - begin_time.tv_nsec;
    //take care of nsec carryover
    if (real_time.tv_nsec < 0)
    {
     real_time.tv_sec--;
     real_time.tv_nsec += 1000000000.0;
    }
    Real_time = real_time.tv_sec + (real_time.tv_nsec/1000000000.0);

    self_pid = getpid();
    //get UTC_time
    clock_gettime(CLOCK_REALTIME, &UTC_time);
    UTC_Time = UTC_time.tv_sec + (UTC_time.tv_nsec/1000000000.0);
    snprintf(buffer, 1023, "%.6f %.6f %.6f %.6f [%d]\n", UTC_Time, Real_time, User_time, CPU_Time, self_pid);
    write(profiling, buffer, strlen(buffer));
  }

  return print_tree || !last_command ? 0 : command_status (last_command);
}
