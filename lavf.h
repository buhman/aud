#pragma once

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

AVFrame*
aud_next_frame(AVFormatContext *format_context,
	       AVCodecContext *codec_context,
	       int stream_index);

int
aud_open_codec(AVFormatContext *format_context,
	       AVCodecContext **codec_context,
	       int stream_index);

int
aud_find_audio_index(AVFormatContext *format_context);

int
aud_open_input(char *filename,
	       AVFormatContext **format_context);
