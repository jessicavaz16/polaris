#include<stdint.h>
#include"memory.h"
#include"defs.h"

struct instr_t {
    uint32_t value;     // 32 bits   // undecoded instruction
    uint8_t  opcode;    // 7 bits
    uint8_t  funct3;    // 3 bits
    uint8_t  funct7;    // 7 bits
    uint8_t  rs1_s;     // 5 bits
    uint8_t  rs2_s;     // 5 bits
    uint8_t  rd_s;      // 5 bits
    int32_t  imm_i;     // 32 bits
    int32_t  imm_s;     // 32 bits
    int32_t  imm_u;     // 32 bits
    int32_t  imm_j;     // 32 bits
    int32_t  imm_b;     // 32 bits
};

class Core {
    private:
        Memory *mem;    // Pointer to the memory object
        xlen_t pc;      // Program counter
        xlen_t rf[32];  // Register file (32 registers)
        instr_t instr;  // Instruction structure

        // Read data from the specified address
        uint32_t mem_read (uint32_t address);

        // Write data to the specified address
        void mem_write (uint32_t address, uint32_t value, uint8_t mask = 0b1111);
 

    public:
        // Constructor
        Core(Memory *mem);
        
        // Destructor
        ~Core();

        // Reset the core with a new program counter
        void reset(xlen_t pc = 0);

        // Simulate a clock tick
        int tick();

        // Dump the register file
        void dumpRF(bool miniview = false);

        xlen_t getPC() const { return pc; } // Get the current program counter
        uint32_t getIR() const { return instr.value; } // Get the current instruction
};
