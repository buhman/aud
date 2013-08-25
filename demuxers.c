#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <string.h>
#include <errno.h>

const int demuxer_table_size = 2;

const char * const demuxer_mimetypes[] = {
  "audio/x-flac",
  "application/ogg"
};

struct hsearch_data*
aud_create_demuxer_table()
{
  ENTRY d, *ret;
  int i;
  
  struct hsearch_data *htab = calloc(demuxer_table_size,
				     sizeof(struct hsearch_data));

  hcreate_r(demuxer_table_size, htab);
  
  for (i = 0; i < demuxer_table_size; i++) {

    const char *sc = demuxer_mimetypes[i];
    char *s = malloc(strlen(sc));
    strcpy(s, sc);

    d.key = s;

    int *data = malloc(sizeof(int));
    memcpy(data, &i, sizeof(int));
    d.data = data;

    if (hsearch_r(d, ENTER, &ret, htab) == 0) {
      fprintf(stderr, "failed entry, %s", d.key);
    }

  }

  return htab;
}

void
aud_destroy_demuxer_table(struct hsearch_data *htab)
{
  hdestroy_r(htab);
}

int
aud_get_demuxer_from_mimetype(char *mime,
			      struct hsearch_data *demuxer_table)
{
  ENTRY c, *ret;

  c.key = mime;
    
  if (hsearch_r(c, FIND, &ret, demuxer_table) == 0) {
    fprintf(stderr, "unknown demuxer: %s\n", c.key);
    return -1;
  }

  return *(int*)ret->data;
}
