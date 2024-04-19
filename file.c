/*****************************************
*** it should work on 1 blk_size
*** invariants are head tail and in between
***
***
*****************************************/

//https://stackoverflow.com/questions/1716296/why-does-printf-not-flush-after-the-call-unless-a-newline-is-in-the-format-strin

#include<sys/ioctl.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>		// For terminal manipulation

typedef struct Buffer
{
  char *data;
  off_t index;
  struct Buffer *next;
  struct Buffer *prev;
} Buffer;

typedef struct File
{
  int fd;
  char *filename;
  blksize_t st_blksize;

  struct Buffer *current;
  struct Buffer *head;
  struct Buffer *tail;

} File;
int getch ();
Buffer *read_buffer (File * f, off_t index);
void move_left (File *);
void move_right (File *);
File *initialize (char *);
void test (File * f);

int
main (int argc, char **argv)
{
  if(argc < 2){
    puts("bad usage");
    exit(EXIT_FAILURE);
  }
  File *f = initialize (argv[1]);
  f->head = f->tail = f->current = read_buffer (f, 0);
  test (f);
  return 0;

  const char *clear_command = "clear";	// Replace with "cls" for Windows
  system (clear_command);
  printf ("%s\n", f->current->data);



  int key;
  while (1)
    {
      key = getch ();

      switch (key)
	{
	case 'A':
	  printf ("Up arrow pressed\n");
	  break;
	case 'B':
	  printf ("Down arrow pressed\n");
	  break;
	case 'C':		//righ
	  move_right (f);
	  system (clear_command);
	  printf ("%s\n", f->current->data);
	  break;
	case 'D':		//left
	  move_left (f);
	  system (clear_command);
	  printf ("%s\n", f->current->data);
	  break;
	default:
	  printf ("Unknown key: %d\n", key);
	}
    }
  free (f->filename);


  return 0;
}


File *
initialize (char *filename)
{
  struct stat finfo;
  int fd;

  if (!(access (filename, F_OK) == 0))
    {
      perror ("file doesn't exist");
      exit (EXIT_FAILURE);
    }

  if (!(fd = open (filename, O_RDONLY)))
    {
      perror (" ");
    }
  if (stat (filename, &finfo) == -1)
    {
      perror ("");
      exit (EXIT_FAILURE);
    }
  File *f = (File *) malloc (sizeof (File));
  f->filename = strdup (filename);
  f->fd = fd;
  f->st_blksize = finfo.st_blksize;
  //f->st_blksize = 5;
  f->head = f->tail = f->current = NULL;
  return f;
}


Buffer *
read_buffer (File * f, off_t index)
{

  Buffer *buff = (Buffer *) malloc (sizeof (Buffer));
  buff->data = NULL;
  buff->index = index;
  buff->next = NULL;
  buff->prev = NULL;
  if (lseek (f->fd, buff->index, SEEK_SET) == -1)
    {
      perror ("");
      exit (EXIT_FAILURE);
    }

  buff->data = (char *) malloc (f->st_blksize * sizeof (char));
  int result = read (f->fd, buff->data, f->st_blksize);
  if (result == -1)
    {
      perror ("");
      exit (EXIT_FAILURE);
    }
  if (result == 0)
    {
      buff->index = -1;
    }
  return buff;
}


void
move_right (File * f)
{
  if (f->current->next != NULL)
    {
      f->current = f->current->next;
      return;
    }

  f->current->next = read_buffer (f, f->current->index + f->st_blksize);
  if (f->current->next->index == -1)
    {
      free (f->current->next->data);
      free (f->current->next);
      f->tail = f->current;
      f->current->next = f->head;
      f->head->prev = f->current;
      f->current = f->current->next;
      return;
    }

  f->current->next->prev = f->current;
  if (f->current == f->tail)
    f->tail = f->current->next;
  f->current = f->current->next;
  return;
}

void
move_left (File * f)
{

  if (f->current->prev != NULL)
    {
      f->current = f->current->prev;
      return;
    }
  else if (f->current == f->head)
    {
      return;
    }

  f->current->prev = read_buffer (f, f->current->index - f->st_blksize);

  f->current->prev->next = f->current;
  f->current = f->current->prev;
  return;
}

char* read_file_name(){
  char* filename;filename = (char *) malloc(256 * sizeof (char));
  fgets (filename, 256, stdin);
  int len = strlen(filename);
  if (filename[len] == '\0')
  filename[len - 1] = '\0';
  return filename;
}

void
test (File * f)
{
  printf ("%s", f->head->data);
  fflush(stdout);
  move_right (f);

  while (f->current != f->head)
    {
      printf ("%s", f->current->data);
      fflush(stdout);
      move_right (f);
    }
  return;
}


int
getch ()
{
  int ch;
  struct termios oldt;
  struct termios newt;

  tcgetattr (STDIN_FILENO, &oldt);	// Get current terminal settings
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);	// Disable buffering and echo
  tcsetattr (STDIN_FILENO, TCSANOW, &newt);	// Set new terminal settings

  ch = getchar ();

  if (ch == 27)
    {				// Check for escape sequence (27 is ASCII for Esc)
      ch = getchar ();		// Discard following '['
      ch = getchar ();		// Get the actual arrow key code
    }

  tcsetattr (STDIN_FILENO, TCSANOW, &oldt);	// Restore old terminal settings

  return ch;
}
