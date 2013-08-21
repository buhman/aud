#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>

void* buf;

void set_hw_params(snd_pcm_t *handle,		    
		   snd_pcm_access_t access_type,
		   snd_pcm_format_t sample_format,
		   unsigned int sample_rate,
		   unsigned int channels)
{
  snd_pcm_hw_params_t *hw_params;
  
  snd_pcm_hw_params_malloc(&hw_params);

  snd_pcm_hw_params_any(handle, hw_params);

  snd_pcm_hw_params_set_access(handle,
			       hw_params,
			       access_type);
  snd_pcm_hw_params_set_format(handle,
			       hw_params,
			       sample_format);

  snd_pcm_hw_params_set_rate(handle,
			     hw_params,
			     sample_rate,
			     0);

  snd_pcm_hw_params_set_channels(handle,
				 hw_params,
				 channels);

  snd_pcm_hw_params(handle, hw_params);

  snd_pcm_hw_params_free(hw_params);
}

void set_sw_params(snd_pcm_t *handle,
		   snd_pcm_uframes_t bufsize)
{
  snd_pcm_sw_params_t *sw_params;
  
  snd_pcm_sw_params_malloc(&sw_params);

  snd_pcm_sw_params_current(handle, sw_params);

  snd_pcm_sw_params_set_avail_min(handle, sw_params, bufsize);

  snd_pcm_sw_params_set_start_threshold(handle, sw_params, 0U);

  snd_pcm_sw_params(handle, sw_params);
  
  snd_pcm_sw_params_free(sw_params);
}

int playback(snd_pcm_t *handle, uint16_t frames, int fd) {
  //printf("playback(): %d\n", frames);

  int bytes = read(fd, buf, sizeof(uint16_t) * 4096);

  int err;

  if ((err = snd_pcm_writei(handle, buf, 4096)) < 0) {
    printf("snd_pcm_writei(): %d : %s", err, snd_strerror(err));
  }

  return err;
}

int main(int argc, char **argv)
{
  int fd = open("foo.raw", O_RDONLY);
  buf = malloc(sizeof(uint16_t) * 4096);
  
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
      fprintf(stderr, "snd_pcm_avail_update() : %d : %s\n", strerror(errno));
      break;
    }

    if (playback(handle, frames, fd) != 4096) {
      //break;
    }
  }

  snd_pcm_close(handle);
}
