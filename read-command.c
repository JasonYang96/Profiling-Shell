// UCLA CS 111 Lab 1 command reading

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
#include "alloc.h"
#include <error.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
struct command_stream {
  command_t* cmd;
  int cmd_total;
};

//enum of different token types
enum token_type
  {
    IF_TOKEN,    // if A then B else C fi
    PIPE_TOKEN,        // A | B
    SEQUENCE_TOKEN,    // A ; B
    SIMPLE_TOKEN,      // a simple command
    SUBSHELL_TOKEN,    // ( A )
    UNTIL_TOKEN,   // until A do B done
    WHILE_TOKEN,   // while A do B done
    INPUT_TOKEN, // A < B
    OUTPUT_TOKEN, // A > B
    HEAD_TOKEN,
  };

//linked list of token
typedef struct token token_t;
struct token
{
  enum token_type type;
  char* storage;
  size_t storage_size;
  token_t* next;
};

//create a buffer for entire file, stream in all chars.
char* stream(int (*get_next_byte) (void *), void *get_next_byte_argument, size_t* size)
{
  char* buffer = (char*) checked_malloc(32*sizeof(char));
  size_t buffer_size = 32;
  size_t count = 0;

  int c;
  while( (c = get_next_byte(get_next_byte_argument)) != EOF)
  {
    //realloc if buffer_size needs to be increased
    if (count == buffer_size)
    {
      checked_realloc(buffer, (buffer_size + 32)*sizeof(char));
      buffer_size += 32;
    }
    buffer[count] = c;
    count++;
  }

  //update total # of chars in file
  *size = count;

  return buffer;
}

//creating new tokens
token_t* create_token(enum token_type type, char* storage, size_t size)
{
  token_t* t = checked_malloc(sizeof(token_t));
  t->type= type;
  t->storage = storage;
  t->storage_size = size;
  t->next = NULL;
  return t;
}

token_t* tokenize_stream(char* stream, size_t* stream_size)
{
  token_t* head = (token_t*) checked_malloc(sizeof(token_t));
  head = create_token(HEAD_TOKEN, NULL, 0);
  token_t* current = head;

  size_t stream_index;
  for(stream_index = 0; stream_index < *stream_size; stream_index++)
  {
    char c = stream[stream_index];

    //tokenize subshell command
    if (c == '(')
    {
      //ignore whitespace
      while (stream[stream_index+1] == (' ' || '\n' || '\t'))
      {
        stream_index++;
      }
      c = stream[++stream_index];

      char* buffer = (char*) checked_malloc(32*sizeof(char));
      size_t buffer_size = 32;
      size_t buffer_index = 0;
      int open_counter = 1;
      while (open_counter > 0)
      {
        //put everything until ) into storage of subshell token
        stream[++stream_index] = c;

        if (c == '(')
        {
          open_counter++;
        }
        else if (c == ')')
        {
          open_counter--;
          //make subshell token if final closed parentheses
          if (open_counter == 0)
          {
            current->next = create_token(SUBSHELL_TOKEN, buffer, buffer_index - 1);
            current = current->next;
          }
        }
        buffer[buffer_index++] = c;
        //realloc if buffer_size needs to be increased
        if (buffer_index == buffer_size)
        {
          checked_realloc(buffer, (buffer_size + 32)*sizeof(char));
          buffer_size += 32;
        }
      }
    }
    //tokenize if command
    if (c == 'i' && stream[stream_index+1] == 'f'
      && (stream[stream_index+2] == ' ' || stream[stream_index+2] == '\n'))
    {
      stream_index += 3;
      int if_counter = 1;
      while(if_counter > 0)
      {
        //put everything until "fi" into storage of if token
      }
    }
    //tokenize until/while command
    else if (c == 'u'
      && stream[stream_index+1] == 'n'
      && stream[stream_index+2] == 't'
      && stream[stream_index+3] == 'i'
      && stream[stream_index+4] == 'l'
      && (stream[stream_index+5] == ' ' || stream[stream_index+5] == '\n'))
    {
      stream_index += 6;
      int until_counter = 1;
      while(until_counter > 0)
      {
        //put everything until "done" into storage of until token
      }
    }
    //tokenize while command
    else if (c == 'w'
      && stream[stream_index+1] == 'h'
      && stream[stream_index+2] == 'i'
      && stream[stream_index+3] == 'l'
      && stream[stream_index+4] == 'e'
      && (stream[stream_index+5] == ' ' || stream[stream_index+5] == '\n'))
    {
      stream_index += 6;
      int while_counter = 1;
      while(while_counter > 0)
      {
        //put everything until "done" into storage of while token
      }
    }
    //tokenize simple command
    else if (isalnum(c) || strchr("!%%+,-./:@^_", c) != NULL)
    {
      char* buffer = (char*) checked_malloc(32*sizeof(char));
      size_t buffer_size = 32;
      size_t buffer_index = 0;
      while(isalnum(c) || strchr("!%%+,-./:@^_", c) != NULL)
      {
        //realloc if buffer_size needs to be increased
        if (buffer_index == buffer_size)
        {
          buffer = (char *) checked_realloc(buffer, (buffer_size + 32)*sizeof(char));
          buffer_size += 32;
        }

        buffer[buffer_index] = c;

        //tokenize sequence or pipe command
        if (stream[stream_index+1] == ';')
        {
          //put everything into storage of sequence token
        }
        else if (stream[stream_index+1] == '|')
        {
          //put everything into storage of pipe token
        }
        //tokenize input or output "command"
        else if (stream[stream_index+1] == '<')
        {
          //put everything into storage of input token
        }
        else if (stream[stream_index+1] == '>')
        {
          //put everything into storage of output token
        }
      }
      //put everything into storage of simple token
    }
    //ignore whitespace
	else if (c == ' ' || c == '\t' || c == '\n')
    {
      //skip whitespace
    }
    //don't recognize char
    else
    {
      //output error message
    }
  }
  return head;
}

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME */
	//testing
  size_t init = 0;
  size_t* size = &init;
  char* file_stream = stream(get_next_byte,get_next_byte_argument, size); //malloc'd
  // file_stream[0] = 'g';
  token_t* t = tokenize_stream(file_stream, size);
  // t->storage[0] = 'g';

  // char test = get_next_byte(get_next_byte_argument);

  command_stream_t cmd_stream;
  cmd_stream->cmd_total = 0;
  cmd_stream->cmd = NULL;
  return cmd_stream;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
	s->cmd_total = 1;
  command_t t;
  t->input = NULL;
  t->output = NULL;
  t->status = -1;
  t->type = IF_COMMAND;
  return t;
}
