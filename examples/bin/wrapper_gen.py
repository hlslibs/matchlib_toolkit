#!/usr/bin/python2

# This example code is unsupported and untested. 

import sys
import os
import re
import subprocess
import argparse
from time import gmtime, strftime


def wait_prompt(proc):
  ret = []
  while True:
    ln = proc.stdout.readline()
    # print('wait: ' + ln)
    if ln.startswith('(gdb)'):
      return ret
    ret.append(ln)

def start_gdb(exec_name):
  proc = subprocess.Popen(
    ['gdb', '--interpreter=mi', exec_name],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
    universal_newlines=True)

  wait_prompt(proc)

  return proc

def stop_gdb(proc):
  proc.stdin.close()
  proc.stdout.close()
  if proc.wait() != 0:
    print("non-zero gdb return code")

def main():
  parser = argparse.ArgumentParser(description='Generate Verilog Wrapper around SystemC DUT.')
  parser.add_argument('--clock_name',help='Name the clock')
  parser.add_argument('--start_clk_low',help='Default the Connections clk to start low')
  parser.add_argument('--clock_period',help='Define the clock period')
  parser.add_argument('--not_top_level',help='When true, disable trace_hierarchy code')
  parser.add_argument('exec_name',help='Executable name')
  parser.add_argument('obj_name',help='Simulation path to instance to wrap')
  parser.add_argument('--exec_args',help='Optional argc/argv required by sc_main')
  parser.add_argument('--header',help='Optional override the header file to include for definition of instance')

  args = parser.parse_args()

  exec_name = args.exec_name
  run_args = ""
  if args.exec_args != None:
    run_args = args.exec_args
    print('Run args: "' + run_args + '"')

  if args.header != None:
    print('Header filename override: "' + args.header + '"')

  path = args.obj_name

  is_top_level = False
  clock_period = '1'
  clock_name = 'clk'

  if args.not_top_level == None:
    is_top_level = True

  if args.clock_period == None:
    print('You must specify top level clock period')
    return

  if args.clock_name == None:
    print('You must specify top level clock name')
    return

  clock_period = args.clock_period
  clock_name = args.clock_name

  if os.path.isfile(exec_name) != True:
    print('File ' + exec_name + ' does not exist')
    return

  proc = start_gdb(exec_name)

  proc.stdin.write('help\n')
  help = wait_prompt(proc)
  # print(help)

  fname = path + '_portlist.txt'

  proc.stdin.write('set max-value-size unlimited\n')
  wait_prompt(proc)

  proc.stdin.write('br sc_core::sc_start\n')
  brkpt = wait_prompt(proc)
  brkpt_found = False
  for line in brkpt:
    if line.find('breakpoint-created') != -1:
      brkpt_found = True

  if brkpt_found != True:
    print('Could not set breakpoint for sc_core::sc_start')
    return

  proc.stdin.write('run' + run_args + '\n')
  wait_prompt(proc)
  wait_prompt(proc)
  proc.stdin.write('delete 1\n')
  wait_prompt(proc)
  proc.stdin.write('up\n')
  wait_prompt(proc)
  proc.stdin.write('ptype ' + path + '\n')
  tdecl = wait_prompt(proc)

  typename = ''
  for line in tdecl:
    if line.find('type = ') != -1:
      line = line.replace('~"', '').replace('type = class', '').replace('type = struct', '').strip().lstrip()
      line_split = line.split(' ')
      typename = line_split[0]

  if typename == '':
    print('Object ' + path + ' not found: check pathname, also make sure to use: g++ -g -O0')
    return

  type_split = typename.split('::')
  typename = type_split[-1]

  scin = []
  scout = []
  ports_class = []
  for line in tdecl:
    if line.find('sc_in<') != -1:
      line = line.replace('class', '')
      line = line.replace(';\\n"', '').replace('~"', '').replace(', ', ',').replace(' >', '>').rstrip().lstrip()
      scin.append(line.split(' '))
    if line.find('sc_out<') != -1:
      line = line.replace('class', '')
      line = line.replace(';\\n"', '').replace('~"', '').replace(', ', ',').replace(' >', '>').rstrip().lstrip()
      scout.append(line.split(' '))

    if line.find('_ports') != -1:
      line = line.replace(';\\n"', '').replace('~"', '').replace(', ', ',').replace(' >', '>').rstrip().lstrip()
      ports_class.append(line.split(' ')[-1])
      

  for pc in ports_class:
    proc.stdin.write('ptype ' + path + '.' + pc + '\n')
    lines = wait_prompt(proc)
    for line in lines:
      if line.find('sc_in<') != -1:
        line = line.replace('class', '')
        line = line.replace(';\\n"', '').replace('~"', '').replace(', ', ',').replace(' >', '>').rstrip().lstrip()
        split = line.split(' ')
        scin.append([split[0], pc + '_' + split[1]])
      if line.find('sc_out<') != -1:
        line = line.replace('class', '')
        line = line.replace(';\\n"', '').replace('~"', '').replace(', ', ',').replace(' >', '>').rstrip().lstrip()
        split = line.split(' ')
        scout.append([split[0], pc + '_' + split[1]])
  
  proc.stdin.write('p/x Connections::port_scan("' + path + '", "' + fname + '")\n')
  ret = wait_prompt(proc)
  proc.stdin.write('\n')
  wait_prompt(proc)

  for line in ret:
    if line.find('No symbol') != -1:
      print('Connections::port_scan function not found in executable - make sure to use: g++ -g -O0')
      return

  with open(fname, 'r') as f:
    lines = f.readlines()

  os.remove(fname)

  master = []
  for line in lines:
    master.append(line.replace('_dat\n', '').replace('__', '.').replace(path + '.', '').split())

  scdecs = []
  for i in scin:
    scdecs.append('  ' + i[0] + '  CCS_INIT_S1(' + i[1] + ');');
  for i in scout:
    scdecs.append('  ' + i[0] + '  CCS_INIT_S1(' + i[1] + ');');

  for m in master:
    dat = m[2] + '.dat'
    dat = dat.replace('.', '_')
    vld = m[2] + '.vld'
    vld = vld.replace('.', '_')
    rdy = m[2] + '.rdy'
    rdy = rdy.replace('.', '_')
    if m[0] == 'Out':
     scdecs.append('  sc_out<sc_lv<' + m[1] + '>> ' + 'CCS_INIT_S1(' + dat + ');' )
     scdecs.append('  sc_out<bool> ' + '    CCS_INIT_S1(' + vld + ');' )
     scdecs.append('  sc_in<bool> ' + '     CCS_INIT_S1(' + rdy + ');' )
    else:
     scdecs.append('  sc_in<sc_lv<' + m[1] + '>> ' + 'CCS_INIT_S1(' + dat + ');' )
     scdecs.append('  sc_in<bool> ' + '     CCS_INIT_S1(' + vld + ');' )
     scdecs.append('  sc_out<bool> ' + '    CCS_INIT_S1(' + rdy + ');' )

  scbinds = []
  for i in scin:
    nm = i[1]
    nm = nm.replace('ports_', 'ports.')
    scbinds.append('    ' + typename + '_inst.' + nm + '(' + i[1] + ');')
  for i in scout:
    nm = i[1]
    nm = nm.replace('ports_', 'ports.')
    scbinds.append('    ' + typename + '_inst.' + nm + '(' + i[1] + ');')

  for m in master:
    dat = m[2] + '.dat'
    scbinds.append('    ' + typename + '_inst.' + dat + '(' + dat.replace('.', '_') + ');')
    vld = m[2] + '.vld'
    scbinds.append('    ' + typename + '_inst.' + vld + '(' + vld.replace('.', '_') + ');')
    rdy = m[2] + '.rdy'
    scbinds.append('    ' + typename + '_inst.' + rdy + '(' + rdy.replace('.', '_') + ');')


  vports = []
  vdecs  = []
  for i in scin:
    n = re.findall(r'\d+', i[0])
    if (len(n)):
      w = int(n[0])
    else:
      w = 1
    vdecs.append('  input [' + str(w-1) + ':0] ' + i[1] + ';')
    vports.append(i[1])
  for i in scout:
    n = re.findall(r'\d+', i[0])
    if (len(n)):
      w = int(n[0])
    else:
      w = 1
    vdecs.append('  output [' + str(w-1) + ':0] ' + i[1] + ';')
    vports.append(i[1])

  for m in master:
    dat = m[2] + '.dat'
    dat = dat.replace('.', '_')
    vld = m[2] + '.vld'
    vld = vld.replace('.', '_')
    rdy = m[2] + '.rdy'
    rdy = rdy.replace('.', '_')
    vports.append(dat)
    vports.append(vld)
    vports.append(rdy)
    if m[0] == 'Out':
     vdecs.append('  output [' + str(int(m[1]) - 1) + ':0] ' + dat + ';' )
     vdecs.append('  output ' + vld + ';' )
     vdecs.append('  input  ' + rdy + ';' )
    else:
     vdecs.append('  input  [' + str(int(m[1]) - 1) + ':0] ' + dat + ';' )
     vdecs.append('  input  ' + vld + ';' )
     vdecs.append('  output ' + rdy + ';' )


  with open(typename + '_wrap.h', 'w') as f:
    print >> f, '// Autogenerated from wrapper_gen.py on ' + strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
    print >> f, '// Arguments: ' + str(args)
    print >> f, ''
    if args.header == None:
      print >> f, '#include "' + typename + '.h"'
    else:
      print >> f, '#include "' + args.header + '"'
    print >> f, ''
    if is_top_level:
      print >> f, 'extern sc_trace_file* trace_file_ptr;'
    print >> f, ''
    print >> f, 'class ' + typename + '_wrap : public sc_module {'
    print >> f, 'public:'
    print >> f, '  ' + typename + ' CCS_INIT_S1(' + typename + '_inst);'
    print >> f, '  '

    for s in scdecs:
      print >> f, s

    print >> f, ' '
    print >> f, '  sc_clock connections_clk;'
    print >> f, '  sc_event check_event;'
    print >> f, ' '
    print >> f, '  virtual void start_of_simulation() {'
    print >> f, '    Connections::get_sim_clk().add_clock_alias('
    print >> f, '      connections_clk.posedge_event(), ' + clock_name + '.posedge_event());'
    print >> f, '  }'
    print >> f, ' '
    print >> f, '  SC_CTOR(' + typename + '_wrap)'
    if args.start_clk_low == None:
      print >> f, '   : connections_clk("connections_clk", ' + clock_period + ', SC_NS, 0.5,0,SC_NS,true)'
    else:
      print >> f, '   : connections_clk("connections_clk", ' + clock_period + ', SC_NS, 0.5,0,SC_NS,false) // start clk low'
    print >> f, '  {'
    print >> f, '    SC_METHOD(check_clock);'
    print >> f, '    sensitive << connections_clk;'
    print >> f, '    sensitive << ' + clock_name + ';'
    print >> f, '    '
    print >> f, '    SC_METHOD(check_event_method);'
    print >> f, '    sensitive << check_event;'
    print >> f, '    '
    if is_top_level:
      print >> f, '    trace_file_ptr = sc_create_vcd_trace_file("trace");'
      print >> f, '    trace_hierarchy(this, trace_file_ptr);'
      print >> f, '  '

    for s in scbinds:
      print >> f, s

    print >> f, '  }'
    print >> f, '  '
    print >> f, '  void check_clock() { check_event.notify(2, SC_PS);} // Let SC and Vlog delta cycles settle.'
    print >> f, '  '
    print >> f, '  void check_event_method() {'
    print >> f, '    if (connections_clk.read() == ' + clock_name + '.read()) return;'
    print >> f, '    CCS_LOG("clocks misaligned!:"  << connections_clk.read() << " " << ' + clock_name + '.read());'
    print >> f, '  }'
    print >> f, '};'

  with open(typename + '_wrap.cpp', 'w') as f:
    print >> f, '// Autogenerated from wrapper_gen.py on ' + strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
    print >> f, '// Arguments: ' + str(args)
    print >> f, ''
    print >> f, '#include "' + typename + '_wrap.h"'
    print >> f, ''
    if is_top_level:
      print >> f, 'sc_trace_file* trace_file_ptr;'
    print >> f, ''
    print >> f, '#ifdef SC_MODULE_EXPORT'
    print >> f, 'SC_MODULE_EXPORT(' + typename + '_wrap);'
    print >> f, '#endif'

  with open(typename + '_wrap.v', 'w') as f:
    print >> f, 'module ' + typename + ' ('
    first = True
    for s in vports:
      if first == True:
        s = '  ' + s
      else:
        s = ', ' + s
      print >> f, s
      first = False
    print >> f, ');'
    for s in vdecs:
      print >> f, s
    print >> f, 'endmodule;'

  stop_gdb(proc)
  print('wrapper_gen.py finished successfully.')

if __name__ == "__main__":
  main()

