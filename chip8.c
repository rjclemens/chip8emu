#include <stdint.h>
#include <stdlib.h>

typedef struct bussin bussin;
typedef struct cpu cpu;
struct cpu{
    uint16_t registers[0x10];
    uint16_t I;
    uint8_t delay;
    uint8_t sound;

    uint16_t PC;
    uint8_t SP;

    uint16_t stack[0x10];

    bussin* bus;
}

struct bussin{
    uint8_t rammy[0x1000];
    uint64_t display[0x20]; //64x32 bit display

    cpu cpu; //store the cpu instance on the bus
}

void clear_display(cpu* cpu){
    memset(cpu->bus->display, 0, 32*sizeof(uint64_t)); //clears the entire display array
}

uint8_t read_RAM(bussin* bus, uint16_t instr_addr){
    return bus->rammy[instr_addr]; //arrays are pointers
}

void write_RAM(bussin* bus, uint16_t write_addr, uint8_t data){
    bus->rammy[write_addr] = data;
}

uint16_t fetch_instr(cpu* cpu){
    uint16_t hi_byte = read_RAM(cpu->bus, cpu->PC++); //accesses PC, increments after
    uint16_t lo_byte = read_RAM(cpu->bus, cpu->PC++); //now PC goes to next instr

    return (hi_byte << 8) | lo_byte;
}

void cpu_exe(cpu* cpu){ //also does decoding
    uint16_t instr = fetch_instr(cpu);

    uint16_t nnn = instr & 0xFFF; //least significant 12 bits of instr
    uint16_t x = (instr & 0xF00) >> 8; //lower 4 bits of high byte
    uint16_t y = (instr & 0xF0) >> 4; //upper 4 bits of low byte
    uint16_t kk = instr & 0xFF; //lower byte of instr
    uint16_t n = instr & 0Xf; //last nibble

    switch(instr >> 15){ //check most sig bit of instr
        case 0x0:
            switch(instr){
                case 0x00E0:
                    clear_display(cpu);
                    break;

                case 0x00EE:
                    cpu->PC = cpu->stack[cpu->SP--]; //postdecrement SP 
                    break;
            }

            break;
        
        case 0x1:
            cpu->PC = nnn;
            break;

        case 0x2:
            cpu->stack[++cpu->sp] = cpu->PC; //put current pc at top of stack
            cpu->PC = nnn; //set pc to least significant 12 bits of instr
            break;

        case 0x3:
            cpu->PC += (cpu->registers[x] == kk) << 1; //increments PC by 2 if Vx == kk, else does nothing
            break;

        case 0x4:
            cpu->PC += (cpu->registers[x] != kk) << 1;  //increments PC by 2 if Vx != kk, else does nothing
            break;

        case 0x5:
            cpu->PC += (cpu->registers[x] == cpu->registers[y]) << 1;  //increments PC by 2 if Vx == Vy, else does nothing
            break;

        case 0x6:
            cpu->registers[x] = kk;
            break;

        case 0x7:
            cpu->registers[x] += kk;
            break;
        
        case 0x8:
            switch(instr & 0xF){
                case 0x0:
                    cpu->registers[x] = cpu->registers[y];
                    break;

                case 0x1:
                    cpu->registers[x] = cpu->registers[x] | cpu->registers[y];
                    break;
                
                case 0x2:
                    cpu->registers[x] = cpu->registers[x] & cpu->registers[y];
                    break;
                
                case 0x3:
                    cpu->registers[x] = cpu->registers[x] ^ cpu->registers[y];
                    break;
                
                case 0x4:
                    uint16_t temp = cpu->registers[x] + cpu->registers[y];
                    cpu->registers[x] = temp & 0xFF; //takes least significant 8 bits
                    cpu->registers[0xF] = (temp > 0xFF); //if the sum is >8 bits, we have overflow, set flag reg to 1
                    break;

                case 0x5:
                    cpu->registers[0xF] = cpu->registers[x] > cpu->registers[y]; //VF = not borrow
                    cpu->registers[x] -= cpu->registers[y];
                    break;

                case 0x6:
                    cpu->registers[0xF] = cpu->registers[x] & 0x1; //set flag if least sig bit is 1
                    cpu->registers[x] = cpu->registers[x] >> 1; //divide Vx by 2 
                    break;

                case 0x7:
                    cpu->registers[0xF] = cpu->registers[x] < cpu->registers[y]; //VF = not borrow
                    cpu->registers[y] -= cpu->registers[x];
                    break;

                case 0xE:
                    cpu->registers[0xF] = cpu->registers[x] >> 15; //set flag if most sig bit is 1
                    cpu->registers[x] = cpu->registers[x] << 1; //multply Vx by 2 
                    break;

            }

            case 0x9:
                cpu->PC = (cpu->registers[y] != cpu->registers[x] << 1);
                break;

            case 0xA:
                cpu->I = nnn;
                break;

            case 0xB:
                cpu->PC = nnn + cpu->registers[0];
                break;

            case 0xC:
                cpu->registers[x] = rand() & kk;
                break;

            case 0xD;
                cpu->registers[0xF] = 0;

                for(uint8_t y=0; y<n; y++){
                    uint8_t pixel_byte = read_RAM(cpu->bus, cpu->I+y);
                    
                    for(uint8_t x=0; x<8; x++){
                        uint8_t y_coord = cpu->register[y] + y;
                        uint8_t x_coord = cpu->register[x] + x;

                        uint64_t bit = (pixel_byte >> (7-x)) & 0x1;
                        cpu->display[y_coord] ^= 
                    }
                    
                }

            case 0xE:


    }   

}


