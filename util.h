#pragma once

#include <alsa/asoundlib.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

snd_pcm_format_t
aud_avsf_to_spf(enum AVSampleFormat iformat);

typedef struct callback_data callback_data_t;
struct callback_data {
  AVFormatContext *format_context;
  AVCodecContext *codec_context;
  int stream_index;
  SwrContext *swr_context;
};

ssize_t
aud_write_callback(callback_data_t *cdata,
		   void *out_buf,
		   ssize_t out_size);
