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
        "  -h, --help     display this help and exit\n"
	"  -d, --demuxer  manually override demuxer selection\n\n",
	stderr);

  fputs("Demuxers:\n", stderr);

  for (int i = 0; i < demuxer_table_size; i++) {
    fprintf(stderr, " %s\n", demuxer_mimetypes[i]);
  }
  
  exit(-1);
}

int
main(int argc, char **argv)
{
  int err;
  int opt;

  enum demuxer demuxer;
  char *demuxer_mimetype = NULL;
  int demuxer_override = false;
  OggzStreamContent o_codec;

  while ((opt = getopt_long(argc, argv, "hd:c:", opts, NULL)) > 0) {
    
    switch (opt) {
    case 'h':
      usage();
      break;
    case 'd':
      printf("demuxer override: %s\n", optarg);
      demuxer_override = true;
      demuxer_mimetype = optarg;
      break;
    case 'c':
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

    if (!demuxer_override &&
	(demuxer_mimetype = magic(argv[i])) == NULL) {
      
      fprintf(stderr, "magic() : %s\n", strerror(errno));
      continue;
    }
    
    if ((demuxer = aud_get_demuxer_from_mimetype(demuxer_mimetype,
						 demuxer_table)) < 0) {
      continue;
    }

    switch (demuxer) {
    case AUD_DEMUXER_OGG:
      
      if ((int)(o_codec = aud_ogg_content(argv[i])) < 0) {
	fprintf(stderr, "invalid stream: %s", argv[i]);
	continue;
      }
      
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
