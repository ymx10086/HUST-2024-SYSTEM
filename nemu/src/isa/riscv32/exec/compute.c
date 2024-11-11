#include "cpu/exec.h"

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(auipc) {
  rtl_add(&id_dest->val, &cpu.pc, &id_src->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  print_asm_template2(auipc);
}

make_EHelper(addi) {
  rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  
  print_asm_template3(addi);
}

make_EHelper(jal) {
  rtl_sr(id_dest->reg, &decinfo.seq_pc, 4);
  rtl_j(decinfo.seq_pc + id_src->val);

  print_asm_template2(jal);
}

make_EHelper(jalr){
  uint32_t addr = cpu.pc + 4;
  rtl_sr(id_dest->reg, &addr, 4);

  decinfo.jmp_pc = (id_src->val+id_src2->val)&(~1);
  rtl_j(decinfo.jmp_pc);

  difftest_skip_dut(1, 2); 

  print_asm_template2(jalr);
}


