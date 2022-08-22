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

#include "bmp.h"

//--------------- bmpheader member functions -------------------

void bmpheader::read(FILE *fp)
{
  fread(&signature[0], 1, 1, fp);
  fread(&signature[1], 1, 1, fp);
  fread(&filesize, 4, 1, fp);
  fread(&reserved, 4, 1, fp);
  fread(&dataoffset, 4, 1, fp);
#if DEBUG==1
  fprintf(stderr, "=============================\n");
  fprintf(stderr, "Signature = %c%c\n", signature[0], signature[1]);
  fprintf(stderr, "Filesize = %u\n", filesize);
  fprintf(stderr, "Reserved = %u\n", reserved);
  fprintf(stderr, "Dataoffset = %u\n", dataoffset);
#endif
  assert(signature[0] == 'B');
  assert(signature[1] == 'M');
}

bmpheader::bmpheader() {}

void bmpheader::write(unsigned int realfilesize, FILE *fp)
{
  unsigned int realdataoffset = 54;
  fwrite(&signature, 2, 1, fp);
  fwrite(&realfilesize, 4, 1, fp);
  fwrite(&reserved, 4, 1, fp);
  fwrite(&realdataoffset, 4, 1, fp);
}

//------------------- bmpinforheader member functions ------------

void bmpinfoheader::write(FILE *fp)
{
  unsigned short actualbitcount = 24;
  unsigned short actualcompression = 0;
  unsigned int actualimagesize = width * height * 3;
  unsigned int actualcolorsused = 0;
  unsigned int actualcolorsimportant = 0;

  fwrite(&size, 4, 1, fp);
  fwrite(&width, 4, 1, fp);
  fwrite(&height, 4, 1, fp);
  fwrite(&planes, 2, 1, fp);
  fwrite(&actualbitcount, 2, 1, fp);
  fwrite(&actualcompression, 4, 1, fp);
  fwrite(&actualimagesize, 4, 1, fp);
  fwrite(&xpixelsperm, 4, 1, fp);
  fwrite(&ypixelsperm, 4, 1, fp);
  fwrite(&actualcolorsused, 4, 1, fp);
  fwrite(&actualcolorsimportant, 4, 1, fp);
}

void bmpinfoheader::read(FILE *fp)
{
  fread(&size, 4, 1, fp);
  fread(&width, 4, 1, fp);
  fread(&height, 4, 1, fp);
  fread(&planes, 2, 1, fp);
  fread(&bitcount, 2, 1, fp);
  fread(&compression, 4, 1, fp);
  fread(&imagesize, 4, 1, fp);
  fread(&xpixelsperm, 4, 1, fp);
  fread(&ypixelsperm, 4, 1, fp);
  fread(&colorsused, 4, 1, fp);
  fread(&colorsimportant, 4, 1, fp);
#if DEBUG==1
  fprintf(stderr, "=============================\n");
  fprintf(stderr, "Size = %u\n", size);
  fprintf(stderr, "Width = %u\n", width);
  fprintf(stderr, "Height = %u\n", height);
  fprintf(stderr, "Planes = %u\n", planes);
  fprintf(stderr, "Bitcount = %u\n", bitcount);
  fprintf(stderr, "Compression = %u\n", compression);
  fprintf(stderr, "Imagesize = %u\n", imagesize);
  fprintf(stderr, "Xpixelsperm = %u\n", xpixelsperm);
  fprintf(stderr, "Ypixelsperm = %u\n", ypixelsperm);
  fprintf(stderr, "Colorsused = %u\n", colorsused);
  fprintf(stderr, "Colorsimportant = %u\n", colorsimportant);
#endif
}

bmpinfoheader::bmpinfoheader() {}

//------------------- bmpcolortable member functions ------------

void bmpcolortable::dumparray()
{
  // unsigned char * index = array;
  // unsigned count = 0;
  // for (int i = 0; i < 4 * numcolors; i = i + 4) {
  //    unsigned char b = *index;
  //    unsigned char g = *(index + 1);
  //    unsigned char r = *(index + 2);
  //    unsigned char u = *(index + 3);
  //    index = index + 4;
  // }
}

void bmpcolortable::read(int bitsperpixel, FILE *fp)
{
  switch (bitsperpixel) {
    case 1: numcolors = 2;
      break;
    case 4: numcolors = 16;
      break;
    case 8: numcolors = 256;
      break;
    default: assert(false);          // should not be here
  }
  fread(&array, 4 * numcolors, 1, fp);
  dumparray();
}

// Returns pointer to 4 bytes for 24-bit RGB value = 0 value reserved
// index is 0 based, each colortable entry has 4th reserved byte
unsigned char *bmpcolortable::bgr(unsigned int index)
{
  return (unsigned char *)(array + (index * 4));
}

bmpcolortable::bmpcolortable() {}

bmpcolortable::~bmpcolortable() {}

//------------------- bmp member functions -------------------------


//
// Always reads in 4-byte blocks so handles end-line padding
//
unsigned char bmp::getbyte(FILE *fp)
{
  unsigned char ret;
  if (bytesleft == 0) {
    fread(&fourbytebuffer, 1, 4, fp);
    bytesleft = 4;
  }
  bytesleft--;
  ret = (unsigned char) (fourbytebuffer & 255);
  fourbytebuffer >>= 8;
  return ret;
}

//
// returns 4-byte value representing a pixel
// without any colortable expansion
//
unsigned int bmp::getpixel(FILE *fp)
{
  unsigned int ret;
  if (bitsleft == 0) {
    pixelbuffer = getbyte(fp);
    bitsleft = 8;
  }
  switch (infoheader.bitcount) {
    case 1: bitsleft--;
      ret = (unsigned)pixelbuffer & 128;
      pixelbuffer >>= 1;
      break;
    case 2: bitsleft -= 2;
      ret = (unsigned)pixelbuffer & 192;
      pixelbuffer >>= 2;
      break;
    case 4: bitsleft -= 4;
      ret = (unsigned)pixelbuffer & 240;
      pixelbuffer >>= 4;
      break;
    case 8: bitsleft = 0;
      ret = (unsigned)pixelbuffer;
      break;
    case 16: bitsleft = 0;
      ret = (unsigned)(pixelbuffer << 8) | (unsigned) getbyte(fp);
      break;
    case 24: bitsleft = 0;
      ret = ((unsigned)pixelbuffer << 16) |
            ((unsigned)getbyte(fp) << 8) |
            ((unsigned)getbyte(fp));
      break;
    default: assert(1 == 0);
  }

  return ret;
}

//
// writes just the array data in 24-bit bmp format
//
void bmp::writearray(FILE *ofp)
{
  fwrite(array, (infoheader.width * infoheader.height * 3), 1, ofp);
}

//
// reads array data with colortable expansion if necessary
//
void bmp::readarray(bmpcolortable *colortable, FILE *fp)
{
  unsigned char *index = array;
  for (unsigned i = 0; i < infoheader.height; i++) {
    // start a new line ignoring any end padding from the last line
    bytesleft = 0;
    bitsleft = 0;

    for (unsigned j = 0; j < infoheader.width; j++) {
      if (colortable) {
        unsigned char *p = colortable->bgr(getpixel(fp));
        index[2] = *(p+2);
        index[1] = *(p+1);
        index[0] = *p;
        index += 3;
      } else {
        unsigned int p = getpixel(fp);
        index[2] = (unsigned char) (p & 255);
        p >>= 8;
        index[1] = (unsigned char) (p & 255);
        p >>= 8;
        index[0] = (unsigned char) (p & 255);
        index += 3;
      }
    }
  }
}

// reads in a bit-map file on construction
bmp::bmp(const char *f) : filename(f)
{
  FILE *fp = fopen(filename, "rb");

  if (fp==NULL) {
    printf("Error: couldn't open file %s for reading, exiting.\n", f);
    exit(-1);
  }

  header.read(fp);
  infoheader.read(fp);

  // simplify my life by only considering uncompressed images
  assert(infoheader.compression == 0);

  // so for 3 bytes per pixel (RGB)
  array = new unsigned char[3 * infoheader.width * infoheader.height];


  // Need a colortable if <= 8 bits per pixel
  if (infoheader.bitcount <= 8) {
    colortable.read(infoheader.bitcount, fp);
    readarray(&colortable, fp);
  } else {
    // 24-bit color does not need colortable
    assert(infoheader.bitcount == 24); // Not 16 or 32 (or other!)
    readarray(NULL, fp);
  }
  if (fp) {
    fclose (fp);
  }
}

// debug the array data
void bmp::dumparray()
{
  unsigned char *index = array;
  for (unsigned int i = 0; i < infoheader.height; i++) {
    for (unsigned int j = 0; j < infoheader.width; j++) {
      unsigned char r = *index;
      unsigned char g = *(index + 1);
      unsigned char b = *(index + 2);
      fprintf(stderr, "(%u,%u,%u) ", r, g, b);
      index = index + 3;
    }
    fprintf(stderr, "\n");
  }
}

// write out a bmp file called f (in 24-bit irrespective of the resolution read in)
void bmp::write(char *f)
{
  FILE *ofp = fopen(f, "wb");
  if (ofp==NULL) {
    printf("Error: couldn't open file %s for writing, exiting.\n", f);
    exit(-1);
  }
  header.write((infoheader.width * infoheader.height * 3) + 54, ofp);
  infoheader.write(ofp);
  writearray(ofp);
  if (ofp) {
    fclose(ofp);
  }
}

bmp::~bmp() {}

