#include <stdio.h>
#include <getopt.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include <pulse/pulseaudio.h>

#include "aud.h"
#include "pulse.h" 
#include "lavf.h"

static AVFormatContext *format_context;
static AVCodecContext *codec_context;

static pa_mainloop *loop;

static int ret;

static char **opts;
static int optlen;

static AVFrame *frame;
static AVPacket packet;
static AVPacket orig_packet;

static int got_frame;

AVFrame*
aud_next_packet() {
  //printf("packet.stream_index: %d\n", packet.stream_index);
  
  if (packet.stream_index == 0) { //FIXME
    ret = avcodec_decode_audio4(codec_context, frame, &got_frame, &packet);

    packet.data += ret;
    packet.size -= ret;

    ret = FFMIN(ret, packet.size);

    if (got_frame) {
      return frame;
    }
    else {
      printf("no got_frame");
      return NULL;
    }
    
  } else {
    packet.size = 0;
    return aud_next_frame();
  }
}

AVFrame*
aud_next_frame()
{
  if (packet.size > 0) {
    return aud_next_packet();
  }
  else if (av_read_frame(format_context, &packet) >= 0) {
    av_free_packet(&orig_packet);
    orig_packet = packet;
    return aud_next_packet();
  }
  else {
    printf("out of frames\n");
    return NULL;
  }
}

void
aud_next_song(pa_context *context)
{
  aud_open_audio(opts[optind], &format_context);
  aud_open_audio_codec(format_context, &codec_context);

  const pa_sample_spec ss = {
    .format = aud_sample_format[codec_context->sample_fmt],
    .rate = codec_context->sample_rate,
    .channels = codec_context->channels
  };

  optind++;
  
  aud_new_stream(context,
		 &ss);
}

int
main(int argc, char **argv)
{
  opts = argv;
  optlen = argc;
  
  {
    frame = avcodec_alloc_frame();
    
    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
  } /* ... */
  
  {
    loop = pa_mainloop_new();
    
    aud_create_pa_context(pa_mainloop_get_api(loop));

    pa_mainloop_run(loop, &ret);
  } /* ... */

  return ret;
}
