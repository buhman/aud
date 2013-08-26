#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <vorbis/vorbisfile.h>

#include "alsa.h"
#include "status.h"

static int word_size = 2;

int aud_vorbis_play(char *filename,
		    snd_pcm_t *handle) {


  OggVorbis_File *vf = malloc(sizeof(OggVorbis_File));
  int verr;
  int bitstream;

  char *buf = malloc(sizeof(char) * 4096);
    
  if ((verr = ov_fopen(filename, vf)) < 0) {
    fprintf(stderr, "ov_fopen() : %d\n", verr);
    return -1;
  }
  
  vorbis_info *vi = ov_info(vf,-1);

  aud_stream_info(vf->end, vi->rate, vi->channels, 8 * word_size);

  aud_prepare_handle(handle, vi->rate, vi->channels);

  while ((verr = ov_read(vf, buf,
			 4096, 0, word_size, 1,
			 &bitstream)) != 0) {

    aud_stream_status(vf->offset, vf->end);
    
    if (aud_write_buf(handle, buf, verr / (word_size * vi->channels)) < 0) {
      return -1;
    }
  }

  free(buf);
  ov_clear(vf);
  free(vf);

  return 0;
}
