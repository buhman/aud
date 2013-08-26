#include <stdint.h>

void
aud_stream_status(uint64_t frame,
		  uint64_t total_frames);

void
aud_stream_info(uint64_t total_frames,
		int sample_rate,
		int channels,
		int sample_size);
