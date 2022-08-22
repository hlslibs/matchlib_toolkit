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

#pragma once

//#include <process.h>
#include <iostream>
#include <fstream>
#include <mc_connections.h>
#include "bmp_io.hpp"
#include "edge_alg.h"
using namespace std;

void display_bmp_image(const std::string image_file)
{
#ifdef HAVE_DISPLAY
  std::string display_cmd("display ");
  display_cmd.append(image_file);
  display_cmd.append("&");
  std::system(display_cmd.c_str());
#endif
}

SC_MODULE(testbench)
{
  sc_in_clk clk;
  sc_out<bool> rst_bar;
  Connections::Out<uint8>   CCS_INIT_S1(dat_in);
  Connections::In<uint9> CCS_INIT_S1(magn);
  Connections::In<ac_fixed<8,3> > CCS_INIT_S1(angle);

  SC_HAS_PROCESS(testbench);
  testbench(const sc_module_name& name, const std::string &input_image_bmp) :
    rst_bar("rst_bar"),
    dat_in("dat_in"),
    magn("magn"),
    angle("angle"),
    d_input_image_bmp(input_image_bmp) {
    SC_THREAD(stimulus);
    sensitive << clk.pos();
    SC_THREAD(response);
    sensitive << clk.pos();
    dat_in_orig = new unsigned char[864*1296];
    rarray = new unsigned char[864*1296];
    garray = new unsigned char[864*1296];
    barray = new unsigned char[864*1296];
    magn_orig = new double[864*1296];
    angle_orig = new double[864*1296];
  }

  void stimulus() {
    EdgeAlg<1296,864> inst0;
    unsigned long int width=1296;
    long int height=864;
    cout << "Loading input image file '" << d_input_image_bmp << "'" << endl;
    bmp_read((char *)d_input_image_bmp.c_str(), &width,  &height, &rarray,&garray,&barray);
    display_bmp_image(d_input_image_bmp);
    dat_in.Reset();
    // reset sequence
    rst_bar = 0 ;
    wait(3); // three clock cycles          // WAIT
    rst_bar = 1 ;
    wait();                                 // WAIT

    int cnt=0;
    for (int i=0; i<864; i++) {
      for (int j=0; j<1296; j++) {
        dat_in_orig[cnt] = rarray[cnt];
        cnt++;
      }
    }
    // Run the reference algorithm
    inst0.run(dat_in_orig,magn_orig,angle_orig);
    cnt = 0;
    for (int i=0; i<864; i++) {
      wait(100);                            // WAIT
      for (int j=0; j<1296; j++) {
        dat_in.Push(rarray[cnt]);
        cnt++;
        wait(1);                            // WAIT
      }
    }
    wait(1000000);                          // WAIT
  }

  void response() {
    magn.Reset();
    angle.Reset();
    wait(4);                                // WAIT
    int cnt = 0;
    float sumErr = 0;
    float sumAngErr = 0;
    // check error between original and new
    for (int i=0; i<864; i++) {
      for (int j=0; j<1296; j++) {
        int hw = magn.Pop();
        int alg = (int)*(magn_orig+cnt);

        int diff = alg-hw;
        int adiff = abs(diff);
        sumErr += adiff;
        float angO = (double)*(angle_orig+cnt);
        float angHw = angle.Pop().to_double();
        float angAdiff = abs(angO-angHw);
        sumAngErr += angAdiff;
        cnt++;
        rarray[cnt] = hw;
        garray[cnt] = alg;
      }
    }

    printf("Magnitude: Manhattan norm per pixel %f\n",sumErr/(864*1296));
    printf("Angle: Manhattan norm per pixel %f\n",sumAngErr/(864*1296));

    cout << "Writing reference output image to ./Gradient_Magnitude_Original.bmp" << endl;
    bmp_24_write ("./Gradient_Magnitude_Original.bmp", 1296,  864, garray, garray, garray);
    display_bmp_image("./Gradient_Magnitude_Original.bmp");

    cout << "Writing DUT output image to ./Gradient_Magnitude.bmp" << endl;
    bmp_24_write ("./Gradient_Magnitude.bmp", 1296,  864, rarray, rarray, rarray);
    display_bmp_image("./Gradient_Magnitude.bmp");

    delete (dat_in_orig);
    delete (magn_orig);
    delete (angle_orig);
    delete (rarray);
    delete (garray);
    delete (barray);
    cout << "Finished" << endl;
    sc_stop();
  }

  private:
  unsigned char *dat_in_orig;
  unsigned char *rarray;
  unsigned char *garray;
  unsigned char *barray;
  double *magn_orig;
  double *angle_orig;
  std::string d_input_image_bmp;
};

