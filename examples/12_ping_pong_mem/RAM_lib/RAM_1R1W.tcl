flow package require MemGen
flow run /MemGen/MemoryGenerator_BuildLib {
VENDOR           *
RTLTOOL          OasysRTL
TECHNOLOGY       *
LIBRARY          RAM_1R1W
MODULE           RAM_1R1W
OUTPUT_DIR       .
FILES {
  { FILENAME RAM_1R1W.vhd FILETYPE VHDL    MODELTYPE generic PARSE 1 PATHTYPE copy STATICFILE 1 VHDL_LIB_MAPS mgc_hls }
  { FILENAME RAM_1R1W.v   FILETYPE Verilog MODELTYPE generic PARSE 1 PATHTYPE copy STATICFILE 1                       }
}
VHDLARRAYPATH    {}
WRITEDELAY       0.1
INITDELAY        1
READDELAY        0.5
VERILOGARRAYPATH {}
INPUTDELAY       0.01
WIDTH            width
AREA             words*width
WRITELATENCY     1
RDWRRESOLUTION   UNKNOWN
READLATENCY      1
DEPTH            words
PARAMETERS {
  { PARAMETER words      TYPE hdl IGNORE 0 MIN 0 MAX {} DEFAULT 0 }
  { PARAMETER width      TYPE hdl IGNORE 0 MIN 0 MAX {} DEFAULT 0 }
  { PARAMETER addr_width TYPE hdl IGNORE 0 MIN 0 MAX {} DEFAULT 0 }
}
PORTS {
  { NAME rd0 MODE Read  }
  { NAME wr1 MODE Write }
}
PINMAPS {
  { PHYPIN CK   LOGPIN CLOCK        DIRECTION in  WIDTH 1.0        PHASE 1  DEFAULT {} PORTS {rd0 wr1} }
  { PHYPIN RCSN LOGPIN READ_ENABLE  DIRECTION in  WIDTH 1.0        PHASE 0  DEFAULT {} PORTS rd0       }
  { PHYPIN WCSN LOGPIN WRITE_ENABLE DIRECTION in  WIDTH 1.0        PHASE 0  DEFAULT {} PORTS wr1       }
  { PHYPIN RA   LOGPIN ADDRESS      DIRECTION in  WIDTH addr_width PHASE {} DEFAULT {} PORTS rd0       }
  { PHYPIN WA   LOGPIN ADDRESS      DIRECTION in  WIDTH addr_width PHASE {} DEFAULT {} PORTS wr1       }
  { PHYPIN D    LOGPIN DATA_IN      DIRECTION in  WIDTH width      PHASE {} DEFAULT {} PORTS wr1       }
  { PHYPIN Q    LOGPIN DATA_OUT     DIRECTION out WIDTH width      PHASE {} DEFAULT {} PORTS rd0       }
}

}
