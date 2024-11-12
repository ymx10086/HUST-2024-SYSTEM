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
      print_asm_template3(addi);
      break;
    case 0x1: // slli
      rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(slli);
      break;
    case 0x2: // slti
      rtl_setrelop(RELOP_LT, &id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(slti);
      break;
    case 0x3: // sltiu
      rtl_setrelop(RELOP_LTU, &id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(sltiu);
      break;
    case 0x4: // xori
      rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(xori);
      break;
    case 0x5: // srli/srai
      if (decinfo.isa.instr.imm11 == 0) {
        rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
        print_asm_template3(srli);
      } else {
        rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
        print_asm_template3(srai);
      }
      break;
    case 0x6: // ori
      rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(ori);
      break;
    case 0x7: // andi
      rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(andi);
      break;
    default:
      assert(0);
  }
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

/**
 * Execute the J-type instruction.
*/
make_EHelper(jal) {
  rtl_sr(id_dest->reg, &decinfo.seq_pc, 4);
  rtl_j(decinfo.seq_pc + id_src->val);

  print_asm_template2(jal);
}

/**
 * Execute the R-type instruction.
*/
make_EHelper(R){
  switch (decinfo.isa.instr.funct3) {
    case 0x0:
      switch (decinfo.isa.instr.funct7){
        case 0x0: // add
          rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(add);
          break;
        case 0x20: // sub
          rtl_sub(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(sub);
          break;
        default: // mul
          rtl_imul_lo(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(mul);
          break;
      }
      break;
    case 0x1: // sll
      switch (decinfo.isa.instr.funct7){
        case 0x0: // sll
          rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
          break;
        default:
          assert(0);
      }
      break;
    case 0x2: // slt
      rtl_setrelop(RELOP_LT, &id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x3: // sltu
      rtl_setrelop(RELOP_LTU, &id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x4: // xor
      rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x5: // srl/sra
      if (decinfo.isa.instr.funct7 == 0) {
        rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
      } else {
        rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
      }
      break;
    case 0x6: // or
      rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
      break;
    case 0x7: // and
      rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
      break;
    default:
      assert(0);
  }
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  print_asm_template3(R);
}

make_EHelper(jalr){
  uint32_t addr = cpu.pc + 4;
  rtl_sr(id_dest->reg, &addr, 4);

  decinfo.jmp_pc = (id_src->val+id_src2->val)&(~1);
  rtl_j(decinfo.jmp_pc);

  difftest_skip_dut(1, 2); 

  print_asm_template2(jalr);
}


