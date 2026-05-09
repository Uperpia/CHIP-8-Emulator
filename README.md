# CHIP-8-Emulator

Emulatore veramente mal scritto (in C99) ma funzionante per il CHIP 8. Il progetto è figlio dell'omonimo corso di [Esadecimale.it](esadecimale.it), tuttavia vi sono alcune fondamentali differenze:
+ Grafica e gestione dell'input è implementata tramite Raylib, unica dipendenza necessaria per la compilazione, mentre il corso usava SDL
+ Il sonoro non è stato implementato
+ Invece di gestire l'esecuzione dell'opcode tramite uno switch-case di 35 casi si è preferito usare un'array di puntatori a funzione

## Link utili
+ [Repo del progetto di Esadecimale](https://github.com/Esadecimale/chip-8)
+ [Prime due ore del corso disponibili gratuitamente su YouTube](https://www.youtube.com/watch?v=1N8DqgSgnhE)
+ [Archivio di ROM per il CHIP 8](https://github.com/kripod/chip8-roms)
+ [Tabella degli opcode](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Se)
+ [Per installare raylib/le dipendenze necessarie](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux)
