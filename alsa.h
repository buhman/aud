void set_hw_params(snd_pcm_t *handle,		    
		   snd_pcm_access_t access_type,
		   snd_pcm_format_t sample_format,
		   unsigned int sample_rate,
		   unsigned int channels);

void set_sw_params(snd_pcm_t *handle,
		   snd_pcm_uframes_t bufsize);

int playback(snd_pcm_t *handle, uint16_t frames, void *buf);
