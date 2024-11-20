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
      if((decinfo.isa.instr.funct7) == 0b0000000) {
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
  // rtl_sr(id_dest->reg, &decinfo.seq_pc, 4);
  // rtl_j(decinfo.seq_pc + id_src->val);

  // print_asm_template2(jal);
  uint32_t addr = cpu.pc + 4;
  rtl_sr(id_dest->reg, &addr, 4);

  rtl_add(&decinfo.jmp_pc, &cpu.pc, &id_src->val);
  rtl_j(decinfo.jmp_pc);

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

/**
 * Execute the R-type instruction.
*/
make_EHelper(R_instr){
  switch (decinfo.isa.instr.funct3) {
    case 0x0: // add | sub | mul
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
    case 0x1: // sll | mulh
      switch (decinfo.isa.instr.funct7){
        case 0x0: // sll
          rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(sll);
          break;
        default: // mulh
          rtl_imul_hi(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(mulh);
          break;
      }
      break;
    case 0x2: // slt | mulhsu
      switch (decinfo.isa.instr.funct7){
        case 0x0: // slt
          id_dest->val = (signed)id_src->val < (signed)id_src2->val;
          print_asm_template3(slt);
          break;
        default: // mulhsu
          // rtl_imul_hi_u(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(mulhsu);
          break;
      }
      break;
    case 0x3: // sltu | mulhu
      switch (decinfo.isa.instr.funct7){
        case 0x0: // sltu
          rtl_setrelop(RELOP_LTU, &id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(sltu);
          break;
        default: // mulhu
          // rtl_imul_hi_u(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(mulhu);
          break;
      }
      break;
    case 0x4: // xor | div
      switch (decinfo.isa.instr.funct7){
        case 0x0: // xor
          rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(xor);
          break;
        default: // div
          rtl_idiv_q(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(div);
          break;
      }
      break;
    case 0x5: // srl | sra | divu
      switch (decinfo.isa.instr.funct7){
        case 0x0: // srl
          rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(srl);
          break;
        case 0x20: // sra
          rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(sra);
          break;
        default: // divu
          // rtl_idiv_q_u(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(divu);
          break;
      }
      break;
    case 0x6: // or | rem
      switch (decinfo.isa.instr.funct7){
        case 0x0: // or
          rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(or);
          break;
        default: // rem
          rtl_idiv_r(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(rem);
          break;
      }
      break;
    case 0x7: // and | remu
      switch (decinfo.isa.instr.funct7){
        case 0x0: // and
          rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(and);
          break;
        default: // remu
          // rtl_idiv_r_u(&id_dest->val, &id_src->val, &id_src2->val);
          print_asm_template3(remu);
          break;
      }
      break;
    default:
      assert(0);
  }
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

/**
 * Execute the B-type instruction.
*/
make_EHelper(B_instr){
  decinfo.jmp_pc = cpu.pc + id_dest->val;
  switch(decinfo.isa.instr.funct3){
    case 0b000:     // beq||beqz
      rtl_jrelop(RELOP_EQ, &id_src->val, &id_src2->val, decinfo.jmp_pc);
      print_asm_template3(beq);
      break;
    case 0b001:     // bne||bnez
      rtl_jrelop(RELOP_NE, &id_src->val, &id_src2->val, decinfo.jmp_pc);
      print_asm_template3(bne);
      break;
    case 0b100:     // blt||bltz
      rtl_jrelop(RELOP_LT, &id_src->val, &id_src2->val, decinfo.jmp_pc);
      print_asm_template3(blt);
      break;
    case 0b101:     // bge||bgez
      rtl_jrelop(RELOP_GE, &id_src->val, &id_src2->val, decinfo.jmp_pc);
      print_asm_template3(bge);
      break;
    case 0b110:     // bltu
      rtl_jrelop(RELOP_LTU, &id_src->val, &id_src2->val, decinfo.jmp_pc);
      print_asm_template3(bltu);
      break;
    case 0b111:   // bgeu
      rtl_jrelop(RELOP_GEU, &id_src->val, &id_src2->val, decinfo.jmp_pc);
      print_asm_template3(bgeu);
      break;
    default:
      assert(0);
      break;
  }
}

