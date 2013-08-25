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

int
aud_write_buf(snd_pcm_t *handle,
	      void *buf,
	      int frames)
{
  int alsa_err;
  
  if ((alsa_err = snd_pcm_wait(handle, 1000)) < 0) {
    fprintf(stderr, "snd_pcm_wait() : %s\n", strerror(errno));
    return -1;
  }

  if ((alsa_err = snd_pcm_writei(handle, buf, frames)) < 0) {
    fprintf(stderr, "%s\n", snd_strerror(alsa_err));
    return -1;
  }

  return 0;
}

int
aud_prepare_handle(snd_pcm_t *handle,
		   int sample_rate,
		   int channels)
{
  int err;
  
  set_hw_params(handle,
		SND_PCM_ACCESS_RW_INTERLEAVED,
		SND_PCM_FORMAT_S16_LE,
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
