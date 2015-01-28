// UCLA CS 111 Lab 1 command execution

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

#include "command.h"
#include "command-internals.h"
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdbool.h>

bool write_error = false;

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
prepare_profiling (char const *name)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
    return open(name, O_CREAT | O_WRONLY | O_APPEND, 0644);
}

int
command_status (command_t c)
{
  return c->status;
}

void
execute_command (command_t c, int profiling)
{
  /* FIXME: Replace this with your implementation, like 'prepare_profiling'.  */
      struct rusage hi;
      struct timespec begin_time;
      char buffer[1023] = "";
      struct timespec end_time;
      struct timespec real_time;
      struct timespec UTC_time;
      double Real_time;
      double UTC_Time;
      double CPU_Time;
      double User_time;

    if (!c)
    {
        error(1, 0, "NULL command");
    }

	switch (c->type)
    {
    case IF_COMMAND:
        c->status = 0;
        execute_command(c->u.command[0], profiling);
        //if A is true
        if (command_status(c->u.command[0]) == 0)
        {
            execute_command(c->u.command[1], profiling);
            c->status = command_status(c->u.command[1]);
        }
        //else if A is false and there's an else statement
        else if (c->u.command[2] != NULL)
        {
            execute_command(c->u.command[2], profiling);
            c->status = command_status(c->u.command[2]);
        }
        break;

    case WHILE_COMMAND:
        c->status = 0;
        execute_command(c->u.command[0], profiling);
        while (command_status(c->u.command[0]) == 0) //true
        {
            //run "do" part
            execute_command(c->u.command[1], profiling);
            c->status = command_status(c->u.command[1]);
        }
        break;
    case UNTIL_COMMAND:
        c->status = 0;
        execute_command(c->u.command[0], profiling);
        while (command_status(c->u.command[0]) == 1) //false
        {
            //run "do" part
            execute_command(c->u.command[1], profiling);
            c->status = command_status(c->u.command[1]);
        }
        break;
    case SEQUENCE_COMMAND:
        execute_command(c->u.command[0], profiling);
        execute_command(c->u.command[1], profiling);
        c->status = command_status(c->u.command[1]);
        break;
    case PIPE_COMMAND:
      {
        int pipefd[2];
        int status;
        //piping error
        if (pipe(pipefd) == -1) {
            error(2,0, "Could not set up piping command");
        }
        pid_t left, right;
        left = fork();
        if (left == -1) {
            error(2,0, "Could not fork left properly");
        }
        else if (left == 0) //child
        {
            if (close(pipefd[0]) == -1)
            {
                error(2, 0, "Error closing right pipefd");
            }
            if (dup2(pipefd[1], STDOUT_FILENO) == -1)
            {
                perror(NULL);
                _exit(1);
            }
            execute_command(c->u.command[0], profiling);
            _exit(c->u.command[0]->status);
        }
        else
        {
            right = fork();
            if (right == -1)
            {
                error(2, 0, "Could not fork right properly");
            }
            else if (right == 0)
            {
                if (close(pipefd[1]) == -1)
                {
                    error(2, 0, "Error closing left pipefd");
                }
                if (dup2(pipefd[0], STDIN_FILENO) == -1)
                {
                    perror(NULL);
                    _exit(1);
                }
                execute_command(c->u.command[1], profiling);
                _exit(c->status);
            }
            else
            {
                if(waitpid(left, &status, 0) == -1)
                {
                    error(2, 0, "Error waiting for left pipe");
                }
                if(close(pipefd[1]) == -1)
                {
                    error(2, 0, "Error closing left pipefd");
                }
                if(waitpid(right, &status, 0) == -1)
                {
                    error(2, 0, "Error closing left pipefd");
                }
                if(close(pipefd[0]) == -1)
                {
                    error(2, 0, "Error waiting for left pipe");
                }
                c->status = WEXITSTATUS(status);
            }
        }
        break;
      }

    case SIMPLE_COMMAND:
      {
        clock_gettime(CLOCK_MONOTONIC, &begin_time);
        pid_t pid = fork();
        char** word = NULL;
        if (pid == -1) //forking problems
        {
            error(2,0,"Problem Forking");
        }
        else if(pid == 0) //children process
        {
            // handle input < redirect
            if (c->input)
            {
                // open input file
                int in;
                if ((in = open(c->input, O_RDONLY)) == -1)
                {
                    perror(c->input);
                    _exit(1);
                }

                // dup STDIN and in
                if (dup2(in, STDIN_FILENO) == -1)
                {
                    perror(NULL);
                    _exit(1);
                }

                close(in);
            }

            // handle output > redirect
            if (c->output)
            {
                // open output file
                int out;
                if ((out = open(c->output, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1)
                {
                    perror(c->output);
                    _exit(1);
                }

                // dup STDOUT and out
                if (dup2(out, STDOUT_FILENO) == -1)
                {
                    perror(NULL);
                    _exit(1);
                }

                close(out);
            }

            if (execvp(c->u.word[0], c->u.word) == -1)
            {
                error(2, 0, "Error running execvp");
            }
        }
        else //parent process
        {
            int status;
            while(wait4(pid, &status, 0, &hi) == 0)
            {
                continue;
            }
            //get endtime
            c->status = WEXITSTATUS(status);
        }
        if (profiling != -1 && !write_error)
          {
            //get user and CPU time
            User_time = hi.ru_utime.tv_sec + (hi.ru_utime.tv_usec/1000000.0);
            CPU_Time = hi.ru_stime.tv_sec + (hi.ru_stime.tv_usec/1000000.0);

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

            //get UTC_time
            clock_gettime(CLOCK_REALTIME, &UTC_time);
            UTC_Time = UTC_time.tv_sec + (UTC_time.tv_nsec/1000000000.0);
            snprintf(buffer, 1023, "%.6f %.6f %.6f %.6f", UTC_Time, Real_time, User_time, CPU_Time);
            word = c->u.word;
            while(*word != NULL && strlen(buffer) != 1023)
            {
                snprintf(buffer + strlen(buffer), 1023 - strlen(buffer), " %s", *word);
                word++;
            }
            sprintf(buffer + strlen(buffer), "\n");
            if (write(profiling, buffer, strlen(buffer)) == -1)
            {
                write_error = true;
            }
          }
        break;
      }

    case SUBSHELL_COMMAND:
    {
        pid_t subshell = fork();
        if (subshell == -1)
        {
            error(2, 0, "forking subshell problem");
        }
        else if (subshell == 0) //child
        {
            execute_command(c->u.command[0], profiling);
        }
        else //parent
        {
            int status;
            while(waitpid(subshell, &status, 0) == 0)
            {
                continue;
            }
            c->status = command_status(c->u.command[0]);
        }
        break;
    }
    
    default:
      abort ();
    }
}