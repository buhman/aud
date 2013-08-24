#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>

#include "alsa.h"
#include "flac.h"
#include "vorbis.h"

void* buf;

static snd_pcm_t *handle;

int main(int argc, char **argv)
{
  
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

  if (argc > 1)
    aud_vorbis_play(argv[1], handle);
  else
    fprintf(stderr, "filename required");
  
  snd_pcm_close(handle);
}
