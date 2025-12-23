
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
    union {
        struct {
            u8 lo;
            u8 hi;
        };
        u16 operand16;
    };
        //Registers
    cpu_regs regs;
	u8 cop;
} cpu_6502;

cpu_6502 cpu = {0};

/*
 * Takes in the address mode of an instruction
 * and adjusts the operands accordingly so
 * that the instruction performs properly
 */
void addressMode(enum ADDR_MODE mode);

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
#define STACK_ADDR 0x0100

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
    //cpu.operand = 0x00;
    cpu.operand16 = 0x00;


    //TODO: RESPEC FETCH SWITCH TO MATCH NEW ADDRESS MODE OPERAND USEAGE
    switch (byte_count) {
        case 1: {
            cpu.lo = rom[cpu.regs.PC + 1];
            cpu.regs.PC += 2;
            break;
        }
        case 2: {
            cpu.lo = rom[cpu.regs.PC + 1];
            cpu.hi = rom[cpu.regs.PC + 2];

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
			break;
        case accumulator:
			
            //Implied/Immediate/AC, no adjust required
			cpu.operand16 = cpu.regs.AC;
            break;
        case absolute: {
            //Address will be the operand16
            break;
        }
        case zeroPage: {
            //zeroPage
            //operand stays the same
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
            cpu.operand16 = (cpu.operand16 + cpu.regs.X) & 0xFF;
            break;
        }
        case zerPageY: {
            cpu.operand16 = (cpu.operand16 + cpu.regs.Y) & 0xFF;
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
            u16 zero_page_lookupX = (cpu.operand16 + cpu.regs.X) & 0xFF;
            u8 lo = bus_read(zero_page_lookupX);
            u8 hi = bus_read((zero_page_lookupX + 1) & 0xFF);
            cpu.operand16 = ((u16) hi << 8) | lo;
            break;
        }
        case indirectYIndexed: {
            u8 lo = bus_read(cpu.operand16);
            u8 hi = bus_read((cpu.operand16 + 1) & 0xFF);
            cpu.operand16 = ((u16) hi << 8) | lo;
            cpu.operand16 += cpu.regs.Y;
            break;
        }
        case relative: {
            cpu.lo = (int8_t) cpu.lo;
            break;
        }

        default:
            break;


    }
}

void cpu_cycle() {
    //fetch
    cpu.cop = rom[cpu.regs.PC]; //This is our instruction opcode in hex
    LINE
    printf("%02X\n", cpu.cop);
    LINE
    //decode

    //pull needed bytes and adjust operands for address mode
    AddressMode(instruction_set[cpu.cop].addr_mode);

    //execute
    instruction_set[cpu.cop].instruction_ptr();
}

void print_registers() {
    LINE
    printf("PC: %04X | AC: %04X | X: %02X | Y:%02X | SR: %02X | SP: %02X | OPER L: %u | OPER H: %u \n",
           cpu.regs.PC, cpu.regs.AC, cpu.regs.X, cpu.regs.Y, cpu.regs.SR, cpu.regs.SP, cpu.lo, cpu.hi);
}

void enable_flag(u8 flag) {
    cpu.regs.SR |= flag;
}

void disable_flag(u8 flag) {
    cpu.regs.SR &= ~flag;
}

void check_flag(enum FLAGS flag, u16 byte) {
    switch (flag) {
        case N:
            if (byte & 0x80) {
                enable_flag(N_FLAG);
                
            }
            else {
                disable_flag(N_FLAG);
                
            }
            break;
        case Z:
            if (byte == 0) {
                enable_flag(Z_FLAG);
            }
            else {
                disable_flag(Z_FLAG);
            }
           break;
		    case C:
		      if(byte > 0xFF){
				    enable_flag(C_FLAG);
		      }
		      else{
			    disable_flag(C_FLAG);
			    }
		      break;

        default:
            printf("\n Undefined Flag \n");
    }
}

int is_flag_set(u8 flag){
  return((cpu.regs.SR & flag)!=0);
}

#pragma endregion

#pragma region Transfer_Instructions

void LDA(){
	cpu.regs.AC = bus_read(cpu.operand16);
	check_flag(N, cpu.regs.AC);
	check_flag(Z, cpu.regs.AC);

}

void LDX(){

	cpu.regs.X = bus_read(cpu.operand16);
	check_flag(N, cpu.regs.X);
	check_flag(Z, cpu.regs.X); 


}

void LDY(){
    cpu.regs.Y = bus_read(cpu.operand16);
	check_flag(N, cpu.regs.Y);
	check_flag(Z, cpu.regs.Y);

}

void STA(){
	bus_write(cpu.regs.AC, cpu.operand16);

}

void STX(){
	bus_write(cpu.regs.X, cpu.operand16);
}

void STY(){
	bus_write(cpu.regs.Y, cpu.operand16);
}

//we americans don't like that
void TAX(){
	cpu.regs.X = cpu.regs.AC;
	check_flag(N, cpu.regs.X);
	check_flag(Z, cpu.regs.X);
}

void TAY(){
	cpu.regs.Y = cpu.regs.AC;
	check_flag(N, cpu.regs.Y);
	check_flag(Z, cpu.regs.Y);
}

void TSX(){
	cpu.regs.X = cpu.regs.SP;
	check_flag(N, cpu.regs.X);
	check_flag(Z, cpu.regs.X);
}

void TXA(){
	cpu.regs.AC = cpu.regs.X;
	check_flag(N, cpu.regs.AC);
	check_flag(Z, cpu.regs.AC);
}

//Texas instruments?
void TXS(){
	cpu.regs.SP = cpu.regs.X;
}

void TYA(){
	cpu.regs.SP = cpu.regs.Y;
}


#pragma endregion


#pragma region AluFunctions

void OR() {
    u8 byte = bus_read(cpu.operand16);
    cpu.regs.AC |= byte;
    check_flag(N, cpu.regs.AC);
    check_flag(Z, cpu.regs.AC);

}

void AND() {
    u8 byte = bus_read(cpu.operand16);
    cpu.regs.AC &= byte;
    check_flag(N, cpu.regs.AC);
    check_flag(Z, cpu.regs.AC);

}

void XOR() {
    u8 byte = bus_read(cpu.operand16);
    cpu.regs.AC ^= byte;
    check_flag(N, cpu.regs.AC);
    check_flag(Z, cpu.regs.AC);

}

void DEC() {
    u8 byte = bus_read(cpu.operand16)-1;
    check_flag(N, byte);
    check_flag(Z, byte);
    bus_write(cpu.operand16, byte);
}
void DEX() {
    cpu.regs.X -=1;
    check_flag(N, cpu.regs.X);
    check_flag(Z, cpu.regs.X);
}
void DEY() {
    cpu.regs.Y -=1;
    check_flag(N, cpu.regs.Y);
    check_flag(Z, cpu.regs.Y);
}

void INC() {
    u8 byte = bus_read(cpu.operand16)+ 1;
    check_flag(N, byte);
    check_flag(Z, byte);
    bus_write(cpu.operand16, byte);
}
void INX() {
    cpu.regs.X += 1;
    check_flag(N, cpu.regs.X);
    check_flag(Z, cpu.regs.X);
}
void INY() {
    cpu.regs.Y += 1;
    check_flag(N, cpu.regs.Y);
    check_flag(Z, cpu.regs.Y);
}
void ADC() {
	//Add memory to A with carry
	//check NZC flags following op
	u8 byte = bus_read(cpu.operand16);
	cpu.regs.AC += byte += (cpu.regs.SR & C_FLAG);
	check_flag(C, cpu.regs.AC);
	check_flag(N, cpu.regs.AC);
	check_flag(Z, cpu.regs.AC);
	
	
}

void SBC() {
	u8 byte = bus_read(cpu.operand16);
	cpu.regs.AC -= byte -= (cpu.regs.SR & C_FLAG);
	check_flag(C, cpu.regs.AC);
	check_flag(N, cpu.regs.AC);
	check_flag(Z, cpu.regs.AC);
}

void ASL(){
   cpu.regs.SR &=~1;
   if (cpu.cop == 0x0A){
    //Accumulator mode
    //reset C flag
    cpu.regs.SR |= (cpu.regs.AC & 0x80 ? 1: 0);
    cpu.regs.AC <<=1;
    check_flag(Z, cpu.regs.AC);
    check_flag(N, cpu.regs.AC);
    

  } else{
    //use operand
    u8 byte = bus_read(cpu.operand16);
    cpu.regs.SR |= (byte & 0x80 ? 1: 0);
    byte <<=1;
    bus_write(byte, cpu.operand16);
    check_flag(Z, byte);
    check_flag(N, byte);
  }

}

void LSR(){
  cpu.regs.SR &=~1;
  if(cpu.cop == 0x4A){
    //Accumulator mode  
    cpu.regs.SR |= (cpu.regs.AC & 1 ? 1:0);
    cpu.regs.AC >>=1;
    check_flag(N, cpu.regs.AC);
    check_flag(Z, cpu.regs.AC);

  } else{
    //other address mode, use
    //operand
    u8 byte = bus_read(cpu.operand16);
    cpu.regs.SR |= (byte & 1 ? 1:0);
    byte >>=1;
    bus_write(byte, cpu.operand16);
    check_flag(N, byte);
    check_flag(Z, byte);

  }


}

void ROL(){
/*
 * Step-by-step algorithm
  Save old Carry
  Set Carry from original bit 7
  Shift left
  Insert old Carry into bit 0
  Update Z and N
*/
  u8 old_carry = cpu.regs.SR & 1;
  cpu.regs.SR &=~1;
  if(cpu.cop == 0x2A){
      cpu.regs.SR |= (cpu.regs.AC & 0x80 ? 1:0);
      cpu.regs.AC <<=1;
      cpu.regs.AC |= old_carry;
      check_flag(N, cpu.regs.AC);
      check_flag(Z, cpu.regs.AC);
      
  } else{
      u8 byte = bus_read(cpu.operand16);
      cpu.regs.SR |= (byte & 0x80 ? 1:0);
      byte <<=1;
      byte |= old_carry;
      bus_write(byte, cpu.operand16);
      check_flag(N, byte);
      check_flag(Z, byte);
      
  }

}

void ROR(){
/*
 * Step-by-step algorithm
  Save old Carry
  Set Carry from original bit 0
  Shift right
  Insert old Carry into bit 7
  Update Z and N 
 */
  u8 old_carry = cpu.regs.SR &1;
  cpu.regs.SR &=~1;
  if(cpu.cop == 0x6A){
    cpu.regs.SR |= (cpu.regs.AC & 0x1);
    cpu.regs.AC >>=1;
    check_flag(N, cpu.regs.AC);
    check_flag(Z, cpu.regs.AC);

  } else{
    u8 byte = bus_read(cpu.operand16);
    cpu.regs.SR |= (byte & 1);
    byte >>=1;
    bus_write(byte, cpu.operand16);
    check_flag(N, byte);
    check_flag(Z, byte);


  }
  cpu.regs.SR |= (old_carry <<7);


}

#pragma endregion


#pragma region Stack_Instructions

void Push(u8 value){
  bus_write(value, STACK_ADDR | cpu.regs.SP);
  cpu.regs.SP --;
  
}

u8 Pop(){
  cpu.regs.SP ++;
  return bus_read(STACK_ADDR | cpu.regs.SP);

}

void Push16(u16 value){
  Push((value >> 8) & 0xFF);
  Push(value & 0xFF);
}

u16 Pop16(){
  u8 lo = Pop();
  u8 hi = Pop();
  return ((hi << 8) | lo);
}

//
void PHA(){
  Push(cpu.regs.AC); 
}

void PHP(){
  //The status register will be pushed with
  //the break flag and bit 5 set to 1
  Push(cpu.regs.SR | B_FLAG);
}

void PLA(){
  cpu.regs.AC = Pop();
  check_flag(N, cpu.regs.AC);
  check_flag(Z, cpu.regs.AC);
}

void PLP(){
 cpu.regs.SR = Pop();
    cpu.regs.SR &= ~B_FLAG;
}

#pragma endregion


#pragma region MISC_Instructions

void BIT(){
  //to do:
  // grab AC and mem byte
  // AND them and use result to:
  // 1. Store bit 7 in N flag
  // 2. Store bit 6 in V flag
  // 3. Check Z flag
  u8 res = cpu.regs.AC & bus_read(cpu.operand16);
  res & 0x80 ? enable_flag(N_FLAG): disable_flag(N_FLAG);
res & 0x40 ? enable_flag(V_FLAG): disable_flag(V_FLAG);
  check_flag(Z, res);
  

}

void NOP(){
	//Nothing to see here
}

#pragma endregion MISC_Instructions

#pragma region Conditional_Branch_Instructions

void BRANCH_IF(int cond){
  if(cond){
    cpu.regs.PC += (int8_t)cpu.lo;
  }
}

void BCC(){
  BRANCH_IF(!is_flag_set(C_FLAG));
}

void BCS(){
  BRANCH_IF(is_flag_set(C_FLAG));
}

void BEQ(){
  BRANCH_IF(is_flag_set(Z_FLAG));
}

void BNE(){
 BRANCH_IF(!is_flag_set(Z_FLAG));
}

void BMI(){
  BRANCH_IF(is_flag_set(N_FLAG));
}

void BPL(){
BRANCH_IF(is_flag_set(N_FLAG));
}

void BVS(){
  BRANCH_IF(is_flag_set(V_FLAG));
}

void BVC(){
  BRANCH_IF(!is_flag_set(V_FLAG));
}




#pragma endregion Conditional_Branch_Instructions

#pragma region Jumps_and_Subroutines

void JMP(){

  if(cpu.cop == 0x4C){
    cpu.regs.PC = cpu.operand16;
  }
  else {

    u16 ptr = cpu.operand16;
    u8 lo = bus_read(ptr);
    u8 hi = bus_read((ptr & 0xFF00) | ((ptr + 1) & 0x00FF));

    cpu.regs.PC = ((hi << 8 ) | lo);
  }
}
/*
void INDIR_JMP(){
  u16 ptr = cpu.operand16;
  u8 lo = bus_read(ptr);
  u8 hi = bus_read((ptr & 0xFF00) | ((ptr + 1) & 0x00FF));

  cpu.regs.PC = ((hi << 8 ) | lo);
}
*/
void JSR(){
  //Push the program counter by 2
  //PC low = operand lo 
  //PC hi = operand hi 
  Push16(cpu.regs.PC -1);
  cpu.regs.PC = cpu.operand16;
}

void RTS(){
  cpu.regs.PC = Pop16() + 1;

}


#pragma endregion Jumps_and_Subroutines

#pragma region Interrupts

void BRK(){
 cpu.regs.PC ++;
  Push16(cpu.regs.PC);
  Push(cpu.regs.SR | B_FLAG);
  cpu.regs.SR |=I_FLAG;

  u8 lo = bus_read(0xFFEE);
  u8 hi = bus_read(0xFFFF);

  cpu.regs.PC = (hi << 8) | lo;


}


void RTI(){
  cpu.regs.SR = Pop();
  cpu.regs.SR &= ~B_FLAG;
  cpu.regs.PC = Pop16();
}

#pragma endregion Interrupts

#pragma region Flag_Instructions

void CLC(){
	disable_flag(C_FLAG);
}

void CLD(){

	disable_flag(D_FLAG);

}


void CLI(){

	disable_flag(I_FLAG);
}

void CLV(){

	disable_flag(V_FLAG);
}

void SEC(){

	enable_flag(C_FLAG);
}

void SED(){
	
	enable_flag(D_FLAG);

}

void SEI(){

	enable_flag(I_FLAG);
}

#pragma endregion Flag_Instructions

#pragma region Compare_Instructions

void CMP(){

	u16 res = cpu.regs.AC - cpu.operand16;
	check_flag(C, res);
	check_flag(N, res);
	check_flag(Z, res);


}

void CPX(){
	u16 res = cpu.regs.X -cpu.operand16;
	check_flag(C, res);
	check_flag(N, res);
	check_flag(Z, res);
}

void CPY(){
	u16 res = cpu.regs.Y -cpu.operand16;
	check_flag(C, res);
	check_flag(N, res);
	check_flag(Z, res);
}


#pragma endregion Compare_Instructions

int main(int argc, char *argv[]) {
    // if (argc < 2) {
    //     perror("No ROM file provided");
    //     return -1;
    // }

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
