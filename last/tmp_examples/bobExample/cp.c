#include "ucode.c"

#define BUFSIZE 1024
#define FILE_NAME_LENGTH 32
#define LINE_SIZE 128

main(int argc, char *argv[])
{
	char old_filename[FILE_NAME_LENGTH], copy_filename[FILE_NAME_LENGTH];
	char buf[BUFSIZE];
	int old_file_fd, i, total_bytes_written;
	int copy_file_fd, bytes_read, bytes_written;

	printf("\n^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^\n");
	printf("^v                  cp                   v^\n");
	printf("^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^\n\n");

	// Check if the number of arguments is correct.
	if ((argc < 3) || (argc > 3))
	{
		// There are not enough arguments.
		printf("Invalid arguments.  Expected format:\n\n");
		printf("cp old_filename copy_filename\n\n");
		return;
	}

	total_bytes_written = 0;
	bytes_read = BUFSIZE;

	strcpy(old_filename, argv[1]);
	strcpy(copy_filename, argv[2]);

	old_file_fd = open(old_filename, O_RDONLY);

	if (old_file_fd == -1)
	{
		// This file does not exist in the specified directory.
		printf("Invalid first parameter.  Expected format:\n\n");
		printf("cp old_filename copy_filename\n\n");
		return;
	}

	// Create and open destination file for writing.
	copy_file_fd = open(copy_filename, O_WRONLY | O_CREAT);

	if (copy_file_fd == -1)
	{
		// This file does not exist in the specified directory.
		printf("Error! Could not open destination file for write mode.\n");
		return;
	}

	printf("Copying %s to %s\n", old_filename, copy_filename);

	bytes_read = read(old_file_fd, buf, BUFSIZE);

	while (bytes_read > 0)
	{
		// Write read in byte to destination file.
		bytes_written = write(copy_file_fd, buf, bytes_read);
		total_bytes_written += bytes_written;
		if (bytes_written == -1)
			printf("Error writing to %s\n", copy_filename);

		bytes_read = read(old_file_fd, buf, BUFSIZE);
	}

	printf("Finished copying %dbytes to %s\n", total_bytes_written, copy_filename);

	close(old_file_fd);
	close(copy_file_fd);
}