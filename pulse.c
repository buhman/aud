#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <libavutil/frame.h>

#include <pulse/pulseaudio.h>

#include "pulse.h"
#include "aud.h"

static const char* aud_context_state[] = {
  "PA_CONTEXT_UNCONNECTED",
  "PA_CONTEXT_CONNECTING",
  "PA_CONTEXT_AUTHORIZING",
  "PA_CONTEXT_SETTING_NAME",
  "PA_CONTEXT_READY",
  "PA_CONTEXT_FAILED",
  "PA_CONTEXT_TERMINATED",
};

static const char* aud_stream_state[] = {
  "PA_STREAM_UNCONNECTED",
  "PA_STREAM_CREATING",
  "PA_STREAM_READY",
  "PA_STREAM_FAILED",
  "PA_STREAM_TERMINATED",
};

// maximum latency
static const pa_buffer_attr aud_buffer_attr = {
  (uint32_t)-1,
  (uint32_t)-1,
  (uint32_t)-1,
  (uint32_t)-1,
  (uint32_t)-1
};

void
aud_stream_write_cb(pa_stream *stream,
		    size_t nbytes,
		    void *data)
{
  AVFrame *frame;
  {
    printf("aud_stream_write_cb(): nbytes: %d\n", (int)nbytes);

    int written = 0;
    int bytes = 0;

    while (written + bytes < nbytes) {
      frame = aud_next_frame();

      bytes = av_get_bytes_per_sample(frame->format) * frame->nb_samples;
      written += bytes;
    
      //printf("nb_samples: %d ; bytes: %d; written: %d\n",
      //     frame->nb_samples,
      //     bytes, written);
    
      pa_stream_write(stream,
		      frame->extended_data[0],
		      bytes,
		      NULL,
		      0,
		      PA_SEEK_RELATIVE);
    }
  }
}

void
aud_stream_state_cb(pa_stream *stream,
		    void *data)
{
  pa_stream_state_t s_state;

  {
    s_state = pa_stream_get_state(stream);
    printf("pa_stream_get_state(): %s\n", aud_stream_state[s_state]);
    
    switch (s_state) {
    case PA_STREAM_READY:
      pa_stream_set_write_callback(stream,
				   aud_stream_write_cb,
				   NULL);
      break;
    default:
      break;
    }
  } /* ... */
}

void
aud_context_state_cb(pa_context *context,
		     void *data)
{
  pa_context_state_t c_state;

  {
    c_state = pa_context_get_state(context);
    printf("pa_context_get_state(): %s\n", aud_context_state[c_state]);

    switch (c_state) {
    case PA_CONTEXT_READY:
      aud_next_song(context);
      break;
    default:
      break;
    }
  } /* ... */
}

void
aud_new_stream(pa_context *context,
	       const pa_sample_spec *ss)
{
  pa_stream *stream;
  
  {
    stream = pa_stream_new(context,
			   "aud playback",
			   ss,
			   NULL);
      
    pa_stream_set_state_callback(stream,
				 aud_stream_state_cb,
				 NULL);

    pa_stream_connect_playback(stream,
			       NULL,
			       &aud_buffer_attr,
			       PA_STREAM_ADJUST_LATENCY,
			       NULL,
			       NULL);
  } /* ... */
}

int
aud_create_pa_context(pa_mainloop_api *loop_api)
{
  pa_context *context;

  {
    context = pa_context_new(loop_api, "aud");
    pa_context_set_state_callback(context,
				  aud_context_state_cb,
				  NULL);
    pa_context_connect(context,
		       NULL,
		       PA_CONTEXT_NOAUTOSPAWN | PA_CONTEXT_NOFAIL,
		       NULL);
  } /* ... */

  return 0;
}
