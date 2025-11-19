#include "qol.h"

#pragma region Cpu

typedef struct {
    //Not exactly the most efficient when I could use a single u8,
    //this is more of a readability thing

    u8 reg; //Serves as the register for stack operations and what not

    //The flag u8's will be constants to OR / AND with the
    //register when needed, just so I don't have to type them out manually
    const u8 N;
    const u8 V;
    const u8 B;
    const u8 D;
    const u8 I;
    const u8 Z;
    const u8 C;
} status_register;

typedef struct {

    u16 PC; //Program Counter
    u8 AC; //Accumulator
    u8 X; //
    u8 Y; //
    status_register SR; //Status Register, also known as P
    u8 SP; //Stack Pointer

} cpu_regs;

typedef struct {

    // Opcode hex num, cycleCount, ptr to function
    char* opcode;
    int cycles_count;
    int (*instruction_ptr) ();
    //TODO: make enum for addressing modes


} cpu_instruction;

typedef struct {
    //Cpu vars
    u8 operand;
    u16 operand16;
    //Registers
    cpu_regs regs;
    //Instruction Set
    cpu_instruction instruction_set[];
} cpu_6502;

cpu_6502 cpu = {0};
#pragma endregion

#pragma region Memory

//memory vars
u8 memory [65536];
u8 *rom;

#pragma endregion

#pragma region Instructions

/*
 * List of instructions detailing opcode, cycle count and instruction pointer
 * should also match instruction to addressing mode
 */

#pragma endregion

/*
 * Take a filename and malloc it into our memory array
 */
int load_rom(char *filename);


/*
 * Takes an opcode and matches it to an instruction using
 * the instruction struct detailed above
 */
void decode_instruction(char* opcode);


//functiond decs
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
 * returns a single byte from the file
 */
u8 bus_read(u16 address);

/*
 * writes a byte to memory
 */
void bus_write(u8 value, u16 address);



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
        cpu.regs.PC, cpu.regs.AC, cpu.regs.X, cpu.regs.Y,cpu.regs.SR.reg, cpu.regs.SP);
}

int load_rom(char filename[]) {
    FILE *rom_file = fopen(filename, "rb");
    if (!rom_file) {
        printf("Error opening file %s\n", filename);
        perror("Error opening file");
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

int main(int argc, char *argv[]) {

    if (argc <2){perror("No ROM file provided");return -1;}

    printf("\nOpening file\n");
    if (load_rom(argv[1]) == 1){return 1;}

    printf("Starting emulator\n");


    //Test
    printf("TEST: %02X\n", rom[0]);


    return 0;
}



