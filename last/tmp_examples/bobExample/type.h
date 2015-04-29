#define SSIZE 512

#define NMINODES			  20
#define NMOUNT				   4
#define NFD					  10
#define NOFT				  20
#define NPIPES				  20
#define PSIZE				 256

// MTX's open flag
#define READ				   0
#define WRITE				   1
#define RW					   2
#define APPEND				   3
#define READ_PIPE			   4
#define WRITE_PIPE			   5

// UNIX <fcntl.h> constants: <asm/fcntl.h> in Linux
#define O_RDONLY			   00
#define O_WRONLY			   01
#define O_RDWR				   02
#define O_CREAT				 0100// not fcntl
#define O_TRUNC				01000// not fcntl
#define O_APPEND			02000

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef unsigned short ushort;
typedef unsigned long  ulong;

typedef struct ext2_super_block
{
	u32	s_inodes_count;			// Inodes count
	u32	s_blocks_count;			// Blocks count
	u32	s_r_blocks_count;		// Reserved blocks count
	u32	s_free_blocks_count;	// Free blocks count
	u32	s_free_inodes_count;	// Free inodes count
	u32	s_first_data_block;		// First Data Block
	u32	s_log_block_size;		// Block size
	u32	s_log_frag_size;		// Fragment size
	u32	s_blocks_per_group;		// # Blocks per group
	u32	s_frags_per_group;		// # Fragments per group
	u32	s_inodes_per_group;		// # Inodes per group
	u32	s_mtime;				// Mount time
	u32	s_wtime;				// Write time
	u16	s_mnt_count;			// Mount count
	u16	s_max_mnt_count;		// Maximal mount count
	u16	s_magic;				// Magic signature
	u16	s_state;				// File system state
	u16	s_errors;				// Behaviour when detecting errors
} SUPER;

typedef struct ext2_group_desc
{
	u32	bg_block_bitmap;		// Blocks bitmap block
	u32	bg_inode_bitmap;		// Inodes bitmap block
	u32	bg_inode_table;			// Inodes table block
	u16	bg_free_blocks_count;	// Free blocks count
	u16	bg_free_inodes_count;	// Free inodes count
	u16	bg_used_dirs_count;		// Directories count
	u16	bg_pad;
	u32	bg_reserved[3];
} GD;

typedef struct ext2_inode
{
	u16	i_mode;					// File mode
	u16	i_uid;					// Owner Uid
	u32	i_size;					// Size in bytes
	u32	i_atime;				// Access time
	u32	i_ctime;				// Creation time
	u32	i_mtime;				// Modification time
	u32	i_dtime;				// Deletion Time
	u16	i_gid;					// Group Id
	u16	i_links_count;			// Links count
	u32	i_blocks;				// Blocks count
	u32	i_flags;				// File flags
	u32 dummy;
	u32	i_block[15];			// Pointers to blocks
	u32 pad[7];					// inode size MUST be 128 bytes
} INODE;

typedef struct ext2_dir_entry_2
{
	u32	inode;					// Inode number
	u16	rec_len;				// Directory entry length
	u8	name_len;				// Name length
	u8	file_type;
	char name[255];				// File name
} DIR;

typedef struct Oft
{
	int		mode;
	int		refCount;
	struct	Minode *inodeptr;
	struct	pipe *pipe_ptr;
	long	offset;
	char	name[32];
} OFT;

typedef struct Minode
{		
	INODE	INODE; 
	ushort	dev;
	ushort	ino;
	ushort	refCount;
	ushort	dirty;
	ushort	mounted;
	struct	Mount *mountptr;
	char	name[32];
} MINODE;


typedef struct Mount
{
	int		ninodes;
	int		nblocks;
	int		dev;
	int		busy;
	struct	Minode *mounted_inode;
	char	name[32]; 
	char	mount_name[32];
} MOUNT;

typedef struct pipe
{
	char	buf[PSIZE];
	int		head;
	int		tail;
	int		hasData;
	int		hasRoom;
	OFT		*read;
	OFT		*write;
	int		busy;
}PIPE;


typedef struct stat
{
	ushort	st_dev;				// major/minor device number
	ushort	st_ino;				// i-node number
	ushort	st_mode;			// file mode, protection bits, etc.
	ushort	st_nlink;			// # links; TEMPORARY HACK: should be nlink_t
	ushort	st_uid;				// uid of the file's owner
	ushort	st_gid;				// gid; TEMPORARY HACK: should be gid_t
	ushort	st_rdev;
	long	st_size;			// file size
	long	st_atime;			// time of last access
	long	st_mtime;			// time of last data modification
	long	st_ctime;			// time of last file status change
} STAT;

typedef struct stty TTY;

typedef struct proc
{
	struct	proc *next;
	int		*saved_ksp;
	int		uss,usp;			// at 4,6: saved Umode SS, SP

	int		uid;
	int		gid;
	int		pid;
	int		ppid;
	int		status;
	int		pri;				// scheduling priority
	char	name[16];			// name string
	int		event;				// sleep event

	int		exitValue;			// enforce exit code in this MTX

	OFT		*fd[NFD];			// file descriptors
	MINODE	*cwd;				// CWD
	char	tty[16];			// opened /dev/ttyXX
	int		time;
	int		signal;
	int		sig[10];
	struct	semaphore *sem;		// ptr to semaphore currently BLOCKed on
	int		pause;
	ushort	segment;
	ushort	tsize;
	ushort	dsize;
	ushort	size;
	int		kstack[SSIZE];
}PROC;        

// Default dir and regulsr file modes
#define DIR_MODE		0040777 
#define FILE_MODE		0100644
#define SUPER_MAGIC		 0xEF53
#define SUPER_USER			  0



/* * * * * * * * * * * * * * * * * *
 *             ucode.c             *
 * * * * * * * * * * * * * * * * * */
int getc();
int getline(char *s);
int gets(char *s);
int getpid();
int getppid();
int getpri();
int chpri(int value);
int getuid();
int chuid(int uid, int gid);
int tswitch();
int fork();
int exec(char *cmd_line);
int wait(int *status);
int vfork();
int thread(int fn, int stack, int flag, int ptr);
int mutex_creat();
int mutex_lock(int *m);
int mutex_unlock(int *m);
int mutex_destroy(int *m);
int mkdir(char *name);
int rmdir(char *name);
int creat(char *filename);
int link(char *oldfile, char *newfile);
int unlink(char *file);
int symlink(char *oldfile, char *newfile);
int readlink(char *file, char *linkname);
int chdir(char *name);
int getcwd(char *cwdname);
int stat(char *filename, struct stat *sPtr);
int fstat(int fd, struct stat *sptr);
int open(char *file, int flag);
int close(int fd);
u32 lseek(int fd, u32 offset, int ww);
int read(int fd, char *buf, int nbytes);
int write(int fd, char *buf, int nbytes);
int pipe(int *pd);
int chmod(char *file, u16 mode);
int chown(char *file, int uid);
int touch(char *filename);
int fixtty(char *tty);
int gettty(char *tty);
int dup(int fd);
int dup2(int fd, int gd);
int ps(char *y);
int mount(char *dev, char **mpt);
int umount(char *dev);
int getSector(u32 sector, char *ubuf, u16 nsector);
int do_cmd(u16 cmd, u16 value);
int kill(int sig, int pid);
int signal(u16 sig, u16 catcher);
int pause(u16 t);
int itimer(u16 t);
int send(char *msg, int pid);
int recv(char *msg);
int do_texit();
int tjoin(int n);
int texit(int v);
int khits(int *requests, int *hits);
int setcolor(int color);
int sync();
int thinit();
int exit(int value);
int pwd();