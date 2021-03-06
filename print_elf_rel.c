#include <stdio.h>
#include <stdlib.h>
#include "elf.h"
#include "util_bis.h"
#include "elf_linker-1.0/util.h"

#include "print_elf_rel.h"
#include "read_elf_rel.h"


void afficher_rel(FILE *f,Elf32_Ehdr head,Elf32_Shdr section_tab[],char strtab[],Elf32_Sym symtab[]){
    int entry;
    int num_sec;
    Elf32_Rel rel;
    Elf32_Rela rela;

    for(num_sec=0;num_sec<head.e_shnum;num_sec++){
        if(section_tab[num_sec].sh_type == SHT_REL){

            printf("Relocation section '");
            afficher_nom(strtab,section_tab[num_sec].sh_name);
            printf("' at offset 0x%3.3x contains %d entrie(s) :\n",section_tab[num_sec].sh_offset,section_tab[num_sec].sh_size/8);
            printf("Offset      Info        Type           Sym.Index\n");

            for(entry=0;entry<section_tab[num_sec].sh_size;entry=entry+8){

                rel = read_rel_entry(f,section_tab[num_sec].sh_offset+entry);
                printf("%8.8x    ",rel.r_offset);
                printf("%8.8x    ",rel.r_info);
                switch(ELF32_R_TYPE(rel.r_info)){
                    case R_ARM_ABS32: printf("R_ARM_ABS32"); break;
                    case R_ARM_CALL: printf("R_ARM_CALL"); break;
                    case R_ARM_JUMP24: printf("R_ARM_JUMP24"); break;
                }
                espaces(4);
                printf("%8.8x    \n",ELF32_R_SYM(rel.r_info));
            }
            printf("\n");
        }
        if(section_tab[num_sec].sh_type == SHT_RELA){

            printf("Relocation section '");
            afficher_nom(strtab,section_tab[num_sec].sh_name);
            printf("' at offset 0x%3.3x contains %d entrie(s) :\n",section_tab[num_sec].sh_offset,section_tab[num_sec].sh_size/8);
            printf("Offset      Info        Type           Sym.Index      +Addend\n");

            for(entry=0;entry<section_tab[num_sec].sh_size;entry=entry+8){

                rela = read_rela_entry(f,section_tab[num_sec].sh_offset+entry);
                printf("%8.8x    ",rela.r_offset);
                printf("%8.8x    ",rela.r_info);
                switch(ELF32_R_TYPE(rela.r_info)){
                    case R_ARM_ABS32: printf("R_ARM_ABS32"); break;
                    case R_ARM_CALL: printf("R_ARM_CALL"); break;
                    case R_ARM_JUMP24: printf("R_ARM_JUMP24"); break;
                }
                espaces(4);
                printf("%8.8x    \n",ELF32_R_SYM(rela.r_info));
                espaces(4);
                printf("%d\n",rela.r_addend);
            }
            printf("\n");
        }
    }
}