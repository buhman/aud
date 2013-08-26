#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

void
aud_stream_status(uint64_t frame,
		  uint64_t total_frames)
{
		
  printf("\r\e[Jframe: %" PRIu64 "/%" PRIu64 " ; %.1f%%",
	 frame,
	 total_frames,
	 100 * (double)frame / (double)total_frames);
  fflush(stdout);

}

void
aud_stream_info(uint64_t total_frames,
		int sample_rate,
		int channels,
		int sample_size)
{
  printf("t: %" PRIu64 " ; r: %d ; c: %d ; b: %d\n",
	 total_frames,
	 sample_rate,
	 channels,
	 sample_size);
}
