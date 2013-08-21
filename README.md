aud
===

As per my plan for world domination, I continue with simple code that does nothing but play audio.

Specifically, single-channel 48kHz s16le PCM from a file at `../foo.raw`. You can obtain one such file by encoding one of your favorite audio files with:

    ffmpeg -i my_favorite_song.mp3 -acodec pcm_s16le -ac 1 -ar 48000 -f s16le foo.raw

requirements
------------

This one requires, at the moment, just `libasound`.

building
--------

Running `make` will create an `aud` binary.