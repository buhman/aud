aud
===

As per my plan for world domination, I continue with simple code that does nothing but play audio.

Currently, only audio with s16le sample format are supported. No sample rate conversion is done by aud; any selected sample rate must be supported by your underlying libasound configuration. 

Multiple codecs are also supported:

*  [flac][xflac]
*  [vorbis][xvorbis]

No channel mixing is provided by aud; furthermore, aud's handling of any arbitrary stream decoder may or may not be valid for non-stereo audio.

requirements
------------

`libasound`, `libFLAC`, `libvorbis`, `libopus`, `libmagic`, and `liboggz>1.1.1` are mandatory dependencies.

building
--------

Running `make` will create an `aud` binary.

[xflac]: https://xiph.org/flac/
[xvorbis]: http://www.vorbis.com/
[xopus]: http://www.opus-codec.org/