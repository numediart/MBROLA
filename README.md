# Welcome to MBROLA project

__Table of contents__

- [A brief description of MBROLA](#a-brief-description-of-mbrola)
- [Compilation](#compilation)
- [Installation](#installation)
- [Execution](#execution)
  - [Using Pipes](#using-pipes)
  - [Renaming and Cloning phonemes](#renaming-and-cloning-phonemes)
  - [On MS-DOS/Windows](#on-msdoswindows)
  - [On modern Linux](#on-modern-linux)
  - [On Sun4 or with machines with an old audio interface](#on-sun4-or-with-machines-with-an-old-audio-interface)
  - [On VAX or AXP workstations](#on-vax-or-axp-workstations)
- [Format of input and output files - Limitations](#format-of-input-and-output-files---limitations)
  - [Phoneme commands](#phoneme-commands)
  - [Changing the Freq Ratio or Time Ratio](#changing-the-freq-ratio-or-time-ratio)
  - [Flush the output stream](#flush-the-output-stream)
- [Limitations of the program](#limitations-of-the-program)
- [BUGS](#bugs)
- [Feedback](#feedback)
- [License](#license)

# A brief description of MBROLA

MBROLA is a speech synthesizer based on the concatenation of
diphones. It takes a list of phonemes as input, together with prosodic
information (duration of phonemes and a piecewise linear description
of pitch), and produces speech samples on 16 bits (linear), at the
sampling frequency of the diphone database.

Please look at [MBROLA-voices](https://github.com/numediart/MBROLA-voices)
project homepage to get the voices. You may also develop your own voices using
the [MBROLATOR](https://github.com/numediart/MBROLATOR).

It is therefore NOT a Text-To-Speech (TTS) synthesizer, since it does
not accept raw text as input. In order to obtain a full TTS system,
you need to use this synthesizer in combination with a text processing
system that produces phonetic and prosodic commands.

The synthesizer uses a synthesis method known itself as MBROLA.

See more at [development history](Documentation/HISTORY.txt).

# Compilation

You need C compiler and libraries to compile the project. On Debian-based Linux or MinGW
you need to install following packages:

```
sudo apt-get install make gcc
```

To compile your own binary, execute command:

```
make
```

To clean project, execute command:

```
make clean
```

Look for [makefile details](README_Makefile.md), if compilation is not successful.

On Windows, you can build standalone program and mbrola.dll using Microsoft Visual C++ by using project
solution in `VisualC` directory for VC 2015 or later (recommended), or `VisualC6` for older version
from VC 6.0. To build, open `mbrola.sln` on Visual Studio (or `mbrola.dsw` for VC6 version), then build
all projects on solution or you can select either `mbrola` to build mbrola standalone program or
`mbrolalib` to  build mbrola.dll library.

More documentation for developers is located in [Documentation](Documentation) folder
of the project.

# Installation

To make executable file accessible using just its name, copy it to the standard folder for executable files:

```
sudo cp Bin/mbrola /usr/bin/mbrola
```

# Execution

First try:

```
mbrola
```

from `Bin` folder of the project to see the terms and conditions on the use of this software.

Then try:

```
mbrola -h
```

to get some help on how to use the software, e.g.:

```
 USAGE: mbrola [COMMAND LINE OPTIONS] database pho_file+ output_file

A - instead of pho_file or output_file means stdin or stdout
Extension of output_file ( raw, au, wav, aiff ) tells the wanted audio format

Options can be any of the following:
-i = display the database information if any
-e = IGNORE fatal errors on unknown diphone
-c CC = set COMMENT char (escape sequence in pho files)
-F FC = set FLUSH command name
-v VR = VOLUME ratio, float ratio applied to ouput samples
-f FR = FREQ ratio, float ratio applied to pitch points
-t TR = TIME ratio, float ratio applied to phone durations
-l VF = VOICE freq, target freq for voice quality
-R RL = Phoneme RENAME list of the form a A b B ...
-C CL = Phoneme CLONE list of the form a A b B ...

-I IF = Initialization file containing one command per line
 CLONE, RENAME, VOICE, TIME, FREQ, VOLUME, FLUSH, COMMENT,
 and IGNORE are available
-W = store the database in ROM format
-w = the database in a ROM dump
-d = Show list of diphones in the database
```

Now in order to go further, you need to get a version of an MBROLA
language/voice database from the [MBROLA-voices](https://github.com/numediart/MBROLA-voices) page or create one using the [MBROLATOR] (https://github.com/numediart/MBROLATOR). Let us
assume you have copied the FR1 database and referred to
the accompanying fr1.txt file for its installation.

Then try e.g.: `mbrola fr1/fr1 fr1/TEST/bonjour.pho bonjour.wav`

it uses the format:

`mbrola diphone_database command_file1 command_file2 ... output_file`

and creates a sound file for the word 'bonjour'.

Basically the output file is composed of signed integer numbers on 16
bits, corresponding to samples at the sampling frequency of the MBROLA
voice/language database (16 kHz for the diphone database supplied by
the author of MBROLA : Fr1). MBROLA can produce different audio file
formats: .au, .wav, .aiff, .aif, and .raw files depending on the
ouput_file extension. If the extension is not recognized, the format
is RAW (no header). We recommend .wav for Windows, and .au for Unix
platforms.

To display information about the phoneme set used by the database,
type:

```
mbrola -i fr1/fr1
```

It displays the phonetic alphabet as well as copyright information
about the database.

Option `-e` makes Mbrola ignore wrong or missing diphone sequences
(replaced by silence) which can be quite useful when debugging your
TTS. Equivalent to "IGNORE" directive in the initialization file (N.B
replace the obsolete ;;E=OFF , unsupported in .pho file).

Optional parameters let you shorten or lengthen synthetic speech and
transpose it by providing optional time and frequency ratios:

```
mbrola -t 1.2 -f 0.8 fr1/fr1 TEST/bonjour.pho bonjour.wav
```

or its equivalent in the initialization file:

```
TIME 1.2
FREQ 0.8
```

for instance, will result in a RIFF Wav file bonjour.wav 1.2 times
longer than the previous one (slower rate), and containing speech in
which all fundamental frequency values have been multiplied by 0.8
(sounds lower).

You can also set the values of these coefficients directly in a .pho
file by adding special escape sequences like :

```
;; F=0.8
;; T=1.2
```

You can change the voice characteristics with the -l parameter. If the
sampling rate of your database is 16000, indicating -l 18000 allows
you to shorten the vocal tract by a ratio of 16/18 (children voice, or
women voice depending on the voice you're working on). With -l
10000, you can lengthen the vocal tract by a ratio of 16/10 (namely
the voice of a Troll). The same command in an initialization file
becomes "VOICE 10000".

Option `-v` specifies a VolumeRatio which multiplies each output
sample. In the example below, each sample is multiplied by 0.7 (the
loudness goes down). Warning: setting VolumeRatio too high generates
saturation.

```
mbrola -v 0.7 fr1/fr1 TEST/bonjour.pho bonjour.wav
```

or add `VOLUME 0.7` in an initialization file

The `-c` option lets you specify which symbol will be used as an escape
sequence for comments and commands in .pho files. The default value is
the semi-colon `;`, but you may want to change this if your phonetic
alphabet uses this symbol, like in:

```
mbrola -c ! fr1/fr1 TEST/test1.pho test2.pho test.wav
```

equivalent to "COMMENT !" in an initialization file

The `-F` option lets you specify which symbol will be used to Flush the
audio output. The default value is #, you may want to change the
symbol like in:

```
mbrola -F FLUSH_COMMAND fr1/fr1 test.pho test.wav
```

equivalent to "FLUSH FLUSH_COMMAND" in the initialization file.

## Using Pipes

A - instead of command_file or output_file means stdin or stdout. On
multitasking machines, it is easy to run the synthesizer in real time
to obtain audio output from the audio device, by using pipes.

## Renaming and Cloning phonemes

It may happen that the language processing module connected to MBROLA
doesn't use the same phonemic alphabet as the voice used. The Renaming
and Cloning mechanisms help you to quickly solve such problems
(without adding extra CPU load). The only limitation about phoneme
names is that they can't contain blank characters.

If, for instance, phoneme "a" in the mbrola voice you use is called
"my_a" in your alphabet, and phoneme "b" is called "my_b", then the
following command solves the problem:

```
mbrola -R "a my_a b my_b" fr1/fr1 test.pho test.wav
```

You can give as many renaming pairs as you want. Circular definition
are not a problem -> "a b b c" will rename original [a] into [b] and
original [b] into [c] independently ([a] won't be renamed to [c]).

LIMITATION: you can't rename a phoneme into another that already
exists.

The cloning mechanism does exactly the same thing, though the old
phoneme still exists after renaming. This is useful if you have 2
allophones in your alphabet, but the Mbrola voice only provides one.

Imagine for instance, that you make the distinction between the
voiced [r] and its unvoiced counterpart [r0] and that you are using a
syllabic version [r=]. If as a first approximation using [r] for both
is OK, then you may use an Mbrola voice that only provides one version
of [r] by running:

```
mbrola -C "r r0 r r=" fr1/fr1 test.pho test.wav
```

which tells the synthesizer that [r0] and [r=] should be both
synthesized as [r]. You can write a long cloning list of phoneme
pairs to fit your needs.

Renaming and cloning eats CPU since the complete diphone hash table
has to be rebuilt, but once the renaming or cloning has occurred there
is absolutely NO RELATED PERFORMANCE DROP. So using this feature
is more efficient than a pre-processor, though incompatibilities
cannot always be solved by a simple phoneme mapping.

Before renaming anything as #, check paragraph 5.4

When you have long cloning and renaming lists, you can conveniently
write them into an initialization file according to the following
format:

```
RENAME a my_a
RENAME b my_b
CLONE r r0
CLONE r r=
```

The obsolete `;; RENAME a my_a` can't be used in .pho file anymore,
but is correctly parsed in initialization files.

Note to Festival and EN1 users: the consequence of the change above is
that you must change the previous call format `mbrola en1 en1mrpa ...`
into `mbrola -I en1mrpa en1 ...`.


BELOW ARE A NUMBER OF MACHINE DEPENDANT HINTS FOR BEST USING MBROLA

## On MS-DOS/Windows

Type:

```
mbrola fr1/fr1 TEST/bonjour.pho bonjour.wav
```

Then you can play the RIFF Wav file with windows sound utility or
pipes may be used just like below.

## On modern Linux

```
mbrola fr1/fr1 TEST/bonjour.pho -.au | aplay
```

where `aplay` is your audio file player (`aplay` for ALSA
`paplay` for PulseAudio).

If your audioplayer has problems with sun .AU files, try with .raw
Never use .wav format when you pipe the ouput (mbrola can't rewind the
file to write the audio size in the header). Wav format was not
developed for Unix (on the contrary Au format let you specify in the
header "we're on a pipe, read until end of file").

## On Sun4 or with machines with an old audio interface

Those machines are now quite old and only provide a mulaw 8Khz
output. A hack is:

```
mbrola fr1/fr1 input.pho - | sox -t raw -sw -r 16000 - -t raw -Ub -r 8000 - > /dev/audio
```

(providing you have the public domain sox utility developed by Ircam).
You should hear 'bonjour' without the need to create intermediate
files. Note that we strongly recommend that you DON'T use SOX, since
its resampling method (linear interpolation) will permanently damage
the sound.

## On VAX or AXP workstations

To make it easier for users to find MBROLA, you should add the
following command to your system startup procedure:

```
$ DEFINE/SYSTEM/EXEC MBROLA_DIR disk:[dir]

where "disk:[dir]" is the name of the directory you created for the
MBROLA_DIR files. You could also add the following command to your
system login command procedure:

$ MBROLA :== $MBROLA_DIR:MBROLA.EXE
$ RAW2SUN :== $MBROLA_DIR:RAW2SUN.EXE

to use the decsound device:

$ MCR DECSOUND - volume 40 -play sound.au
```

See also the MBR_OLA.COM batch file in the UTILITY.ZIP file available
from the MBROLA Homepage if you cannot play 16 bits sound files on
your machine.


# Format of input and output files - Limitations


## Phoneme commands

The input file bonjour.pho in the above example simply contains :

```
; bonjour
_ 51 25 114
b 62
o~ 127 48 170.42
Z 110 53.5 116
u 211
R 150 50 91
_ 91
```

This shows the format of the input data required by MBROLA. Each line
contains a phoneme name, a duration (in ms), and a series (possibly
none) of pitch targets composed of two float numbers each : the
position of the pitch target within the phoneme (in % of its
total duration), and the pitch value (in Hz) at this position.

In order to increase readability, it is also possible to enclose pitch
target in parentheses. Hence, the first line of bonjour.pho could
be written :

```
_ 51 (25,114)
```

it tells the synthesizer to produce a silence of 51 ms, and to put a
pitch target of 114 Hz at 25% of 51 ms. Pitch targets define a
piecewise linear pitch curve. Notice that the intonation curve they
define is continuous, since the program automatically drops pitch
information when synthesizing unvoiced phones.

The data on each line is separated by blank characters or tabs.
Comments can optionally be introduced in command files, starting with
a semi-colon `;`. This default can be overrun with the `-c` option
of the command line.

Another special escape sequence `;;` allows the user to introduce
commands in the middle of .pho files as described below. This escape
sequence is also affected by the -c option.

## Changing the Freq Ratio or Time Ratio

A command escape sequence containing a line like "T=xx" modifies the
time ratio to xx, the same result is obtained on the fundamental
frequency by replacing T with F, like in:

```
;; T = 1.2
;;F=0.8
```

## Flush the output stream

Note, finally, that the synthesizer outputs chunks of synthetic speech
determined as sections of the piecewise linear pitch curve. Phones
inside a section of this curve are synthesized in one go. The last
phone of each chunk, however, cannot be properly synthesized while the
next phone is not known (since the program uses diphones as base
speech units). When using mbrola with pipes, this may be a
problem. Imagine, for instance, that mbrola is used to create a
pipe-based speaking clock on an HP:

```
speaking_clock | mbrola - -.au | splayer
```

which tells the time, say, every 30 seconds. The last phone of each
time announcement will only be synthesized when the next announcement
starts. To bypass this problem, mbrola accepts a special command
phone, which flushes the synthesis buffer : "#"

This default character can be replaced by another symbol thanks to the
command:

```
;; FLUSH new_flush_symbol
```

Another important issue with piping under UNIX, is the possibility to
prematurely end the audio output, if for example the user presses the
stop button of your application. Since release 3.01, Mbrola handles
signals.

If in the previous example the user wants to interrupt the speaking
clock message, the application just needs to send the USR1 signal. You
can send such a signal from the console with:

```
kill -SIGUSR1 mbrola_process_number
```

Once mbrola catches the signal, it reads its input stream until it
gets EOF or a FLUSH command (hence, surrounding sections with flush is
a good habit).

# Limitations of the program

Phones can be synthesized with a maximum duration which depends on
the fundamental frequency with which they are produced. The higher the
frequency, the lower the duration. For a frequency of 133 Hz, the
maximum duration is 7.5 sec. For a frequency of 66.5 Hz, it is 15 sec.
For a frequency of 266 Hz, it is 3.75 sec.

# Feedback

Report bugs to [MBROLA Issues](https://github.com/numediart/MBROLA/issues) page
on GitHub.

Look at and subscribe to [MBROLA mailing list](https://groups.io/g/mbrola)
to view and discuss other related topics.

# License

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
[GNU Affero General Public License](LICENSE) for more details.
