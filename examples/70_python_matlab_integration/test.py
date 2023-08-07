
from ctypes import cdll
from ctypes import *
lib = cdll.LoadLibrary('./libpy.so')

class sc_simulator(object):
    def __init__(self):
        self.obj = lib.sc_simulator_new()

    def process_one_sample(self, arg):
        return lib.process_one_sample(self.obj, arg)

sim = sc_simulator()
for i in range(10):
  print("Python TB sees: " + str(sim.process_one_sample(i)))

