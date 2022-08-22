/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.2                                                 *
 *                                                                        *
 *  Release Date    : Thu Aug 11 16:24:59 PDT 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.2.9                                               *
 *                                                                        *
 *  Copyright 2020 Siemens                                                *
 *                                                                        *
 **************************************************************************
 *  Licensed under the Apache License, Version 2.0 (the "License");       *
 *  you may not use this file except in compliance with the License.      * 
 *  You may obtain a copy of the License at                               *
 *                                                                        *
 *      http://www.apache.org/licenses/LICENSE-2.0                        *
 *                                                                        *
 *  Unless required by applicable law or agreed to in writing, software   * 
 *  distributed under the License is distributed on an "AS IS" BASIS,     * 
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or       *
 *  implied.                                                              * 
 *  See the License for the specific language governing permissions and   * 
 *  limitations under the License.                                        *
 **************************************************************************
 *                                                                        *
 *  The most recent version of this package is available at github.       *
 *                                                                        *
 *************************************************************************/

#include "assert.h"
#include "bitstream.h"

#include "tables.h"

unsigned short bitstream::swap_bytes(unsigned short b)
{
  unsigned short t = b & 0xFF;
  b >>= 8;
  t <<= 8;
  return (b | t);
}

void bitstream::writebits()
{
  unsigned char o = outbyte;
  writebytes(&o, 1);
}

void bitstream::flush()
{
  if (bytebitcounter == 0) { return; }
  while (bytebitcounter != 8) {
    pushbit(1);
  }
  writebits();
  if (outbyte == 0xFF) {
    outbyte = 0;
    writebits();
  }
  bytebitcounter = 0;
}

void bitstream::pushbit(unsigned char c)   // 1 or 0
{
  // first bit pushed will end up at MSB position
  outbyte <<= 1;
  outbyte |= c;
  bytebitcounter++;
}

// writes numbits of bits out
void bitstream::write(unsigned char numbits, unsigned int bits)
{

  if (numbits>32) {
    assert(numbits <= 32);
  }

  // move msb up to int boundary
  bits <<= (32 - numbits);

  for (unsigned char i = 0; i < numbits; i++) {
    pushbit(bits >> 31);
    if (bytebitcounter == 8) {
      writebits();

      // force 0xFF to not be interpreted as a marker if in middle of data
      // by following it with a zero byte
      if (outbyte == 0xFF) {
        outbyte = 0;
        writebits();
      }

      // reset the counter and the outbyte
      outbyte = 0;
      bytebitcounter = 0;
    }
    bits <<= 1;
  }
}


const unsigned char ff = 0xFF;

// Writes a Start of Image Part
void bitstream::write_SOI()
{
  const unsigned char marker = 0xD8;
  writebytes(&ff, 1);
  writebytes(&marker, 1);
}

// Writes a End of Image Part
void bitstream::write_EOI()
{
  const unsigned char marker = 0xD9;
  writebytes(&ff, 1);
  writebytes(&marker, 1);
}

// Writes a Start of Scan Part
void bitstream::write_SOS()
{
  const unsigned char marker = 0xDA;
  unsigned short length = swap_bytes(6 + 2*3);
  unsigned char numcomponents = 3; // YCbCr

  writebytes(&ff, 1);
  writebytes(&marker, 1);
  writebytes(&length, 2);
  writebytes(&numcomponents, 1);

  unsigned char componentid;
  unsigned char huffmantablenumber; // AC bits 0..3, DC bits 4..7

  componentid = 1; // Y
  huffmantablenumber = 0; // Tables 0 for AC and 0 for DC
  writebytes(&componentid, 1);
  writebytes(&huffmantablenumber, 1);

  componentid = 2; // Cb
  huffmantablenumber = 0x11; // Tables 1 for AC and 1 for DC
  writebytes(&componentid, 1);
  writebytes(&huffmantablenumber, 1);

  componentid = 3; // Cr
  huffmantablenumber = 0x11; // Tables 1 for AC and 1 for DC
  writebytes(&componentid, 1);
  writebytes(&huffmantablenumber, 1);

  // must leave 3 bytes which will be skipped
  unsigned char zeros[3] = {0xFF,0xFF,0xFF};
  writebytes(&zeros, 3);
}

// Writes the Start of Frame 0 part
void bitstream::write_SOF0(unsigned short height, unsigned short width)
{
  const unsigned char marker = 0xC0;
  unsigned short length = swap_bytes(8 + 3*3);
  unsigned char precision = 8;
  unsigned short image_height = swap_bytes(height);
  unsigned short image_width = swap_bytes(width);
  unsigned char numcomponents = 3; // YCbCr

  writebytes(&ff, 1);
  writebytes(&marker, 1);
  writebytes(&length, 2);
  writebytes(&precision, 1);
  writebytes(&image_height, 2);
  writebytes(&image_width, 2);
  writebytes(&numcomponents, 1);

  unsigned char componentid;
  unsigned char samplingfactors = 0x11; // bits 0..3 vertical, 4-7 horizontal
  unsigned char quantizationtablenumber;

  componentid = 1; // Y
  quantizationtablenumber = 0;
  writebytes(&componentid, 1);
  writebytes(&samplingfactors, 1);
  writebytes(&quantizationtablenumber, 1);

  componentid = 2; // Cb
  quantizationtablenumber = 1;
  writebytes(&componentid, 1);
  writebytes(&samplingfactors, 1);
  writebytes(&quantizationtablenumber, 1);

  componentid = 3; // Cr
  quantizationtablenumber = 1;
  writebytes(&componentid, 1);
  writebytes(&samplingfactors, 1);
  writebytes(&quantizationtablenumber, 1);
}

// Write the Quantization tables out
void bitstream::write_DQT()
{
  const unsigned char marker = 0xDB;
  unsigned short length;
  unsigned char qtinfo;

  // Lumanescence quantization table zig-zag reordered
  length = swap_bytes(67+65);
  qtinfo = 0; // bits 4..7 are 0 to denote 8-bit precision
  qtinfo |= 0; // bits 0..3 = number of quantization table
  writebytes(&ff, 1);
  writebytes(&marker, 1);
  writebytes(&length, 2);
  writebytes(&qtinfo, 1);
  writebytes(qvector[LUMA], 64);

  // Chromescence quantization table zig-zag reordered
  qtinfo = 0; // bits 4..7 are 0 to denote 8-bit precision
  qtinfo |= 1; // bits 0..3 = number of quantization table
  writebytes(&qtinfo, 1);
  writebytes(qvector[CHROMA], 64);

}

// Write the Define Huffman Table Part
void bitstream::write_DHT()
{
  const unsigned char marker = 0xC4;
  unsigned short length;
  unsigned char htinfo;

  writebytes(&ff, 1);
  writebytes(&marker, 1);
  length = swap_bytes(2 + 17 + 12 + 17 + 12 + 17 + 162 + 17 + 162);
  writebytes(&length, 2);

  // DC Luminescence
  htinfo = 0x00;  // bits 0-3 = table #, 4 = 0 for DC, 1 for AC, 5-7 not used must be 0s
  writebytes(&htinfo, 1);
  writebytes(dc_lum_code_len, 16);
  writebytes(dc_lum_vals, 12);

  // DC Chrominescence
  htinfo = 0x01;  // bits 0-3 = table #, 4 = 0 for DC, 1 for AC, 5-7 not used must be 0s
  writebytes(&htinfo, 1);
  writebytes(dc_chrom_code_len, 16);
  writebytes(dc_chrom_vals, 12);

  // AC Luminescence
  htinfo = 0x10;  // bits 0-3 = table #, 4 = 0 for DC, 1 for AC, 5-7 not used must be 0s
  writebytes(&htinfo, 1);
  writebytes(ac_lum_code_len, 16);
  writebytes(ac_lum_vals, 162);

  // AC Chrominescence
  htinfo = 0x11;  // bits 0-3 = table #, 4 = 0 for DC, 1 for AC, 5-7 not used must be 0s
  writebytes(&htinfo, 1);
  writebytes(ac_chrom_code_len, 16);
  writebytes(ac_chrom_vals, 162);
}

// Write the JFIF segment marker
void bitstream::write_APP0()
{
  const unsigned char marker = 0xE0;
  unsigned short length = swap_bytes(16);
  unsigned char fileid[5] = { 0x4A, 0x46, 0x49, 0x46, 0x00 }; // JFIF
  unsigned char majorrev = 1;
  unsigned char minorrev = 0;
  unsigned char units = 0;
  unsigned short x_density = swap_bytes(1);
  unsigned short y_density = swap_bytes(1);
  unsigned char thumbnailwidth = 0;
  unsigned char thumbnailheight = 0;

  writebytes(&ff, 1);
  writebytes(&marker, 1);
  writebytes(&length, 2);
  writebytes(fileid, 5);
  writebytes(&majorrev, 1);
  writebytes(&minorrev, 1);
  writebytes(&units, 1);

  writebytes(&x_density, 2);
  writebytes(&y_density, 2);

  writebytes(&thumbnailwidth, 1);
  writebytes(&thumbnailheight, 1);
}

// write a jpeg file upto the actual scan data
void bitstream::writepreamble(unsigned short height, unsigned short width)
{
  write_SOI();
  write_APP0();
  write_DQT();
  write_SOF0(height, width);
  write_DHT();
  write_SOS();
}

// complete the jpeg file
void bitstream::writepostscript()
{
  // make sure we are byte aligned
  flush();
  write_EOI();
}

