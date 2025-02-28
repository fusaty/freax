.. SPDX-License-Identifier: GPL-2.0

==============================
Using the Digital TV Framework
==============================

Introduction
~~~~~~~~~~~~

One significant difference between Digital TV and Analogue TV that the
unwary (like myself) should consider is that, although the component
structure of DVB-T cards are substantially similar to Analogue TV cards,
they function in substantially different ways.

The purpose of an Analogue TV is to receive and display an Analogue
Television signal. An Analogue TV signal (otherwise known as composite
video) is an analogue encoding of a sequence of image frames (25 frames
per second in Europe) rasterised using an interlacing technique.
Interlacing takes two fields to represent one frame. Therefore, an
Analogue TV card for a PC has the following purpose:

* Tune the receiver to receive a broadcast signal
* demodulate the broadcast signal
* demultiplex the analogue video signal and analogue audio
  signal.

  .. note::

     some countries employ a digital audio signal
     embedded within the modulated composite analogue signal -
     using NICAM signaling.)

* digitize the analogue video signal and make the resulting datastream
  available to the data bus.

The digital datastream from an Analogue TV card is generated by
circuitry on the card and is often presented uncompressed. For a PAL TV
signal encoded at a resolution of 768x576 24-bit color pixels over 25
frames per second - a fair amount of data is generated and must be
processed by the PC before it can be displayed on the video monitor
screen. Some Analogue TV cards for PCs have onboard MPEG2 encoders which
permit the raw digital data stream to be presented to the PC in an
encoded and compressed form - similar to the form that is used in
Digital TV.

The purpose of a simple budget digital TV card (DVB-T,C or S) is to
simply:

* Tune the received to receive a broadcast signal. * Extract the encoded
  digital datastream from the broadcast signal.
* Make the encoded digital datastream (MPEG2) available to the data bus.

The significant difference between the two is that the tuner on the
analogue TV card spits out an Analogue signal, whereas the tuner on the
digital TV card spits out a compressed encoded digital datastream. As
the signal is already digitised, it is trivial to pass this datastream
to the PC databus with minimal additional processing and then extract
the digital video and audio datastreams passing them to the appropriate
software or hardware for decoding and viewing.

Getting the card going
~~~~~~~~~~~~~~~~~~~~~~

The Device Driver API for DVB under freax will the following
device nodes via the devfs filesystem:

* /dev/dvb/adapter0/demux0
* /dev/dvb/adapter0/dvr0
* /dev/dvb/adapter0/frontend0

The ``/dev/dvb/adapter0/dvr0`` device node is used to read the MPEG2
Data Stream and the ``/dev/dvb/adapter0/frontend0`` device node is used
to tune the frontend tuner module. The ``/dev/dvb/adapter0/demux0`` is
used to control what programs will be received.

Depending on the card's feature set, the Device Driver API could also
expose other device nodes:

* /dev/dvb/adapter0/ca0
* /dev/dvb/adapter0/audio0
* /dev/dvb/adapter0/net0
* /dev/dvb/adapter0/osd0
* /dev/dvb/adapter0/video0

The ``/dev/dvb/adapter0/ca0`` is used to decode encrypted channels. The
other device nodes are found only on devices that use the av7110
driver, with is now obsoleted, together with the extra API whose such
devices use.

Receiving a digital TV channel
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section attempts to explain how it works and how this affects the
configuration of a Digital TV card.

On this example, we're considering tuning into DVB-T channels in
Australia, at the Melbourne region.

The frequencies broadcast by Mount Dandenong transmitters are,
currently:

Table 1. Transponder Frequencies Mount Dandenong, Vic, Aus.

===========	===========
Broadcaster	Frequency
===========	===========
Seven		177.500 Mhz
SBS		184.500 Mhz
Nine		191.625 Mhz
Ten		219.500 Mhz
ABC		226.500 Mhz
Channel 31	557.625 Mhz
===========	===========

The digital TV Scan utilities (like dvbv5-scan) have use a set of
compiled-in defaults for various countries and regions. Those are
currently provided as a separate package, called dtv-scan-tables. It's
git tree is located at freaxTV.org:

    https://git.freaxtv.org/dtv-scan-tables.git/

If none of the tables there suit, you can specify a data file on the
command line which contains the transponder frequencies. Here is a
sample file for the above channel transponders, in the old "channel"
format::

	# Data file for DVB scan program
	#
	# C Frequency SymbolRate FEC QAM
	# S Frequency Polarisation SymbolRate FEC
	# T Frequency Bandwidth FEC FEC2 QAM Mode Guard Hier

	T 177500000 7MHz AUTO AUTO QAM64 8k 1/16 NONE
	T 184500000 7MHz AUTO AUTO QAM64 8k 1/8 NONE
	T 191625000 7MHz AUTO AUTO QAM64 8k 1/16 NONE
	T 219500000 7MHz AUTO AUTO QAM64 8k 1/16 NONE
	T 226500000 7MHz AUTO AUTO QAM64 8k 1/16 NONE
	T 557625000 7MHz AUTO AUTO QPSK 8k 1/16 NONE

Nowadays, we prefer to use a newer format, with is more verbose and easier
to understand. With the new format, the "Seven" channel transponder's
data is represented by::

	[Seven]
		DELIVERY_SYSTEM = DVBT
		FREQUENCY = 177500000
		BANDWIDTH_HZ = 7000000
		CODE_RATE_HP = AUTO
		CODE_RATE_LP = AUTO
		MODULATION = QAM/64
		TRANSMISSION_MODE = 8K
		GUARD_INTERVAL = 1/16
		HIERARCHY = NONE
		INVERSION = AUTO

For an updated version of the complete table, please see:

    https://git.freaxtv.org/dtv-scan-tables.git/tree/dvb-t/au-Melbourne

When the Digital TV scanning utility runs, it will output a file
containing the information for all the audio and video programs that
exists into each channel's transponders which the card's frontend can
lock onto. (i.e. any whose signal is strong enough at your antenna).

Here's the output of the dvbv5 tools from a channel scan took from
Melburne::

    [ABC HDTV]
	    SERVICE_ID = 560
	    VIDEO_PID = 2307
	    AUDIO_PID = 0
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 226500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 3/4
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [ABC TV Melbourne]
	    SERVICE_ID = 561
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 226500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 3/4
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [ABC TV 2]
	    SERVICE_ID = 562
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 226500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 3/4
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [ABC TV 3]
	    SERVICE_ID = 563
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 226500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 3/4
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [ABC TV 4]
	    SERVICE_ID = 564
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 226500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 3/4
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [ABC DiG Radio]
	    SERVICE_ID = 566
	    VIDEO_PID = 0
	    AUDIO_PID = 2311
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 226500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 3/4
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [TEN Digital]
	    SERVICE_ID = 1585
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 219500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [TEN Digital 1]
	    SERVICE_ID = 1586
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 219500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [TEN Digital 2]
	    SERVICE_ID = 1587
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 219500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [TEN Digital 3]
	    SERVICE_ID = 1588
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 219500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [TEN Digital]
	    SERVICE_ID = 1589
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 219500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [TEN Digital 4]
	    SERVICE_ID = 1590
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 219500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [TEN Digital]
	    SERVICE_ID = 1591
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 219500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [TEN HD]
	    SERVICE_ID = 1592
	    VIDEO_PID = 514
	    AUDIO_PID = 0
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 219500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [TEN Digital]
	    SERVICE_ID = 1593
	    VIDEO_PID = 512
	    AUDIO_PID = 650
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 219500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [Nine Digital]
	    SERVICE_ID = 1072
	    VIDEO_PID = 513
	    AUDIO_PID = 660
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 191625000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [Nine Digital HD]
	    SERVICE_ID = 1073
	    VIDEO_PID = 512
	    AUDIO_PID = 0
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 191625000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [Nine Guide]
	    SERVICE_ID = 1074
	    VIDEO_PID = 514
	    AUDIO_PID = 670
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 191625000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 3/4
	    CODE_RATE_LP = 1/2
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/16
	    HIERARCHY = NONE

    [7 Digital]
	    SERVICE_ID = 1328
	    VIDEO_PID = 769
	    AUDIO_PID = 770
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 177500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [7 Digital 1]
	    SERVICE_ID = 1329
	    VIDEO_PID = 769
	    AUDIO_PID = 770
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 177500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [7 Digital 2]
	    SERVICE_ID = 1330
	    VIDEO_PID = 769
	    AUDIO_PID = 770
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 177500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [7 Digital 3]
	    SERVICE_ID = 1331
	    VIDEO_PID = 769
	    AUDIO_PID = 770
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 177500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [7 HD Digital]
	    SERVICE_ID = 1332
	    VIDEO_PID = 833
	    AUDIO_PID = 834
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 177500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [7 Program Guide]
	    SERVICE_ID = 1334
	    VIDEO_PID = 865
	    AUDIO_PID = 866
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 177500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [SBS HD]
	    SERVICE_ID = 784
	    VIDEO_PID = 102
	    AUDIO_PID = 103
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 536500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [SBS DIGITAL 1]
	    SERVICE_ID = 785
	    VIDEO_PID = 161
	    AUDIO_PID = 81
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 536500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [SBS DIGITAL 2]
	    SERVICE_ID = 786
	    VIDEO_PID = 162
	    AUDIO_PID = 83
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 536500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [SBS EPG]
	    SERVICE_ID = 787
	    VIDEO_PID = 163
	    AUDIO_PID = 85
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 536500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [SBS RADIO 1]
	    SERVICE_ID = 798
	    VIDEO_PID = 0
	    AUDIO_PID = 201
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 536500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE

    [SBS RADIO 2]
	    SERVICE_ID = 799
	    VIDEO_PID = 0
	    AUDIO_PID = 202
	    DELIVERY_SYSTEM = DVBT
	    FREQUENCY = 536500000
	    INVERSION = OFF
	    BANDWIDTH_HZ = 7000000
	    CODE_RATE_HP = 2/3
	    CODE_RATE_LP = 2/3
	    MODULATION = QAM/64
	    TRANSMISSION_MODE = 8K
	    GUARD_INTERVAL = 1/8
	    HIERARCHY = NONE
