#include "ucode.c"

char *tty;

main(int argc, char *argv[])	// invoked by exec("login /dev/ttyxx") .
{
	int stdin, stdout, stderr, uid_int, gid_int;
	char login[64], password[64], uid[32], gid[32], homedir[32], program[32];
	
	tty =  argv[1];

//	1.	login process may run on different terminals.
	close(0);
	close(1);
	close(2);

//	2.	Open its own tty (passed in by INIT) as stdin, stdout, stderr.
	stdin  = open(tty, O_RDONLY);
	stdout = open(tty, O_WRONLY);
	stderr = open(tty, O_RDWR);

//	3.	Store tty string in PROC.tty[] for putc().
	settty(tty);

//	NOW we can use printf, which calls putc() to our tty.
	printf("\n^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^\n");
	printf("^v                login                  v^\n");
	printf("^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^v^\n\n");
	
	printf("Login : open %s as stdin, stdout, stderr\n", tty);


// Ignore Control-C interrupts so that .
// Control-C KILLs other procs on this tty but not the main sh.
	signal(2,1);

	while(1)
	{
//	1. Show login: to stdout.
		printf("login:    ");
//	2. Read user name from stdin.
		gets(login);
		if (strcmp(login, "") == 0)
		{
			strcpy(login, "root");
			strcpy(password, "12345");
		}
		else
		{	
//	3. Show passwd:
			printf("password: ");
//	4. Read user passwd.
			gets(password);
		}

//	5. Verify user name and passwd from /etc/passwd file.
		if (verifyLogin(login, password, uid, gid, homedir, program) == 1)
		{
//  6. Login was successful.
//		setuid to user uid, chdir to user HOME directory, exec to the program in users's account.
			printf("Dun un dun ta! Login as %s Successful!\n", login);
			uid_int = atoi(uid);
			gid_int = atoi(gid);
			chuid(uid_int, gid_int);

			chdir(homedir);

			exec(program);
		}
//	else
		printf("Womp Womp. Login failed, try again\n");
	}
}

int verifyLogin(char *login, char *password, char *userID, char *groupID, char *homeDirectory, char *exec_program)
{
	// Verify user name and passwd from /etc/passwd file.
	int etc_passwd_fd, psswd_file_type;
	char etc_passwd_file[4096];
	char cpy_passwd_file[4096];
	char etc_passwd_line[128];
	int i, j, read_size;
	char uname[32],
		 psswd[32],
		 gid[32],
		 uid[32],
		 fullname[32],
		 homedir[32],
		 program[32];

	char *tmp;

	psswd_file_type = 0;

	etc_passwd_fd = open("/etc/passwd", O_RDONLY);
	read_size = read(etc_passwd_fd, &etc_passwd_file[0], 4096);
	
	// Make a copy of input to use strtok on and only mangle the copy.
	strcpy(cpy_passwd_file, etc_passwd_file);

	tmp = strtok(cpy_passwd_file, "\n");
	if (tmp != 0)
		strcpy(etc_passwd_line, tmp);
	
	while (tmp != 0)
	{
		j = 0;
		i = 0;
		psswd_file_type = 0;
		clear_str(uname, 32);
		clear_str(psswd, 32);
		clear_str(gid, 32);
		clear_str(uid, 32);
		clear_str(fullname, 32);
		clear_str(homedir, 32);
		clear_str(program, 32);

		while ((etc_passwd_line[i] != 0) && (etc_passwd_line[i] != '\n'))
		{
			if (etc_passwd_line[i] == ':')
			{
				psswd_file_type++;
				i++;
				j = 0;
				continue;
			}
			switch (psswd_file_type)
			{
				case 0:
					uname[j] = etc_passwd_line[i];
					break;
				case 1:
					psswd[j] = etc_passwd_line[i];
					break;
				case 2:
					gid[j] = etc_passwd_line[i];
					break;
				case 3:
					uid[j] = etc_passwd_line[i];
					break;
				case 4:
					fullname[j] = etc_passwd_line[i];
					break;
				case 5:
					homedir[j] = etc_passwd_line[i];
					break;
				case 6:
					program[j] = etc_passwd_line[i];
					break;
			}
			i++;
			j++;
		}

		if ((strcmp(login, uname) == 0) && (strcmp(password, psswd) == 0))
		{
			// Found a match!
			// Copy uid and homedir to program calling this function.
			strcpy(userID, uid);
			strcpy(groupID, gid);
			strcpy(homeDirectory, homedir);
			strcpy(exec_program, program);
			return 1;
		}

		tmp = strtok(0, "\n");
		strcpy(etc_passwd_line, tmp);
	}

	return 0;
}