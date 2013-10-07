#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include <pulse/pulseaudio.h>

#include "lavf.h"

static int ret;

/*
enum AVSampleFormat {
  AV_SAMPLE_FMT_NONE = -1,
  AV_SAMPLE_FMT_U8, ///< unsigned 8 bits
  AV_SAMPLE_FMT_S16, ///< signed 16 bits
  AV_SAMPLE_FMT_S32, ///< signed 32 bits
  AV_SAMPLE_FMT_FLT, ///< float
  AV_SAMPLE_FMT_DBL, ///< double
  AV_SAMPLE_FMT_U8P, ///< unsigned 8 bits, planar
  AV_SAMPLE_FMT_S16P, ///< signed 16 bits, planar
  AV_SAMPLE_FMT_S32P, ///< signed 32 bits, planar
  AV_SAMPLE_FMT_FLTP, ///< float, planar
  AV_SAMPLE_FMT_DBLP, ///< double, planar
};
*/

const pa_sample_format_t aud_sample_format[] = {
  PA_SAMPLE_U8,
  PA_SAMPLE_S16LE,
  PA_SAMPLE_S32LE,
  PA_SAMPLE_FLOAT32LE,
  PA_SAMPLE_INVALID, // No double in PA?
  PA_SAMPLE_U8, // U8P
  PA_SAMPLE_S16LE, // S16P
  PA_SAMPLE_S32LE, // S32P
  PA_SAMPLE_FLOAT32LE, // FLTP
  PA_SAMPLE_INVALID, // DBLP
};

int
aud_open_codec(AVFormatContext *format_context,
	       int stream_index,
	       AVCodecContext **codec_context)
{
  AVCodec *codec;
  AVStream *stream;

  {
    stream = format_context->streams[stream_index];
    *codec_context = stream->codec;

    if (!(codec = avcodec_find_decoder((*codec_context)->codec_id))) {
      printf("avcodec_find_decoder(): NULL");
      return ret;
    }

    if ((ret = avcodec_open2(*codec_context, codec, NULL)) < 0) {
      printf("avcodec_open2(): %d: %s", ret, av_err2str(ret));
      return ret;
    }
    
  } /* ... */

  return 0;
}

int
aud_open_audio_codec(AVFormatContext *format_context,
		     AVCodecContext **codec_context)
{
  {
    if ((ret = av_find_best_stream(format_context,
				   AVMEDIA_TYPE_AUDIO,
				   -1, -1, NULL, 0)) < 0) {
      printf("av_find_best_stream(): %d: %s", ret, av_err2str(ret));
      return ret;
    }
  } /* ... */

  return aud_open_codec(format_context, ret, codec_context);
}

int
aud_open_audio(char *filename,
	       AVFormatContext **format_context)
{
  {
    av_register_all();

    if ((ret = avformat_open_input(format_context,
				   filename, NULL, NULL)) < 0) {
      printf("avformat_open_input(): %d: %s", ret, av_err2str(ret));
      return ret;
    }

    if ((ret = avformat_find_stream_info(*format_context, NULL)) < 0) {
      printf("avformat_find_stream_info(): %d: %s", ret, av_err2str(ret));
      return ret;
    }

    av_dump_format(*format_context, 0, filename, 0);
  } /* ... */

  return 0;
}
