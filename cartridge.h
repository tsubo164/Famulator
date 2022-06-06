#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdlib.h>
#include <stdint.h>

struct cartridge {
    uint8_t *prog_rom;
    uint8_t *char_rom;
    size_t prog_size;
    size_t char_size;

    uint8_t mirroring;
    uint8_t mapper;
    uint8_t nbanks;
};

extern struct cartridge *open_cartridge(const char *filename);
extern void close_cartridge(struct cartridge *cart);

extern uint8_t rom_read(const struct cartridge *cart, uint16_t addr);
extern uint8_t read_chr_rom(const struct cartridge *cart, uint16_t addr);

extern int is_vertical_mirroring(const struct cartridge *cart);

#endif /* _H */
