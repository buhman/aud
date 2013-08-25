#include <search.h>

enum demuxer {
  AUD_DEMUXER_FLAC = 0,
  AUD_DEMUXER_OGG
};

struct hsearch_data*
aud_create_demuxer_table();

void
aud_destroy_demuxer_table(struct hsearch_data *htab);
