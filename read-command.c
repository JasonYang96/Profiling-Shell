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

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */
struct command_stream {
  command* cmd = NULL;
  int cmd_total = 0;
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
  };

//linked list of token
typedef struc token token_t;
struct token
{
  enum token_type type;
  char* storage;
  token_t* next;
};

//linked list of token_list
typedef struct token_list token_list_t;
struct token_list
{
  token_t* head;
  token_list_t* next;
};

//creating new tokens
token_t* create_token(enum token_type, char* storage)
{
  token_t* t = checked_malloc(sizeof(token_t));
  t->type= token_type;
  t->storage = storage;
  t->next = NULL;
  return t;
}

token_list* tokenize_stream(int (*get_next_byte) (void *), void *get_next_byte_argument)
{
  token_list_t* t_list = (token_list_t*) checked_malloc(sizeof(token_list_t));
  token_list_t* current_list = t_list;

  char* buffer = (char*) checked_malloc(32*sizeof(char));
  size_t buffer_size = 32;
  int count = 0;

  while( (char c = get_next_byte) != EOF)
  {
    buffer[count] = c;
    count++;

    //realloc if buffer_size needs to be increased
    if (count == buffer_size)
    {
      checked_realloc(buffer, (buffer_size + 32)*sizeof(char);
    }

    //tokenize if command
    if (c == 'f' && count >= 2 && buffer[count - 2] == 'i')
    {
      int if_counter = 1;
      while(if_counter > 0)
      {
        //put everything until "fi" into storage of if token
      }
    }
    //tokenize until command
    else if (c == 'l' && count >= 5
      && buffer[count-5] == 'u'
      && buffer[count-4] == 'n'
      && buffer[count-3] == 't'
      && buffer[count-2] == 'i')
    {
      int until_counter = 1;
      while(until_counter > 0)
      {
        //put everything until "done" into storage of until token
      }
    }
    //tokenize while command
    else if (c == 'e' && count >= 5
      && buffer[count-5] == 'w'
      && buffer[count-4] == 'h'
      && buffer[count-3] == 'i'
      && buffer[count-2] == 'l')
    {
      int while_counter = 1;
      while(while_counter > 0)
      {
        //put everything until "done" into storage of while token
      }
    }
    //tokenize sequence or pipe command
    else if (c == ';' || c == '|')
    {
      //put everything into storage of sequence or pipe token
    }
    //tokenize input or output "command"
    else if (c == '<' || c =='>')
    {
      //put everything into storage of < or > token
    }
    //tokenize subshell command
    else if (c == '(')
    {
      int open_counter = 1;
      while (open_counter > 0)
      {
        //put everything until ) into storage of subshell token
      }
    }
    //ignore whitespace
    else if (c == ' ' || c == '\t')
    {
      //skip whitespace
    }
    //tokenize simple command
    else if (//still not sure)
    {
      //put everything into storage of simple token
    }
    //don't recognize char
    else
    {
      //output error message
    }
  }
  return t_list;
}

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME */
  command_stream_t cmd_stream;
  parse_command(get_next_byte, get_next_byte_argument, cmd_stream)
  return 0;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  error (1, 0, "command reading not yet implemented");
  return 0;
}
