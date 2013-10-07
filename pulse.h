#pragma once

void
aud_stream_write_cb(pa_stream *stream,
		    size_t nbytes,
		    void *data);

void
aud_stream_state_cb(pa_stream *stream,
		    void *data);

void
aud_context_state_cb(pa_context *context,
		     void *data);

void
aud_new_stream(pa_context *context,
	       const pa_sample_spec *ss);

int
aud_create_pa_context(pa_mainloop_api *loop_api);

