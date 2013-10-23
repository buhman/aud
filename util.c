#include <inttypes.h>

#include <alsa/asoundlib.h>

#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>

static const snd_pcm_format_t avsf_spf[] = {
  SND_PCM_FORMAT_U8, //AV_SAMPLE_FMT_U8
  SND_PCM_FORMAT_S16, //AV_SAMPLE_FMT_S16 
  SND_PCM_FORMAT_S32, //AV_SAMPLE_FMT_S32
  SND_PCM_FORMAT_FLOAT, //AV_SAMPLE_FMT_FLT
  SND_PCM_FORMAT_FLOAT64, //AV_SAMPLE_FMT_DBL
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

int
aud_open_resampler(SwrContext **swr_context,
		   AVCodecContext *codec_context,
		   unsigned int rate,
		   unsigned int channels)
{
  uint64_t output_layout;
  
  int err;

  {
    swr_free(swr_context);
  } /* ... */

  {
    if (av_sample_fmt_is_planar(codec_context->sample_fmt) ||
	codec_context->sample_rate != rate ||
	codec_context->channels != channels) {

      *swr_context = swr_alloc();

      err = av_get_standard_channel_layout(channels, &output_layout,
					   NULL);
      if (err < 0)
	return err;

      printf("DEBUG channel map: %" PRIu64 " -> %" PRIu64 "\n",
	     codec_context->channel_layout, output_layout);
      
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
    }
  }

  return 0;
}
