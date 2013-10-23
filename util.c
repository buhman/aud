#include <alsa/asoundlib.h>

#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>

#include "lavf.h"
#include "util.h"

static const snd_pcm_format_t avsf_spf[] = {
  SND_PCM_FORMAT_U8, //AV_SAMPLE_FMT_U8
  SND_PCM_FORMAT_S16_LE, //AV_SAMPLE_FMT_S16 
  SND_PCM_FORMAT_S32_LE, //AV_SAMPLE_FMT_S32
  SND_PCM_FORMAT_FLOAT_LE, //AV_SAMPLE_FMT_FLT
  SND_PCM_FORMAT_FLOAT64_LE, //AV_SAMPLE_FMT_DBL
}; 

snd_pcm_format_t
aud_avsf_to_spf(enum AVSampleFormat iformat)
{
  {
    iformat = av_get_packed_sample_fmt(iformat);
    if (iformat > sizeof(avsf_spf) / sizeof(snd_pcm_format_t) ||
	iformat < 0)
      return SND_PCM_FORMAT_UNKNOWN;
    return avsf_spf[iformat];
  }
}

ssize_t
aud_write_callback(callback_data_t *cdata,
		   void *out_buf,
		   ssize_t out_size)
{
  ssize_t commit = 0;
  ssize_t written = 0;
  AVFrame *frame;

  int channels;
  int sample_size;
  
  while (1) {
    frame = aud_next_frame(cdata->format_context,
			   cdata->codec_context,
			   cdata->stream_index);

    if (cdata->swr_context) {
      //printf("DEBUG: swr_context != NULL\n");

      //channels = av_get_channel_layout_nb_channels(cdata->swr_context->out_ch_layout);
      //sample_size = av_get_bytes_per_sample(cdata->swr_context->out_sample_fmt);
      channels = av_get_channel_layout_nb_channels(frame->channel_layout);
      sample_size = av_get_bytes_per_sample(frame->format);

      commit = frame->nb_samples * sample_size * channels;
      if (commit > out_size) {
	printf("DEBUG: OVERFLOW! didn't handle it; just bailed lul (FIXME)\n");
	return written;
      }

      swr_convert(cdata->swr_context, (uint8_t**)&out_buf, frame->nb_samples,
		  (const uint8_t**)frame->extended_data, frame->nb_samples);

      written += commit;

      printf("commit: %lu; out_size: %lu\n", commit, out_size);
      if (commit * 4 < out_size) {
	//printf("commit * 2 < out_size\n");
	out_size -= commit;
	out_buf = ((char*)(out_buf)) + (commit);
	continue;
      }

      return written;
    }
    else {
      printf("DEBUG: unimplemented non-swr write\n");
      return written;
    }
  }
}
