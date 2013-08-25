int
aud_write_buf(snd_pcm_t *handle,
	      void *buf,
	      int frames);

void
aud_prepare_handle(snd_pcm_t *handle,
		   int sample_rate,
		   int channels);
