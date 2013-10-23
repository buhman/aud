#pragma once

#include <alsa/asoundlib.h>

int
aud_snd_pcm_open(char *name,
		 snd_pcm_t **pcm);

int
aud_snd_pcm_init(snd_pcm_t *pcm,
		 snd_pcm_format_t *format,
		 unsigned int *rate,
		 unsigned int *channels);

int
aud_snd_pcm_start(snd_pcm_t *pcm,
		  snd_async_handler_t **handler,
		  void *data);
