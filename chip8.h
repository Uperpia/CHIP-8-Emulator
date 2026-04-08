// TODO quirks

#ifndef CHIP8_H_
#define CHIP8_H_

#define HEIGHT 32
#define WIDTH 64
#define SCALE 15

#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef unsigned char Bool;
enum bool_key { FALSE, TRUE };

typedef struct {
  Bool running; // flag che ci dice se la macchina è attiva

  uint16_t opcode; // il CHIP 8 ha 35 opcode da 2 byte l'uno

  uint8_t memory[4096]; // 4KB di RAM
  /* Memory Map:
          0x000-0x1FF - sezione destinata all'interprete del CHIP 8 (in questo
     caso non serve) 0x050-0x0A0 - 4x5 pixel font set (0-F) 0x200-0xFFF -
     Program ROM and Data RAM
  */

  uint8_t V[16]; // 16 registi V0-VF ad uso generale

  uint16_t i, pc, sp; // index register, program counter e stack pointer

  uint16_t stack[16]; // per salvare gli indirizzi di ritorno quando un
                      // programma chiama una subroutine

  Bool gfx[WIDTH * HEIGHT]; // schermo di 2048 (64*32) pixel, ognuno può essero
                            // nero/spento (FALSE) o bianco/acceso (TRUE)

  uint8_t delay_timer, sound_timer; // entrambi i timer se posti a un valore >0
                                    // cominciano a decrementare fino a 0 a 60Hz

  Bool key[16]; // i 16 tasti del CHIP 8

} chip8;

/* typedef enum {
  C8_KEY_1,
  C8_KEY_2,
  C8_KEY_3,
  C8_KEY_4,
  C8_KEY_Q,
  C8_KEY_W,
  C8_KEY_E,
  C8_KEY_R,
  C8_KEY_A,
  C8_KEY_S,
  C8_KEY_D,
  C8_KEY_F,
  C8_KEY_Z,
  C8_KEY_X,
  C8_KEY_C,
  C8_KEY_V
} key; */ // non serve più....

#define LOG_INFO(msg, ...) fprintf(stdout, "INFO - " msg "\n", ##__VA_ARGS__);
#define LOG_ERR(msg, ...)                                                      \
  fprintf(stderr, "\033[0;31mERR - " msg "\033[0m\n", ##__VA_ARGS__);
// per ambedue, se devi stampare un messaggio senza parametri ti basta fare
// LOG_XXX("[...]%s", "") dove [...] è un letterale costante

#define FETCH(cpu)                                                             \
  (uint16_t)((cpu->memory[cpu->pc]) << 8) | (uint16_t)(cpu->memory[cpu->pc + 1])

chip8 *init(void);

void clean(chip8 *cpu);

void loadProgram(chip8 *cpu, char *rom);

void step(chip8 *cpu);

void drawScreen(Bool *gfx);

void handleInput(Bool *key);
#endif // CHIP8_H_
