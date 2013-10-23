#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <alsa/asoundlib.h>

static snd_pcm_uframes_t buffer_size, period_size;

static int
set_hwparams(snd_pcm_t *pcm,
	     snd_pcm_hw_params_t *params,
	     snd_pcm_format_t *format,
	     unsigned int *rate,
	     unsigned int *channels)
	     
{
  int err;
  
  err = snd_pcm_hw_params_any(pcm, params);
  if (err < 0) {
    printf("snd_pcm_hw_params_any(): %s\n", snd_strerror(err));
    return err;
  }

  {
    err = snd_pcm_hw_params_set_rate_resample(pcm, params, 0);
    if (err < 0) {
      printf("snd_pcm_hw_params_set_rate_resample(): %s\n", snd_strerror(err));
      return err;
    }
    
    err = snd_pcm_hw_params_set_access(pcm, params,
				       SND_PCM_ACCESS_MMAP_INTERLEAVED);
    if (err < 0) {
      printf("snd_pcm_hw_params_set_access(): %s\n", snd_strerror(err));
      return err;
    }
  } /* ... */

  {
    err = snd_pcm_hw_params_set_format(pcm, params, *format);
    if (err < 0) {
      printf("snd_pcm_hw_params_set_format(): %s\n", snd_strerror(err));
      return err;
    }

    int dir;
    err = snd_pcm_hw_params_set_rate_near(pcm, params, rate, &dir);
    if (err < 0) {
      printf("snd_pcm_hw_params_set_rate(): %s\n", snd_strerror(err));
      return err;
    }

    err = snd_pcm_hw_params_set_channels_near(pcm, params, channels);
    if (err < 0) {
      printf("snd_pcm_hw_params_set_channels(): %s\n", snd_strerror(err));
      return err;
    }
  } /* ... */
  
  {
    snd_pcm_uframes_t bs;

    err = snd_pcm_hw_params_set_buffer_size_last(pcm,
						 params,
						 &bs);
    if (err < 0) {
      printf("snd_pcm_hw_params_set_buffer_size_last(): %s\n",
	     snd_strerror(err));
      return err;
    }

    err = snd_pcm_hw_params_get_buffer_size(params,
					    &buffer_size);
    if (err < 0) {
      printf("snd_pcm_hw_params_get_buffer_size(): %s\n",
	     snd_strerror(err));
      return err;
    }
  } /* ... */
  
  {
    int dir;
    unsigned int p;
    
    err = snd_pcm_hw_params_set_periods_last(pcm,
					     params,
					     &p,
					     &dir);
    if (err < 0) {
      printf("snd_pcm_hw_params_set_periods_last(): %s\n",
	     snd_strerror(err));
      return err;
    }

    err = snd_pcm_hw_params_get_period_size(params,
					    &period_size,
					    &dir);
    if (err < 0) {
      printf("snd_pcm_hw_params_get_period_size(): %s\n",
	     snd_strerror(err));
      return err;
    }
  } /* ... */
  
  err = snd_pcm_hw_params(pcm, params);
  if (err < 0) {
    printf("snd_pcm_hw_params(): %s\n", snd_strerror(err));
    return err;
  }
  
  return 0;
}

static int
set_swparams(snd_pcm_t *pcm,
	     snd_pcm_sw_params_t *params)
{
  int err;
  
  {
    err = snd_pcm_sw_params_current(pcm, params);
    if (err < 0) {
      printf("snd_pcm_sw_params_current(): %s\n", snd_strerror(err));
      return err;
    }

    err = snd_pcm_sw_params_set_start_threshold(pcm, params,
						(buffer_size / period_size) * period_size);
    if (err < 0) {
      printf("snd_pcm_sw_params_set_start_threshold(): %s\n", snd_strerror(err));
      return err;
    }

    err = snd_pcm_sw_params_set_avail_min(pcm, params, buffer_size);
    if (err < 0) {
      printf("snd_pcm_sw_params_set_avail_min(): %s\n", snd_strerror(err));
      return err;
    }

    err = snd_pcm_sw_params_set_period_event(pcm, params, 1);
    if (err < 0) {
      printf("snd_pcm_sw_params_set_period_event(): %s\n", snd_strerror(err));
      return err;
    }

    err = snd_pcm_sw_params(pcm, params);
    if (err < 0) {
      printf("snd_pcm_sw_params(): %s\n", snd_strerror(err));
      return err;
    }
  } /* ... */

  return 0;
}

static void
data_write_callback(snd_async_handler_t *handler)
{
  int err;
  snd_pcm_t *pcm;

  pcm = snd_async_handler_get_pcm(handler);

  //int state = snd_pcm_state(pcm);
  
  // fix the state if it's not what we want
  
  {
    const snd_pcm_channel_area_t *area;
    snd_pcm_uframes_t offset, frames;
    snd_pcm_sframes_t commit;    
    
    frames = snd_pcm_avail_update(pcm);
    if (frames < 0) {
      printf("snd_pcm_avail_update(): %s\n", snd_strerror(commit));
      return;
    }
    err = snd_pcm_mmap_begin(pcm, &area, &offset, &frames);
    if (err < 0) {
      printf("snd_pcm_mmap_begin(): %s\n", snd_strerror(err));
      return;
    }

    void *addr = ((char*)(area->addr)) + (offset * 2);
    ssize_t size = 0;//read(fd, addr, frames * 2);
    
    if (size == 0 && frames != 0) {
      //running = 0;
    }
    
    if (size != frames * 2) {
      printf("read(): f%lu != s%lu\n", frames, size);
      //return;
    }
    
    commit = snd_pcm_mmap_commit(pcm, offset, frames);
    if (commit < 0) {
      printf("snd_pcm_mmap_commit(): %s\n", snd_strerror(commit));
      return;
    }
    if (commit != frames) {
      printf("snd_pcm_mmap_commit(): f%lu != c%lu\n", frames, commit);
      return;
    }

    snd_pcm_start(pcm);
  }
}

int
aud_snd_pcm_open(char *name,
		 snd_pcm_t **pcm)
{
  int err;
  
  {
    err = snd_pcm_open(pcm, name,
		       SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
      printf("snd_pcm_open(): %s\n", snd_strerror(err));
      return err;
    }
  } /* ... */

  return 0;
}

int
aud_snd_pcm_init(snd_pcm_t *pcm,
		 snd_pcm_format_t *format,
		 unsigned int *rate,
		 unsigned int *channels)
{
  {
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_hw_params_alloca(&hwparams);
    
    if (set_hwparams(pcm, hwparams,
		     format, rate, channels) < 0) {
      return 1;
    }

    
  } /* ... */
  
  {
    snd_pcm_sw_params_t *swparams;
    snd_pcm_sw_params_alloca(&swparams);
    
    if (set_swparams(pcm, swparams) < 0) {
      return 1;
    }
  } /* ... */
  
  
  return 0;
}

int
aud_snd_pcm_start(snd_pcm_t *pcm,
		  snd_async_handler_t **handler,
		  void* data)
{
  {
    snd_async_del_handler(*handler);
    *handler = NULL;
  } /* ... */
  {
    snd_async_add_pcm_handler(handler, pcm, &data_write_callback, data);

    data_write_callback(*handler);
  } /* ... */

  return 0;
}
