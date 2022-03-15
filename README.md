# wav-audio-worker
Analyse and manipulate wav audio files

## Scope of the program

This is a command line tool written in C to perform an in depth analysis of a provides audio wav file. The information in the file header (metadata) is presented to the user, the audio data is analysed and various audio processing operations are implemented.

## Usage

To use the compiled executable binary, following command line prompt must be used:
'./wav-audio-worker youraudiofile.wav -x (optional)'

### Suported Files

Only uncompressed wave files are supported, meaning linear PCM files in a wav-container:
yourfile.wav
yourfile.wave

### Command line arguments (optional)

*-s  split channels*
Split multichannel wav file to mono wave files, appending numbering of tracks:
yourstereofile.wav -> yourstereofile_1.wav, yourstereofile_2.wav

*-n normalize audio*
Normalize audio to max out the usable amplitude, i.e. normalize to 0dbFS (digital full scale)
yourfile.wav -> yourfile_n.wav

*-p phase invert*
Invert the phase of the given audio file, a.k.a. phase inversion
yourfile.wav -> yourfile_p.wav

### Batch processing

Providing an asterisk instead of an audiofile will batch process all wave files in a given folder
'./wav-audio-worker ./yourpath/* -x (optional)' 

### Installation

Try the compiled version for a linux-based system, run in terminal
Or compile from source ...

## Background

What's under the hood and what specs are used

### Analyzing the header info and metadata

To make sense of the 0s and 1s in the audio data, you need to know how many channels there are and what the bit depth and sample rate are. This information is provided in the file header as follows.

[WAV Header Reference](http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html)

Header structure to follow: xxx

### Analyzing the audio data

Each audio sample of each channel is analyzed and it's respective amplitude is tracked. The provided operations (see [command line arguments](###command-line-arguments-(optional))) are executed on a sample-per-sample basis writing the result to a new file.

### Analyzing additional meta data

Wave files might include additional data to further describe the file or provide copyright information. There are various audio tools that write tags for archiving, searching and tagging to the end of the file. This information is retrieved and presented as output.

