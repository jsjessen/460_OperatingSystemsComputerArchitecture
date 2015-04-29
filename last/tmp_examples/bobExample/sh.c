#include "ucode.c"

#define MAX_INPUT_LEN 128
#define MAX_NUM_PIPES 32
#define MAX_FILENAME_LEN 32

int child, current_pipe_index;
// pipe_index[] is an array of index locations within input[] where '|' appears in the order of RIGHT TO LEFT.
int pipe_index[MAX_NUM_PIPES], infile_index, outfile_index, append_index;
int pipe_index_counter;

char input[MAX_INPUT_LEN];
char infilename[MAX_FILENAME_LEN], outfilename[MAX_FILENAME_LEN], appendfilename[MAX_FILENAME_LEN];

main(int argc, char *argv[])
{
	int has_special_character;
	int fd, infile_fd, outfile_fd, appendfile_fd;
	int id, status;

	char tty[64];

	gettty(tty);

	while (1)
	{
		// New input so no known locations of pipes, infile, outfile, and append indexes.
		reset_input();
		fd = infile_fd = outfile_fd = appendfile_fd = id = status = -1;

		printf("\n$ ");
		gets(input);

		has_special_character = find_special_characters();

		if (has_special_character == 1)
		{
			// we have at least 1 of the following: pipe(s), infile, outfile, append.
			if (infile_index > -1)
			{
				get_infilename();
			}
			if (outfile_index > -1)
			{
				get_outfilename();
				outfile_fd = open_outfile();
				if (outfile_fd == -1)
					continue;
			}
			else if (append_index > -1)
			{
				get_appendfilename();
				appendfile_fd = open_appendfile();
				if (appendfile_fd == -1)
					continue;

			}
			if (pipe_index[0] > -1)
			{
				// There is at least one pipe in the input string.
				child = fork();
				if (child)	// Parent process.
				{
					// Reopen tty as fd0/stdin and fd1/stdout
					close(0);
					open(tty, O_RDONLY);
					close(1);
					open(tty, O_WRONLY);

					while (id != child)
						id = wait(&status);
				}
				else
				{
					handle_pipe(0, current_pipe_index, tty);
					exit(1);
				}
			}
			else
			{
				if (infile_index > -1)
				{
					infile_fd = open_infile();
					if (infile_fd == -1)
						continue;
				}
				child = fork();
				if (child)	// parent process.
				{
					if (infile_index > -1)
					{	// Reopen tty as stdin.
						close(0);
						open(tty, O_RDONLY);
					}
					if ((outfile_index > -1) || (append_index > -1))
					{	// Reopen tty as stdout.
						close(1);
						fd = open(tty, O_WRONLY);
						if (fd == -1)
							printf("Error opening stdout back up... so this message will never be displayed to the screen...");
						else if (fd != 1)
						{
							dup2(fd, 1);
							close(fd);
						}
					}

					wait(&status);
				}	// Child Process.
				else
				{
//printf("exec(%s)\n", input);
					exec(input);
					// If exec fails then exit the child process.
					exit(-1);
				}
			}
				
		}
		else
		{
			child = fork();
			if (child)
				wait(child);
			else
			{
				exec(input);
				// If exec fails then exit the child process.
				exit(-1);
			}
		}
	}
}

int find_special_characters()
{
	int value, i;
	int reverse_pipe_index[MAX_NUM_PIPES];
	pipe_index_counter = 0;

	value = 0;

	for (i = 0; ((input[i] != '\n') && (input[i] != 0)); i++)//while ((input[i] != '\n') && (input[i] != 0))
	{
		switch (input[i])
		{
			case '|' :	reverse_pipe_index[pipe_index_counter] = i;
						pipe_index_counter++;	// This index controls the location in the pipe_index[] array.
												// It also represents the number of pipes identified.
						value = 1;
						current_pipe_index = 0;	// This variable represents the current pipe.
						break;
			case '<' :	infile_index = i;
						value = 1;
						break;
			case '>' :	i++;
						if (input[i] == '>')
							append_index = i - 1;
						else
							outfile_index = i - 1;
						value = 1;
						break;
		}

		// i++;
	}

	i = 0;

	// 	Store the pipe indexes in the reverse order as read in from input, pipe_index[] will 
	//		hold pipes read in from right to left.
	while (pipe_index_counter > 0)
	{
		pipe_index[i] = reverse_pipe_index[pipe_index_counter-1];
// printf("pipe_index[%d] = %d\n", i, pipe_index[i]);
		i++;
		pipe_index_counter--;
	}

	return value;
}

int reset_input ()
{
	int i;

	i = 0;
	pipe_index_counter = 0;
	current_pipe_index = -1;

	for (i = 0; i < MAX_NUM_PIPES; i++)
		pipe_index[i] = -1;

	for (i = 0; i < MAX_FILENAME_LEN; i++)
	{
		infilename[i] = 0;
		outfilename[i] = 0;
		appendfilename[i] = 0;
	}

	for (i = 0; i < MAX_INPUT_LEN; i++)
		input[i] = 0;

	infile_index = outfile_index = append_index = -1;

	return 1;
}

// Set infilename[] with the name of the file to recieve input from.
int get_infilename()
{
	int value, offset;
	int input_i, infile_j;
	
	value = input_i = infile_j = 0;

	while (input_i < infile_index)
	{
		switch (input[input_i])
		{
			case ' '  :	
			case '\t' :	
			case '\r' :	
			case '\n' :	break;
			default   :	infilename[infile_j] = input[input_i];
						infile_j++;
						value = 1;
						break;
		}

		input_i++;
	}

	// Shift input buffer to the left to get rid of the input file name and '>' symbol.
	offset = infile_index + 1;

	for (input_i = 0; (input[input_i] == ' ' || input[input_i] == '\t'); input_i++)
		offset++;
	offset++;

	infile_j = offset;
	for (input_i = 0; ((input[input_i] != 0) && (input[input_i] != '\n')); input_i++)
		input[input_i] = input[infile_j++];

	find_special_characters();

	return value;
}

// Set outfilename[] with the name of the file to send output to.
int get_outfilename()
{
	int value;
	int input_i, outfile_j;
	
	value 	  = 0;
	input_i   = outfile_index + 1;
	outfile_j = 0;

	while (input[input_i] != 0)
	{
		switch (input[input_i])
		{
			case ' '  :	
			case '\t' :	break;
			default   :	outfilename[outfile_j] = input[input_i];
						outfile_j++;
						value = 1;
						break;
		}

		input_i++;
	}

	return value;
}

// Set appendfilename[] with the name of the file to append to.
int get_appendfilename()
{
	int value;
	int input_i, appendfile_j;
	
	value 	  = 0;
	input_i   = append_index + 2;
	appendfile_j = 0;

	while (input[input_i] != 0)
	{
		switch (input[input_i])
		{
			case ' '  :	
			case '\t' :	break;
			default   :	appendfilename[appendfile_j] = input[input_i];
						appendfile_j++;
						value = 1;
						break;
		}

		input_i++;
	}

	return value;
}

// Close the Get-Input-From-Keyboard FD and open the given filename in its place with read-only permissions.
int open_infile()
{
	int infile_fd, i;

	close(0);
	infile_fd = open(infilename, O_RDONLY);

	if (infile_fd == -1)
		printf("Unable to open %s.\n", infilename);
	else if (infile_fd != 0)
	{
		dup2(infile_fd, 0);
		close(infile_fd);
	}

	return infile_fd;
}

// Close the Write-to-Screen FD and open the given filename in its place with write-only permissions.
int open_outfile()
{
	int outfile_fd;

	close(1);

	outfile_fd = open(outfilename, O_WRONLY | O_CREAT | O_TRUNC);
	if (outfile_fd == -1)
	{
		printf("Error opening stdout back up... so this message will never be displayed to the screen...");
	}
	else if (outfile_fd != 1)
	{
		dup2(outfile_fd, 1);
		close(outfile_fd);
	}

	input[outfile_index-1] = 0;

	return outfile_fd;
}

// Close the Write-to-Screen FD and open the given filename in its place with append permissions.
int open_appendfile()
{
	int appendfile_fd;

	close(1);
// this isnt opening correctly for some reason...
	appendfile_fd = open(appendfilename, O_WRONLY | O_CREAT | O_APPEND);
	if (appendfile_fd == -1)
	{
		printf("Unable to open %s.\n", appendfilename);
	}
	else if (appendfile_fd != 1)
	{
		dup2(appendfile_fd, 1);
		close(appendfile_fd);
	}

	input[append_index-1] = 0;

	return appendfile_fd;
}

// Recursive function for setting up process' with pipes.
int handle_pipe(int has_pipe_output, int current_index, int tty/*, int first_child*/)
{
	int pd[2];
	int child, i, j, fd, id, status;
	char cmd[MAX_INPUT_LEN];

	id = -1;

	if (pipe_index[current_index] != -1)
	{
		// Create a pipe for both read and write.
		pipe(pd);

		// Setup fd 0.
		close(0);
		dup2(pd[0], 0);
		close(pd[0]);
	}
	else
	{
		if (infile_index > -1)
		{
			// Redirect fd0 to input file instead of keyboard.
			open_infile();
		}
	}
	
	child = fork();
	// if (has_pipe_output == 0)
	// {
	// 	first_child = child;
	// }
	if (child)	// parent process
	{
		if (pipe_index[current_index] != -1)
		{
			// Open fd0 as stdin, fd1 as pipe.
			close(1);
			dup2(pd[1], 1);
			close(pd[1]);
			close(0);
			open(tty, O_RDONLY);

			child = handle_pipe(1, (current_index+1), tty);
		}

		if (has_pipe_output == 0)
		{
			while (id != child)
				id = wait(&status);
		}
	}
	else		// child process
	{
		if (pipe_index[current_index] != -1)
			close(pd[1]);

// if (pipe_index[current_index] != -1)
// {fd = dup(1);
// close(1);
// open(tty, O_WRONLY);}
// printf("pipe_index[%d] = %d\n", current_index, pipe_index[current_index]);
// if (pipe_index[current_index] != -1)
// {close(1);
// dup2(fd, 1);
// close(fd);}
		j = 0;
		for (i = (pipe_index[current_index] + 1); ((input[i] != '\n') && (input[i] != 0) && (input[i] != '|')); i++)
		{
			if (j == 0)
			{
				switch (input[i])
				{
					case ' ' :	
					case '\t':	break;
					default  :	cmd[j++] = input[i];
								break;
				}
			}
			else
			{
				cmd[j++] = input[i];
			}
		}
		cmd[j++] = 0;

// if (pipe_index[current_index] != -1)
// {fd = dup(1);
// close(1);
// open(tty, O_WRONLY);}
// printf("cmd = \"%s\"\n", cmd);
// if (pipe_index[current_index] != -1)
// {close(1);
// dup2(fd, 1);
// close(fd);}
		exec(cmd);

		// If exec fails then kill the child process.
		exit(-1);
	}

	return child;
}
