#pragma once

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

int
aud_open_codec(AVFormatContext *format_context,
	       int stream_index,
	       AVCodecContext **codec_context);

int
aud_open_audio_codec(AVFormatContext *format_context,
		     AVCodecContext **codec_context);

int
aud_open_audio(char *filename,
	       AVFormatContext **format_context);

extern const pa_sample_format_t aud_sample_format[];
