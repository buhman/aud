aud
===

As per my plan for world domination, I continue with simple code that does nothing but play audio.

Currently, only audio encoded at 48kHz, 2ch, s16le are supported. Multiple codecs are also supported:

*  [flac][xflac]
*  [vorbis][xvorbis]

Stream content detection is only partially complete.

requirements
------------

`libasound`, `libFLAC`, `libvorbis`, `libmagic`, and `liboggz>1.1.1` are mandatory dependencies.

> Note: you *must* use a post-1.1.1 release of liboggz

building
--------

Running `make` will create an `aud` binary.

[xflac]: https://xiph.org/flac/
[xvorbis]: http://www.vorbis.com/
[xopus]: http://www.opus-codec.org/