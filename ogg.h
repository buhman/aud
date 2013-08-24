#include <oggz/oggz.h>

#define BLOCKSIZE 16384

typedef struct oi_info oi_info;

struct oi_info {
  OGGZ *oggz;
  OggzTable *tracks;
};
