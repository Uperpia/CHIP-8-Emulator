#include "chip8.h"

#define X ((cpu->opcode & 0x0F00) >> 8)
#define Y ((cpu->opcode & 0x00F0) >> 4)
#define N (cpu->opcode & 0x000F)
#define NN (cpu->opcode & 0x00FF)
#define NNN (cpu->opcode & 0x0FFF)

#define VX (cpu->V[X])
#define VY (cpu->V[Y])
#define VI (cpu->V[i])
#define VF (cpu->V[0xF])
#define V0 (cpu->V[0])
#define STACK (cpu->stack)
#define SP (cpu->sp)
#define PC (cpu->pc)
#define GFX (cpu->gfx)
#define I (cpu->i)
#define KEY (cpu->key)
#define SOUND (cpu->sound_timer)
#define DELAY (cpu->delay_timer)
#define MEM (cpu->memory)

typedef void (*CASE)(chip8 *);

void f0(chip8 *cpu) {
  switch (NNN) {
  case 0x0E0: // 0x00E0, clears the screen
    memset(GFX, 0, HEIGHT * WIDTH);
    break;
  case 0x0EE: // 0x00EE, return from a subroutine
    PC = STACK[--SP];
    break;
  default: // 0x0NNN, execute machine language subroutine at address NNN
           /* questo comando è dipendente dalla macchina su cui viene eseguito,
           generalmente le ROM non lo usano in quanto implica che il loro funzionamento
           non è valido su ogni architettura e ne ometto dunque l'implementazione*/
    LOG_ERR("Invalid cmd: %#06x", cpu->opcode);
    exit(FALSE);
  } // end switch
}

void f1(chip8 *cpu) {
  PC = NNN;
  // 0x1NNN, jump to address NNN
}

void f2(chip8 *cpu) {
  if (SP == 16) {
    LOG_ERR("Stack overflow%s", "");
    exit(FALSE);
  }

  STACK[SP++] = PC;
  PC = NNN;
  // 0x2NNN, call a subroutin at address NNN
}

void f3(chip8 *cpu) {
  if (VX == NN)
    PC += 2;
} // 0x3XNN, skip next istruction if VX=NN

void f4(chip8 *cpu) {
  if (VX != NN)
    PC += 2;
} // 0x4XNN, skip next istruction if not VX=NN

void f5(chip8 *cpu) {
  if (N) {
    LOG_ERR("Invalid cmd: %#06x", cpu->opcode);
    exit(FALSE);
  }
  if (VX == VY)
    PC += 2;
} // 0x5XY0, if VX=VY skip next istruction (in this case N is always 0, if not
  // it's a error o another quirk)

void f6(chip8 *cpu) { VX = NN; } // 0x6XNN, save NN in VX

void f7(chip8 *cpu) { VX += NN; } // 0x7XNN, add NN to VX and ignore carry out

void f8(chip8 *cpu) {
  switch (N) {
  case 0: // assign VY to VX
    VX = VY;
    break;
  case 1: // assign (VX OR VY) to VX
    VX |= VY;
    break;
  case 2: // assign (VX AND VY) to VX
    VX &= VY;
    break;
  case 3: // assign (VX XOR VY) to VX
    VX ^= VY;
    break;
  case 4: // add VY to VX and set VF to 1 if there is overflow, 0 if not (255
          // max 8 bit integer)
  {
    Bool flow = (VX + VY > 255);
    VX += VY;
    VF = flow;
  } break;
  case 5: //  sub VY to VX and set VF to 0 if there is underflow, 1 if not
  {
    Bool flow = (VX > VY);
    VX -= VY;
    VF = flow;
  } break;
  case 6: // right shift VX and set VF to least significant VX bit prior shift
  {
    Bool bit = VX & 0x01;
    VX >>= 1;
    VF = bit;
  } break;
  case 7: // set VX to VY-VX. VF is set like case 5
  {
    Bool flow = (VY > VX);
    VX = VY - VX;
    VF = flow;
  } break;
  case 0xE: // left shift VX and set VF to most significat VX bit prior shift
  {
    Bool bit = (VX & 0x80) >> 7;
    VX <<= 1;
    VF = bit;
  } break;
  default:
    LOG_ERR("Invalid cmd: %#06x", cpu->opcode);
    exit(FALSE);
  } // end switch
} // 0x8XYN

void f9(chip8 *cpu) {
  if (N) {
    LOG_ERR("Invalid cmd: %#06x", cpu->opcode);
    exit(FALSE);
  }
  if (VX != VY)
    PC += 2;
} // 0x9XY0, skip next istruction if VX is not equal to VY (N always equal to 0)

void fA(chip8 *cpu) { I = NNN; } // 0xANNN, set I to NNN

void fB(chip8 *cpu) { PC = V0 + NNN; } // 0xBNNN, jump to VO + NNN

void fC(chip8 *cpu) {
  VX = rand() & NN;
} // 0xCNNN, set VX to a random number & NN

void fD(chip8 *cpu) { // TODO RIVEDERE OPCODE 6
  VF = FALSE;
  for (uint8_t yline = 0; yline < N; yline++) {
    if (VY + yline >= HEIGHT)
      break;
    uint8_t row = MEM[I + yline];
    for (uint8_t xline = 0; xline < 8; xline++) {
      if (VX + xline >= WIDTH)
        break;
      uint16_t bit_value = row & (0x80 >> xline);
      if (bit_value) {
        uint16_t bit_index = (VX + xline) + (VY + yline) * WIDTH;
        if (GFX[bit_index])
          VF = TRUE;
        GFX[bit_index] ^= TRUE;
      } // endif
    } // endfor xline
  } // endfor yline

} // 0xDXYN draw a sprite at position (VX,VY) with N bytes of font data started
  // at address I, also set VF to 1 if any pixel flipped to unset during draw

void fE(chip8 *cpu) {
  switch (NN) {
  case 0x9E: // skip next istruction if key[VX & 0x0F] is pressed
    if (KEY[VX & 0x0F])
      PC += 2;
    break;
  case 0xA1: // skip if not presseed
    if (!KEY[VX & 0x0F])
      PC += 2;
    break;
  default:
    LOG_ERR("Invalid cmd: %#06x", cpu->opcode);
    exit(FALSE);
  } // end switch
} // 0xEXNN

void fF(chip8 *cpu) {
  switch (NN) {
  case 0x07: // set VX to delay timer
    VX = DELAY;
    break;
  case 0x0A: // wait for a key press and set VX to the hex of pressed key
  {
    Bool key_press = FALSE;
    for (int i = 0; i < 16; i++)
      if (KEY[i]) {
        VX = i;
        key_press = TRUE;
        break;
      }
    if (!key_press) {
      PC -= 2;
      return;
    }
    // se nessun tasto viene premuto il PC viene decrementato e quindi la
    // corrente istruzione verrà ripetuta, così facendo abbiamo gli stessi
    // effetti dell'attesa ma senza entrare in un ciclo infinito bloccando le
    // altre parti del programma (come grafica e sono ad esempio!)
  } break;
  case 0x15: // set delay timer to VX
    DELAY = VX;
    break;
  case 0x18: // set sound timer to VX
    SOUND = VX;
    break;
  case 0x1E: // add VX to I
    I += VX;
    break;
  case 0x29: // set I to MEM address of the font data corresponding to hex digit
             // (& 0x0F) store in VX
    I = 0 + (VX & 0x0F) * 0x5;
    break;
  case 0x33: // store the binary-coded decimal digits* of VX to MEM at addresses
             // I, I+1 and I+2. * Max value of VX is 0xFF or 255, it has max 3
             // decimal digits!
    MEM[I] = VX / 100;
    MEM[I + 1] = (VX / 10) % 10;
    MEM[I + 2] = VX % 10;
    break;
  case 0x55: // store from V0 ... VX in MEM at address I ... I+X, then set I to
             // I + X + 1;
    if (I + X >= 4096) {
      LOG_ERR("MEM out of bound in 0xF55%s", "");
      exit(FALSE);
    }
    for (uint8_t i = 0; i <= X; i++)
      MEM[I + i] = VI;
    I += X + 1; // prova a commentare questa riga se riscontri bug in alcune ROM
    break;
  case 0x65: // same of 0x55 but store from MEM to VO ... VX
    if (I + X >= 4096) {
      LOG_ERR("MEM out of bound in 0xF55%s", "");
      exit(FALSE);
    }
    for (uint8_t i = 0; i < X; i++)
      VI = MEM[I + 1];
    I += X + 1; // prova a commentare questa riga se riscontri bug in alcune ROM
    break;
  default:
    LOG_ERR("Invalid cmd: %#06x", cpu->opcode);
    exit(FALSE);
  } // end switch
} // 0xFXNN
