#include "chip8.h"
#include <cstdio>
#include <iostream>
#include <iomanip>

#ifdef WINDOWS
#include <Windows.h>
#endif


static constexpr uint8_t FONT_[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


Chip8::Chip8()
        : gfx_          {false},
          need_redraw_  {true},
          r_pc_         {0x200u},
          r_i_          {0},
          r_dt_         {0},
          r_st_         {0},
          r_            {0},
          mem_          {0},
          key_          {false},
          random_device_{},
          rng_          {random_device_()},
          dist_0_255_   {0, 255}
{
    // Load font into memory
    for (auto i = 0uL; i < sizeof(FONT_) / sizeof(*FONT_); ++i) {
        mem_[i] = FONT_[i];
    }
}

bool Chip8::load_application(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == nullptr) return false;


    fseek(fp, 0, SEEK_END);
    size_t flen = (size_t) ftell(fp);
    rewind(fp);

    char* buf = (char*) malloc(sizeof(char) * flen);
    if (buf == nullptr) return false;

    size_t result = fread(buf, 1, flen, fp);
    if (result != flen) return false;

    if (0x1000 - 0x200 <= flen) return false;

    for (auto i = 0uL; i < flen; ++i) {
        mem_[i + 0x200] = (uint8_t) buf[i];
    }

    fclose(fp);
    free(buf);
    return true;
}

void Chip8::step() {
    // Fetch the opcode from memory;
    uint16_t op = (mem_[r_pc_] << 8u);
             op |= mem_[r_pc_ + 1];

//    std::cout << "Op: " << std::hex << op << std::endl;
//    std::cout << *this << std::endl;

    r_pc_ += 2u;

    uint8_t  x =   (op & 0x0F00u) >> 8u;
    uint8_t  y =   (op & 0x00F0u) >> 4u;
    uint16_t n =    op & 0x000Fu;
    uint8_t  nn =   op & 0x00FFu;
    uint16_t nnn =  op & 0x0FFFu;
    switch (op & 0xF000u) {
        case 0x0000u:
            if (op == 0x00E0u) { // 00E0; clear the screen
                for (auto& pix : gfx_) pix = false;
                need_redraw_ = true;
            } else if (op == 0x00EEu) { // 00EE; return from subroutine
                r_pc_ = stack_.top();
                stack_.pop();
            } else {
                // TODO: Call RCA 1802 at op & 0x0FFFu
                throw UnknownOpcodeError{op};
            }
            break;
        case 0x1000u: // 1NNN; jump to address NNN
            r_pc_ = nnn;
            break;
        case 0x2000u: // 2NNN; call subroutine at NNN
            stack_.push(r_pc_);
            r_pc_ = nnn;
            break;
        case 0x3000u: // 3XNN; skip next instr if VX == NN
            if (r_[x] == nn) r_pc_ += 2u;
            break;
        case 0x4000u: // 4XNN; skip next instr if VX != NN
            if (r_[x] != nn) r_pc_ += 2u;
            break;
        case 0x5000u: // 5XY0; skip next instr if VX == VY
            if (r_[x] == r_[y]) r_pc_ += 2u;
            break;
        case 0x6000u: // 6XNN; set VX to NN
            r_[x] = nn;
            break;
        case 0x7000u: // 7XNN; add NN to VX
            r_[x] += nn;
            break;
        case 0x8000u: {
            switch (op & 0x000Fu) {
                case 0x0000u: // 8XY0; set VX to VY
                    r_[x] = r_[y];
                    break;
                case 0x0001u: // 8XY1; set VX to VX OR VY
                    r_[x] |= r_[y];
                    break;
                case 0x0002u: // 8XY2; set VX to VX AND VY
                    r_[x] &= r_[y];
                    break;
                case 0x0003u: // 8XY3; set VX to VX XOR VY
                    r_[x] ^= r_[y];
                    break;
                case 0x0004u: // 8XY4; add VY to VX; set VF to 1 if carry occurs
                    r_[VF] = r_[x] > (UINT8_MAX - r_[y]);
                    r_[x] += r_[y];
                    break;
                case 0x0005u: // 8XY5; sub VY from VX; VF set to 1 if no borrow
                    r_[VF] = r_[y] < r_[x];
                    r_[x] -= r_[y];
                    break;
                case 0x0006u: // 8XY6; right shift VX by 1; VF set to LSB of VX
                    r_[VF] = r_[x] & 0x0001u;
                    r_[x] >>= 1u;
                    break;
                case 0x0007u: // 8XY7; set VX to VY - VX; VF set to 1 if no borrow
                    r_[VF] = r_[x] < r_[y];
                    r_[x] = r_[y] - r_[x];
                    break;
                case 0x000Eu: // 8XYE; left shift VX by 1; VF set to MSB of VX
                    r_[VF] = (r_[x] & 0x8000u) != 0u;
                    r_[y] <<= 1u;
                    break;
                default:
                    throw UnknownOpcodeError{op};
            }
            break;
        }
        case 0x9000u: // 9XY0; skip next instr if VX != VY
            if (r_[x] != r_[y]) r_pc_ += 2u;
            break;
        case 0xA000u: // ANNN; set I to NNN
            r_i_ = nnn;
            break;
        case 0xB000u: // BNNN; jump to address NNN + V0
            r_pc_ = r_[V0] + nnn;
            break;
        case 0xC000u: { // CXNN; set VX to rand[0,255] AND NN
            r_[x] = dist_0_255_(rng_) & nn;
            break;
        }
        case 0xD000u: // DXYN; draw sprite at (VX,VY), set VF if pixel flips from 1 to 0
            r_[VF] = 0;
            for (auto row = 0u; row < n; ++row) {
                auto pix = mem_[r_i_ + row];
                for (auto col = 0u; col < 8; ++col) {
                    if ((pix & (0x80u >> col)) != 0) {
                        if (gfx_[(r_[x] + col + ((r_[y] + row) * 64))] == 1)
                            r_[VF] = 1;
                        gfx_[(r_[x] + col + ((r_[y] + row) * 64))] ^= 1u;
                    }
                }
            }
            need_redraw_ = true;
            break;
        case 0xE000u:
            switch (nn) {
                case 0x009E: // EX9E; skip next instr if key in VX is pressed
                    if (key_[r_[x]] != 0) r_pc_ += 2;
                    break;
                case 0x00A1: // EXA1; skip next instr if key in VX isn't pressed
                    if (key_[r_[x]] == 0) r_pc_ += 2;
                    break;
                default:
                    throw UnknownOpcodeError{op};
            }
            break;
        case 0xF000u:
            switch (op & 0x00FFu) {
                case 0x0007u: // FX07; set VX to value of DT
                    r_[x] = (uint8_t) r_dt_;
                    break;
                case 0x000Au: { // FX0A; await keypress, store in VX
                    bool press = false;
                    for (uint8_t i = 0; i < NUM_REGISTERS; ++i) {
                        if (key_[i] != 0) {
                            r_[x] = i;
                            press = true;
                            break;
                        }
                    }
                    // If we didn't find a keypress, keep PC the same and try again
                    if (!press) r_pc_ -= 2;
                    break;
                }
                case 0x0015u: // FX15; set DT to VX
                    r_dt_ = r_[x];
                    break;
                case 0x0018u: // FX18; set ST to VX
                    r_st_ = r_[x];
                    break;
                case 0x001Eu: // FX1E; add VX to I; set VF to 1 if overflow occurs
                    r_[VF] = r_[x] > (UINT16_MAX - r_i_);
                    r_i_ += r_[x];
                    break;
                case 0x0029u: // FX29; set I to location of sprite for char in VX
                    r_i_ = x * 5u;
                    break;
                case 0x0033u: // FX33; store decimal rep of VX in I, I+1, I+2
                    mem_[r_i_] = r_[x] / 100;
                    mem_[r_i_ + 1] = (r_[x] / 10) % 10;
                    mem_[r_i_ + 2] = (r_[x] % 100) % 10;
                    break;
                case 0x0055u: // FX55; store [V0,VX] in mem starting at I
                    for (auto i = 0u; i <= x; ++i) mem_[r_i_ + i] = r_[i];
                    break;
                case 0x0065u: // FX65; fill [V0,VX] from mem starting at I
                    for (auto i = 0u; i <= x; ++i) r_[i] = mem_[r_i_ + i];
                    break;
                default:
                    std::ostringstream ss;
                    ss << "Unknown opcode: " << std::hex << op;
                    throw std::runtime_error(ss.str());
            }
            break;
        default:
            throw UnknownOpcodeError{op};
    }

    if (r_dt_ > 0) {
        --r_dt_;
    }

    if (r_st_ > 0) {
        #ifdef WINDOWS
        Beep(440, 1000);
        #elif LINUX
        std::system("echo -e \"\007\" >/dev/tty10");
        #else
        std::cout << "\a" << std::flush;
        #endif
        --r_st_;
    }
}

void Chip8::set_key(int key, bool val) {
    key_[key] = val;
}


std::ostream &operator<<(std::ostream &out, Chip8 &c8) {
    for (int i = 0; i < c8.NUM_REGISTERS; ++i) {
        if (i % 4 == 0) out << std::endl;
        out << 'V' << i << ": " << std::setfill('0') << std::setw(5) << (unsigned int) c8.r_[i] << ", ";
    }
    out << std::endl;
    out << "PC: " << c8.r_pc_ << ", I: " << c8.r_i_ << ", DT:" << c8.r_dt_ << ", ST: " << c8.r_st_;
    return out << std::endl;
}
