#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <vorbis/vorbisfile.h>

int aud_vorbis_play(char *filename,
		    snd_pcm_t *handle) {


  OggVorbis_File *vf = malloc(sizeof(OggVorbis_File));
  int verr;
  int alsa_err;
  int bitstream;

  char *buf = malloc(sizeof(char) * 4096);
    
  if ((verr = ov_fopen(filename, vf)) < 0) {
    fprintf(stderr, "ov_fopen() : %d\n", verr);
  }
  
  vorbis_info *vi = ov_info(vf,-1);
  printf("c: %d ; r: %ld\n", vi->channels, vi->rate);
  

  while ((verr = ov_read(vf, buf,
			 4096, 0, 2, 1,
			 &bitstream)) != 0) {
    
    if ((alsa_err = snd_pcm_wait(handle, 1000)) < 0) {
      fprintf(stderr, "snd_pcm_wait() : %s\n", strerror(errno));
      return -1;
    }

    if ((alsa_err = snd_pcm_writei(handle, (void*)buf, verr / 4)) < 0) {
      fprintf(stderr, "%s\n", snd_strerror(alsa_err));
      return -1;
    }
    
  }

  free(buf);
  ov_clear(vf);
  free(vf);

  return 0;
}
