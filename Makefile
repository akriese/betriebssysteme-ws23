#
# Kurzanleitung
# =============
#
# make		-- Baut den Kernel.
# make all
#
# make install	-- Baut den Kernel und transferiert ihn auf den Server.
# 		   Das Board holt sich diesen Kernel beim nächsten Reset.
#
# make clean	-- Löscht alle erzeugten Dateien.
#


#
# Quellen
#
LSCRIPT = kernel.lds

OBJ = driver/dbgu.o
OBJ += lib/print.o lib/util.o lib/mem.o
OBJ += system/main.o system/start.o system/memory_controller.o system/exceptions.o
OBJ += system/start_asm.o system/exceptions_asm.o system/cpu_asm.o

#
# Konfiguration
#
IMP_LIB = LD_LIBRARY_PATH=/home/mi/linnert/arm/lib
CC = $(IMP_LIB) arm-none-eabi-gcc
LD = $(IMP_LIB) arm-none-eabi-ld
OBJCOPY = $(IMP_LIB) arm-none-eabi-objcopy
OBJDUMP = $(IMP_LIB) arm-none-eabi-objdump

CFLAGS = -Wall -Wextra -ffreestanding -mcpu=arm920t -O2 -g
# --unresolved-symbols=ignore-in-object-files
CFLAGS += -Iinclude
#LIBGCC := $(shell $(CC) -print-libgcc-file-name)

DEP = $(OBJ:.o=.d)

# if the qemu installation is done via https://git.imp.fu-berlin.de/koenigl/qemu-portux
SERIAL_SOCKET = unix:/tmp/serial.socket
SERIAL_SOCKET_MINICOM = unix\#/tmp/serial.socket

QEMU_NAME = qemu-system-arm
QEMU_LIB = LD_LIBRARY_PATH=/usr/local/lib:/import/sage-7.4/local/lib
QEMU = $(QEMU_LIB) $(QEMU_NAME)

QEMU_FLAGS = -nographic -M portux920t -m 64M

# uncomment for serial exposure from qemu; accessible via e.g. `minicom`
# QEMU_FLAGS += -serial $(SERIAL_SOCKET),server

# uncomment for telnet connection
# QEMU_FLAGS += -piotelnet

#
# Regeln
#
.PHONY: all
all: kernel

-include $(DEP)

%.o: %.S
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

kernel: $(LSCRIPT) $(OBJ)
	$(LD) -T$(LSCRIPT) --unresolved-symbols=ignore-in-object-files -o $@ $(OBJ) $(LIBGCC)

kernel.bin: kernel
	$(OBJCOPY) -Obinary --set-section-flags .bss=contents,alloc,load,data $< $@

kernel.img: kernel.bin
	mkimage -A arm -T standalone -C none -a 0x20000000 -d $< $@

.PHONY: install
install: kernel.img
	arm-install-image $<

.PHONY: clean
clean:
	rm -f kernel kernel.bin kernel.img
	rm -f $(OBJ)
	rm -f $(DEP)

.PHONY: run
run:
	$(QEMU) $(QEMU_FLAGS) -kernel kernel

.PHONY: debug
debug:
	$(QEMU) $(QEMU_FLAGS) -s -S -kernel kernel

.PHONY: gdb
gdb:
	gdb-multiarch -x .gdbinit

# If there is a problem connecting with minicom, try replacing the : with \#
.PHONY: minicom
minicom:
	minicom -D $(SERIAL_SOCKET_MINICOM)

.PHONY: dump
dump:
	$(OBJDUMP) -d kernel
