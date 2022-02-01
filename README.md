# MatchLib Toolkit

MatchLib Toolkit is a collection of design examples that demonstrate various aspects the MatchLib Connections library for modelling synthesizable hardware.

Each example contains a short README, a Makefile to drive pre-HLS compilation/simulation and a Catapult TCL script for high-level synthesis.

## Downloading
To download the MatchLib Toolkit examples:
> shell> mkdir MCToolkit<br>
> shell> cd MCToolkit<br>
> shell> git clone http://github.com/hlslibs/matchlib_toolkit.git<br>

The toolkit examples require numerous open-source packages including:
- SystemC
- MatchLib Connections
- MatchLib
- Boost Preprocessor
- RapidJSON
- AC Dataypes
- AC Math
- AC SimUtils
- BMP IO

All of these packages are already available in a Catapult install tree and will be properly located by the toolkit Makefiles if your CATAPULT_HOME environment variable is set properly. 
However, if you do not have Catapult you can always download all of the required packages using the set_vars.csh (or set_vars.sh) script contained in this repository.

For a complete, standalone environment, do the following
> shell> cd MCToolkit/matchlib_toolkit<br>
> shell> source examples/set_vars.csh<br>
    OR<br>
> shell> . examples/set_vars.sh<br>

The first time the set_vars.* script is sourced it will download into your current directory all of the required open-source packages. It will also compile the OSCI SystemC simulator.
Subsequent sourcing of set_vars.* from the same working directory will just point to the downloaded packages.
You will need to source this script in each new shell when you want to run an example.


## Running an example to test the environment
Now try running the first example '01_and_gate' in order to verify the environment:<br>
> shell> cd examples/01_and_gate<br>
> shell> gmake all<br><br>

You should see:<br>
> ./sim_sc<br><br>
>         SystemC 2.3.2-Accellera --- Feb  1 2022 11:27:36<br>
>         Copyright (c) 1996-2017 by all Contributors,<br>
>         ALL RIGHTS RESERVED<br>
> 0 s top Stimulus started<br><br>
> Info: (I702) default timescale unit used for tracing: 1 ps (trace.vcd)<br>
> 12 ns top stopping sim at end of stimulus<br><br>
> Info: /OSCI/SystemC: Simulation stopped by user.<br>
> Simulation PASSED

## Documentation
It is recommended that you browse all of the documents in matchlib_toolkit/doc

#Waveform Viewing
You can use Questa (vsim), VCS, or similar simulators to view the trace.vcd files produced from the SystemC simulations in the examples. Alternatively you can download the open source gtkwave waveform viewer (google gtkwave).

## Cleanup
You can clean up the results from all runs using:<br>
> shell> cd MCToolkit/matchlib_toolkit/examples<br>
> shell> sh clean_all.sh<br>

