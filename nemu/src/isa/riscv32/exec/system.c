#include "cpu/exec.h"

extern void raise_intr(uint32_t NO, vaddr_t epc);

int32_t readcsr(int csr_num) {
  switch (csr_num) {
    case 0x105: return decinfo.isa.stvec;
    case 0x142: return decinfo.isa.scause;
    case 0x100: return decinfo.isa.sstatus;
    case 0x141: return decinfo.isa.sepc;
    default: panic("read_csr: invalid csr_num = %x", csr_num);
  }
}

int32_t writecsr(int csr_num, int32_t val) {
  switch (csr_num) {
    case 0x105: decinfo.isa.stvec = val; break;
    case 0x142: decinfo.isa.scause = val; break;
    case 0x100: decinfo.isa.sstatus = val; break;
    case 0x141: decinfo.isa.sepc = val; break;
    default: panic("write_csr: invalid csr_num = %x", csr_num);
  }
  return 0;
}

make_EHelper(syscall){
    Instr instr = decinfo.isa.instr;
    switch(instr.funct3){
        //ecall
        case 0b0:
            if((instr.val & ~(0x7f))==0){
                raise_intr(reg_l(17), decinfo.seq_pc-4);
            }
            else if(instr.val == 0x10200073){
                decinfo.jmp_pc = decinfo.isa.sepc + 4;
                rtl_j(decinfo.jmp_pc);
            }
            else{
                assert(0 && "system code unfinish");
            }
            break;
        // csrrw
        case 0b001:
            s0 = readcsr(instr.csr);
            writecsr(instr.csr, id_src->val);
            rtl_sr(id_dest->reg, &s0, 4);
            break;
        case 0b010:
            s0 = readcsr(instr.csr);
            writecsr(instr.csr, s0 | id_src->val);
            rtl_sr(id_dest->reg, &s0, 4);
            break;
        default:
            assert(0 && "Unfinished system op");
    }
}