#include "types.h"

#define PAGING_FLAG 0x80000000 // 31st bit of CR0 register

#define PAGE_PRESENT 0x1       // Indique que la page est présente en mémoire
#define PAGE_RW 0x2            // Indique que la page est en lecture/écriture
#define PAGE_USER 0x4          // Indique que la page est accessible par les utilisateurs
#define PAGE_SIZE 0x1000         // Taille d'une page (4 Kio)

#define PD_BASE 0x20000 // Address of Pages Directory 
#define PT_BASE 0x21000 // Address of first Pages Table

// Fonction d'initialisation de la pagination (Adresse virtuelle = adresse physique) sur 4 Mio
void init_paging(void) {
    u16 i;

    // Initialiser le répertoire des pages avec des entrées nulles
    u32 *page_directory = (u32*) PD_BASE;
    
    for (i = 0; i < 1024; i++) {
        page_directory[i] = 0x0;
    }

    // Set first entry
    page_directory[0] = (PT_BASE | PAGE_PRESENT | PAGE_RW);

    // Initialiser la première table des pages d'indice
    u32 *page_table0 = (u32*) PT_BASE;
    u32 page_addr = 0x0;
    
    for (i = 0; i < 1024; i++) {
        page_table0[i] = (page_addr | PAGE_PRESENT | PAGE_RW);
        page_addr += PAGE_SIZE;
    }

    /*
     * Passage de l'adresse du répertoire de pages au registre CR3
     * Mise à 1 du bit de pagination (n°31) dans le registre CR0
     */
    asm("mov %0, %%eax \n \
	 mov %%eax, %%cr3 \n \
	 mov %%cr0, %%eax \n \
	 or %1, %%eax \n \
	 mov %%eax, %%cr0" :: "i"(PD_BASE), "i"(PAGING_FLAG));
}

// Fonction pour mapper une adresse virtuelle à une adresse physique
/*void map_page(u32 virt_addr, u32 phys_addr, u32 flags) {
    u32 page_dir_index = (virt_addr >> 22) & 0x3FF; // Bits 31:22
    u32 page_table_index = (virt_addr >> 12) & 0x3FF; // Bits 21:12

    // Vérifier si la table des pages associée existe
    if (!(page_directory[page_dir_index] & PAGE_PRESENT)) {
        // erreur
        return;
    }


    // Récupérer l'adresse de la table des pages
    u32 *table = (uint32_t *)(page_directory[page_dir_index] & ~0xFFF);

        // Ajouter une entrée dans la table des pages
    table[page_table_index] = (phys_addr & ~0xFFF) | (flags & 0xFFF);

    // Invalider le TLB pour cette page
    asm volatile("invlpg (%0)" :: "r"(virt_addr) : "memory");
}*/
