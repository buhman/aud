#include <stdint.h>
#include <alsa/asoundlib.h>

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

int playback(snd_pcm_t *handle, uint16_t frames, void *buf) {

  int err;

  if ((err = snd_pcm_writei(handle, buf, frames)) < 0) {
    printf("snd_pcm_writei(): %d : %s\n", err, snd_strerror(err));
  }

  return err;
}
