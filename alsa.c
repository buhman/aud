#include <stdint.h>
#include <errno.h>
#include <strings.h>
#include <alsa/asoundlib.h>

static void
set_hw_params(snd_pcm_t *handle,		    
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

static void
set_sw_params(snd_pcm_t *handle,
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


static int
stream_recovery(snd_pcm_t *handle, int err)
{
  if (err == -EPIPE) {
    err = snd_pcm_prepare(handle);
    if (err < 0)
      fprintf(stderr, "snd_pcm_prepare() : %s\n", snd_strerror(err));
    return 0;
  }
  else if (err == -ESTRPIPE) {
    while ((err = snd_pcm_resume(handle)) == -EAGAIN)
      sleep(1); /* wait until the suspend flag is released */
    if (err < 0) {
      err = snd_pcm_prepare(handle);
      if (err < 0)
	fprintf(stderr, "snd_pcm_prepare() : %s\n", snd_strerror(err));
    }
    return 0;
  }
  return err;
}

int
aud_write_buf(snd_pcm_t *handle,
	      void *buf,
	      int frames)
{
  int alsa_err;
  
  if ((alsa_err = snd_pcm_wait(handle, 1000)) < 0) {
    fprintf(stderr, "\nsnd_pcm_wait() : %s\n", strerror(errno));
    return -1;
  }

  if ((alsa_err = snd_pcm_writei(handle, buf, frames)) < 0) {
    fprintf(stderr, "\nsnd_pcm_writei(): %s\n", snd_strerror(alsa_err));
    if (alsa_err != -EAGAIN &&
	(alsa_err = stream_recovery(handle, alsa_err)) < 0) {
      return -1;
    }

    printf("stream_recovery() : %s\n", snd_strerror(alsa_err));
  }

  return 0;
}

int
aud_prepare_handle(snd_pcm_t *handle,
		   int sample_rate,
		   int channels,
		   int sample_size)
{
  int err;
  int sample_format;

  switch (sample_size) {
  case 16:
    sample_format = SND_PCM_FORMAT_S16_LE;
    break;
  case 24:
    sample_format = SND_PCM_FORMAT_S24_LE;
    break;
  default:
    fprintf(stderr, "BUG: untested/unsupported sample size: %d\n",
	    sample_size);
    break;
  }
		  
  set_hw_params(handle,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		sample_format,
		sample_rate,
		channels);

  set_sw_params(handle,
		4096);

  if ((err = snd_pcm_prepare(handle)) < 0) {
    fprintf(stderr, "snd_pcm_prepare() : %s\n", snd_strerror(err));
    return -1;
  }

  printf("pcm state: %s\n", snd_pcm_state_name(snd_pcm_state(handle)));

  return 0;
}
