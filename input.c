#include <stdio.h>

#include "term.h"

int
aud_input_iterate(int *running)
{
  int err;
  
  {
    err = aud_term_unbuffered();
    if (err < 0) {
      printf("aud_term_unbuffered(): %d\n", err);
      return err;
    }
  }

  while (*running) {

    switch (getchar()) {
    case 27:
      printf("read(): 'ESC");
      switch (getchar()) {	
      case '[':
	printf("[");
	switch(getchar()) {
	case 'D': // left
	  printf("seekleft'\n");
	  break;
	case 'C': // right
	  printf("seekright'\n");
	  break;
	default:
	  continue;
	}
	break; /* [ */
      }
      break; /* ESC */
    }
  }
  
  return 0;
}
