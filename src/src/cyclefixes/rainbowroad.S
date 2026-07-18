.include "asm_setup.S"

.global AuroraExtension, AuroraExtension2

.set Divisor, 0x41855555 #50 rounds / 3 rounds

AuroraExtension:
#original instruction
fdivs f0, f0, f26

#divide by divisor
lis r12, Divisor@ha
ori r12, r12, Divisor@l
stw r12, -4 (r1)
lfs f4, -4 (r1)
fdivs f0, f0, f4
blr

AuroraExtension2:
#original instruction
fdivs f2, f2, f3

#divide by divisor
lis r12, Divisor@ha
ori r12, r12, Divisor@l
stw r12, -4 (r1)
lfs f6, -4 (r1)
fdivs f2, f2, f6
blr