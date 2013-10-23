#include <termios.h>
#include <unistd.h>
#include <stdio.h>

int
aud_term_unbuffered()
{
  int err;
  struct termios t;

  err = tcgetattr(0, &t);
  if (err < 0) {
    perror("tcgetattr(): ");
    return err;
  }

  t.c_lflag &= ~ICANON;
  t.c_lflag &= ~ECHO;
  t.c_cc[VMIN] = 1;
  t.c_cc[VTIME] = 0;

  err = tcsetattr(0, TCSANOW, &t);
  if (err < 0) {
    perror("tcsetattr() :");
    return err;
  }
  
  return 0;
}
