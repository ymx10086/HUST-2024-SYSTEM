#include "cpu/exec.h"

make_EHelper(lui);

make_EHelper(ld);
make_EHelper(st);
make_EHelper(lb);
make_EHelper(lh);

make_EHelper(inv);
make_EHelper(nemu_trap);

// make_EHelper(addi);
make_EHelper(I_instr);
make_EHelper(auipc);
make_EHelper(R_instr);
make_EHelper(B_instr);
make_EHelper(jal);
make_EHelper(jalr);
make_EHelper(syscall);
