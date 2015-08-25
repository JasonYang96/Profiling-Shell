#Profiling-Shell CS 111 Lab 1

Jason Yang (804331785) & Kelly Ou (404146997)

##What it does:
- For just Lab1a, we parse the shell script and print out the commands in standard form.
- For Lab1a and Lab1b, we have the ability to parse the commands and print it out in standard form or perform what the shell would do with the shell script.
- For Lab1a, Lab1b, and Lab1c combined together, we have the ability to first print out the shell script in standard form with the flag "-t". We then also have the option to run the script regularly by running the command with no arguments. We can also run the script with some performance stats by running it with the "-p" flag.

###Lab 1a Implementations:
- -t flag to indicate you only want to print the shell script
- Simple Commands
- Subshell Commands
- Until Commands
- While Commands
- If Commands
- Comments
- Simple Syntax Error Checking

###Lab1a Semi-Implementations:
- Sequence Commands: only works with simple commands
- Pipe Commands: only works with simple commands
- Input/Output Commands: only simple cases work.

###Lab1b Implementation:
- Should be implemented fully
- Simple Commands
- Subshell Commands
- Until Commands
- While Commands
- If Commands

###Lab1c Implementation:
- Time statistics for all types of commands.
- Returns PID instead of command name if process did not exec anything.
- Using CLOCK_REALTIME for Unix Time, as well as CLOCK_MONOTONIC for all other time statistics.

##Test Shell Scripts:
###Testing -t:
- test-t-ok.sh -- tests if -t is working properly
- test-t-bad.sh -- tests if read-command.c is catching syntax errors correctly
- test-my-t-ok.sh -- tests our implementation of -t as we could not implement it all

###Testing running:
- test-bad.sh -- tests if running the shell script should fail/provide syntax errors
- test-ok.sh -- tests if running the shell script succeeds.

###Testing -p:
- test-true.sh and test-subshell.sh -- tests simple commands and subshell commands to see if they were implemented correctly
- Testing of pipe commands was included in the test-ok.sh script.

##Limitations:
- We could not code it in time to include compound commands. Other than that, we have it fully implemented.

##1c Implementation Questions:
1. Not all the time. For example, a forked process may finish its process before another process finishes its own. Therefore, the time of the forked process would be before the 2nd process.
2. There will always be errors with the time due to the precision of our own system clocks as well as the time delay that it takes to relay the information. Therefore, it may be possible that the real time can be less than the sum of the user and system CPU times.
3. These numbers are recieved from CLOCK_MONOTONIC, therefore the times will always represent the time from a certain point. These times will never be negative.
4. I made the log lines first print into a buffer, then write the entire buffer into the log when it was the parent process. Therefore, the log lines are not interleaved. I used the testscripts defined above.
