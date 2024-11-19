#include "proc.h"
#include "fs.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  //pa3.2
   Elf_Ehdr ehdr;      // read the ELF Header
   ramdisk_read(&ehdr,0,sizeof(Elf_Ehdr));
   assert((*(uint32_t *)ehdr.e_ident == 0x464c457f));
   for(size_t i=0;i<ehdr.e_phnum;i++)
   {
    	Elf_Phdr phdr;
    	ramdisk_read(&phdr, ehdr.e_phoff + i*ehdr.e_phentsize, sizeof(Elf_Phdr));
    	if(phdr.p_type==PT_LOAD)
    	{

    		ramdisk_read((void*)phdr.p_vaddr,phdr.p_offset,phdr.p_filesz);
    		memset((void *)(phdr.p_vaddr+phdr.p_filesz),0,phdr.p_memsz-phdr.p_filesz);
    	}
   }
   return ehdr.e_entry;

}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
