#define MAIN_STACK 0x80004000

.global _start

_start:
    /* Only run on one core if multiple are available */
    csrr    t0, mhartid
    bnez    t0, safety_net

    li sp, MAIN_STACK
    call __main

safety_net:
    wfi
    j       safety_net
