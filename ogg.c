#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <oggz/oggz.h>

#include "ogg.h"

OggzStreamContent
oggz_info_apply(oi_info *info) {

  long serialno;
  
  //int ts = oggz_table_size(info->tracks);

  /* FIXME: here we take just the first track; but there could be more */
  oggz_table_nth(info->tracks, 0, &serialno);
  
  return oggz_stream_get_content(info->oggz, serialno);
}

static int
read_page_pass(OGGZ *oggz,
	       const ogg_page *og,
	       long serialno,
	       void *user_data) {

  oi_info *info = (oi_info*)user_data;

  oggz_table_insert(info->tracks, serialno, &read_page_pass);
  
  if (ogg_page_bos((ogg_page*)og)) {
    return 0;
  }
  else {
    return OGGZ_STOP_OK;
  }
}

static void
oi_pass(oi_info *info) {

  oggz_seek(info->oggz, 0, SEEK_SET);
  oggz_set_read_page(info->oggz, -1, &read_page_pass, info);

  while(oggz_read(info->oggz, BLOCKSIZE) > 0);
}
OggzStreamContent
aud_ogg_content(char *filename) {

  OGGZ *oggz;
  oi_info info;
  OggzStreamContent content;
  
  if (!(oggz = oggz_open("test.ogg", OGGZ_READ|OGGZ_AUTO))) {
    fprintf(stderr, "oggz_open(): %s", strerror(errno));
    return -1;
  }

  info.oggz = oggz;
  info.tracks = oggz_table_new();

  oi_pass(&info);

  content = oggz_info_apply(&info);

  oggz_table_delete(info.tracks);
  oggz_close(oggz);
  
  return content;
}
