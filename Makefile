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
OBJ = start.o serial.o printf.o

#
# Konfiguration
#
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -Wall -Wextra -ffreestanding -mcpu=arm920t -O2
#LIBGCC := $(shell $(CC) -print-libgcc-file-name)

DEP = $(OBJ:.o=.d)

# if the qemu installation is done via https://git.imp.fu-berlin.de/koenigl/qemu-portux
SERIAL_SOCKET = unix:/tmp/serial.socket
SERIAL_SOCKET_MINICOM = unix\#/tmp/serial.socket
QEMU = qemu-system-arm
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
	$(LD) -T$(LSCRIPT) -o $@ $(OBJ) $(LIBGCC)

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

# If there is a problem connecting with minicom, try replacing the : with \#
.PHONY: minicom
minicom:
	minicom -D $(SERIAL_SOCKET_MINICOM)
