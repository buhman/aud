#pragma once

void
aud_next_song(pa_context *context);

AVFrame*
aud_next_packet();

AVFrame*
aud_next_frame();
