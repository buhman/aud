#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>
#include <FLAC/stream_decoder.h>

#include "alsa.h"
#include "flac.h"

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
		2);

  set_sw_params(handle,
		4096);

  int err = snd_pcm_prepare(handle);

  fprintf(stderr, "snd_pcm_prepare() : %d : %s\n", err, snd_strerror(err));

  FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new();
  
  FLAC__StreamDecoderInitStatus status = aud_open_flac("foo.flac",
						       decoder,
						       handle);

  if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
    fprintf(stderr,
	    "FLAC__stream_decoder_init_file() : %s",
	    FLAC__StreamDecoderInitStatusString[status]);
    goto cleanup;
  }

  FLAC__stream_decoder_process_until_end_of_stream(decoder);
  printf(FLAC__StreamDecoderStateString[FLAC__stream_decoder_get_state(decoder)]);

  FLAC__stream_decoder_delete(decoder);
  
 cleanup:
  snd_pcm_close(handle);
}
