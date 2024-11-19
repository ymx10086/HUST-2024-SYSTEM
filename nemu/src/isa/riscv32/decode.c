#include "cpu/decode.h"
#include "rtl/rtl.h"

// decode operand helper
#define make_DopHelper(name) void concat(decode_op_, name) (Operand *op, uint32_t val, bool load_val)

static inline make_DopHelper(i) {
  op->type = OP_TYPE_IMM;
  op->imm = val;
  rtl_li(&op->val, op->imm);

  print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline make_DopHelper(r) {
  op->type = OP_TYPE_REG;
  op->reg = val;
  if (load_val) {
    rtl_lr(&op->val, op->reg, 4);
  }

  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg, 4));
}

/**
 * Decode the U-type instruction.
 * U-type instructions structure:
 *  imm[31:12] rd
 */
make_DHelper(U) {
  decode_op_i(id_src, decinfo.isa.instr.imm31_12 << 12, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.imm31_12);
}

/**
 * Decode the I-type instruction.
 * I-type instructions structure:
 *  imm[11:0] rs1 rd
 */
make_DHelper(I) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, decinfo.isa.instr.simm11_0, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

/**
 * Decode the S-type instruction.
 * S-type instructions structure:
 *  imm[11:5] rs2 rs1 imm[4:0]
 */
make_DHelper(S){
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);

  s0 = (decinfo.isa.instr.simm11_5 << 5) | 
        decinfo.isa.instr.imm4_0;
  decode_op_i(id_dest, s0, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_dest->val, reg_name(id_src->reg, 4));
  print_Dop(id_src2->str, OP_STR_SIZE, "%s", reg_name(id_src2->reg, 4));

}

/**
 * Decode the B-type instruction.
 * B-type instructions structure:
 *  imm[12] imm[10:5] rs2 rs1 imm[4:1] imm[11]
 */
make_DHelper(B){
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);
  
  print_Dop(id_src->str, OP_STR_SIZE, "%s", reg_name(id_src->reg, 4));
  print_Dop(id_src2->str, OP_STR_SIZE, "%s", reg_name(id_src2->reg, 4));

  s0 = decinfo.isa.instr.simm12 << 12 | 
       decinfo.isa.instr.imm10_5 << 5 | 
       decinfo.isa.instr.imm4_1 << 1 | 
       decinfo.isa.instr.imm11 << 11;
  decode_op_i(id_dest, s0, true);

}

/**
 * Decode the R-type instruction.
 * R-type instructions structure:
 *  rs2 rs1 rd
 */
make_DHelper(R){
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%s", reg_name(id_src->reg, 4));
  print_Dop(id_src2->str, OP_STR_SIZE, "%s", reg_name(id_src2->reg, 4));
  
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

/**
 * Decode the J-type instruction.
 * J-type instructions structure:
 *  imm[20] imm[10:1] imm[11] imm[19:12] rd
 */
make_DHelper(J){
  s0 =  (decinfo.isa.instr.simm20 << 20) |
        (decinfo.isa.instr.imm19_12 << 12) | 
        (decinfo.isa.instr.imm11_ << 11) | 
        (decinfo.isa.instr.imm10_1 << 1);   
  decode_op_i(id_src, s0, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", s0);
}

make_DHelper(ld) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, decinfo.isa.instr.simm11_0, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(st) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  s0 = (decinfo.isa.instr.simm11_5 << 5) | 
        decinfo.isa.instr.imm4_0;
  decode_op_i(id_src2, s0, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rs2, true);
}

make_DHelper(SYSTEM)
{
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, decinfo.isa.instr.csr, true);
  print_Dop(id_src->str, OP_STR_SIZE, "%d", decinfo.isa.instr.csr);
  
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}