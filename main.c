#include <stdio.h>
#include <string.h>

#include "nes.h"
#include "cartridge.h"
#include "debug.h"

int main(int argc, char **argv)
{
    struct NES nes = {{0}};
    struct cartridge *cart = NULL;
    const char *filename = NULL;
    int log_mode = 0;

    if (argc == 3 && !strcmp(argv[1], "--log-mode")) {
        log_mode = 1;
        filename = argv[2];
    }
    else if (argc ==2) {
        filename = argv[1];
    }
    else {
        fprintf(stderr, "missing file name\n");
        return -1;
    }

    cart = open_cartridge(filename);
    if (!cart) {
        fprintf(stderr, "not a *.nes file\n");
        return -1;
    }

    insert_cartridge(&nes, cart);
    power_up_nes(&nes);

    if (log_mode)
        log_cpu_status(&nes.cpu, 8980);
    else
        play_game(&nes);

    shut_down_nes(&nes);
    close_cartridge(cart);

    return 0;
}
