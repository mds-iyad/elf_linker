#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <elf.h>

#include "elf_linker-1.0/util.h"
#include "util_bis.h"
#include "read_elf_sect.h"
#include "read_elf_head.h"
#include "read_elf_symt.h"
#include "read_elf_rel.h"
#include "section.h"
#include "fusion_progbits.h"
#include "fusion_symtab.h"

#define ERROR_NB_FILES 2001 
#define ERROR_READ_FILE_1 2002 
#define ERROR_READ_FILE_2 2003
#define ERROR_WRITE_FILE 2004

#define SECTION_TAB_SIZE 30 
#define SYM_TAB_SIZE 30 
#define STR_TAB_SIZE 512 

void get_header(FILE *f, Elf32_Ehdr *header)
{
	*header = read_header(f) ;
}

void get_sections(FILE *f, Elf32_Ehdr header, Elf32_Shdr *sections)
{
    read_section(f, sections, header.e_shoff, header.e_shnum) ;
}

int get_symtab(FILE *f, Elf32_Shdr sections[], int index, Elf32_Sym *symtab)
{
    read_symtab(f, symtab, sections, sections[index].sh_offset, 
			sections[index].sh_size / sizeof(Elf32_Sym)) ;
    return sections[index].sh_size / sizeof(Elf32_Sym);
}

void get_strtab(FILE *f, Elf32_Ehdr header, Elf32_Shdr sections[], char *strtab)
{
    read_string_table(f, header, sections, strtab, header.e_shstrndx) ;
}

void get_symtrtab(FILE *f, Elf32_Ehdr header, Elf32_Shdr sections[], char *strtab)
{
    read_string_table(f, header, sections, strtab, header.e_shstrndx+2) ;
}

int main(int argc, char *argv[])
{
    // Declaration des variables
    FILE *f1, *f2;
    Elf32_Ehdr header1, header2 ;
    Elf32_Shdr sections1[SECTION_TAB_SIZE], sections2[SECTION_TAB_SIZE] ;
    Elf32_Sym symtab[SYM_TAB_SIZE], symtab1[SYM_TAB_SIZE], symtab2[SYM_TAB_SIZE] ;
	char strtab[STR_TAB_SIZE], shstrtab[STR_TAB_SIZE], 
		 hstrtab1[STR_TAB_SIZE],hstrtab2[STR_TAB_SIZE], 
		 strtab1[STR_TAB_SIZE], strtab2[STR_TAB_SIZE] ;
	tab_section tab_section ;
    int nb_sym1, nb_sym2;
	int nb_shstr, nb_sym ;
    // Gestion erreurs nombre arguments
    if (argc < 3)
	{
        fprintf(stderr, "ERREUR : utilisation \"./prog nom_src1 nom_src2\"\n") ; 
        return ERROR_NB_FILES ;
    }
    // Ouverture fichiers
    f1 = fopen(argv[1], "r") ;
    f2 = fopen(argv[2], "r") ;
    // Gestion erreurs sur les fichiers en lecture
    if (! f1) 
	{
        fprintf(stderr, "ERREUR : premier fichier non accessible en lecture\n") ; 
        return ERROR_READ_FILE_1 ;
    }
    // Gestion erreurs sur les fichiers en lecture
    if (! f2) 
	{
        fprintf(stderr, "ERREUR : second fichier non accessible en lecture\n") ; 
        return ERROR_READ_FILE_2 ;
    }

	// Initialisation
	creer_tab_section(&tab_section) ;
	get_header(f1, &header1) ;
	get_header(f2, &header2) ;
	get_sections(f1, header1, sections1) ;
	get_sections(f2, header2, sections2) ;
	nb_sym1 = get_symtab(f1, sections1, index_symtab(sections1), symtab1) ;
	nb_sym2 = get_symtab(f2, sections2, index_symtab(sections2), symtab2) ;
	get_symtrtab(f1, header1, sections1, strtab1) ;
	get_symtrtab(f2, header2, sections2, strtab2) ;
    get_strtab(f1, header1, sections1, hstrtab1) ;
	get_strtab(f2, header2, sections2, hstrtab2) ;
	// On fusionne les sections de type 'PROGBITS'
	fusion_progbits(&nb_shstr, shstrtab, f1, f2, &tab_section, 
			sections1, header1.e_shnum, sections2, header2.e_shnum, 
			strtab1, strtab2,
			hstrtab1,hstrtab2,symtab2,nb_sym2) ;
    fusion_symtab(strtab, &nb_sym, symtab, symtab1,symtab2,nb_sym1,nb_sym2,strtab1,strtab2);

	// Libere la memoire
	fclose(f1) ;
	fclose(f2) ;

    return 0 ;
}
