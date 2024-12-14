#include <stdint.h>
#include <stddef.h>

#define PAGE_PRESENT 0x1       // Indique que la page est présente en mémoire
#define PAGE_RW 0x2            // Indique que la page est en lecture/écriture
#define PAGE_USER 0x4          // Indique que la page est accessible par les utilisateurs
#define PAGE_SIZE 4096         // Taille d'une page (4 Kio)

// Définition alignée pour les structures de pagination
__attribute__((aligned(PAGE_SIZE))) uint32_t page_directory[1024];
__attribute__((aligned(PAGE_SIZE))) uint32_t page_table[1024];


// Fonction d'initialisation de la pagination
void init_pagination() {
    // Initialiser le répertoire des pages avec des entrées nulles
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0;
    }

    // Initialiser la table des pages avec des entrées nulles
    for (int i = 0; i < 1024; i++) {
        page_table[i] = 0;
    }

        // Mapper les 4 premiers Kio : adresse physique 0x00000000 vers virtuelle 0x00000000
    page_table[0] = (0x00000000 | PAGE_PRESENT | PAGE_RW); // Physique 0x0, Présent, R/W

    // Associer cette table au répertoire des pages (index 0)
    page_directory[0] = ((uint32_t)page_table | PAGE_PRESENT | PAGE_RW);

    // Charger le répertoire des pages dans CR3
    asm volatile("mov %0, %%cr3" :: "r"(page_directory));

    // Activer la pagination en définissant le bit PG dans CR0
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1U << 31); // Activer le bit PG (bit 31)
    asm volatile("mov %0, %%cr0" :: "r"(cr0));

}

// Fonction pour mapper une adresse virtuelle à une adresse physique
void map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags) {
    uint32_t page_dir_index = (virt_addr >> 22) & 0x3FF; // Bits 31:22
    uint32_t page_table_index = (virt_addr >> 12) & 0x3FF; // Bits 21:12

    // Vérifier si la table des pages associée existe
    if (!(page_directory[page_dir_index] & PAGE_PRESENT)) {
        // erreur
        return;
    }


    // Récupérer l'adresse de la table des pages
    uint32_t *table = (uint32_t *)(page_directory[page_dir_index] & ~0xFFF);

        // Ajouter une entrée dans la table des pages
    table[page_table_index] = (phys_addr & ~0xFFF) | (flags & 0xFFF);

    // Invalider le TLB pour cette page
    asm volatile("invlpg (%0)" :: "r"(virt_addr) : "memory");
}
