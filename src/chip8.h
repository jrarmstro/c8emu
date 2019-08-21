#ifndef C8EMU_CHIP8_H
#define C8EMU_CHIP8_H
#include <cstdint>
#include <random>
#include <sstream>
#include <stack>
#include <stdexcept>
#include "window.h"


class UnknownOpcodeError : public std::exception {

    uint16_t opcode_;

public:

    explicit UnknownOpcodeError(uint16_t opcode)
            : std::exception{}, opcode_{opcode} {}

    [[nodiscard]] char const* what() const noexcept override {
        char* err = static_cast<char*>(malloc(80));
        sprintf(err, "Unknown opcode: %04X", opcode_);
        return err;
    }
};


class Chip8 {
public:

    Chip8();

    bool load_application(const char* filename);

    void step();

    void set_key(int key, bool val);

    [[nodiscard]] bool need_redraw() const;

    friend void Window::drawChip8(Chip8* c8);

    friend std::ostream& operator<<(std::ostream& out, Chip8& c8);

private:

    enum Register {
        V0 = 0,
        V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF,
        NUM_REGISTERS
    };

    uint16_t r_pc_; // Program counter
    uint16_t r_i_;  // Index register
    uint16_t r_dt_; // Delay timer
    uint16_t r_st_; // Sound timer

    uint8_t r_[NUM_REGISTERS];

    uint8_t mem_[4096];

    std::stack<uint16_t> stack_;

    bool gfx_[64 * 32];
    bool need_redraw_;

    bool key_[16];

    std::random_device random_device_;
    std::mt19937 rng_;
    std::uniform_int_distribution<std::mt19937::result_type> dist_0_255_;

};

std::ostream& operator<<(std::ostream& out, Chip8& c8);



#endif
