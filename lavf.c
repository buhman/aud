#include <inttypes.h>

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>

AVFrame *frame = NULL;
AVPacket packet;

AVFrame*
aud_next_frame(AVFormatContext *format_context,
	       AVCodecContext *codec_context,
	       int stream_index)
{
  int err;
  int got_frame;

  while (1) {
    if (packet.size <= 0) {
      av_free_packet(&packet);
      err = av_read_frame(format_context, &packet);

      if (err < 0) {
	printf("av_read_frame(): %d", err);
	return NULL;
      }
    }

    if (packet.stream_index == stream_index) {
      if (codec_context->refcounted_frames)
	av_frame_unref(frame);
      err = avcodec_decode_audio4(codec_context, frame, &got_frame, &packet);

      if (err < 0) {
	printf("avcodec_decode_audio4(): %d\n", err);
	return NULL;
      }

      packet.data += err;
      packet.size -= err;

      if (got_frame) {
	return frame;
      }

      printf("avcodec_decode_audio4(): got_frame == 0\n");
    }
    else {
      // discard packet
      packet.size = 0;
    }
  }
}

int
aud_open_resampler(SwrContext **swr_context,
		   AVCodecContext *codec_context,
		   unsigned int rate,
		   unsigned int channels)
{
  uint64_t output_layout;

  int err;

  {
    if (*swr_context)
      swr_free(swr_context);
  } /* ... */

  {
    if (av_sample_fmt_is_planar(codec_context->sample_fmt) ||
	codec_context->sample_rate != rate ||
	codec_context->channels != channels) {

      *swr_context = swr_alloc();
      if (!(*swr_context)) {
	printf("swr_alloc(): NULL\n");
	return -1;
      }

      err = av_get_standard_channel_layout(channels, &output_layout,
					   NULL);
      if (err < 0) {
	printf("av_get_standard_channel_layout(): %d\n", err);
	return err;
      }

      printf("DEBUG: channel map: %" PRIu64 " -> %" PRIu64 "\n",
	     codec_context->channel_layout, output_layout);

      /*
      av_opt_set_int(*swr_context, "in_channel_layout",
		     codec_context->channel_layout, 0);
      av_opt_set_int(*swr_context, "in_sample_rate",
		     codec_context->sample_rate, 0);
      av_opt_set_sample_fmt(*swr_context, "in_sample_fmt",
			    codec_context->sample_fmt, 0);

      av_opt_set_int(*swr_context, "out_channel_layout", output_layout, 0);
      av_opt_set_int(*swr_context, "out_sample_rate", rate, 0);
      av_opt_set_sample_fmt(*swr_context, "out_sample_format",
			    av_get_packed_sample_fmt(codec_context->sample_fmt),
			    0);
      */
      swr_alloc_set_opts(*swr_context,
			 //output
			 codec_context->channel_layout, //FIXEM
			 //output_layout,
			 av_get_packed_sample_fmt(codec_context->sample_fmt),
			 rate,
			 //input
			 codec_context->channel_layout,
			 codec_context->sample_fmt,
			 codec_context->sample_rate,
			 //log
			 0, 0);

      err = swr_init(*swr_context);
      if (err < 0) {
	printf("swr_init(): %d %s\n", err, av_err2str(err));
	return err;
      }
    }
  }

  return 0;
}

int
aud_open_codec(AVFormatContext *format_context,
	       AVCodecContext **codec_context,
	       int stream_index)
{
  AVCodec *codec;
  AVStream *stream;
  int err;

  {
    stream = format_context->streams[stream_index];
    *codec_context = stream->codec;

    if (!(codec = avcodec_find_decoder((*codec_context)->codec_id))) {
      printf("avcodec_find_decoder(): NULL");
      return -1;
    }

    if ((err = avcodec_open2(*codec_context, codec, NULL)) < 0) {
      printf("avcodec_open2(): %d: %s", err, av_err2str(err));
      return err;
    }

  } /* ... */

  {
    if (frame)
      av_frame_free(&frame);
    frame = av_frame_alloc();

    av_init_packet(&packet);
    packet.data = NULL;
    packet.size = 0;
  } /* ... */

  return 0;
}

int
aud_find_audio_index(AVFormatContext *format_context)
{
  int err;

  {
    if ((err = av_find_best_stream(format_context,
				   AVMEDIA_TYPE_AUDIO,
				   -1, -1, NULL, 0)) < 0) {
      printf("av_find_best_stream(): %d: %s", err, av_err2str(err));
      return err;
    }
  } /* ... */

  return err;
}

int
aud_open_input(char *filename,
	       AVFormatContext **format_context)
{
  int err;

  {
    av_register_all();

    if (*format_context)
      avformat_close_input(format_context);
    if ((err = avformat_open_input(format_context,
				   filename, NULL, NULL)) < 0) {
      printf("avformat_open_input(): %d: %s", err, av_err2str(err));
      return err;
    }

    if ((err = avformat_find_stream_info(*format_context, NULL)) < 0) {
      printf("avformat_find_stream_info(): %d: %s", err, av_err2str(err));
      return err;
    }

    av_dump_format(*format_context, 0, filename, 0);
  } /* ... */

  return 0;
}
