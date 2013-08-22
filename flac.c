#include <inttypes.h>
#include <stdint.h>
#include <errno.h>
#include <alsa/asoundlib.h>
#include <FLAC/stream_decoder.h>

static uint64_t total_samples = 0;
static unsigned sample_rate = 0;
static unsigned channels = 0;
static unsigned bps = 0;

static int alsa_err;

void
error_cb(const FLAC__StreamDecoder *decoder,
	 const FLAC__StreamDecoderErrorStatus status,
	 void *client_data) {

  fprintf(stderr, "StreamDecoderError: %s",
	  FLAC__StreamDecoderErrorStatusString[status]);
}

void
meta_cb(const FLAC__StreamDecoder *decoder,
	const FLAC__StreamMetadata *metadata,
	void *client_data) {
  
  if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
    
    total_samples = metadata->data.stream_info.total_samples;
    sample_rate = metadata->data.stream_info.sample_rate;
    channels = metadata->data.stream_info.channels;
    bps = metadata->data.stream_info.bits_per_sample;

    fprintf(stderr, "t: %" PRIu64 " ; r: %d ; c: %d ; b %d\n",
	    total_samples,
	    sample_rate,
	    channels,
	    bps);
  }
}

FLAC__StreamDecoderWriteStatus
write_cb(const FLAC__StreamDecoder *decoder,
	 const FLAC__Frame *frame,
	 const int32_t * const buffer[],
	 void *handle) {

  if ((alsa_err = snd_pcm_wait(handle, 1000)) < 0) {
    fprintf(stderr, "snd_pcm_wait() : %s", strerror(errno));
  }

  printf("bs: %d sn: %" PRIu64 "\n",
	 frame->header.blocksize,
	 frame->header.number.sample_number);


  uint16_t* l = malloc(sizeof(uint16_t) * frame->header.blocksize);
  uint16_t* r = malloc(sizeof(uint16_t) * frame->header.blocksize);
  
  for (int i = 0; i < frame->header.blocksize; i++) {
    
    l[i] = (uint16_t)buffer[0][i];
    r[i] = (uint16_t)buffer[1][i];
  }

  uint16_t * buf[] = {l, r};
  
  if ((alsa_err = snd_pcm_writen(handle, buf, frame->header.blocksize)) < 0) {
    fprintf(stderr, "%s", snd_strerror(alsa_err));
    return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
  }

  free(l);
  free(r);
  
  return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

FLAC__StreamDecoderInitStatus
aud_open_flac(const char *filename,
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
