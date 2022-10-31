#include <stdio.h>
#include <string.h>
#include "log.h"
#include "cpu.h"

namespace nes {

static uint16_t abs_indirect(const struct CPU *cpu, uint16_t abs)
{
    if ((abs & 0x00FF) == 0x00FF)
        /* emulate page boundary hardware bug */
        return (peek_cpu_data(cpu, abs & 0xFF00) << 8) | peek_cpu_data(cpu, abs);
    else
    {
        /* normal behavior */
        const uint16_t lo = peek_cpu_data(cpu, abs);
        const uint16_t hi = peek_cpu_data(cpu, abs + 1);

        return (hi << 8) | lo;
    }
}

static uint16_t zp_indirect(const struct CPU *cpu, uint8_t zp)
{
    const uint16_t lo = peek_cpu_data(cpu, zp & 0xFF);
    const uint16_t hi = peek_cpu_data(cpu, (zp + 1) & 0xFF);

    return (hi << 8) | lo;
}

void print_cpu_status(const struct CPU *cpu)
{
    struct cpu_status stat;
    get_cpu_status(cpu, &stat);

    const uint16_t pc = stat.pc;
    const uint8_t  a = stat.a;
    const uint8_t  x = stat.x;
    const uint8_t  y = stat.y;
    const uint8_t  p = stat.p;
    const uint8_t  s = stat.s;
    const uint8_t  code = stat.code;
    const uint8_t  lo   = stat.lo;
    const uint8_t  hi   = stat.hi;
    const uint16_t wd   = stat.wd;
    const char *inst_name = stat.inst_name;
    const char *mode_name = stat.mode_name;

    int N = 0, n = 0;

    printf("%04X  %02X %n", pc, code, &n);
    N += n;

    if (!strcmp(mode_name, "IND")) {
        printf("%02X %02X  %s ($%04X) = %04X%n",
                lo, hi, inst_name, wd, abs_indirect(cpu, wd), &n);
        N += n;
    }
    else if (!strcmp(mode_name, "ABS")) {
        printf("%02X %02X  %s $%04X%n", lo, hi, inst_name, wd, &n);
        N += n;
        if (strcmp(inst_name, "JMP") && strcmp(inst_name, "JSR")) {
            printf(" = %02X%n", peek_cpu_data(cpu, wd), &n);
            N += n;
        }
    }
    else if (!strcmp(mode_name, "ABX")) {
        printf("%02X %02X  %s $%04X,X @ %04X = %02X%n",
                lo, hi, inst_name, wd, wd + x, peek_cpu_data(cpu, wd + x), &n);
        N += n;
    }
    else if (!strcmp(mode_name, "ABY")) {
        printf("%02X %02X  %s $%04X,Y @ %04X = %02X%n",
                lo, hi, inst_name, wd, (wd + y) & 0xFFFF,
                peek_cpu_data(cpu, (wd + y) & 0xFFFF), &n);
        N += n;
    }
    else if (!strcmp(mode_name, "IZX")) {
        const uint16_t addr = zp_indirect(cpu, lo + x);
        printf("%02X     %s ($%02X,X) @ %02X = %04X = %02X%n",
                lo, inst_name, lo, (lo + x) & 0xFF, addr, peek_cpu_data(cpu, addr), &n);
        N += n;
    }
    else if (!strcmp(mode_name, "IZY")) {
        const uint16_t addr = zp_indirect(cpu, lo);
        printf("%02X     %s ($%02X),Y = %04X @ %04X = %02X%n",
                lo, inst_name, lo, addr, (addr + y) & 0xFFFF, peek_cpu_data(cpu, addr + y), &n);
        N += n;
    }
    else if (!strcmp(mode_name, "ZPX")) {
        printf("%02X     %s $%02X,X @ %02X = %02X%n",
                lo, inst_name, lo, (lo + x) & 0xFF, peek_cpu_data(cpu, (lo + x) & 0xFF), &n);
        N += n;
    }
    else if (!strcmp(mode_name, "ZPY")) {
        printf("%02X     %s $%02X,Y @ %02X = %02X%n",
                lo, inst_name, lo, (lo + y) & 0xFF, peek_cpu_data(cpu, (lo + y) & 0xFF), &n);
        N += n;
    }
    else if (!strcmp(mode_name, "REL")) {
        printf("%02X     %s $%04X%n", lo, inst_name, (pc + 2) + (int8_t)lo, &n);
        N += n;
    }
    else if (!strcmp(mode_name, "IMM")) {
        printf("%02X     %s #$%02X%n", lo, inst_name, lo, &n);
        N += n;
    }
    else if (!strcmp(mode_name, "ZPG")) {
        printf("%02X     %s $%02X = %02X %n",
                lo, inst_name, lo, peek_cpu_data(cpu, lo), &n);
        N += n;
    }
    else if (!strcmp(mode_name, "ACC")) {
        printf("       %s A%n", inst_name, &n);
        N += n;
    }
    else if (!strcmp(mode_name, "IMP")) {
        printf("       %s%n", inst_name, &n);
        N += n;
    }

    printf("%*s", 48 - N, " ");
    printf("A:%02X X:%02X Y:%02X P:%02X SP:%02X", a, x, y, p, s);
    printf("\n");
}

} // namespace