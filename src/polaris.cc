#include "memory.h"
#include "core.h"
#include <stdexcept>
#include <iostream>
#include "argparse.h"

#define DEFAULT_MEM_SIZE 1024

std::string header = 
" _____      _            _\n"
"|  __ \\    | |          (_)\n"
"| |__) |__ | | __ _ _ __ _ ___\n"
"|  ___/ _ \\| |/ _` | '__| / __|\n"
"| |  | (_) | | (_| | |  | \\__ \\\n"
"|_|   \\___/|_|\\__,_|_|  |_|___/\n"
"==================================\n"
" RISC-V ISA Simulator  (v1.0)\n"
"==================================\n";

int interactive(Core &core, Memory &mem, bool verbose=false) {
    std::string help =
        "Commands:\n"
        " h, help: Show this help message\n";

    while(true) {
        std::cout << "\033[34m" << "polaris> " << "\033[0m";
        
        // get a command from the user
        std::string cmd;
        std::getline(std::cin, cmd);

        // strip leading and trailing whitespace
        cmd.erase(0, cmd.find_first_not_of(" \t\n\r"));
        cmd.erase(cmd.find_last_not_of(" \t\n\r") + 1);


        if (cmd == "h" || cmd == "help") {
            std::cout << help;
        } else if (cmd == "q" || cmd == "quit") {
            return 0;
        } else if (cmd == "s" || cmd == "step") {
            int rc = core.tick();
            core.dumpRF(!verbose);
            if (rc != 0) {
                return rc;
            }
        } else if (cmd == "ir" || cmd == "reg") {
            core.dumpRF();
        } else if (cmd == "m" || cmd == "mem") {
            mem.dump(0x00000000, 8);
        }
        else {
            std::cout << "Unknown command: " << cmd << "\n";
        }    
    }
    return 0;
}

    
int main(int argc, char** argv) {
    std::cout << "\033[32m" << header << "\033[0m";

    // Parse Arguments
    ArgParse::ArgumentParser parser("polaris", "RISC-V simulator");
    parser.add_argument({"-d", "--debug"}, "Enable debug mode", ArgParse::ArgType_t::BOOL, "false");
    parser.add_argument({"-v", "--verbose"}, "Enable verbose output", ArgParse::ArgType_t::BOOL, "false");

    if(parser.parse_args(argc, argv) != 0) {
        return 1;
    }
    auto opt_args = parser.get_opt_args();
    auto pos_args = parser.get_pos_args();

    bool verbose = opt_args["verbose"].value.as_bool;

    int rc = 0;
    try {
        // Construct memory object
        Memory mem(DEFAULT_MEM_SIZE); 

        // Create a core object
        Core core(&mem);

        // Load the program file into memory
        if(pos_args.size() > 0) {
            std::string hex_file = pos_args[0];
            mem.load_hex(hex_file);
        } else {
            fprintf(stderr, "Error: No program file specified\n");
            return 1;
        }

        // Run the simulator
        if(opt_args["debug"].value.as_bool) {
            std::cout << "Debug mode enabled\n";
            rc = interactive(core, mem, verbose);
        }
        else {
            std::cout << "Running in normal mode\n";
            while(rc == 0) {
                rc = core.tick(); // Simulate a clock tick
            }
        }

        // Check the return code
        switch(rc) {
            case 0:
                break;
            case -1:
                printf("EBREAK encountered at PC: 0x%08x\n", core.getPC()); // Print the program counter
                break;
            default:
                printf("Program terminated with unknown error\n");
                break;
        }
        printf("Exiting...\n");

    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}   
