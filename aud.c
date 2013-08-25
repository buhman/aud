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

static void* buf;

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
  
  snd_pcm_open(&handle,
	       "default",
	       SND_PCM_STREAM_PLAYBACK,
	       0);
 
  for (int i = optind; i < argc; ++i) {

    printf("opening %s\n", argv[i]);
    
    if ((c.key = magic(argv[i])) == NULL) {
      fprintf(stderr, "magic() : %s", strerror(errno));
      continue;
    }
    
    if ((demuxer = hsearch_r(c, FIND, &ret, demuxer_table)) == 0) {
      fprintf(stderr, "unknown demuxer: %s", c.key);
      continue;
    }

    printf("demuxer: %s\n", c.key);

    switch (demuxer) {
    case AUD_DEMUXER_OGG:
      
      o_codec = aud_ogg_content(argv[i]);
      
      switch (o_codec) {
      case OGGZ_CONTENT_VORBIS:
	printf("using vorbis\n");
	break;
      case OGGZ_CONTENT_FLAC:
	printf("using flac\n");
	break;
      default:
	fprintf(stderr, "unimplemented codec: %s\n", mime_type_names[o_codec]);
	continue;
	break;
      }
      break;
    case AUD_DEMUXER_FLAC:
      printf("using flac");
      break;
    default:
      fprintf(stderr, "unimplemented demuxer: %d\n", demuxer);
      continue;
      break;
    }

    set_hw_params(handle,
		  SND_PCM_ACCESS_RW_INTERLEAVED,
		  SND_PCM_FORMAT_S16_LE,
		  48000,
		  2);

    set_sw_params(handle,
		  4096);

    if ((err = snd_pcm_prepare(handle)) < 0) {
      fprintf(stderr, "snd_pcm_prepare() : %s\n", snd_strerror(err));
      goto cleanup;
    }
  }
  
 cleanup:
  snd_pcm_close(handle);
}
