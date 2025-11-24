#include "qol.h"

#pragma region Cpu


typedef struct {

    u16 PC; //Program Counter
    u8 AC; //Accumulator
    u8 X; //
    u8 Y; //
    u8 SR; //Status Register, also known as P
    u8 SP; //Stack Pointer

} cpu_regs;

typedef struct {
    //Cpu vars
    u8 operand;
    u16 operand16;
    //Registers
    cpu_regs regs;

} cpu_6502;

cpu_6502 cpu = {0};

#pragma endregion

#pragma region Memory

//memory vars
u8 memory [65536];
u8 *rom;

#pragma endregion

/*
 * Take a filename and malloc it into our memory array
 */
int load_rom(char *filename);

/*
 * fetch ->  pull hex instruction from file
 * decode -> {ptr array of instruction set}
 * execute -> call function from pointer array
 */
void cpu_cycle();

/*Prints the cpu register values, as well as the Interrupt request vector
 *
 *  PC  IRQ  SR AC X  Y  SP
 * XXXX XXXX XX XX XX XX XX
 *
*/
void print_registers();

/*
 * returns a single byte from memory
 */
u8 bus_read(u16 address);

/*
 * writes a byte to memory
 */
void bus_write(u8 value, u16 address);

/*
 * checks if we hit a BRK instruction, and exits if we did
 */
int check_terminate();

/*------------------------------------IMPLEMENTATION-----------------------------------------*/



u8 bus_read(u16 address) {
    return memory[address];
}

void bus_write(u8 value, u16 address) {
    memory[address] = value;
}

void print_registers() {
    LINE
    printf("PC: %04X | AC: %02X | X: %02X | Y:%02X | SR: %02X | SP: %02X \n",
        cpu.regs.PC, cpu.regs.AC, cpu.regs.X, cpu.regs.Y,cpu.regs.SR, cpu.regs.SP);
}

int load_rom(char filename[]) {
    FILE *rom_file = fopen(filename, "rb");
    if (!rom_file) {
        printf("Error opening file %s\n", filename);
        perror("Error:");
        return 1;
    }
    fseek(rom_file, 0, SEEK_END);
    int size = ftell(rom_file);
    rewind(rom_file);
    rom = calloc(1, size);
    fread(rom, size, 1, rom_file);
    fclose(rom_file);
    return 0;
}

void cpu_cycle() {
    //fetch
    u8 op = rom[cpu.regs.PC]; //This is our instruction opcode in hex
    LINE
    printf("%02X\n", op);
    LINE
    //decode
    if (instruction_set[op].addr_mode == 1) {
        cpu.operand = rom[cpu.regs.PC + 1];
        cpu.regs.PC += 2;
    }
    else if (instruction_set[op].addr_mode == 2) {
        u16 lo = rom[cpu.regs.PC + 1];
        u8 hi = rom[cpu.regs.PC + 2];
        cpu.operand16 = (lo << 8) | (hi) ; // $LLHH operand
        cpu.regs.PC += 3;
    }
    else{cpu.regs.PC +=1;}

    //execute
    instruction_set[op].instruction_ptr();
}

int check_terminate() {
    if (rom[cpu.regs.PC] == 0x00) {
        LINE
        printf("Terminating...\n");
        LINE
        free(rom);
        return 1;
    }
    return 0;

}

#pragma region instruction_set_functions

//Set interrupt disable status
void SEI() {
    cpu.regs.SR |= I_FLAG;
}

#pragma endregion

int main(int argc, char *argv[]) {

    if (argc <2){perror("No ROM file provided");return -1;}

    printf("\nOpening file\n");
    if (load_rom(argv[1]) == 1){return 1;}

    printf("Starting emulator\n");

    // //Test
    // printf("\n %s \n",instruction_set[rom[0]+1].opcode);
    // instruction_set[rom[0]+1].instruction_ptr();
    // print_registers();

    while (1) {
        cpu_cycle();
        print_registers();
        if (check_terminate()){return 1;}

    }


    return 0;
}



