MatchLib Toolkit
========

MatchLib Toolkit is a collection of design examples that demonstrate various aspects the MatchLib Connections library for modelling synthesizable hardware.

Each example contains a short README, a Makefile to drive pre-HLS compilation/simulation and a Catapult TCL script for high-level synthesis.

The toolkit examples require numerous open-source packages including:
  SystemC
  MatchLib Connections
  MatchLib
  Boost Preprocessor
  RapidJSON
  AC Dataypes
  AC Math
  AC SimUtils
  BMP IO

All of these packages are already available in a Catapult install tree and will be properly located by the toolkit Makefiles if your CATAPULT_HOME environment variable is set properly. 
However, if you do not have Catapult you can always download all of the required packages using the set_vars.csh (or set_vars.sh) script contained in this repository.

For a complete, standalone environment, do the following
shell> mkdir MCToolkit
shell> cd MCToolkit
shell> git clone http://github.com/hlslibs/matchlib_toolkit.git
shell> cd matchlib_toolkit
shell> source examples/set_vars.csh
  OR
shell> . examples/set_vars.sh
shell> cd examples/<a particular example>
shell> gmake all


