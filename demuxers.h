#include <search.h>

const int demuxer_table_size;

extern const char * const demuxer_mimetypes[];

enum demuxer {
  AUD_DEMUXER_FLAC = 0,
  AUD_DEMUXER_OGG
};

struct hsearch_data*
aud_create_demuxer_table();

void
aud_destroy_demuxer_table(struct hsearch_data *htab);

enum demuxer
aud_get_demuxer_from_mimetype(const char *mime,
			      struct hsearch_data *demuxer_table);
