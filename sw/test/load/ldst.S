.text
.globl _start

_start:
    # li x2, 0xdeadbeef 
    # sw x2, 16(x0)
    # lw x3, 16(x0)

    la  x2, _data
    li  x3, 0xdeadbeef  # Load immediate value 0xdeadbeef into x3

    sw  x3, 4(x2)
    lw  x4, 4(x2)      # Load word from address in x2 into x4

    ebreak

.data
.align 4

_data:
.word 0xdeadbeef
.word 0xbabecafe

