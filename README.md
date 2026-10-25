# CHIP-8-Emulator

CHIP-8 emulator (badly) written in C99. The project is an offshoot of the namesake course by [Esadecimale.it](esadecimale.it), however it differs by:
+ Graphics and input handling are implemented using Raylib (the only dependency required for compilation) whereas the course used SDL.
+ Sound has not been implemented.
+ Instead of handling opcode execution via a 35-case `switch` statement, an array of function pointers was used. 

https://github.com/user-attachments/assets/593c54dd-5c50-49f6-8c75-9f0ccf118c5d

## Useful links
+ [Esadecimale's course repo](https://github.com/Esadecimale/chip-8)
+ [First two hours of the course, free on YouTube](https://www.youtube.com/watch?v=1N8DqgSgnhE)
+ [CHIP-8 roms archive](https://github.com/kripod/chip8-roms)
+ [CHIP-8 opcodes table used as reference](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Se)
+ [How to install Raylib](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux)
