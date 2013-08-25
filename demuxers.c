#include <stdlib.h>
#include <search.h>

static const int demuxer_table_size = 2;

static char *demuxer_mimetypes[] = {
  "audio/x-flac",
  "application/ogg"
};

struct hsearch_data*
aud_create_demuxer_table()
{
  ENTRY d, *ret;
  
  struct hsearch_data *htab = calloc(demuxer_table_size,
				     sizeof(struct hsearch_data));

  hcreate_r(demuxer_table_size, htab);
  
  for (int i = 0; i < demuxer_table_size; i++) {
    d.key = demuxer_mimetypes[i];
    d.data = (void*)&i;

    hsearch_r(d, ENTER, &ret, htab);
  }

  return htab;
}

void
aud_destroy_demuxer_table(struct hsearch_data *htab)
{
  hdestroy_r(htab);
}
