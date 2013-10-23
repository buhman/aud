#include "alsa.h"
#include "lavf.h"
#include "util.h"

static snd_pcm_t *pcm = NULL;
static snd_async_handler_t *handler = NULL;

static AVFormatContext *format_context = NULL;
static AVCodecContext *codec_context = NULL;
static SwrContext *swr_context = NULL;
static int stream_index;

static snd_pcm_format_t pcm_format;
static unsigned int pcm_rate;
static unsigned int pcm_channels;

static int
file_playback(char *filename)
{
  int err;
  
  {
    err = aud_open_input(filename, &format_context);
    if (err < 0)
      return err;

    stream_index = aud_find_audio_index(format_context);
    if (stream_index < 0)
      return err;

    err = aud_open_codec(format_context, &codec_context, stream_index);
    if (err < 0)
      return err;

    printf("DEBUG: refcounted_frames: %d\n", codec_context->refcounted_frames);
  } /* ... */

  {
    pcm_format = aud_avsf_to_spf(codec_context->sample_fmt);
    pcm_rate = codec_context->sample_rate;
    pcm_channels = codec_context->channels;

    printf("DEBUG: requested: %d; %d; %d\n",
	   pcm_format, pcm_rate, pcm_channels);
    
    err = aud_snd_pcm_init(pcm, &pcm_format, &pcm_rate, &pcm_channels);
    if (err < 0)
      return err;

    printf("DEBUG: actual: %d; %d; %d\n",
	   pcm_format, pcm_rate, pcm_channels);

  } /* ... */

  {
    //err = aud_open_resampler(&swr_context, codec_context,
    //		     pcm_rate, pcm_channels);
    if (err < 0)
      return err;
    aud_snd_pcm_start(pcm, &handler, NULL);
  } /* ... */

  return 0;
}

int
main(int argc,
     char **argv)
{
  int err;

  {
    err = aud_snd_pcm_open("hw:1,0", &pcm);
    if (err < 0)
      return err;
  } /* ... */

  file_playback(argv[1]);

  {
    snd_pcm_close(pcm);
  } /* ... */
  
  return 0;
}
