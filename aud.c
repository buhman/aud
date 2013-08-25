#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <search.h>

#include <magic.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>

#include "alsa.h"
#include "flac.h"
#include "vorbis.h"
#include "ogg.h"
#include "mimetypes.h"
#include "demuxers.h"

static snd_pcm_t *handle;

static struct hsearch_data *demuxer_table;

static const struct option opts[] = {
  {"help", no_argument, 0, 'h'},
  {"demuxer", required_argument, 0, 'd'},
  {"codec", required_argument, 0, 'c'},
  {0, 0, 0, 0},
};

static char*
magic(const char *filename)
{
  magic_t cookie = magic_open(MAGIC_MIME_TYPE);
  magic_load(cookie, NULL);
  char *m = (char*)magic_file(cookie, filename);

  return m;
}

static void
usage()
{
  fprintf(stderr, "usage: %s [options] <filename(s)>\n",
	  program_invocation_short_name);
  
  fputs("Options:\n"
        "  -h, --help display this help and exit\n",
	stderr);
  
  exit(-1);
}

int
main(int argc, char **argv)
{
  int err;
  int opt;

  ENTRY c, *ret; /* container, return */
  enum demuxer demuxer;
  OggzStreamContent o_codec;

  while ((opt = getopt_long(argc, argv, "h", opts, NULL)) > 0) {
    
    switch (opt) {
    case 'h':
      usage();
      break;
    }
  }

  if (optind == argc) {
    usage();
  } 

  demuxer_table = aud_create_demuxer_table();
  
  if ((err = snd_pcm_open(&handle,
			  "default",
			  SND_PCM_STREAM_PLAYBACK,
			  0)) < 0) {
    fprintf(stderr, "snd_pcm_open() : %s\n", snd_strerror(err));
    return -1;
  }

  printf("pcm name: %s\n", snd_pcm_name(handle));

  for (int i = optind; i < argc; ++i) {

    printf("opening %s\n", argv[i]);
    
    if ((c.key = magic(argv[i])) == NULL) {
      fprintf(stderr, "magic() : %s\n", strerror(errno));
      continue;
    }
    
    if (hsearch_r(c, FIND, &ret, demuxer_table) == 0) {
      fprintf(stderr, "unknown demuxer: %s\n", c.key);
      continue;
    }

    demuxer = *(int*)ret->data;

    switch (demuxer) {
    case AUD_DEMUXER_OGG:
      
      o_codec = aud_ogg_content(argv[i]);
      
      switch (o_codec) {
      case OGGZ_CONTENT_VORBIS:
	aud_vorbis_play(argv[i], handle);
	break;
      case OGGZ_CONTENT_FLAC:
        aud_flac_play(argv[i], handle);
	break;
      default:
	fprintf(stderr, "unimplemented codec: %s\n", mime_type_names[o_codec]);
	continue;
	break;
      }
      break;
    case AUD_DEMUXER_FLAC:
      aud_flac_play(argv[i], handle);
      break;
    default:
      fprintf(stderr, "unimplemented demuxer: %d\n", demuxer);
      continue;
      break;
    }
  }
  
  snd_pcm_close(handle);

  return 0;
}
