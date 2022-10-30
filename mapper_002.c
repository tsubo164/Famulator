#include <stdio.h>
#include "mapper_002.h"

static int prog_nbanks = 1;
static int char_nbanks = 1;

static int32_t map_prog_addr_002(uint16_t addr)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
        return addr & (prog_nbanks == 1 ? 0x3FFF : 0x7FFF);
    else
        return -1;
}

static int32_t map_char_addr_002(uint16_t addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
        return addr;
    else
        return -1;
}

static void init_mapper_002(void)
{
}

static void finish_mapper_002(void)
{
}

void open_mapper_002(struct mapper *m, size_t prog_size, size_t char_size)
{
    m->map_char_addr_func = map_char_addr_002;
    m->map_prog_addr_func = map_prog_addr_002;
    m->init_func = init_mapper_002;
    m->finish_func = finish_mapper_002;

    prog_nbanks = prog_size / 0x4000; /* 16KB */
    char_nbanks = char_size / 0x2000; /* 8KB */
}