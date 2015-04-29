#include "ucode.c"

#define BUFSIZE 256
#define FILE_NAME_LENGTH 32

main(int argc, char *argv[])
{
	char filename[FILE_NAME_LENGTH], buf[BUFSIZE];
	int file_fd, bytes_read, i;

	// Check if the number of arguments is correct.
	if (argc < 1)
	{
		// There are not enough arguments.
		printf("Invalid arguments.  Expected format:\n\n");
		printf("cat [filename]\n\n");
		printf("If filename is optionally left off then input\n is read from stdin until ctrl-D is pressed.\n\n");
		return;
	}
	else if (argc > 2)
	{
		// There are too many arguments.
		printf("Invalid arguments.  Expected format:\n\n");
		printf("cat [filename]\n\n");
		return;
	}

	if (argc == 1)
	{
		// cat from stdin.
		while (1)
		{
			if (gets(buf) == 0)
				return;

			// Clear out buf.
			for (i = 0; i < BUFSIZE; i++)
			{
				buf[i] = 0;
			}
		}
	}
	else
	{
		bytes_read = BUFSIZE;

		strcpy(filename, argv[1]);

		file_fd = open(filename, O_RDONLY);

		if (file_fd == -1)
		{
			// This file does not exist in the specified directory.
			printf("Invalid arguments.  Expected format:\n\n");
			printf("cat filename\n\n");
			return;
		}

		while (bytes_read == BUFSIZE)
		{
			bytes_read = read(file_fd, buf, BUFSIZE);

			i = 0;

			while (i < bytes_read)
			{
				if ((buf[i] != '\r') && (buf[i] != 0))
					putc(buf[i]);
				i++;
			}
		}

		close(file_fd);
	}

	close(0);
	close(1);
}