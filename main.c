#include "chip8.h"
#include <raylib.h>

int main(int argc, char **argv) {
  // il programma andrà eseguito nella seguente forma: ./emulatore <NOME_ROM>
  if (argc < 2 || !strcmp(argv[1], "-help")) {
    LOG_ERR("How to use: %s", "./chip8 <rom-name>");
    return 1;
  }

  char *rom = argv[1];
  LOG_INFO("Select OK: %s", rom);

  // init
  chip8 *cpu = init();
  LOG_INFO("Init OK%s", "");
  loadProgram(cpu, rom);
  LOG_INFO("Loading OK%s", "");

  InitWindow(WIDTH * SCALE, HEIGHT * SCALE, "CHIP 8 Emulator");
  SetTargetFPS(60);
  // emulaton loop
  while (!WindowShouldClose()) {
    handleInput(cpu->key);
    for (int i = 0; i < SCALE; i++)
      step(cpu);

    BeginDrawing();
    ClearBackground(BLACK);
    drawScreen(cpu->gfx);
    EndDrawing();

    if (cpu->sound_timer > 0)
      cpu->sound_timer--;
    if (cpu->delay_timer > 0)
      cpu->delay_timer--;
    // SetTargetFPS() garantisce di default che questo loop venga eseguito ogni
    // 1/60 secondi
  } // non è un metodo infallibile in generale ma dovrebbe andare bene per un
    // programma così semplice

  // cleanup
  clean(cpu);
  return TRUE;
}
