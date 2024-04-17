// INSERT_EULA_COPYRIGHT: 2020

#pragma once

#include <stdio.h>

class bitstream
{
private:

  unsigned char bytebitcounter;
  FILE *fp;
  unsigned char outbyte;

  void pushbit(unsigned char c);
  void writebits();

  // utility to swap byte order in a short (intel to moto)
  unsigned short swap_bytes(unsigned short b);

  void flush();

  // write bytes into the output stream
  inline void writebytes(const void *ptr, unsigned char bytestowrite) {
    fwrite(ptr, bytestowrite, 1, fp);
  }

  // Writes a Start of Image Part
  void write_SOI();
  // Writes a End of Image Part
  void write_EOI();
  // Writes a Start of Scan Part
  void write_SOS();
  // Writes the Start of Frame 0 part
  void write_SOF0(unsigned short height, unsigned short width);
  // Write the Quantization tables out
  void write_DQT();
  // Write the Define Huffman Table Part
  void write_DHT();
  // Write the JFIF segment marker
  void write_APP0();

public:

  // write numbits of bits onto the output stream
  void write(unsigned char numbits, unsigned int bits);

  // write a jpeg file's preable before encoded bits
  void writepreamble(unsigned short height, unsigned short width);
  // complete the jpeg file
  void writepostscript();
  // close the jpeg file
  void close() {
    fclose(fp);
  }

  inline  bitstream(const char *filename) : bytebitcounter(0), outbyte(0) { fp = fopen(filename, "wb"); }
  inline ~bitstream() { close(); }
};

