#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

void aud_status(uint64_t frame,
		uint64_t total_frames)
{
		
  printf("\r\e[Jframe: %" PRIu64 "/%" PRIu64 " ; %.2f%%",
	 frame,
	 total_frames,
	 (double)frame / (double)total_frames);
  fflush(stdout);

}
