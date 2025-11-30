#include "qol.h"

#pragma region CPU

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

/*
 * Takes in the address mode of an instruction
 * and adjusts the operands accordingly so
 * that the instruction performs properly
 */
void addressMode(enum ADDR_MODE mode) {
}

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
 * Flag enable function,
 * takes in the "flag" u8's we defined in qol
 * and or's it with the status register
 */
void enable_flag(u8 flag);

/*
 * Flag disable function,
 * takes in the "flag" u8's we defined in qol
 * negates it, and &'s it with the status register
 */
void disable_flag(u8 flag);

#pragma endregion

#pragma region Memory

//memory vars
u8 memory[65536];
u8 *rom;

/*
 * returns a single byte from memory
 */
u8 bus_read(u16 address);

/*
 * writes a byte to memory
 */
void bus_write(u8 value, u16 address);

#pragma endregion

#pragma region MainloopDeclarations
/*
 * Take a filename and malloc it into our memory array
 */
int load_rom(char *filename);

/*
 * checks if we hit a BRK instruction, and exits if we did
 */
int check_terminate();

#pragma endregion

#pragma region ALU


#pragma endregion

//TBD: Unsure where to put these yet:

/*
 *Not the same as Bit Test
 *This returns true if the specified
 *bit is enabled and false if otherwise
 */
int testBit(u8 byte, int bitNum);

/*------------------------------------IMPLEMENTATION-----------------------------------------*/

#pragma region MainloopFunctions

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

#pragma endregion

#pragma region MemoryFunctions

u8 bus_read(u16 address) {
    return memory[address];
}

void bus_write(u8 value, u16 address) {
    memory[address] = value;
}

#pragma endregion

#pragma region CpuFunctions

void AddressMode(enum ADDR_MODE mode) {
    int byte_count;
    switch (mode) {
        case implied:
        case accumulator:
            byte_count = 0;
            break;
        case immediate:
        case zeroPage:
        case zerPageX:
        case zerPageY:
        case relative:
        case xIndexedIndirect:
        case indirectYIndexed:
            byte_count = 1;
            break;
        case absolute:
        case absoluteX:
        case absoluteY:
        case indirect:
            byte_count = 2;
            break;

        default:
            byte_count = 0;
            break;
    }

    //Resetting the operands
    cpu.operand = 0x00;
    cpu.operand16 = 0x00;


    //TODO: RESPEC FETCH SWITCH TO MATCH NEW ADDRESS MODE OPERAND USEAGE
    switch (byte_count) {
        case 1: {
            cpu.operand = rom[cpu.regs.PC + 1];
            cpu.regs.PC += 2;
            break;
        }
        case 2: {
            u8 lo = rom[cpu.regs.PC + 1];
            u16 hi = rom[cpu.regs.PC + 2];
            cpu.operand16 = (hi << 8) | (lo); // $LLHH operand
            cpu.regs.PC += 3;
            break;
        }
        default: {
            cpu.regs.PC += 1;
        }
    }

    switch (mode) {
        case implied:
        case immediate:
        case accumulator:

            //Implied/Immediate/AC, no adjust required
            break;
        case absolute: {
            //Address will be the operand16
            break;
        }
        case zeroPage: {
            //zeroPage
            cpu.operand16 = cpu.operand;
            break;
        }
        case absoluteX: {
            cpu.operand16 += cpu.regs.X;

            //TODO FOR CYCLE ACCURACY IMPLEMENT DELAY HERE
            break;
        }
        case absoluteY: {
            cpu.operand16 += cpu.regs.Y;

            //TODO FOR CYCLE ACCURACY IMPLEMENT DELAY HERE
            break;
        }
        case zerPageX: {
            cpu.operand16 = (cpu.operand + cpu.regs.X) & 0xFF;
            break;
        }
        case zerPageY: {
            cpu.operand16 = (cpu.operand + cpu.regs.Y) & 0xFF;
            break;
        }
        case indirect: {
            //read lo byte, read high byte, combine and set pc
            u16 addr = cpu.operand16;
            u8 lo = bus_read(addr);
            u8 hi = bus_read((addr & 0xFF00) | ((addr + 1) & 0x00FF));
            cpu.operand16 = ((u16) hi << 8) | lo;
            break;
        }
        case xIndexedIndirect: {
            u8 zero_page_lookupX = (cpu.operand + cpu.regs.X) & 0xFF;
            u8 lo = bus_read(zero_page_lookupX);
            u8 hi = bus_read((zero_page_lookupX + 1) & 0xFF);
            cpu.operand16 = ((u16) hi << 8) | lo;
            break;
        }
        case indirectYIndexed: {
            u8 lo = bus_read(cpu.operand);
            u8 hi = bus_read((cpu.operand + 1) & 0xFF);
            cpu.operand16 = ((u16) hi << 8) | lo;
            cpu.operand16 += cpu.regs.Y;
            break;
        }
        case relative: {
            cpu.operand = (int8_t) cpu.operand;
            break;
        }

        default:


    }
}

void cpu_cycle() {
    //fetch
    u8 op = rom[cpu.regs.PC]; //This is our instruction opcode in hex
    LINE
    printf("%02X\n", op);
    LINE
    //decode

    //pull needed bytes and adjust operands for address mode
    AddressMode(instruction_set[op].addr_mode);

    //execute
    instruction_set[op].instruction_ptr();
}

void print_registers() {
    LINE
    printf("PC: %04X | AC: %02X | X: %02X | Y:%02X | SR: %02X | SP: %02X \n",
           cpu.regs.PC, cpu.regs.AC, cpu.regs.X, cpu.regs.Y, cpu.regs.SR, cpu.regs.SP);
}

void enable_flag(u8 flag) {
    cpu.regs.SR |= flag;
}

void disable_flag(u8 flag) {
    cpu.regs.SR &= ~flag;
}

#pragma endregion

#pragma region AluFunctions


#pragma endregion

int testBit(u8 byte, int bitNum) {
    return (byte & (1 << bitNum)) > 0;
}


#pragma region Instruction_Set_Functions

void ORA() {
    cpu.regs.AC |= cpu.operand;
    if (cpu.regs.AC & (1 << 7)) {
        enable_flag(N_FLAG);
    } else { disable_flag(N_FLAG); }

    if (cpu.regs.AC == 0) {
        enable_flag(Z_FLAG);
    } else { disable_flag(Z_FLAG); }
}

//Set interrupt disable status
void SEI() {
    cpu.regs.SR |= I_FLAG;
}

#pragma endregion

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("No ROM file provided");
        return -1;
    }

    printf("\nOpening file\n");
    if (load_rom(argv[1]) == 1) { return 1; }

    printf("Starting emulator\n");

    // //Test
    // printf("\n %s \n",instruction_set[rom[0]+1].opcode);
    // instruction_set[rom[0]+1].instruction_ptr();
    // print_registers();

    while (1) {
        cpu_cycle();
        print_registers();
        if (check_terminate()) { return 1; }
    }


    return 0;
}
