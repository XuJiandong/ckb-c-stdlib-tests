TARGET := riscv64-unknown-linux-gnu
CC := $(TARGET)-gcc
LD := $(TARGET)-gcc
OBJCOPY := $(TARGET)-objcopy
CFLAGS := -fPIC -O3 -fno-builtin-printf -fno-builtin-memcmp -nostdinc -nostdlib -nostartfiles -fvisibility=hidden -fdata-sections -ffunction-sections -I deps/ckb-c-stdlib-dev -I deps/ckb-c-stdlib-dev/libc -I deps/ckb-c-stdlib-dev/molecule -Wall -Werror -Wno-nonnull -Wno-nonnull-compare -Wno-unused-function -g
LDFLAGS := -Wl,-static -fdata-sections -ffunction-sections -Wl,--gc-sections

# docker pull nervos/ckb-riscv-gnu-toolchain:gnu-bionic-20191012
BUILDER_DOCKER := nervos/ckb-riscv-gnu-toolchain@sha256:aae8a3f79705f67d505d1f1d5ddc694a4fd537ed1c7e9622420a470d59ba2ec3
PORT := 9099


all: build/test_malloc

all-via-docker: ${PROTOCOL_HEADER}
	docker run --rm -v `pwd`:/code ${BUILDER_DOCKER} bash -c "cd /code && make"

build/test_malloc: c/test_malloc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

fmt:
	clang-format -style="{BasedOnStyle: google, IndentWidth: 4}" -i c/*.c

run:
	RUST_LOG=debug ckb-debugger --bin build/test_malloc

run-debugger:
	ckb-debugger --mode gdb --gdb-listen 127.0.0.1:${PORT} --bin build/test_malloc

run-gdb:
	riscv64-unknown-linux-gnu-gdb -ex "target remote 127.0.0.1:${PORT}" build/test_malloc

run-docker:
	docker run --net=host --rm -it -v `pwd`:/code ${BUILDER_DOCKER} bash

clean:
	rm -f build/*
	
install:
	cargo install --git https://github.com/nervosnetwork/ckb-standalone-debugger
