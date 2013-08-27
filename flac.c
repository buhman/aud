#include <inttypes.h>
#include <stdint.h>
#include <errno.h>
#include <alsa/asoundlib.h>
#include <FLAC/stream_decoder.h>

#include "status.h"
#include "alsa.h"

static uint64_t total_samples = 0;
static unsigned sample_rate = 0;
static unsigned channels = 0;
static unsigned bps = 0;

static int stop;

static void*
interleave_samples_s32(const FLAC__Frame *frame,
		       const int32_t * const buffer[])
{
  int32_t* ibuf = malloc(sizeof(uint32_t) * frame->header.blocksize * 2);
  
  for (int i = 0; i < frame->header.blocksize; i++) {
    
    ibuf[i * 2] = (int32_t)buffer[0][i];
    ibuf[i * 2 + 1] = (int32_t)buffer[1][i];
  }

  return ibuf;
}

static void*
interleave_samples_s16(const FLAC__Frame *frame,
		       const int32_t * const buffer[])
{
  int16_t* ibuf = malloc(sizeof(uint16_t) * frame->header.blocksize * 2);
  
  for (int i = 0; i < frame->header.blocksize; i++) {
    
    ibuf[i * 2] = (int16_t)buffer[0][i];
    ibuf[i * 2 + 1] = (int16_t)buffer[1][i];
  }

  return ibuf;
}

static void
error_cb(const FLAC__StreamDecoder *decoder,
	 const FLAC__StreamDecoderErrorStatus status,
	 void *client_data) {

  fprintf(stderr, "StreamDecoderError: %s\n",
	  FLAC__StreamDecoderErrorStatusString[status]);

  FLAC__stream_decoder_finish(decoder);
}

static void
meta_cb(const FLAC__StreamDecoder *decoder,
	const FLAC__StreamMetadata *metadata,
	void *handle) {
  
  if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
    
    total_samples = metadata->data.stream_info.total_samples;
    sample_rate = metadata->data.stream_info.sample_rate;
    channels = metadata->data.stream_info.channels;
    bps = metadata->data.stream_info.bits_per_sample;

    aud_stream_info(total_samples, sample_rate, channels, bps);

    aud_prepare_handle(handle, sample_rate, channels, bps);
  }
}

static FLAC__StreamDecoderWriteStatus
write_cb(const FLAC__StreamDecoder *decoder,
	 const FLAC__Frame *frame,
	 const int32_t * const buffer[],
	 void *handle) {

  void *ibuf = NULL;

  switch (bps) {
  case 16:
    ibuf = interleave_samples_s16(frame, buffer);
    break;
  case 24:
    ibuf = interleave_samples_s32(frame, buffer);
    break;
  default:
    fprintf(stderr, "unsupported sample size: %d\n", bps);
    return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    break;
  }
  
  aud_stream_status(frame->header.number.sample_number, total_samples);

  if (aud_write_buf(handle, ibuf, frame->header.blocksize) < 0) {
    return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
  }

  free(ibuf);
  
  return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

FLAC__StreamDecoderInitStatus
aud_flac_open(const char *filename,
	      FLAC__StreamDecoder *decoder,
	      snd_pcm_t *handle) {

  FLAC__StreamDecoderInitStatus status;

  FLAC__stream_decoder_set_md5_checking(decoder, true);

  status = FLAC__stream_decoder_init_file(decoder,
					  filename,
					  write_cb,
					  meta_cb,
					  error_cb,
					  handle);

  return status;
}

int
aud_flac_play(char *filename,
	      snd_pcm_t *handle) {
  
  FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new();
  
  FLAC__StreamDecoderInitStatus status = aud_flac_open(filename,
						       decoder,
						       handle);

  if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
    fprintf(stderr,
	    "FLAC__stream_decoder_init_file() : %s\n",
	    FLAC__StreamDecoderInitStatusString[status]);
    return -1;
  }

  FLAC__stream_decoder_process_until_end_of_stream(decoder);
  printf("\n%s\n", FLAC__StreamDecoderStateString[FLAC__stream_decoder_get_state(decoder)]);

  FLAC__stream_decoder_delete(decoder);

  return 0;
}
