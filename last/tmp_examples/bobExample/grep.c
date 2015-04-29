#include "ucode.c"

#define BUFSIZE 1
#define FILE_NAME_LENGTH 32
#define MAX_PATTERN_SIZE 32
#define LINE_SIZE 128

main(int argc, char *argv[])
{
	char filename[FILE_NAME_LENGTH], buf[BUFSIZE];
	char pattern[MAX_PATTERN_SIZE], line[LINE_SIZE];
	int file_fd, bytes_read, i;
	
	// Check if the number of arguments is correct.
	if (argc < 2)
	{
		// There are not enough arguments.
		printf("Invalid arguments.  Expected format:\n\n");
		printf("grep pattern filename\n\n");
		printf("If filename is optionally left off then input\n is read from stdin until ctrl-D is pressed.\n\n");
		return;
	}
	else if (argc > 3)
	{
		// There are too many arguments.
		printf("Invalid arguments.  Expected format:\n\n");
		printf("grep pattern filename\n\n");
		return;
	}

	bytes_read = BUFSIZE;
	strcpy(pattern, argv[1]);

	if (argc == 2)
		file_fd = 0;

	else
	{
		strcpy(filename, argv[2]);
		file_fd = open(filename, O_RDONLY);

		if (file_fd == -1)
		{	// This file does not exist in the specified directory.
			printf("Invalid arguments.  Expected format:\n\n");
			printf("grep pattern filename\n\n");
			printf("If filename is optionally left off then input\nis read from stdin until ctrl-D is pressed.\n\n");
			return;
		}
	}

	i = 0;

	while (bytes_read == BUFSIZE)
	{
		bytes_read = read(file_fd, buf, BUFSIZE);
		// Copy the input char into line.
		line[i] = buf[0];

		i++;

		if ((buf[0] == '\n') || (buf[0] == '\r'))
		{
			line[i-1] = 0;
			while (i < LINE_SIZE)
			{
				line[i] = 0;
				i++;
			}
			
			i = 0;

			if (find_pattern(pattern, line) == 1)
				printf("%s\n", line);
		}
	}

	close(file_fd);
}

int find_pattern(char *pattern, char *line)
{
	int pattern_iterator, line_iterator;
	int possible_pattern_start_index, pattern_end_index;

	pattern_end_index = strlen(pattern) - 1;

	pattern_iterator = line_iterator = 0;
	possible_pattern_start_index = -1;

	while (line[line_iterator] != 0)
	{
		if (line[line_iterator] == pattern[pattern_iterator])
		{
			if (possible_pattern_start_index == -1)
				possible_pattern_start_index = line_iterator;

			if (pattern_iterator == pattern_end_index)
				return 1;	// We have found the pattern in this line.

			pattern_iterator++;
		}
		else
		{
			// Set line_iterator to the character after the one it began to see the pattern in.
			if (pattern_iterator != 0)
				line_iterator -= pattern_iterator;
			// Reset all other index variables.
			possible_pattern_start_index = -1;
			pattern_iterator = 0;
		}

		line_iterator++;
	}

	return 0;
}