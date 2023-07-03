BASE    = riscv64-unknown-elf
ARCH	= rv32imac
ABI	= ilp32
CC      = $(BASE)-gcc
FLAGS   = -mabi=$(ABI) -march=$(ARCH) -nostartfiles -g
LDFLAGS = -m elf32lriscv
LD      = $(BASE)-ld
OBJCOPY = $(BASE)-objcopy

GDB          = gdb-multiarch
QEMU	     = qemu-system-riscv32
QEMUOPTS     = -M virt -bios none -serial stdio -display none -kernel main.img
QEMUOPTS_GDB = -gdb tcp::27000 -S ${QEMUOPTS}

all: clean main.img

main.img: main.elf
	$(OBJCOPY) main.elf -I binary main.img

main.elf: main.o
	$(LD) $(LDFLAGS) -T linker.ld -o main.elf *.o libtommath.a

main.o:
	$(CC) $(FLAGS) -c src/* -I inc

clean:
	rm -f *.o main.elf main.img

run: main.img
	$(QEMU) $(QEMUOPTS) &

gdb: main.img
	$(QEMU) $(QEMUOPTS_GDB) &
	${GDB} -x .gdbrc ./main.elf
