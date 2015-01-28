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
    int capacity;
    int idx;
};


void find_error(char* buff, size_t* size)
{
    char c;
    char n;
    int cmd_counter = 0;
    int if_counter = 0;
    int then_counter = 0;
    int while_counter = 0;
    int until_counter = 0;
    int do_counter = 0;
    size_t index;
    for(index = 0; index < *size; index++)
    {
        c = buff[index];
        if (index+1 < *size)
            n = buff[++index];
        else
            break;
        // checks for invalid characters
        while (buff[index] == ' '
               || buff[index] == '\t')
        {
            index++;
        }
        if (strchr("!%%+,-./:@^_", c) != NULL)
        {
            //error(1, 0, "Syntax Error");
        }
        // checks for several characters that are not allowed side by side
        if ((strchr("|<>&", c) != NULL && strchr("|;<>&\n", n) != NULL)
            || (strchr("|;<>&\n", c) != NULL && strchr("|;<>&", n) != NULL))
        {
            //error(1, 0, "Syntax Error");
        }
        // if
        else if (index+2 < *size
                 && c == 'i' && buff[index + 1] == 'f'
                 && (buff[index + 2] == ' '
                     || buff[index + 2] == '\n'
                     || buff[index + 2] == '\t'))
        {
            cmd_counter++;
            if_counter++;
            index += 2;
        }
        // while
        else if (index+5 < *size
                 && c == 'w'
                 && buff[index + 1] == 'h'
                 && buff[index + 2] == 'i'
                 && buff[index + 3] == 'l'
                 && buff[index + 4] == 'e'
                 && (buff[index + 5] == ' '
                     || buff[index + 5] == '\n'
                     || buff[index + 5] == '\t'))
        {
            cmd_counter++;
            index += 5;
        }
        // until
        else if (index+5 < *size
                 && c == 'u'
                 && buff[index + 1] == 'n'
                 && buff[index + 2] == 't'
                 && buff[index + 3] == 'i'
                 && buff[index + 4] == 'l'
                 && (buff[index + 5] == ' '
                     || buff[index + 5] == '\n'
                     || buff[index + 5] == '\t'))
        {
            cmd_counter++;
            until_counter++;
            index += 5;
        }
        // (
        else if (index+1 < *size
                 && c == '('
                 && (buff[index + 1] == ' '
                     || buff[index + 1] == '\n'
                     || buff[index + 1] == '\t'))
        {
            cmd_counter++;
            index += 1;
        }
        // do
        else if (index+2 < *size
                 && c == 'd' && buff[index + 1] == 'o'
                 && (buff[index + 2] == ' '
                     || buff[index + 2] == '\n'
                     || buff[index + 2] == '\t'))
        {
            do_counter++;
            index += 2;
        }
        // then
        else if (index+4 < *size
                 && c == 't'
                 && buff[index + 1] == 'h'
                 && buff[index + 2] == 'e'
                 && buff[index + 3] == 'n'
                 && (buff[index + 4] == ' '
                     || buff[index + 4] == '\n'
                     || buff[index + 4] == '\t'))
        {
            then_counter++;
            index += 4;
        }
        // fi
        else if (index+2 < *size
                 && c == 'f' && buff[index + 1] == 'i'
                 && (buff[index + 2] == ' '
                     || buff[index + 2] == '\n'
                     || buff[index + 2] == '\t'))
        {
            cmd_counter--;
            index += 2;
        }
        // done
        else if (index+4 < *size
                 && c == 'd'
                 && buff[index + 1] == 'o'
                 && buff[index + 2] == 'n'
                 && buff[index + 3] == 'e'
                 && (buff[index + 4] == ' '
                     || buff[index + 4] == '\n'
                     || buff[index + 4] == '\t'))
        {
            cmd_counter--;
            index += 4;
        }
        // )
        else if (index+1 < *size
                 && c == ')'
                 && (buff[index + 1] == ' '
                     || buff[index + 1] == '\n'
                     || buff[index + 1] == '\t'))
        {
            cmd_counter--;
            index += 1;
        }
    }
    
    // the left/right, left/middle tokens don't match
    // if/fi, if/then, while/do, while/done, until/do, until/done
    if (cmd_counter != 0
        || if_counter != then_counter
        || (while_counter + until_counter != do_counter))
        //error(1, 0, "Syntax Error");
    {
    }
}

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
            buffer = checked_realloc(buffer, (buffer_size + 32)*sizeof(char));
            buffer_size += 32;
        }
        buffer[count] = c;
        count++;
    }
    
    //update total # of chars in file
    *size = count;
    
    find_error(buffer, size);
    
    return buffer;
}

//creating new commands
command_t create_command(enum command_type type, char* storage_input, size_t size, char* input, char* output)
{
    command_t cmd = checked_malloc(sizeof(struct command));
    size_t word_count = 0;
    size_t word_index = 0;
    if (type == SIMPLE_COMMAND)
    {
        cmd->type = type;
        cmd->storage = checked_malloc(sizeof(char));
        cmd->storage = "";
        cmd->storage_size = 0;
        if (output != NULL)
        {
            cmd->output = checked_malloc((strlen(output) + 1) * sizeof(char));
            strcpy(cmd->output, output);
        }
        else
        {
            cmd->output = NULL;
        }
        if (input != NULL)
        {
            cmd->input = checked_malloc((strlen(input) + 1) * sizeof(char));
            strcpy(cmd->input, input);
        }
        else
        {
            cmd->input = NULL;
        }
        cmd->u.word = checked_malloc(32*sizeof(char*));
        word_count = 32;
        char * pch;
        pch = strtok(storage_input, " ");
        while (pch != NULL)
        {
            cmd->u.word[word_index] = checked_malloc((strlen(pch))*sizeof(char));
            cmd->u.word[word_index] = pch;
            word_index++;
            pch = strtok(NULL, " ");

            if (word_index == word_count)
            {
                cmd->u.word = checked_realloc(cmd->u.word, (word_count + 32)*sizeof(char*));
            }
        }
        cmd->u.word[word_index] = NULL;
    }
    else
    {
        cmd->type = type;
        cmd->storage = checked_malloc((size + 1) * sizeof(char));
        cmd->storage = storage_input;
        cmd->storage_size = size;
        if (output != NULL)
        {
            cmd->output = checked_malloc((strlen(output) + 1) * sizeof(char));
            strcpy(cmd->output, output);
        }
        else
        {
            cmd->output = NULL;
        }
        if (input != NULL)
        {
            cmd->input = checked_malloc((strlen(input) + 1) * sizeof(char));
            strcpy(cmd->input, input);
        }
        else
        {
            cmd->input = NULL;
        }
    }
    return cmd;
}



command_t commandize_stream(char** stream, size_t* stream_size)
{
    command_t cmd;
    int command_counter = 0;
    
    size_t stream_index;
    for(stream_index = 0; stream_index < *stream_size; stream_index++)
    {
        char c = (*stream)[stream_index];
        
        //tokenize subshell command
        if (c == '(')
        {
            command_counter++;
            //ignore whitespace
            while ((*stream)[stream_index+1] == ' '
                   || (*stream)[stream_index+1] == '\n'
                   || (*stream)[stream_index+1] == '\t')
            {
                stream_index++;
            }
            c = (*stream)[++stream_index];
            
            char* buffer = (char*) checked_malloc(32*sizeof(char));
            size_t buffer_size = 32;
            size_t buffer_index = 0;
            int open_counter = 1;

            //put everything until ) into storage of subshell token
            while (open_counter > 0)
            {
                if (c == '(')
                {
                    open_counter++;
                    command_counter++;
                }
                else if (c == ')')
                {
                    open_counter--;
                    command_counter--;
                    
                    //make subshell token if final closed parentheses
                    if (open_counter == 0)
                    {

                        cmd = create_command(SUBSHELL_COMMAND, buffer, buffer_index, NULL, NULL);
                        cmd->u.command[0] = commandize_stream(&cmd->storage, &cmd->storage_size);
                        if (command_counter == 0)
                        {
                            if (stream_index < *stream_size)
                            {
                                stream_index++;
                                while ((*stream)[stream_index] == ' '
                                       || (*stream)[stream_index] == '\t'
                                       || (*stream)[stream_index] == '\n')
                                {
                                    stream_index++;
                                }
                                
                                *stream += stream_index;
                                *stream_size -= stream_index;
                                
                            }
                            else
                            {
                                *stream = NULL;
                                stream_size = 0;
                            }
                            return cmd;
                        }
                        else
                        {
                            break;
                        }
                    }
                }

                //if not ( or ), add to buffer
                buffer[buffer_index] = c;
                buffer_index++;
                
                //update c
                c = (*stream)[++stream_index];

                //realloc if buffer_size needs to be increased
                if (buffer_index == buffer_size)
                {
                    buffer = checked_realloc(buffer, (buffer_size + 32)*sizeof(char));
                    buffer_size += 32;
                }
            }
        }
        //tokenize if command
        else if (c == 'i' && (stream_index + 2 <= *stream_size) &&
                (*stream)[stream_index + 1] == 'f' &&
                ((*stream)[stream_index + 2] == ' ' ||
                (*stream)[stream_index + 2] == '\n' ||
                (*stream)[stream_index + 2] == '\t'))
        {
            command_counter++;
            stream_index += 3;

            //ignore whitespace after "if"
            while ((*stream)[stream_index] == ' '
                   || (*stream)[stream_index] == '\n'
                   || (*stream)[stream_index] == '\t')
            {
                stream_index++;
            }


            c = (*stream)[stream_index];
            char* buffer = (char*)checked_malloc(32 * sizeof(char));
            size_t buffer_size = 32;
            size_t buffer_index = 0;
            
            //put everything until "fi" into storage of if command
            int if_counter = 1;
            while (if_counter > 0)
            {
                c = (*stream)[stream_index];
                if (c == 'i' && (stream_index + 2 <= *stream_size) &&
                    (*stream)[stream_index + 1] == 'f' &&
                    ((*stream)[stream_index + 2] == ' ' ||
                    (*stream)[stream_index + 2] == '\n' ||
                    (*stream)[stream_index + 2] == '\t'))
                {
                    if_counter++;
                    command_counter++;
                }
                else if (c == 'f' && (stream_index + 2 <= *stream_size) &&
                        (*stream)[stream_index + 1] == 'i' &&
                        ((*stream)[stream_index + 2] == ' ' ||
                        (*stream)[stream_index + 2] == '\n' ||
                        (*stream)[stream_index + 2] == '\t'))
                {
                    if_counter--;
                    command_counter--;
                    //if final fi, create if command
                    if (if_counter == 0)
                    {
                        cmd = create_command(IF_COMMAND, buffer, buffer_index, NULL, NULL);
                        
                        //setting up command[0]
                        char* buffer_A = checked_malloc(buffer_size * sizeof(char));
                        char* buffer_after;
                        size_t buffer_A_index = 0;
                        size_t index = 0;
                        int then_counter = 1;
                        char a;
                        while (then_counter > 0)
                        {
                            a = buffer[index];
                            
                            if (a == 'i' && (buffer_index + 2 <= buffer_size)
                                && buffer[index + 1] == 'f'
                                && (buffer[index + 2] == ' '
                                || buffer[index + 2] == '\n'
                                || buffer[index + 2] == '\t'))
                            {
                                then_counter++;
                            }
                            else if (a == 't' && (buffer_index + 4 <= buffer_size)
                                     && buffer[index + 1] == 'h'
                                     && buffer[index + 2] == 'e'
                                     && buffer[index + 3] == 'n'
                                     && (buffer[index + 4] == ' '
                                         || buffer[index + 4] == '\n'
                                         || buffer[index + 4] == '\t'))
                            {
                                then_counter--;
                                if (then_counter == 0)
                                {
                                    while (buffer_A[buffer_A_index-1] == ';'
                                        || buffer_A[buffer_A_index-1] == '\n'
                                        || buffer_A[buffer_A_index-1] == '\t'
                                        || buffer_A[buffer_A_index-1] == ' ')
                                    {
                                        buffer_A_index--;
                                    }
                                    buffer_after = checked_malloc((buffer_A_index+1)*sizeof(char));
                                    strncpy(buffer_after,buffer_A, buffer_A_index);
                                    buffer_after[buffer_A_index] = '\n';
                                    cmd->u.command[0] = commandize_stream(&buffer_after, &buffer_A_index);
                                    break;
                                }
                            }
                            buffer_A[buffer_A_index++] = a;
                            index++;
                        }
                        
                        //update buffer_index to reflect position after "then"
                        index += 4;
                        
                        //get rid of whitespace after then
                        while (buffer[index] == ' '
                               || buffer[index] == '\n'
                               || buffer[index] == '\t')
                        {
                            index++;
                        }
                        
                        //setting up command[1]
                        char* buffer_B = checked_malloc(buffer_size * sizeof(char));
                        size_t buffer_B_index = 0;
                        while (index < buffer_index + 1)
                        {
                            a = buffer[index];
                            if (a == 'e'
                                && buffer[index + 1] == 'l'
                                && buffer[index + 2] == 's'
                                && buffer[index + 3] == 'e'
                                && (buffer[index + 4] == ' '
                                    || buffer[index + 4] == '\n'
                                    || buffer[index + 4] == '\t'))
                            {
                                while(buffer_B[buffer_B_index-1] == ';'
                                    || buffer_B[buffer_B_index-1] == '\n'
                                    || buffer_B[buffer_B_index-1] == '\t'
                                    || buffer_B[buffer_B_index-1] == ' ')
                                {
                                    buffer_B_index--;
                                }
                                buffer_after = checked_malloc(buffer_B_index*sizeof(char));
                                strncpy(buffer_after,buffer_B, buffer_B_index);
                                buffer_after[buffer_B_index++] = '\n';
                                cmd->u.command[1] = commandize_stream(&buffer_after, &buffer_B_index);

                                //update buffer_index to reflect position after "else"
                                index += 4;
                                
                                //setting up command[2]
                                a = buffer[index];
                                char* buffer_C = checked_malloc(buffer_size * sizeof(char));
                                size_t buffer_C_index = 0;
                                while (index < buffer_index + 1)
                                {
                                    buffer_C[buffer_C_index++] = a;
                                    a = buffer[++index];
                                }
                                
                                buffer_C_index--;
                                
                                while (buffer_C[buffer_C_index-1] == ';'
                                    || buffer_C[buffer_C_index-1] == '\n'
                                    || buffer_C[buffer_C_index-1] == '\t'
                                    || buffer_C[buffer_C_index-1] == ' ')
                                {
                                    buffer_C_index--;
                                }
                                buffer_after = checked_malloc(buffer_C_index*sizeof(char));
                                strncpy(buffer_after,buffer_C, buffer_C_index);
                                buffer_after[buffer_C_index++] = '\n';
                                cmd->u.command[2] = commandize_stream(&buffer_after, &buffer_C_index);
                                if (command_counter == 0)
                                {
                                    stream_index += 2;
                                    if (stream_index < *stream_size)
                                    {
                                        while ((*stream)[stream_index] == ' '
                                            || (*stream)[stream_index] == '\t'
                                            || (*stream)[stream_index] == '\n')
                                        {
                                            stream_index++;
                                        }
                                
                                    *stream += stream_index;
                                    *stream_size -= stream_index;
                                    }
                                     else
                                    {
                                      *stream = NULL;
                                      stream_size = 0;
                                    }
                                    return cmd;
                                }
                               else
                                {
                                break;
                                }
                            }
                            else if (index == buffer_index)
                            {
                                while(buffer_B[buffer_B_index-1] == ';'
                                    || buffer_B[buffer_B_index-1] == '\n'
                                    || buffer_B[buffer_B_index-1] == '\t'
                                    || buffer_B[buffer_B_index-1] == ' ')
                                {
                                    buffer_B_index--;
                                }
                                buffer_after = checked_malloc(buffer_B_index*sizeof(char));
                                strncpy(buffer_after,buffer_B, buffer_B_index);
                                buffer_after[buffer_B_index++] = '\n';
                                cmd->u.command[1] = commandize_stream(&buffer_after, &buffer_B_index);
                                if(command_counter == 0)
                                {
                                    stream_index += 2;
                                    while ((*stream)[stream_index] == ' '
                                           || (*stream)[stream_index] == '\t'
                                           || (*stream)[stream_index] == '\n')
                                    {
                                        stream_index++;
                                    }
                                    *stream += stream_index;
                                    *stream_size -= stream_index;
                                    return cmd;
                                }
                                else
                                {
                                    break;
                                }
                            }
                            buffer_B[buffer_B_index++] = a;
                            index++;
                        }
                        
                        //get rid of whitespace
                        while (buffer[index] == ' '
                               || buffer[index] == '\n'
                               || buffer[index] == '\t')
                        {
                            index++;
                        }
                    }
                }
                buffer[buffer_index] = c;
                buffer_index++;
                
                c = (*stream)[++stream_index];
                //realloc if buffer_size needs to be increased
                if (buffer_index == buffer_size)
                {
                    buffer = checked_realloc(buffer, (buffer_size + 32)*sizeof(char));
                    buffer_size += 32;
                }
            }
        }
        //tokenize until command
        else if (c == 'u' && (stream_index + 5 < *stream_size)
                 && (*stream)[stream_index + 1] == 'n'
                 && (*stream)[stream_index + 2] == 't'
                 && (*stream)[stream_index + 3] == 'i'
                 && (*stream)[stream_index + 4] == 'l'
                 && ((*stream)[stream_index + 5] == ' '
                     || (*stream)[stream_index + 5] == '\n'
                     || (*stream)[stream_index + 5] == '\t'))
        {
            command_counter++;
            stream_index += 6;
            //ignore whitespace
            while ((*stream)[stream_index] == ' '
                   || (*stream)[stream_index] == '\n'
                   || (*stream)[stream_index] == '\t')
            {
                stream_index++;
            }
            c = (*stream)[stream_index];
            
            char* buffer = (char*)checked_malloc(32 * sizeof(char));
            size_t buffer_size = 32;
            size_t buffer_index = 0;
            
            int until_counter = 1;
            while (until_counter > 0)
            {
                //put everything until "done" into storage of until token
                c = (*stream)[stream_index];
                if ((c == 'u' && (stream_index + 5 < *stream_size)
                     && (*stream)[stream_index + 1] == 'n'
                     && (*stream)[stream_index + 2] == 't'
                     && (*stream)[stream_index + 3] == 'i'
                     && (*stream)[stream_index + 4] == 'l'
                     && ((*stream)[stream_index + 5] == ' '
                         || (*stream)[stream_index + 5] == '\n'
                         || (*stream)[stream_index + 5] == '\t'))
                    || (c == 'w' && (stream_index + 5 < *stream_size)
                        && (*stream)[stream_index + 1] == 'h'
                        && (*stream)[stream_index + 2] == 'i'
                        && (*stream)[stream_index + 3] == 'l'
                        && (*stream)[stream_index + 4] == 'e'
                        && ((*stream)[stream_index + 5] == ' '
                            || (*stream)[stream_index + 5] == '\n'
                            || (*stream)[stream_index + 5] == '\t')))
                {
                    command_counter++;
                    until_counter++;
                }
                else if (c == 'd' && (stream_index + 4 < *stream_size)
                         && (*stream)[stream_index + 1] == 'o'
                         && (*stream)[stream_index + 2] == 'n'
                         && (*stream)[stream_index + 3] == 'e'
                         && ((*stream)[stream_index + 4] == ' '
                             || (*stream)[stream_index + 4] == '\n'
                             || (*stream)[stream_index + 4] == '\t'
                             || (*stream)[stream_index + 4] == EOF))
                {
                    until_counter--;
                    command_counter--;
                    if (until_counter == 0)
                    {
                        cmd = create_command(UNTIL_COMMAND, buffer, buffer_index, NULL, NULL);
                        
                        //setting up command[0]
                        char* buffer_A = checked_malloc(buffer_size * sizeof(char));
                        char* buffer_after;
                        size_t buffer_A_index = 0;
                        size_t index = 0;
                        int do_counter = 1;
                        char a;
                        while (do_counter > 0)
                        {
                            a = buffer[index];
                            
                            if (a == 'u' && (index + 5 < buffer_size)
                                && buffer[index + 1] == 'n'
                                && buffer[index + 2] == 't'
                                && buffer[index + 3] == 'i'
                                && buffer[index + 4] == 'l'
                                && (buffer[index + 5] == ' '
                                    || buffer[index + 5] == '\n'
                                    || buffer[index + 5] == '\t'))
                            {
                                do_counter++;
                            }
                            else if (a == 'w' && (index + 5 < buffer_size)
                                     && buffer[index + 1] == 'h'
                                     && buffer[index + 2] == 'i'
                                     && buffer[index + 3] == 'l'
                                     && buffer[index + 4] == 'e'
                                     && (buffer[index + 5] == ' '
                                         || buffer[index + 5] == '\n'
                                         || buffer[index + 5] == '\t'))
                            {
                                do_counter++;
                            }
                            else if (a == 'd' && (index + 2 < buffer_size)
                                    && buffer[index + 1] == 'o'
                                    && (buffer[index + 2] == ' '
                                        || buffer[index + 2] == '\n'
                                        || buffer[index + 2] == '\t'))
                            {
                                do_counter--;
                                if (do_counter == 0)
                                {
                                    while (buffer_A[buffer_A_index-1] == ';'
                                        || buffer_A[buffer_A_index-1] == '\n'
                                        || buffer_A[buffer_A_index-1] == '\t'
                                        || buffer_A[buffer_A_index-1] == ' ')
                                    {
                                        buffer_A_index--;
                                    }
                                    buffer_after = checked_malloc((buffer_A_index+1)*sizeof(char));
                                    strncpy(buffer_after,buffer_A, buffer_A_index);
                                    buffer_after[buffer_A_index++] = '\n';
                                    cmd->u.command[0] = commandize_stream(&buffer_after, &buffer_A_index);
                                    break;
                                }
                            }
                            buffer_A[buffer_A_index++] = a;
                            index++;
                        }
                        
                        //update buffer_index to reflect position after "do"
                        index += 2;
                        
                        //get rid of whitespace
                        while (buffer[index] == ' '
                               || buffer[index] == '\n'
                               || buffer[index] == '\t')
                        {
                            index++;
                        }
                        
                        //setting up command[1]
                        char* buffer_B = checked_malloc(buffer_size * sizeof(char));
                        size_t buffer_B_index = 0;
                        a = buffer[index];
                        while (index < buffer_index + 1)
                        {
                            buffer_B[buffer_B_index++] = a;
                            a = buffer[++index];
                        }
                        
                        buffer_B_index--;
                        

                        while(buffer_B[buffer_B_index-1] == ';'
                            || buffer_B[buffer_B_index-1] == '\n'
                            || buffer_B[buffer_B_index-1] == '\t'
                            || buffer_B[buffer_B_index-1] == ' ')
                        {
                            buffer_B_index--;
                        }
                        buffer_after = checked_malloc((buffer_B_index+1)*sizeof(char));
                        strncpy(buffer_after,buffer_B, buffer_B_index);
                        buffer_after[buffer_B_index++] = '\n';
                        cmd->u.command[1] = commandize_stream(&buffer_after, &buffer_B_index);
                        //cmd->u.command[1] = commandize_stream(&buffer_B, &buffer_B_index);
                        stream_index += 4;
                        if (command_counter == 0)
                        {
                            stream_index++;
                            while ((*stream)[stream_index] == ' '
                                   || (*stream)[stream_index] == '\t'
                                   || (*stream)[stream_index] == '\n')
                            {
                                stream_index++;
                            }
                            *stream += stream_index;
                            *stream_size -= stream_index;
                        }
                        else
                        {
                            *stream = NULL;
                            stream_size = 0;
                        }
                        return cmd;
                    }
                }
                buffer[buffer_index] = c;
                buffer_index++;
                
                c = (*stream)[++stream_index];
                //realloc if buffer_size needs to be increased
                if (buffer_index == buffer_size)
                {
                    buffer = checked_realloc(buffer, (buffer_size + 32)*sizeof(char));
                    buffer_size += 32;
                }
            }
        }
        //tokenize while command
        else if (c == 'w' && (stream_index + 5 < *stream_size)
                 && (*stream)[stream_index+1] == 'h'
                 && (*stream)[stream_index+2] == 'i'
                 && (*stream)[stream_index+3] == 'l'
                 && (*stream)[stream_index+4] == 'e'
                 && ((*stream)[stream_index+5] == ' '
                     || (*stream)[stream_index+5] == '\n'
                     || (*stream)[stream_index+5] == '\t'))
        {
            command_counter++;
            stream_index += 6;
            //ignore whitespace
            while ((*stream)[stream_index] == ' '
                   || (*stream)[stream_index] == '\n'
                   || (*stream)[stream_index] == '\t')
            {
                stream_index++;
            }
            c = (*stream)[stream_index];
            
            char* buffer = (char*)checked_malloc(32 * sizeof(char));
            char* buffer_after;
            size_t buffer_size = 32;
            size_t buffer_index = 0;
            
            int while_counter = 1;
            while (while_counter > 0)
            {
                //put everything until "done" into storage of while token
                c = (*stream)[stream_index];
                if ((c == 'w' && (stream_index + 5 < *stream_size)
                     && (*stream)[stream_index + 1] == 'h'
                     && (*stream)[stream_index + 2] == 'i'
                     && (*stream)[stream_index + 3] == 'l'
                     && (*stream)[stream_index + 4] == 'e'
                     && ((*stream)[stream_index + 5] == ' '
                         || (*stream)[stream_index + 5] == '\n'
                         || (*stream)[stream_index + 5] == '\t')) ||
                    (c == 'u' && (stream_index + 5 < *stream_size)
                     && (*stream)[stream_index + 1] == 'n'
                     && (*stream)[stream_index + 2] == 't'
                     && (*stream)[stream_index + 3] == 'i'
                     && (*stream)[stream_index + 4] == 'l'
                     && ((*stream)[stream_index + 5] == ' '
                         || (*stream)[stream_index + 5] == '\n'
                         || (*stream)[stream_index + 5] == '\t')))
                {
                    while_counter++;
                    command_counter++;
                }
                else if (c == 'd' && (stream_index + 4 < *stream_size)
                         && (*stream)[stream_index + 1] == 'o'
                         && (*stream)[stream_index + 2] == 'n'
                         && (*stream)[stream_index + 3] == 'e'
                         && ((*stream)[stream_index + 4] == ' '
                             || (*stream)[stream_index + 4] == '\n'
                             || (*stream)[stream_index + 4] == '\t'
                             || (*stream)[stream_index + 4] == EOF))
                {
                    while_counter--;
                    command_counter--;
                    if (while_counter == 0)
                    {
                        cmd = create_command(WHILE_COMMAND, buffer, buffer_index, NULL, NULL);
                        //setting up command[0]
                        char* buffer_A = checked_malloc(buffer_size * sizeof(char));
                        size_t buffer_A_index = 0;
                        size_t index = 0;
                        int do_counter = 1;
                        char a;
                        while (do_counter > 0)
                        {
                            a = buffer[index];
                            
                            if (a == 'u' && (index + 5 < buffer_size)
                                && buffer[index + 1] == 'n'
                                && buffer[index + 2] == 't'
                                && buffer[index + 3] == 'i'
                                && buffer[index + 4] == 'l'
                                && (buffer[index + 5] == ' '
                                    || buffer[index + 5] == '\n'
                                    || buffer[index + 5] == '\t'))
                            {
                                do_counter++;
                            }
                            else if (a == 'w' && (index + 5 < buffer_size)
                                     && buffer[index + 1] == 'h'
                                     && buffer[index + 2] == 'i'
                                     && buffer[index + 3] == 'l'
                                     && buffer[index + 4] == 'e'
                                     && (buffer[index + 5] == ' '
                                         || buffer[index + 5] == '\n'
                                         || buffer[index + 5] == '\t'))
                            {
                                do_counter++;
                            }
                            else if (a == 'd' && (index + 2 < buffer_size)
                                    && buffer[index + 1] == 'o'
                                     && (buffer[index + 2] == ' '
                                         || buffer[index + 2] == '\n'
                                         || buffer[index + 2] == '\t'))
                            {
                                do_counter--;
                                if (do_counter == 0)
                                {
                                    while (buffer_A[buffer_A_index-1] == ';'
                                        || buffer_A[buffer_A_index-1] == '\n'
                                        || buffer_A[buffer_A_index-1] == '\t'
                                        || buffer_A[buffer_A_index-1] == ' ')
                                    {
                                        buffer_A_index--;
                                    }
                                    buffer_after = checked_malloc((buffer_A_index+1)*sizeof(char));
                                    strncpy(buffer_after,buffer_A, buffer_A_index);
                                    buffer_after[buffer_A_index++] = '\n';
                                    cmd->u.command[0] = commandize_stream(&buffer_after, &buffer_A_index);
                                    //cmd->u.command[0] = commandize_stream(&buffer_A, &buffer_A_index);
                                    break;
                                }
                            }
                            buffer_A[buffer_A_index++] = a;
                            index++;
                        }
                        
                        //update buffer_index to reflect position after "do"
                        index += 2;
                        
                        //get rid of whitespace
                        while (buffer[index] == ' '
                               || buffer[index] == '\n'
                               || buffer[index] == '\t')
                        {
                            index++;
                        }
                        
                        //setting up command[1]
                        char* buffer_B = checked_malloc(buffer_size * sizeof(char));
                        size_t buffer_B_index = 0;
                        a = buffer[index];
                        while (index < buffer_index + 1)
                        {
                            buffer_B[buffer_B_index++] = a;
                            a = buffer[++index];
                        }
                        buffer_B_index--;
                        

                        while(buffer_B[buffer_B_index-1] == ';'
                            || buffer_B[buffer_B_index-1] == '\n'
                            || buffer_B[buffer_B_index-1] == '\t'
                            || buffer_B[buffer_B_index-1] == ' ')
                        {
                            buffer_B_index--;
                        }
                        buffer_after = checked_malloc((buffer_B_index+1)*sizeof(char));
                        strncpy(buffer_after,buffer_B, buffer_B_index);
                        buffer_after[buffer_B_index++] = '\n';
                        cmd->u.command[1] = commandize_stream(&buffer_after, &buffer_B_index);
                        //cmd->u.command[1] = commandize_stream(&buffer_B, &buffer_B_index);
                        stream_index += 4;
                        if (command_counter == 0)
                        {
                            stream_index++;
                            while ((*stream)[stream_index] == ' '
                                   || (*stream)[stream_index] == '\t'
                                   || (*stream)[stream_index] == '\n')
                            {
                                stream_index++;
                            }
                            *stream += stream_index;
                            *stream_size -= stream_index;
                        }
                        else
                        {
                            *stream = NULL;
                            stream_size = 0;
                        }
                        return cmd;
                    }
                }
                buffer[buffer_index] = c;
                buffer_index++;
                
                c = (*stream)[++stream_index];
                //realloc if buffer_size needs to be increased
                if (buffer_index == buffer_size)
                {
                    buffer = checked_realloc(buffer, (buffer_size + 32)*sizeof(char));
                    buffer_size += 32;
                }
            }
        }
        //parse comments
        else if (c == '#')
        {
            while (stream_index < *stream_size && c != '\n')
            {
                stream_index++;
                c = (*stream)[stream_index];
            }
        }
        // tokenize simple command
        else if (isalnum(c) || strchr("!%%+,-./:@^_", c) != NULL)
        {
            enum command_type buffer_command_type = SIMPLE_COMMAND;
            char* buffer = (char*)checked_malloc(32 * sizeof(char));
            char* buffer_A = (char*)checked_malloc(32 * sizeof(char));
            char* buffer_B = (char*)checked_malloc(32 * sizeof(char));
            char* buffer_redirect = (char*)checked_malloc(32 * sizeof(char));
            char* buffer_before;
            size_t buffer_size = 32;
            size_t buffer_r_size = 32;
            size_t buffer_index = 0;
            size_t buffer_A_index = 0;
            size_t buffer_B_index = 0;
            size_t buffer_r_index = 0;
            char* input = NULL;
            char* output = NULL;
            int is_input = 0;
            int is_output = 0;
            
            while (isalnum(c) || strchr("!%%+,-./:@^_;| ", c) != NULL)
            {
                //realloc if buffer_size needs to be increased
                if (buffer_index == buffer_size)
                {
                    buffer = checked_realloc(buffer, (buffer_size + 32)*sizeof(char));
                    buffer_size += 32;
                }
                
                buffer[buffer_index] = c;
                buffer_index++;
                
                //tokenize sequence or pipe command
                if ((*stream)[stream_index + 1] == ';' || (*stream)[stream_index + 1] == '|')
                {
                    if ((*stream)[stream_index + 1] == ';')
                    {
                        buffer_command_type = SEQUENCE_COMMAND;
                    }
                    else
                    {
                        buffer_command_type = PIPE_COMMAND;
                    }
                    //put everything into storage of command
                  while (buffer[buffer_index - 1] == ' '
                           || buffer[buffer_index - 1] == '\n'
                           || buffer[buffer_index - 1] == '\t')
                    {
                        buffer_index--;
                    }
                    
                    strncpy(buffer_A, buffer, buffer_index);
                    buffer_A_index = buffer_index;
                    
                    //update past ; or |
                    stream_index += 2;

                    c = (*stream)[stream_index];
                    buffer_index++;
                    
                    while ((*stream)[stream_index] == ' '
                           || (*stream)[stream_index] == '\n'
                           || (*stream)[stream_index] == '\t')
                    {
                        stream_index++;
                    }

                    c = (*stream)[stream_index];
                    
                    while ((stream_index + 1 < *stream_size) &&
                            (*stream)[stream_index] != '\n' &&
                            (*stream)[stream_index] != '<' &&
                            (*stream)[stream_index] != '>')
                    {
                        buffer_B[buffer_B_index] = c;
                        buffer_B_index++;
                        c = (*stream)[++stream_index];
                    }

                    //get rid of white space in buffer_B
                    while (buffer_B[buffer_B_index-1] == ' ' ||
                           buffer_B[buffer_B_index-1] == '\t' ||
                           buffer_B[buffer_B_index-1] == '\n')
                    {
                        buffer_B_index--;
                    }
                    break;
                }
                
                //parse redirections
                if ((*stream)[stream_index + 1] == '<' || (*stream)[stream_index + 1] == '>')
                {

                    //get rid of whitespace before < or >
                    while ((*stream)[stream_index] == '\t' ||
                           (*stream)[stream_index] == ' ')
                    {
                        buffer_index--;
                        stream_index++;
                    }
                    stream_index++;

                    buffer_before = checked_malloc(buffer_index*sizeof(char));
                    strncpy(buffer_before, buffer, buffer_index);
                    
                    if ((*stream)[stream_index] == '<')
                    {
                        is_input = 1;
                    }
                    else
                    {
                        is_output = 1;
                    }
                    stream_index++;
                    
                    // get rid of initial whitespace characters (excluding newline characters)
                    while ((*stream)[stream_index] == ' '
                           || (*stream)[stream_index] == '\t')
                    {
                        stream_index++;
                    }
                    
                    c = (*stream)[stream_index];
                    
                    //put succeeding (normal) characters into code
                    while (stream_index < *stream_size
                           && (isalnum(c)
                               || strchr("!%%+,-./:@^_ ", c) != NULL))
                    {
                        buffer_redirect[buffer_r_index] = c;
                        buffer_r_index++;
                        
                        if (buffer_r_index == buffer_r_size - 1)
                        {
                            buffer_redirect = checked_realloc(buffer_redirect, (32 + buffer_r_size)*sizeof(char*));
                            buffer_r_size += 32;
                        }
                        c = (*stream)[++stream_index];
                    }
                    
                    if (is_input == 1)
                    {
                        input = checked_malloc(strlen(buffer_redirect + 1) * sizeof(char));
                        strcpy(input, buffer_redirect);
                    }
                    else if (is_output == 1)
                    {
                        output = checked_malloc(strlen(buffer_redirect + 1) * sizeof(char));
                        strcpy(output, buffer_redirect);
                    }
                    else
                    {
                        input = NULL;
                        output = NULL;
                    }
                    break;
                }
                
                //increment c
                if (stream_index < *stream_size)
                {
                    c = (*stream)[++stream_index];
                }
                else
                {
                    break;
                }
            }
            
            
            //put everything into storage of simple token
            if (buffer_command_type == SIMPLE_COMMAND)
            {
                if(input == NULL || output == NULL)
                {
                    cmd = create_command(SIMPLE_COMMAND, buffer, buffer_index, input, output);
                }
                else
                {
                    cmd = create_command(SIMPLE_COMMAND, buffer_before, buffer_index, input, output);
                }
            }
            else if (buffer_command_type == SEQUENCE_COMMAND)
            {
                cmd = create_command(SEQUENCE_COMMAND, buffer, buffer_index, input, output);
                cmd->u.command[0] = commandize_stream(&buffer_A, &buffer_A_index);
                cmd->u.command[1] = commandize_stream(&buffer_B, &buffer_B_index);
            }
            else
            {
                cmd = create_command(PIPE_COMMAND, buffer, buffer_index, input, output);
                cmd->u.command[0] = commandize_stream(&buffer_A, &buffer_A_index);
                cmd->u.command[1] = commandize_stream(&buffer_B, &buffer_B_index);
            }
            
            if(command_counter == 0)
            {
                while ((*stream)[stream_index] == ' '
                    || (*stream)[stream_index] == '\t'
                    || (*stream)[stream_index] == '\n')
                    {
                        stream_index++;
                    }
                *stream += stream_index;
                *stream_size -= stream_index;
                return cmd;
            }
            else
            {
                break;
            }
        }
        //ignore whitespace
        else if ((c == ' ' || c == '\t' || c == '\n') && (stream_index + 1) < *stream_size)
        {
            continue;
        }
        //don't recognize char
        else
        {
            error(2, 0, "Syntax Error");
        }
    }
    return NULL;
}

command_stream_t
make_command_stream (int (*get_next_byte) (void *),
                     void *get_next_byte_argument)
{
    size_t init = 0;
    size_t* size = &init;
    char* file_stream = stream(get_next_byte,get_next_byte_argument, size);
    
    command_stream_t stream = checked_malloc(sizeof(struct command_stream));
    stream->capacity = 128;
    stream->cmd_total = stream->idx = 0;
    stream->cmd = checked_malloc(128 * sizeof(command_t));
    while(*size != 0)
    {
        if (stream->cmd_total == stream->capacity - 1)
        {
            stream->cmd = checked_realloc(stream->cmd, stream->capacity*sizeof(command_t)*2);
            stream->capacity *= 2;
        }
        stream->cmd[stream->cmd_total++] = commandize_stream(&file_stream, size);
    }
    
    return stream;
}

command_t
read_command_stream (command_stream_t stream)
{
    if (stream->idx < stream->cmd_total)
        return stream->cmd[stream->idx++];
    else return NULL;
}
