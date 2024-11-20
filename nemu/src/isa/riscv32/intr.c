#include "rtl/rtl.h"
#include "cpu/exec.h"

void raise_intr(uint32_t NO, vaddr_t epc) {
  decinfo.isa.sepc = epc;
  decinfo.isa.scause = NO;
  decinfo.jmp_pc = decinfo.isa.stvec;
  rtl_j(decinfo.jmp_pc);
}

bool isa_query_intr(void) {
  return false;
}
