#pragma once

#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>

snd_pcm_format_t
aud_avsf_to_spf(enum AVSampleFormat iformat);
