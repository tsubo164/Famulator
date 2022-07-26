#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include "apu.h"

namespace nes {

class Cartridge;
class PPU;

struct Instruction {
    uint8_t opcode = 0;
    uint8_t addr_mode = 0;
    uint8_t cycles = 0;
};

struct CpuStatus {
    uint16_t pc = 0;
    uint8_t  a = 0, x = 0, y = 0, p = 0, s = 0;
    uint8_t  lo = 0, hi = 0;
    uint16_t wd = 0;
    uint8_t  code = 0;
    char inst_name[4] = {0};
    char mode_name[4] = {0};
};

class CPU {
public:
    CPU(PPU &ppu) : ppu_(ppu) {}
    ~CPU() {}

    void SetCartride(Cartridge *cart);

    // clock
    void PowerUp();
    void Reset();
    void Clock();
    void ClockAPU();

    // interrupts
    void HandleIRQ();
    void HandleNMI();
    void ClearIRQ();
    bool IsSetIRQ() const;

    // DMA
    void InputController(uint8_t controller_id, uint8_t input);
    bool IsSuspended() const;
    void Resume();
    uint8_t GetDmaPage() const;
    uint8_t PeekData(uint16_t addr) const;
    void GetStatus(CpuStatus &stat) const;

    // debug
    void SetPC(uint16_t addr);
    int GetCycles() const;

private:
    Cartridge *cart_ = nullptr;
    PPU &ppu_;
    APU apu_;
    int cycles_ = 0;
    bool suspended_ = false;

    // registers
    uint8_t a_ = 0;
    uint8_t x_ = 0;
    uint8_t y_ = 0;
    uint8_t s_ = 0; // stack pointer
    uint8_t p_ = 0; // processor status
    uint16_t pc_ = 0;

    // 4 2KB ram. 3 of them are mirroring
    uint8_t wram_[2048] = {0};

    uint8_t controller_input_[2] = {0};
    uint8_t controller_state_[2] = {0};

    // dma
    uint8_t dma_page_ = 0;

    // read and write
    void write_byte(uint16_t addr, uint8_t data);
    uint8_t read_byte(uint16_t addr);
    uint16_t read_word(uint16_t addr);
    uint8_t peek_byte(uint16_t addr) const;
    uint16_t peek_word(uint16_t addr) const;
    uint8_t fetch();
    uint16_t fetch_word();
    // address
    uint16_t abs_indirect(uint16_t abs) const;
    uint16_t zp_indirect(uint8_t zp) const;
    uint16_t fetch_address(int mode, bool *page_crossed);
    // flags and registers
    void set_pc(uint16_t addr);
    void set_flag(uint8_t flag, uint8_t val);
    uint8_t get_flag(uint8_t flag) const;
    uint8_t update_zn(uint8_t val);
    void set_a(uint8_t val);
    void set_x(uint8_t val);
    void set_y(uint8_t val);
    void set_s(uint8_t val);
    void set_p(uint8_t val);
    // push and pop
    void push(uint8_t val);
    uint8_t pop();
    void push_word(uint16_t val);
    uint16_t pop_word();
    void compare(uint8_t a, uint8_t b);
    bool branch_if(uint16_t addr, bool cond);
    void add_a_m(uint8_t data);
    // instruction
    int execute(Instruction inst);
};

} // namespace

#endif // _H
