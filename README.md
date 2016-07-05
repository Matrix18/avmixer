# audio&video mixer
A library that merges streams of audio&amp;video.

You can intergate this library into your stream server to do audio&video stream merging.
This library uses fdk-aac(version 0.1.4) to encode/decode pcm data to/from aac format.

# audio mix-down algorithm(LDRC)
The audio mix-down module uses an algorithm, called Logarithmic Dynamic Range Compression(LDRC). You can find it here  http://www.voegler.eu/pub/audio/digital-audio-mixing-and-normalization.html.



