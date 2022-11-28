/**************************************************************************
 *                                                                        *
 *  Catapult(R) MatchLib Toolkit Example Design Library                   *
 *                                                                        *
 *  Software Version: 1.3                                                 *
 *                                                                        *
 *  Release Date    : Mon Oct 17 12:31:50 PDT 2022                        *
 *  Release Type    : Production Release                                  *
 *  Release Build   : 1.3.0                                               *
 *                                                                        *
 *  Copyright 2022 Siemens                                                *
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

#include <ac_int.h>
#include <mc_connections.h>

/**
 *  * \brief dma command sent to the DMA engine
 *  * (Note the transfered memory contents are in a 32bit address space)
*/
struct dma_cmd {
  // Three fields comprise a dma "command"
  ac_int<32, false> src_addr;   // source address to read from
  ac_int<32, false> dst_addr;   // destination address to write to
  ac_int<32, false> len;        // number of elements to transfer

  // Code required to support marshalling this new data type
  static const unsigned int width = 32 + 32 + 32;  // must be sum of above three fields
  template <unsigned int Size> void Marshall(Marshaller<Size> &m) {
    m &src_addr;
    m &dst_addr;
    m &len;
  }

  // Code to support tracing and general I/O
  inline friend void sc_trace(sc_trace_file *tf, const dma_cmd &v, const std::string &NAME ) {
    sc_trace(tf,v.src_addr,  NAME + ".src_addr");
    sc_trace(tf,v.dst_addr,  NAME + ".dst_addr");
    sc_trace(tf,v.len,       NAME + ".len");
  }
  inline friend std::ostream &operator<<(ostream &os, const dma_cmd &rhs) {
    os << rhs.src_addr << " " << rhs.dst_addr << " " << rhs.len;
    return os;
  }
  inline bool operator==(const dma_cmd &other) { return ((src_addr == other.src_addr) && (dst_addr == other.dst_addr) && (len == other.len)); }
};

/**
 *  * \brief dma address map as seen by the CPU (32bit address space)
 *  *     Address    Register
 *  *     0x0000     src_addr
 *  *     0x0004     dst_addr
 *  *     0x0008     len
 *  *     0x000C     start
*/
struct dma_address_map {
  uint32_t  src_addr;
  uint32_t  dst_addr;
  uint32_t  len;
  uint32_t  start;
};

