#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>

#include "alsa.h"

void* buf;

int main(int argc, char **argv)
{
  int fd;
  if ((fd = open("../foo.raw", O_RDONLY)) == -1) {
    fprintf(stderr, "open() : %s\n", snd_strerror(errno));
    return 1;
  }
  buf = malloc(sizeof(uint16_t) * 32768);
  
  snd_pcm_t *handle;

  snd_pcm_open(&handle,
	       "default",
	       SND_PCM_STREAM_PLAYBACK,
	       0);

  set_hw_params(handle,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		SND_PCM_FORMAT_S16_LE,
		48000,
		1);

  set_sw_params(handle,
		4096);

  int err = snd_pcm_prepare(handle);

  fprintf(stderr, "snd_pcm_prepare() : %d : %s\n", err, snd_strerror(err));

  for (;;) {

    int err;
    unsigned int frames;
    
    if ((err = snd_pcm_wait(handle, 10000)) < 0) {
      fprintf(stderr, "snd_pcm_wait() : %d : %s\n", err, strerror(errno));
      break;
    }

    if ((frames = snd_pcm_avail_update(handle)) < 0) {
      fprintf(stderr, "snd_pcm_avail_update() : %d : %s\n", err, strerror(errno));
      break;
    }

    frames = read(fd, buf, sizeof(uint16_t) * frames) / sizeof(uint16_t);
    if (playback(handle, frames, buf) != frames) {
      break;
    }
    if (frames < 4096) {
      printf("end of file");
      break;
    }
  }

  snd_pcm_close(handle);
}
