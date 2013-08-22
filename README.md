aud
===

As per my plan for world domination, I continue with simple code that does nothing but play audio.

This branch currently expects a 2-channel s16 48000kHz flac audio file called `foo.flac` as input, but will only playback one channel (I have no idea how the interleave format works).

You can obtain one such file by encoding one of your favorite audio files with:

    ffmpeg -i my_favorite_song.mp3 ar 48000 -ac 2 -acodec flac foo.flac

requirements
------------

This one requires, at the moment, just `libasound` and `libFLAC`.

building
--------

Running `make` will create an `aud` binary.