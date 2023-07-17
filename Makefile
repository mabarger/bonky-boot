BASE    = riscv64-unknown-elf
ARCH	= rv32imac
ABI		= ilp32
CC      = $(BASE)-gcc
FLAGS   = -mabi=$(ABI) -march=$(ARCH) -nostartfiles -Os -ffreestanding -g
LDFLAGS = -m elf32lriscv -N
LD      = $(BASE)-ld
STRIP   = ${BASE}-strip
OBJCOPY = $(BASE)-objcopy

GDB          = gdb-multiarch
QEMU	     = qemu-system-riscv32
QEMUOPTS     = -M virt -bios none -serial stdio -display none -kernel bonky-boot.img
QEMUOPTS_GDB = -gdb tcp::27000 -S ${QEMUOPTS}

all: clean bonky-boot.img

bonky-boot.img: bonky-boot.elf
	$(OBJCOPY) bonky-boot.elf -I binary bonky-boot.img
	${STRIP} bonky-boot.img

bonky-boot.elf: bonky-boot.o
	$(LD) $(LDFLAGS) -T linker.ld -o bonky-boot.elf *.o

bonky-boot.o:
	$(CC) $(FLAGS) -c src/* -I inc

clean:
	rm -f *.o bonky-boot.elf bonky-boot.img

run: bonky-boot.img
	$(QEMU) $(QEMUOPTS) &

gdb: bonky-boot.img
	$(QEMU) $(QEMUOPTS_GDB) &
	${GDB} -x .gdbrc ./bonky-boot.elf
