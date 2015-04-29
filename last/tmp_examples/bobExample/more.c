#include "ucode.c"

#define BUFSIZE 1
#define FILE_NAME_LENGTH 32
#define SCREEN_HEIGHT 23
#define SCREEN_WIDTH 70

main(int argc, char *argv[])
{
	char filename[FILE_NAME_LENGTH], buf[BUFSIZE], tty[64];
	int file_fd, bytes_read, i, print_height, tmp_fd;
	int input;

	// printf("\n^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^\n");
	// printf("^v                 more                  v^\n");
	// printf("^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^\n\n");
	
	// Check if the number of arguments is correct.
	if (argc < 1)
	{
		// There are not enough arguments.
		printf("Too few arguments.  Expected format:\n\n");
		printf("more filename\n\n");
		return;
	}
	else if (argc > 2)
	{
		// There are too many arguments.
		printf("Too many arguments.  Expected format:\n\n");
		printf("more filename\n\n");
		return;
	}

	bytes_read = BUFSIZE;
	print_height = 0;

	if (argc > 1)
	{
		strcpy(filename, argv[1]);
		file_fd = open(filename, O_RDONLY);
	}
	else if (argc == 1)
		file_fd = 0;

	if (file_fd == -1)
	{
		// This file does not exist in the specified directory.
		printf("Unable to open %s.  Expected format:\n\n", filename);
		printf("more filename\n\n");
		return;
	}
	
	i = 0;

	while (bytes_read == BUFSIZE)
	{
		bytes_read = read(file_fd, buf, BUFSIZE);

		if (bytes_read == 0)
			break;

		if (buf[0] == '\r')
			continue;

		putc(buf[0]);

		if ((buf[0] == '\n') || (i == 70))
		{
			if (i == 70)
				printf("\n");
			i = 0;
			print_height++;
			if (print_height >= SCREEN_HEIGHT)
			{
				// Get a character from the user before printing next line.
				if (argc == 1)
				{
					// stdin needs to come from user terminal.
					tmp_fd = dup(0);
					close(0);
					gettty(tty);
					open(tty, O_RDONLY);
				}
				input = getc();
				if (argc == 1)
				{
					// fd 0 needs to go back to whatever it was.
					close(0);
					dup(tmp_fd);
					close(tmp_fd);
				}
				if (input == -1)		// Pressing Ctrl-D terminates.
					break;
				else if (input == ' ')	// Pressing space bar moves the screen 1 full page.
					print_height = 0;
			}
		}
		i++;
	}

	if (argc > 1)
		close(file_fd);
}