#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

AVFrame *frame;
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
    avcodec_free_frame(&frame);
    frame = avcodec_alloc_frame();
    
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
