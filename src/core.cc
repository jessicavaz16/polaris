#include "core.h"

enum instruction_type {
    RV_LD = 0x03,
    RV_ST = 0x23,
    RV_LUI = 0x37,
    RV_AUIPC = 0x17,
    RV_JAL = 0x6F,
    RV_JALR = 0x67,
    RV_BR = 0x63,
    RV_REG = 0x33,
    RV_IMM = 0x13,
    RV_SYS = 0x73
};

Core::Core(Memory *mem) {
    this->mem = mem; // Initialize the memory pointer
    reset();
}
    
Core::~Core() {
}

void Core::reset(xlen_t pc) { 
    this->pc = pc;
    for (int i = 0; i < 32; ++i) {
        rf[i] = 0; 
    }
}

void Core::dumpRF(bool miniview) {
    if (miniview) {
        printf("PC: 0x%08x    IR: 0x%08x\n", pc, instr.value);    
        return;
    }

    printf("------------------------------------------------\n");
    printf("PC: 0x%08x    IR: 0x%08x\n", pc, instr.value);
    printf("------------------------------------------------\n");
    printf("x0  (zero) : 0x%08x    x16 (a6)   : %08x\n", rf[0], rf[16]);
    printf("x1  (ra)   : 0x%08x    x17 (a7)   : %08x\n", rf[1], rf[17]);
    printf("x2  (sp)   : 0x%08x    x18 (s2)   : %08x\n", rf[2], rf[18]);
    printf("x3  (gp)   : 0x%08x    x19 (s3)   : %08x\n", rf[3], rf[19]);
    printf("x4  (tp)   : 0x%08x    x20 (s4)   : %08x\n", rf[4], rf[20]);
    printf("x5  (t0)   : 0x%08x    x21 (s5)   : %08x\n", rf[5], rf[21]);
    printf("x6  (t1)   : 0x%08x    x22 (s6)   : %08x\n", rf[6], rf[22]);
    printf("x7  (t2)   : 0x%08x    x23 (s7)   : %08x\n", rf[7], rf[23]);
    printf("x8  (s0/fp): 0x%08x    x24 (s8)   : %08x\n", rf[8], rf[24]);
    printf("x9  (s1)   : 0x%08x    x25 (s9)   : %08x\n", rf[9], rf[25]);
    printf("x10 (a0)   : 0x%08x    x26 (s10)  : %08x\n", rf[10], rf[26]);
    printf("x11 (a1)   : 0x%08x    x27 (s11)  : %08x\n", rf[11], rf[27]);
    printf("x12 (a2)   : 0x%08x    x28 (t3)   : %08x\n", rf[12], rf[28]);
    printf("x13 (a3)   : 0x%08x    x29 (t4)   : %08x\n", rf[13], rf[29]);
    printf("x14 (a4)   : 0x%08x    x30 (t5)   : %08x\n", rf[14], rf[30]);
    printf("x15 (a5)   : 0x%08x    x31 (t6)   : %08x\n", rf[15], rf[31]);
}

int Core::tick() { 
    // Implement the core's behavior during a clock tick

    // Fetch the next instruction from memory
    instr.value = mem->read(pc); 
    
    // Increment the program counter by 4 bytes (assuming 32-bit instructions)
    xlen_t pc_next = pc + 4; 

    // Decode the fetched instruction
    instr.opcode = BIT_FIELD(instr.value, 6, 0);               
    instr.funct3 = BIT_FIELD(instr.value, 14, 12); 
    instr.funct7 = BIT_FIELD(instr.value, 31, 25); 
    instr.rs1_s  = BIT_FIELD(instr.value, 19, 15); 
    instr.rs2_s  = BIT_FIELD(instr.value, 24, 20); 
    instr.rd_s   = BIT_FIELD(instr.value, 11, 7);  
    instr.imm_i  = BIT_FIELD_SIGNED(instr.value, 31, 20); 
    instr.imm_s  = BIT_FIELD_SIGNED(instr.value, 31, 25) << 5 | BIT_FIELD(instr.value, 11, 7); 
    instr.imm_u  = BIT_FIELD(instr.value, 31, 12) << 12; 
    instr.imm_j  = BIT_FIELD_SIGNED(instr.value, 31, 31) << 20 | BIT_FIELD(instr.value, 19, 12) << 12 | BIT_FIELD(instr.value, 20, 20) << 11 | BIT_FIELD(instr.value, 30, 21) << 1; 
    instr.imm_b  = BIT_FIELD_SIGNED(instr.value, 31, 31) << 12 | BIT_FIELD(instr.value, 7, 7) << 11 | BIT_FIELD(instr.value, 30, 25) << 5 | BIT_FIELD(instr.value, 11, 8) << 1; 

    // Execute the decoded instruction
    switch (instr.opcode) {
        case RV_SYS: // System instructions
            if (instr.funct3 == 0x0 && instr.funct7 == 0x1 && instr.rs1_s == 0x0 && instr.rd_s == 0x0) { // EBREAK
                return -1; // Return -1 to indicate EBREAK
            }
            break;
        
        case RV_LD: // Load instructions
            {
                uint32_t mem_addr = rf[instr.rs1_s] + instr.imm_i;  
                uint32_t mem_rdata = mem->read(mem_addr & ~0b11);   
                
                switch (instr.funct3) {
                    case 0x0: // LB (Load Byte)
                        rf[instr.rd_s] = BIT_FIELD_SIGNED(mem_rdata, 8*(mem_addr & 0b11)+7, 8*(mem_addr & 0b11));
                        break;
                    case 0x1: // LH (Load Halfword)
                        rf[instr.rd_s] = BIT_FIELD_SIGNED(mem_rdata, 8*(mem_addr & 0b10)+15, 8*(mem_addr & 0b10));
                        break;
                    case 0x2: // LW (Load Word)
                        rf[instr.rd_s] = mem_rdata;
                        break;
                    case 0x4: // LBU (Load Byte Unsigned)
                        rf[instr.rd_s] = BIT_FIELD(mem_rdata, 8*(mem_addr & 0b11)+7, 8*(mem_addr & 0b11));
                        break;
                    case 0x5: // LHU (Load Halfword Unsigned)
                        rf[instr.rd_s] = BIT_FIELD(mem_rdata, 8*(mem_addr & 0b10)+15, 8*(mem_addr & 0b10));
                        break;
                    default:
                        break;
                }
            }
            break;
            
        case RV_ST: // Store instructions
            {
                uint32_t store_addr = rf[instr.rs1_s] + instr.imm_s;     
                uint8_t mem_mask = 0;
                uint32_t mem_wdata = 0;
                
                switch (instr.funct3) {
                    case 0x0: // SB (Store Byte)
                        mem_mask = 0b0001 << BIT_FIELD(store_addr, 1, 0);
                        mem_wdata = BIT_FIELD(rf[instr.rs2_s], 7, 0) << (8 * (store_addr & 0b11));
                        break;
                    case 0x1: // SH (Store Halfword)
                        mem_mask = 0b0011 << BIT_FIELD(store_addr, 1, 0);
                        mem_wdata = BIT_FIELD(rf[instr.rs2_s], 15, 0) << (8 * (store_addr & 0b10));
                        break;
                    case 0x2: // SW (Store Word)
                        mem_mask = 0b1111; 
                        mem_wdata = rf[instr.rs2_s];
                        break;
                    default:
                        break;
                }
                mem->write(store_addr & ~0b11, mem_wdata, mem_mask); // Write the data to memory
            }
            break;

        case RV_LUI: // Load Upper Immediate
            rf[instr.rd_s] = instr.imm_u; 
            break;
        
        case RV_AUIPC: // AUIPC (Add Upper Immediate to PC)
            rf[instr.rd_s] = pc + instr.imm_u; 
            break;
        
        case RV_JAL: // JAL (Jump and Link)
            rf[instr.rd_s] = pc_next; // Store the return address in the link register
            pc_next = (pc + instr.imm_j) & ~0b1;; // Jump to the target address
            break;
            
        case RV_JALR: // JALR (Jump and Link Register)
            rf[instr.rd_s] = pc_next; // Store the return address in the link register
            pc_next = (rf[instr.rs1_s] + instr.imm_i) & ~0b1; // Jump to the target address
            break;

        case RV_BR: // Branch instructions
            switch (instr.funct3) {
                case 0x0: // BEQ (Branch if Equal)
                    if (rf[instr.rs1_s] == rf[instr.rs2_s]) {
                        pc_next = pc + instr.imm_b; 
                    }
                    break;
                case 0x1: // BNE (Branch if Not Equal)
                    if (rf[instr.rs1_s] != rf[instr.rs2_s]) {
                        pc_next = pc + instr.imm_b;
                    }
                    break;
                case 0x4: // BLT (Branch if Less Than)
                    if ((int32_t)rf[instr.rs1_s] < (int32_t)rf[instr.rs2_s]) {
                        pc_next = pc + instr.imm_b; 
                    }
                    break;
                case 0x5: // BGE (Branch if Greater Than or Equal)
                    if ((int32_t)rf[instr.rs1_s] >= (int32_t)rf[instr.rs2_s]) {
                        pc_next = pc + instr.imm_b;
                    }
                    break;
                case 0x6: // BLTU (Branch if Less Than Unsigned)
                    if (rf[instr.rs1_s] < rf[instr.rs2_s]) {
                        pc_next = pc + instr.imm_b;
                    }
                    break;
                case 0x7: // BGEU (Branch if Greater Than or Equal Unsigned)
                    if (rf[instr.rs1_s] >= rf[instr.rs2_s]) {
                        pc_next = pc + instr.imm_b;
                    }
                    break;
                default:
                    break;
            }
            break;
        
        case RV_REG: // Register arithmetic instructions
            switch (instr.funct3) {
                case 0x0: // ADD (Add) or SUB (Subtract)
                    if (instr.funct7 == 0x00) {
                        rf[instr.rd_s] = rf[instr.rs1_s] + rf[instr.rs2_s]; 
                    } else if (instr.funct7 == 0x20) {
                        rf[instr.rd_s] = rf[instr.rs1_s] - rf[instr.rs2_s]; 
                    }
                    break;
                case 0x1: // SLL (Shift Left Logical)
                    rf[instr.rd_s] = rf[instr.rs1_s] << (rf[instr.rs2_s] & 0x1F); 
                    break;
                case 0x2: // SLT (Set Less Than)
                    rf[instr.rd_s] = ((int32_t)rf[instr.rs1_s] < (int32_t)rf[instr.rs2_s]); 
                    break;
                case 0x3: // SLTU (Set Less Than Unsigned)
                    rf[instr.rd_s] = (rf[instr.rs1_s] < rf[instr.rs2_s]); 
                    break;
                case 0x4: // XOR (XOR)
                    rf[instr.rd_s] = rf[instr.rs1_s] ^ rf[instr.rs2_s]; 
                    break;
                case 0x5: // SRL/SRA (Shift Right Logical/Arithmetic)
                    if (instr.funct7 == 0x00) {
                        rf[instr.rd_s] = rf[instr.rs1_s] >> (rf[instr.rs2_s] & 0x1F);
                    } else if (instr.funct7 == 0x20) {
                        rf[instr.rd_s] = ((int32_t)rf[instr.rs1_s]) >> (rf[instr.rs2_s] & 0x1F); 
                    }
                    break;
                case 0x6: // OR (OR)
                    rf[instr.rd_s] = rf[instr.rs1_s] | rf[instr.rs2_s]; 
                    break;
                case 0x7: // AND (AND)
                    rf[instr.rd_s] = rf[instr.rs1_s] & rf[instr.rs2_s]; 
                    break;
                default:
                    break;
            }
            break;

        case RV_IMM: // Immediate arithmetic instructions
            switch (instr.funct3) {
                case 0x0: // ADDI (Add Immediate)  
                    rf[instr.rd_s] = rf[instr.rs1_s] + instr.imm_i;
                    break;
                case 0x1: // SLLI (Shift Left Logical Immediate)
                    rf[instr.rd_s] = rf[instr.rs1_s] << (instr.imm_i & 0x1F); 
                    break;
                case 0x2: // SLTI (Set Less Than Immediate)
                    rf[instr.rd_s] = ((int32_t)rf[instr.rs1_s] < instr.imm_i);
                    break;
                case 0x3: // SLTIU (Set Less Than Immediate Unsigned)
                    rf[instr.rd_s] = (rf[instr.rs1_s] < (uint32_t)instr.imm_i);
                    break;
                case 0x4: // XORI (XOR Immediate)
                    rf[instr.rd_s] = rf[instr.rs1_s] ^ instr.imm_i; 
                    break;
                case 0x5: // SRLI/SRAI (Shift Right Logical/Arithmetic Immediate)
                    if (instr.funct7 == 0x00) {
                        rf[instr.rd_s] = rf[instr.rs1_s] >> (instr.imm_i & 0x1F);
                    } else if (instr.funct7 == 0x20) {
                        rf[instr.rd_s] = ((int32_t)rf[instr.rs1_s]) >> (instr.imm_i & 0x1F);
                    }
                    break;
                case 0x6: // ORI (OR Immediate)
                    rf[instr.rd_s] = rf[instr.rs1_s] | instr.imm_i;
                    break;
                case 0x7: // ANDI (AND Immediate)
                    rf[instr.rd_s] = rf[instr.rs1_s] & instr.imm_i;
                    break;
                default:
                    break;
            }
            break;
        default:
            throw std::runtime_error("Unknown opcode: " + std::to_string(instr.opcode) + "at pc: " + std::to_string(pc)); // Handle unknown opcodes
            break;
    }

    // Update the program counter to the next instruction
    pc = pc_next;

    // Return 0 to indicate successful execution of 1 cycle
    return 0;
}
