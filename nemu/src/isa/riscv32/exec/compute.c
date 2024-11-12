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

// make_EHelper(addi) {
//   rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
//   rtl_sr(id_dest->reg, &id_dest->val, 4);
  
//   print_asm_template3(addi);
// }

/**
 * Execute the I-type instruction.
*/
make_EHelper(I_instr){
  switch (decinfo.isa.instr.funct3) {
    case 0x0: // addi
      rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x1: // slli
      rtl_shli(&id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x2: // slti
      rtl_setrelop(RELOP_LT, &id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x3: // sltiu
      rtl_setrelop(RELOP_LTU, &id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x4: // xori
      rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x5: // srli/srai
      if (decinfo.isa.instr.imm11 == 0) {
        rtl_shri(&id_dest->val, &id_src->val, &id_src2->val);
      } else {
        rtl_sari(&id_dest->val, &id_src->val, &id_src2->val);
      }
      break;
    case 0x6: // ori
      rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x7: // andi
      rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
      break;
    default:
      assert(0);
  }
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  print_asm_template3(I_instr);
}

/**
 * Execute the J-type instruction.
*/
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


