#include "chip8.h"
#include "opcode.h"
#include <raylib.h>

const uint8_t fontset[80] = {
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

const CASE func[] = {f0, f1, f2, f3, f4, f5, f6, f7,
                     f8, f9, fA, fB, fC, fD, fE, fF};

const int layout[] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_Q, KEY_W,
                      KEY_E,   KEY_R,   KEY_A,     KEY_S,    KEY_D, KEY_F,
                      KEY_Z,   KEY_X,   KEY_C,     KEY_V};

chip8 *init(void) {
  chip8 *cpu = malloc(sizeof(chip8));
  if (!cpu) {
    LOG_ERR("Cannot allocate CPU%s", "");
    exit(FALSE);
  }

  memset(cpu, 0, sizeof(chip8));
  /* imposta a zero tutti i bit dell'area di memoria puntata da cpu
     viene da string.h e il suo prototipo è memset(void*, uint8_t, size_t),
     a partire dall'indirizzo di base cpu va a sostituire size_t byte con un
     uint8_t costante
  */

  memcpy(cpu->memory, fontset, 80); // alloca i font nei primi 80 byte della RAM

  cpu->running = TRUE;
  cpu->pc = 0x200; // tutti gli altri campi della struct chip8 sono pari a 0
                   // come conseguenza del: precedente memset

  srand(time(NULL)); // per istruzioni del tipo CXNN

  return cpu;
} // alloca e inizializza la cpu

void clean(chip8 *cpu) { free(cpu); }

void loadProgram(chip8 *cpu, char *rom) {
  FILE *f = fopen(rom, "rb");
  if (!f) {
    LOG_ERR("Cannot open ROM, invalid name: %s", rom);
    free(cpu);
    exit(FALSE);
  }
  fseek(f, 0, SEEK_END); // sposta il cursore alla fine del file
  unsigned long file_size =
      ftell(f); // restituisce l'attuale posizione del cursore, se è alla fine
                // restituisce  quindi la dimensione del file

  if (!file_size ||
      file_size > 0x1000 - 0x200) { // se il file ha dim=0 o dim>4096-0x200
    LOG_ERR("Cannot load ROM, too big: %lu", file_size);
    free(cpu);
    exit(FALSE);
  }

  rewind(f); // il cursore torna a inizio file
  size_t result = fread(cpu->memory + 0x200, 1, file_size, f);
  /* fread legge il file puntato da f e va a copiarlo nell'area di memoria
     puntata da cpu->memory + 0x200 quell'1 vuol dire che legge a blocchi da 1
     byte fino a un massimo di file_size byte oppure finchè non incontra l'EOF
     il ritorno salvato in result è il numero di byte letti
  */
  if (result != file_size) { // ovvero se non abbiamo letto il file nella sua
                             // interezza per qualche motivo
    LOG_ERR("Cannot load ROM%s", "");
    free(cpu);
    exit(FALSE);
  }

  fclose(f);
}

void step(chip8 *cpu) {
  if (cpu->pc >= 4095) {
    LOG_ERR("Could not increment program counter%s", "");
    exit(FALSE);
  }

  cpu->opcode = FETCH(cpu);
  LOG_INFO("Executing %#04x", cpu->opcode);
  cpu->pc += 2;
  (*func[(cpu->opcode & 0xF000) >> 12])(
      cpu); // viene eseguita una funzione di opcode.h identificata dalla prima
            // cifra esadeciamente di cpu->opcode
}

void drawScreen(Bool *gfx) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (gfx[x + y * WIDTH])
        DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, WHITE);
      else
        DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, BLACK);
    }
  }
} // disegna lo schermo tramite una corrispondeza diretta tra la matrice gfx e
  // la finestra di raylib (le cui dimensioni sono le stesse di gfx moltiplicate
  // per SCALE)
//

void handleInput(Bool *key) {
  for (int i = 0; i < 16; i++)
    key[i] = IsKeyDown(layout[i]);
}
