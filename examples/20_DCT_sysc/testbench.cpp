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

#include "testbench.h"

void testbench::stimulus()
{
  blocktype.Reset();
  rgbstream.Reset();
  // Wait for reset to finish in core
  do { wait(); }
  while (!rst.read());
  using std::cout;
  using std::endl;

  cout << "Loading input image file '" << d_input_image_bmp << "'" << endl;

  bmp *bitmap = new bmp(d_input_image_bmp.c_str());

  assert(bitmap != NULL);

  // rows and columns must be divisible by 8, so make bigger if necessary
  if (bitmap->rows() % 8) {
    printf("  ERROR: number of rows (image width) must be divisible by 8. please resize original bitmap image\n");
    exit(-1);
  }
  if (bitmap->columns() % 8) {
    printf("  ERROR: number of columns (image height) must be divisible by 8. please resize original bitmap image\n");
    exit(-1);
  }

  unsigned int img_rows = bitmap->rows();
  unsigned int img_columns = bitmap->columns();

  img_dat.write(img_rows);
  img_dat.write(img_columns);

  rgb_t         rgb[64];           // 8x8 RGB block (R:0, G:1, B:2)

  // for every 8x8 pixel block in the bitmap image
  for (unsigned i = 0; i < img_rows; i += 8) {
    for (unsigned j = 0; j < img_columns; j += 8) {

      // extract 8x8 RGB block from bitmap array
      for (unsigned int x=0; x<8; x++) {
        for (unsigned int y=0; y<8; y++) {
          rgb[x*8+y].r = bitmap->red(i+x, j+y);
          rgb[x*8+y].g = bitmap->green(i+x, j+y);
          rgb[x*8+y].b = bitmap->blue(i+x, j+y);
        }
      }

      // process Y, Cb and Cr blocks, one after the other
      for (uint2 type = 0; type <= 2; type++) {
        blocktype.Push(type);
        for (int i=0; i<64; i++) {
          rgbstream.Push(rgb[i]);
        }
      }
    }
  }
  //flush last macro-block
  for (uint2 type = 0; type <= 2; type++) {
    blocktype.Push(type);
    for (int i=0; i<64; i++) {
      rgbstream.Push(rgb[i]);
    }
  }

  wait(40000);
  SC_REPORT_ERROR("MGC/Catapult", "ERROR: Monitor process did not stop simulation \n");
  sc_stop();
  wait();
}

void testbench::monitor()
{
  hufstream.Reset();
  codes_t       codes[64];         // array of Huffman codes
  // Wait for reset to finish in core
  do { wait(); }
  while (!rst.read());

  std::string outfile(d_input_image_bmp);
  size_t lastindex = outfile.find_last_of("/");
  outfile = outfile.substr(lastindex+1);
  outfile.append(".jpg");
  bitstream fp(outfile.c_str());
  cout << " Compressing to JPG file " << outfile << endl;

  unsigned int img_rows = img_dat.read();
  unsigned int img_columns = img_dat.read();

  // write out all the header info for the jpeg
  fp.writepreamble(img_rows,img_columns);

  // for every 8x8 pixel block in the bitmap image
  for (unsigned i = 0; i < img_rows; i += 8) {
    for (unsigned j = 0; j < img_columns; j += 8) {
#ifdef DEBUG
      printf("reading 8x8 %d %d \n",i,j);
#endif
      // process Y, Cb and Cr blocks, one after the other
      for (uint2 type = 0; type <= 2; type++) {
        for (int i=0; i<64; i++) {
          codes[i] = hufstream.Pop();
        }

        // write out the bitstream
        for (int k=0; k<64; k++) { 
		    fp.write(codes[k].size, codes[k].code); 
		  }
      }
    }
    printf("Finished row %3d out of %3d...", i, img_rows);
    cout << endl;
  }

  // write out trailing info for the jpeg
  fp.writepostscript();

  cout << " ...Done" << endl << endl;
  CCS_LOG("SIMULATION PASSED");
  CCS_LOG("stopping simulation");

  sc_stop();
  wait();
}

