


char* getline(int(*get_next_byte) (void*), void* arg, int* buffer_size, int* count)
{
  char* line = (char*) check_malloc(32 * sizeof(char));
  for (int k = count; k < buffer_size; k++)
  {
    //realloc space if needed
    if (buffer_size - 1 == k)
    {
      (char*) check_realloc(line, (32 + buffer_size) * sizeof(char));
      buffer_size += (32 * sizeof(char));
    }

    //if newline or EOF, break
    if (char temp = get_next_byte(arg) != ('\n' || EOF))
    {
      line[k] = temp;
      count++;
    }
    else
      break;
  }
  return line;
}

char* tokenize_stream(int(*get_next_byte) (void*), void* arg)
{
  int buffer_size = 32;
  char* line = getline(get_next_byte,arg, &buffer_size);
  char* pch;
  pch = strtok (line, "")
  while (pch != NULL)
  {

  }
}

{
  for (int k = 0; k < buffer_size; k++)
  {
    //realloc space if needed
    if (buffer_size - 1 == k)
    {
      (char*) check_realloc(stream, (32 + buffer_size) * sizeof(char));
      buffer_size += (32 * sizeof(char));
    }

    if ((char temp = get_next_byte(arg)) != EOF)
    {
      if (temp == '\n')
      {
        if (stream[k-1] == '\\')
        {
          k = k-1;
        }
      }
      else
        stream[k] = temp;
    }
    //temp == EOF
    else
      break;
  }
}

/* Create a command stream from GETBYTE and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */

bool is_if(char* c)
{
  if (c[0] == 'i')
  {
    if (c[1] == 'f')
    {
      if (c[2] == ' ' || '\n')
        return true;
    }
  }
  else
    return false;
}
bool is_until = false;
bool is_while = false;
bool is_sequence = false;
bool is_pipe = false;
bool is_simple = false;
bool is_subshell = false;

void* parse_command(int (*get_next_byte) (void*), void* (get_next_byte_argument), command_stream_t, cmd_stream)
{
  char* stream = (char*) check_malloc(32 * sizeof(char));
  int buffer_size = 32;
  int count = 0;
  stream = getline(get_next_byte, get_next_byte_argument, &buffer_size, &count);
  //if line ends with a \ character
  while (stream[count] == ''\'')
  {
    stream += getline(get_next_byte, get_next_byte_argument, &buffer_size, &count);
  }
  char* buffer = (char*) check_malloc(buffer_size * sizeof(char));
  buffer = stream;
  for (int k = 0; k < buffer_size; k++)
  {
    if ( (k + 1 < buffer_size) && is_if(buffer[k])
    {
      parse_if();
    }
    else if ( (k + 4 < buffer_size) && (is_until(buffer[k] || is_while(buffer[k])
    {
      parse_until_while();
    }
    else if ( buffer[k] == ';')
      parse_sequence();
    }
    else if ( buffer[k] == '|')
    {
      parse_pipe();
    }
    else if ( buffer[k] == '(')
    {
      parse_subshell();
    }
    else
    {
      parse_simple();
    }
  }
}