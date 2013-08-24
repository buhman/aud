#pragma once

void
error_cb(const FLAC__StreamDecoder *decoder,
	 const FLAC__StreamDecoderErrorStatus status,
	 void *client_data);

void
meta_cb(const FLAC__StreamDecoder *decoder,
	const FLAC__StreamMetadata *metadata,
	void *client_data);

FLAC__StreamDecoderWriteStatus
write_cb(const FLAC__StreamDecoder *decoder,
	 const FLAC__Frame *frame,
	 const uint32_t * const buffer[],
	 snd_pcm_t *handle);

FLAC__StreamDecoderInitStatus
aud_open_flac(const char *filename,
	      FLAC__StreamDecoder *decoder,
	      snd_pcm_t *handle);

int
aud_flac_play(char *filename,
	      snd_pcm_t *handle);
