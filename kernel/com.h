#ifndef __KERNEL_COM__
#define __KERNEL_COM__

// Configuration des ports E/S séries COM1, COM2, COM3 et COM4

#define COM1_BASE 0x3F8 // Les adresses des autres ports sont décalées de 2, 3, 4 et 5
#define DATA_PORT(base) (base)
#define FIFO_CMD_PORT(base) (base + 2)
#define LINE_CMD_PORT(base) (base + 3)
#define MODEM_CMD_PORT(base) (base + 4)
#define LINE_STAT_PORT(base) (base + 5)

/* LINE_DLAB:
 * Dit au port série d'attendre les 8 bits de poids fort, puis l'octet de poids faible
 */
#define LINE_DLAB 0x80

#include "types.h"

// Niveaux de gravité
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_ERROR
} log_level_t;

void line_config_baud_rate(u16, u16);
void serial_line_config(u16);
void serial_fifo_config(u16);
void serial_modem_config(u16);
int serial_is_trans_fifo_empty(u32);

void serial_putchar(u32, char);
void serial_write(log_level_t, const char*);

#endif
