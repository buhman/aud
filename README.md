aud
===

As per my plan for world domination, I continue with simple code that does nothing but play audio.



You can obtain one such file by encoding one of your favorite audio files with:

    ffmpeg -i my_favorite_song.mp3 ar 48000 -ac 2 -acodec flac foo.flac

or

    ffmpeg -i my_favorite_song.mp3 ar 48000 -ac 2 -vn -acodec libvorbis foo.ogg

requirements
------------

This one requires, at the moment, just `libasound` and `libFLAC`.

building
--------

Running `make` will create an `aud` binary.