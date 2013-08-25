#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

void aud_status(uint64_t frame,
		uint64_t total_frames)
{
		
  printf("\r\e[Jframe: %" PRIu64 "/%" PRIu64 " ; %.1f%%",
	 frame,
	 total_frames,
	 100 * (double)frame / (double)total_frames);
  fflush(stdout);

}
