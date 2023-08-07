if { ![batch_mode] } {
  add wave -position insertpoint sim:/sc_main/top/dut1/ccs_rtl/dut_thread1_inst/sync1_vld
  add wave -position insertpoint sim:/sc_main/top/dut1/ccs_rtl/dut_thread1_inst/sync1_rdy
  run -all
  wave zoom full
}

