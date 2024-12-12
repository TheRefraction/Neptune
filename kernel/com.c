#include "io.h"
#include "com.h"

// Définir les préfixes pour les niveaux
const char* LOG_LEVEL_PREFIXES[] = {
    "[DEBUG] ",
    "[INFO] ",
    "[ERROR] "
};

void line_config_baud_rate(u16 com, u16 divisor) {
    outb(LINE_CMD_PORT(com), LINE_DLAB);
    outb(DATA_PORT(com), (divisor >> 8) & 0x00FF); // ET logique pour eliminer les hauts bits
    outb(DATA_PORT(com), divisor & 0x00FF);
}


/* Bit :     | 7 | 6 | 5 4 3 | 2 | 1 0 |
 * Contenu : | d | b | prty  | s | dl  |
 * Valeur :  | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
 * avec:
 * b le controle d'interruption (ici désactivé)
 * prty le nb de bits de parité (ici 0, 0 correspondant a 0 bit)
 * s le nombre de bits d'arret (ici 1, 0 correspondant à 1 bit)
 * dl la longueur (ici 8 bits)
 */
void serial_line_config(u16 com)
{
    outb(LINE_CMD_PORT(com), 0x03);
}

/*
 * Nous utilisons la valeur 0xC7, qui :
 *  active FIFO ;
 *  vide les deux files FIFO, tant celle de transmission que celle du récepteur ;
 *  utilise une taille de file de 14 octets.
 */

void serial_fifo_config(u16 com) {
    outb(FIFO_CMD_PORT(com), 0xc7);
}

void serial_modem_config(u16 com) {
    outb(MODEM_CMD_PORT(com), 0x03);
}

int serial_is_trans_fifo_empty(u32 com) {
    /* 0x20 = 0010 0000 */
    return inb(LINE_STAT_PORT(com)) & 0x20;
}

void serial_putchar(u32 com, char c) {
    // attendre que le port soit pret
    while (!serial_is_trans_fifo_empty(com));

    outb(DATA_PORT(com), c);
}

void serial_write(log_level_t level,const char* data) {

    const char* prefix = LOG_LEVEL_PREFIXES[level];

    u32 i = 0;
    while(prefix[i]!= '\0') {
        serial_putchar(COM1_BASE, prefix[i]);
        i++;
    }

    u32 j = 0;
    while(data[j] != '\0') {
        serial_putchar(COM1_BASE, data[j]);
        j++;
    }
}
