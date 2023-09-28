# Copyright Cartesi and individual authors (see AUTHORS)
# SPDX-License-Identifier: LGPL-3.0-or-later
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with this program (see COPYING). If not, see <https://www.gnu.org/licenses/>.
#

TARGET_OS?=$(shell uname)

# Install settings
DEB_ARCH:= $(shell dpkg --print-architecture 2>/dev/null || echo amd64)
PREFIX= /usr
MACHINE_EMULATOR_VERSION:= $(shell $(MAKE) -sC src version)
MACHINE_EMULATOR_SO_VERSION:= $(shell $(MAKE) -sC src so-version)
DEB_FILENAME= cartesi-machine-v$(MACHINE_EMULATOR_VERSION)_$(DEB_ARCH).deb
WASM_DIRNAME= cartesi-machine-v$(MACHINE_EMULATOR_VERSION)_wasm
WASM_TARGZ_FILENAME= $(WASM_DIRNAME).tar.gz
BIN_RUNTIME_PATH= $(PREFIX)/bin
LIB_RUNTIME_PATH= $(PREFIX)/lib
DOC_RUNTIME_PATH= $(PREFIX)/doc/cartesi-machine
SHARE_RUNTIME_PATH= $(PREFIX)/share/cartesi-machine
IMAGES_RUNTIME_PATH= $(SHARE_RUNTIME_PATH)/images
LUA_RUNTIME_CPATH= $(PREFIX)/lib/lua/5.4
LUA_RUNTIME_PATH= $(PREFIX)/share/lua/5.4
INSTALL_PLAT = install-$(TARGET_OS)

LIBCARTESI_Darwin=libcartesi.dylib
LIBCARTESI_Linux=libcartesi.so
LIBCARTESI_GRPC_Darwin=libcartesi_grpc.dylib
LIBCARTESI_GRPC_Linux=libcartesi_grpc.so

LIBCARTESI_SO_Darwin:=libcartesi-$(MACHINE_EMULATOR_SO_VERSION).dylib
LIBCARTESI_SO_Linux:=libcartesi-$(MACHINE_EMULATOR_SO_VERSION).so
LIBCARTESI_SO_Emscripten:=libcartesi-$(MACHINE_EMULATOR_SO_VERSION).so
LIBCARTESI_SO_GRPC_Darwin:=libcartesi_grpc-$(MACHINE_EMULATOR_SO_VERSION).dylib
LIBCARTESI_SO_GRPC_Linux:=libcartesi_grpc-$(MACHINE_EMULATOR_SO_VERSION).so
LIBCARTESI_WASM:=libcartesi-$(MACHINE_EMULATOR_SO_VERSION).wasm
LIBCARTESI_SO=$(LIBCARTESI_SO_$(TARGET_OS))

BIN_INSTALL_PATH:=    $(DESTDIR)$(BIN_RUNTIME_PATH)
LIB_INSTALL_PATH:=    $(DESTDIR)$(LIB_RUNTIME_PATH)
DOC_INSTALL_PATH:=    $(DESTDIR)$(DOC_RUNTIME_PATH)
SHARE_INSTALL_PATH:=  $(DESTDIR)$(SHARE_RUNTIME_PATH)
IMAGES_INSTALL_PATH:= $(DESTDIR)$(IMAGES_RUNTIME_PATH)
UARCH_INSTALL_PATH:=  $(SHARE_INSTALL_PATH)/uarch
LUA_INSTALL_CPATH:=   $(DESTDIR)$(LUA_RUNTIME_CPATH)
LUA_INSTALL_PATH:=    $(DESTDIR)$(LUA_RUNTIME_PATH)
INC_INSTALL_PATH:=    $(DESTDIR)$(PREFIX)/include/cartesi-machine

INSTALL= cp -RP
CHMOD_EXEC= chmod 0755
CHMOD_DATA= chmod 0644
STRIP=strip
STRIP_EXEC= $(STRIP) -x

EMU_TO_BIN= jsonrpc-remote-cartesi-machine remote-cartesi-machine merkle-tree-hash
EMU_TO_LIB= $(LIBCARTESI_SO_$(TARGET_OS)) $(LIBCARTESI_SO_GRPC_$(TARGET_OS))
EMU_LUA_TO_BIN= cartesi-machine.lua cartesi-machine-stored-hash.lua rollup-memory-range.lua
EMU_LUA_TEST_TO_BIN= cartesi-machine-tests.lua uarch-riscv-tests.lua
EMU_TO_LUA_PATH= cartesi/util.lua cartesi/proof.lua cartesi/gdbstub.lua
EMU_TO_LUA_CPATH= cartesi.so
EMU_TO_LUA_CARTESI_CPATH= cartesi/grpc.so cartesi/jsonrpc.so
EMU_TO_INC= $(addprefix lib/machine-emulator-defines/,pma-defines.h rtc-defines.h) \
            $(addprefix src/,jsonrpc-machine-c-api.h grpc-machine-c-api.h machine-c-api.h machine-c-defines.h machine-c-version.h)
UARCH_TO_SHARE= uarch-ram.bin

# Build settings
DEPDIR := third-party
SRCDIR := $(abspath src)
DOWNLOADDIR := $(DEPDIR)/downloads
SUBCLEAN := $(addsuffix .clean,$(SRCDIR) uarch third-party/riscv-arch-tests)
COREPROTO := lib/grpc-interfaces/core.proto
XKCP_VERSION=f7fe32a80f0c6600d1c5db50392a43265d3bba9a
MONGOOSE_VERSION=7.11
BOOST_VERSION=1_83_0

# Docker image tag
TAG ?= devel
DEBIAN_IMG ?= juztamau5/machine-emulator:$(TAG).deb

# Docker image platform
BUILD_PLATFORM ?=

ifneq ($(BUILD_PLATFORM),)
DOCKER_PLATFORM=--platform $(BUILD_PLATFORM)
endif

# Code instrumentation
release?=no
sanitize?=no
coverage?=no
export sanitize
export release
export coverage

# Mac OS X specific settings
ifeq ($(TARGET_OS),Darwin)
export CC=clang
export CXX=clang++
export TARGET_ARCH=$(shell uname -m)

# Linux specific settings
else ifeq ($(TARGET_OS),Linux)
export CC=gcc
export CXX=g++
export TARGET_ARCH=$(shell uname -m)

# WebAssembly specific settings
else ifeq ($(TARGET_OS),Emscripten)
export CC=emcc
export CXX=em++
export AR=emar
export TARGET_ARCH=wasm

endif

export TARGET_OS

all: source-default

clean: $(SUBCLEAN)

depclean: $(DEPCLEAN) clean
	$(MAKE) -C third-party/riscv-arch-tests depclean
	$(MAKE) -C third-party/xkcp depclean

distclean:
	rm -rf $(DOWNLOADDIR)
	rm -rf third-party/xkcp/XKCP-$(XKCP_VERSION)
	rm -rf third-party/mongoose-$(MONGOOSE_VERSION)
	rm -rf third-party/boost_$(BOOST_VERSION)
	$(MAKE) -C third-party/riscv-arch-tests depclean
	$(MAKE) -C third-party/xkcp depclean
	$(MAKE) clean

pkgclean:
	rm -f $(DEB_FILENAME) $(WASM_TARGZ_FILENAME)
	rm -rf $(WASM_DIRNAME)

$(BIN_INSTALL_PATH) $(LIB_INSTALL_PATH) $(LUA_INSTALL_PATH) $(LUA_INSTALL_CPATH) $(LUA_INSTALL_CPATH)/cartesi $(LUA_INSTALL_PATH)/cartesi $(INC_INSTALL_PATH) $(IMAGES_INSTALL_PATH) $(UARCH_INSTALL_PATH):
	mkdir -m 0755 -p $@

env:
	@echo "export PATH='$(SRCDIR):${PATH}'"
	@echo "export LUA_PATH_5_4='$(SRCDIR)/?.lua;$${LUA_PATH_5_4:-;}'"
	@echo "export LUA_CPATH_5_4='$(SRCDIR)/?.so;$${LUA_CPATH_5_4:-;}'"

doc:
	cd doc && doxygen Doxyfile

help:
	@echo 'Cleaning targets:'
	@echo '  clean                      - clean the src/ artifacts'
	@echo '  depclean                   - clean + dependencies'
	@echo '  distclean                  - depclean + profile information and downloads'
	@echo 'Docker targets:'
	@echo '  build-debian-image         - Build the machine-emulator debian based docker image'
	@echo '  build-debian-package       - BUild the cartesi-machine.deb package from image'
	@echo 'Generic targets:'
	@echo '* all                        - build the src/ code. To build from a clean clone, run: make submodules downloads dep all'
	@echo '  doc                        - build the doxygen documentation (requires doxygen to be installed)'
	@echo '  copy                       - copy generated artifacts out of a docker image'
	@echo '  uarch                      - build microarchitecture'
	@echo '  uarch-with-linux-env       - build microarchitecture using the linux-env docker image'
	@echo '  uarch-tests                - build and run microarchitecture rv64i instruction tests'
	@echo '  uarch-tests-with-linux-env - build and run microarchitecture rv64i instruction tests using the linux-env docker image'

checksum:
	@cd $(DEPDIR) && sha1sum -c shasumfile

$(DOWNLOADDIR):
	@mkdir -p $(DOWNLOADDIR)
	@wget -nc -i $(DEPDIR)/dependencies -P $(DOWNLOADDIR)
	$(MAKE) checksum
	tar -C third-party/xkcp -xzf third-party/downloads/$(XKCP_VERSION).tar.gz
	tar -C third-party -xzf third-party/downloads/$(MONGOOSE_VERSION).tar.gz mongoose-$(MONGOOSE_VERSION)/mongoose.c mongoose-$(MONGOOSE_VERSION)/mongoose.h
	tar -C third-party -xzf third-party/downloads/boost_$(BOOST_VERSION).tar.gz boost_$(BOOST_VERSION)/boost

downloads: $(DOWNLOADDIR)

dep: $(DOWNLOADDIR)
	$(MAKE) -C third-party/xkcp

submodules:
	git submodule update --init --recursive

$(COREPROTO):
	$(info grpc-interfaces submodule not initialized!)
	@exit 1

grpc: | $(COREPROTO)

hash luacartesi libcartesi grpc test lint coverage-report check-format format check-format-lua check-lua format-lua:
	@eval $$($(MAKE) -s -j1 --no-print-directory env); $(MAKE) -C $(SRCDIR) $@

version:
	@eval $$($(MAKE) -s -j1 --no-print-directory env); $(MAKE) -sC $(SRCDIR) $@

test-%:
	@eval $$($(MAKE) -s -j1 --no-print-directory env); $(MAKE) -C $(SRCDIR) $@

uarch-tests:
	@eval $$($(MAKE) -s -j1 --no-print-directory env); $(MAKE) -C third-party/riscv-arch-tests

run-uarch-tests:
	@eval $$($(MAKE) -s -j1 --no-print-directory env); $(MAKE) -C third-party/riscv-arch-tests run

source-default:
	@eval $$($(MAKE) -s -j1 --no-print-directory env); $(MAKE) -C $(SRCDIR)

uarch: $(SRCDIR)/machine-c-version.h
	@eval $$($(MAKE) -s -j1 --no-print-directory env); $(MAKE) -C uarch

$(SRCDIR)/machine-c-version.h:
	@eval $$($(MAKE) -s -j1 --no-print-directory env); $(MAKE) -C $(SRCDIR) machine-c-version.h

$(SUBCLEAN) $(DEPCLEAN): %.clean:
	$(MAKE) -C $* clean

build-linux-env:
	docker build $(DOCKER_PLATFORM) --target linux-env -t juztamau5/linux-env:$(TAG) -f Dockerfile .

build-debian-image:
	docker build $(DOCKER_PLATFORM) --build-arg RELEASE=$(release) --build-arg COVERAGE=$(coverage) --build-arg SANITIZE=$(sanitize) --build-arg MACHINE_EMULATOR_VERSION=$(MACHINE_EMULATOR_VERSION) -t juztamau5/machine-emulator:$(TAG) -f Dockerfile .

build-debian-package:
	docker build $(DOCKER_PLATFORM) --target debian-packager --build-arg RELEASE=$(release) --build-arg COVERAGE=$(coverage) --build-arg SANITIZE=$(sanitize) --build-arg MACHINE_EMULATOR_VERSION=$(MACHINE_EMULATOR_VERSION=) -t $(DEBIAN_IMG) -f Dockerfile .

copy:
	ID=`docker create $(DOCKER_PLATFORM) $(DEBIAN_IMG)` && \
	   docker cp $$ID:/usr/src/emulator/$(DEB_FILENAME) . && \
	   docker cp $$ID:/usr/src/emulator/uarch/uarch-ram.bin . && \
	   docker rm $$ID

check-linux-env:
	@if docker images $(DOCKER_PLATFORM) -q juztamau5/linux-env:$(TAG)$(image_name) 2>/dev/null | grep -q .; then \
		echo "Docker image juztamau5/linux-env:$(TAG) exists"; \
	else \
		echo "Docker image juztamau5/linux-env:$(TAG) does not exist. Creating:"; \
		$(MAKE) build-linux-env; \
	fi

linux-env: check-linux-env
	@docker run $(DOCKER_PLATFORM) --hostname linux-env -it --rm \
		-e USER=$$(id -u -n) \
		-e GROUP=$$(id -g -n) \
		-e UID=$$(id -u) \
		-e GID=$$(id -g) \
		-v `pwd`:/opt/cartesi/machine-emulator \
		-w /opt/cartesi/machine-emulator \
		juztamau5/linux-env:$(TAG) /bin/bash

linux-env-exec: check-linux-env
	@docker run --hostname linux-env --rm \
		-e USER=$$(id -u -n) \
		-e GROUP=$$(id -g -n) \
		-e UID=$$(id -u) \
		-e GID=$$(id -g) \
		-v `pwd`:/opt/cartesi/machine-emulator \
		-w /opt/cartesi/machine-emulator \
		juztamau5/linux-env:$(TAG) /bin/bash -c "$(CONTAINER_COMMAND)"

uarch-with-linux-env:
	@$(MAKE) linux-env-exec CONTAINER_COMMAND="make uarch"

uarch-tests-with-linux-env:
	@$(MAKE) linux-env-exec CONTAINER_COMMAND="make uarch-tests"

install-Darwin:
	install_name_tool -delete_rpath $(SRCDIR) -add_rpath $(LIB_RUNTIME_PATH) $(LUA_INSTALL_CPATH)/cartesi.so
	install_name_tool -delete_rpath $(SRCDIR) -add_rpath $(LIB_RUNTIME_PATH) $(LUA_INSTALL_CPATH)/cartesi/grpc.so
	install_name_tool -delete_rpath $(SRCDIR) -add_rpath $(LIB_RUNTIME_PATH) $(LUA_INSTALL_CPATH)/cartesi/jsonrpc.so
	cd $(BIN_INSTALL_PATH) && \
		for x in $(EMU_TO_BIN); do \
			install_name_tool -delete_rpath $(SRCDIR) -add_rpath $(LIB_RUNTIME_PATH) $$x ;\
		done

install-Linux:
	cd $(BIN_INSTALL_PATH) && for x in $(EMU_TO_BIN); do patchelf --set-rpath $(LIB_RUNTIME_PATH) $$x ; done
	cd $(LIB_INSTALL_PATH) && for x in $(EMU_TO_LIB); do patchelf --set-rpath $(LIB_RUNTIME_PATH) $$x; done
	cd $(LUA_INSTALL_CPATH) && for x in $(EMU_TO_LUA_CPATH) $(EMU_TO_LUA_CARTESI_CPATH); do patchelf --set-rpath $(LIB_RUNTIME_PATH) $$x ; done

install-tests: install
	cd src && $(INSTALL) $(EMU_LUA_TEST_TO_BIN) $(LUA_INSTALL_PATH)
	cd src && $(INSTALL) tests/test-merkle-tree-hash tests/test-machine-c-api $(BIN_INSTALL_PATH)
	cat tools/template/cartesi-machine-tests.template | sed 's|ARG_LUA_PATH|$(LUA_RUNTIME_PATH)/?.lua|g;s|ARG_LUA_CPATH|$(LUA_RUNTIME_CPATH)/?.so|g;s|ARG_LUA_RUNTIME_PATH|$(LUA_RUNTIME_PATH)|g' > $(BIN_INSTALL_PATH)/cartesi-machine-tests
	cat tools/template/uarch-riscv-tests.template | sed 's|ARG_LUA_PATH|$(LUA_RUNTIME_PATH)/?.lua|g;s|ARG_LUA_CPATH|$(LUA_RUNTIME_CPATH)/?.so|g;s|ARG_LUA_RUNTIME_PATH|$(LUA_RUNTIME_PATH)|g' > $(BIN_INSTALL_PATH)/uarch-riscv-tests
	cd $(LUA_INSTALL_PATH) && $(CHMOD_DATA) $(EMU_LUA_TEST_TO_BIN)
	cd $(BIN_INSTALL_PATH) && $(CHMOD_EXEC) cartesi-machine-tests uarch-riscv-tests
	patchelf --set-rpath $(LIB_RUNTIME_PATH) src/tests/test-merkle-tree-hash
	patchelf --set-rpath $(LIB_RUNTIME_PATH) src/tests/test-machine-c-api

install-emulator: $(BIN_INSTALL_PATH) $(LIB_INSTALL_PATH) $(LUA_INSTALL_CPATH)/cartesi $(LUA_INSTALL_PATH)/cartesi $(INC_INSTALL_PATH) $(IMAGES_INSTALL_PATH)
	cd src && $(INSTALL) $(EMU_TO_BIN) $(BIN_INSTALL_PATH)
	cd src && $(INSTALL) $(EMU_TO_LIB) $(LIB_INSTALL_PATH)
	cd src && $(INSTALL) $(EMU_LUA_TO_BIN) $(LUA_INSTALL_PATH)
	cd src && $(INSTALL) $(EMU_TO_LUA_CPATH) $(LUA_INSTALL_CPATH)
	cd src && $(INSTALL) $(EMU_TO_LUA_CARTESI_CPATH) $(LUA_INSTALL_CPATH)/cartesi
	cd src && $(INSTALL) $(EMU_TO_LUA_PATH) $(LUA_INSTALL_PATH)/cartesi
	cat tools/template/cartesi-machine.template | sed 's|ARG_LUA_PATH|$(LUA_RUNTIME_PATH)/?.lua|g;s|ARG_LUA_CPATH|$(LUA_RUNTIME_CPATH)/?.so|g;s|ARG_INSTALL_PATH|$(IMAGES_RUNTIME_PATH)|g;s|ARG_LUA_RUNTIME_PATH|$(LUA_RUNTIME_PATH)|g' > $(BIN_INSTALL_PATH)/cartesi-machine
	cat tools/template/cartesi-machine-stored-hash.template | sed 's|ARG_LUA_PATH|$(LUA_RUNTIME_PATH)/?.lua|g;s|ARG_LUA_CPATH|$(LUA_RUNTIME_CPATH)/?.so|g;s|ARG_LUA_RUNTIME_PATH|$(LUA_RUNTIME_PATH)|g' > $(BIN_INSTALL_PATH)/cartesi-machine-stored-hash
	cat tools/template/rollup-memory-range.template | sed 's|ARG_LUA_PATH|$(LUA_RUNTIME_PATH)/?.lua|g;s|ARG_LUA_CPATH|$(LUA_RUNTIME_CPATH)/?.so|g;s|ARG_LUA_RUNTIME_PATH|$(LUA_RUNTIME_PATH)|g' > $(BIN_INSTALL_PATH)/rollup-memory-range
	cd $(BIN_INSTALL_PATH) && $(CHMOD_EXEC) $(EMU_TO_BIN) cartesi-machine cartesi-machine-stored-hash rollup-memory-range
	cd $(LIB_INSTALL_PATH) && ln -sf $(LIBCARTESI_SO_$(TARGET_OS)) $(LIBCARTESI_$(TARGET_OS))
	cd $(LIB_INSTALL_PATH) && ln -sf $(LIBCARTESI_SO_GRPC_$(TARGET_OS)) $(LIBCARTESI_GRPC_$(TARGET_OS))
	cd $(LUA_INSTALL_PATH) && $(CHMOD_DATA) $(EMU_LUA_TO_BIN)
	$(INSTALL) $(EMU_TO_INC) $(INC_INSTALL_PATH)
	$(INSTALL) tools/gdb $(SHARE_INSTALL_PATH)/gdb
	cd $(LUA_INSTALL_CPATH) && $(CHMOD_EXEC) $(EMU_TO_LUA_CPATH)

install-wasm: $(INC_INSTALL_PATH) $(LIB_INSTALL_PATH)
	$(INSTALL) $(EMU_TO_INC) $(INC_INSTALL_PATH)
	$(INSTALL) src/$(LIBCARTESI_SO) $(LIB_INSTALL_PATH)
	ln -sf $(LIBCARTESI_SO) $(LIB_INSTALL_PATH)/libcartesi.so

install-uarch: install $(UARCH_INSTALL_PATH)
	$(INSTALL) uarch/$(UARCH_TO_SHARE) $(UARCH_INSTALL_PATH)

install-strip: install-emulator
	cd $(BIN_INSTALL_PATH) && $(STRIP_EXEC) $(EMU_TO_BIN)
	cd $(LIB_INSTALL_PATH) && $(STRIP_EXEC) $(EMU_TO_LIB)
	cd $(LUA_INSTALL_CPATH) && $(STRIP_EXEC) $(EMU_TO_LUA_CPATH)

install: install-emulator install-strip $(INSTALL_PLAT)
debian-package: install
	mkdir -p $(DESTDIR)/DEBIAN $(DOC_INSTALL_PATH)
	$(INSTALL) COPYING $(DOC_INSTALL_PATH)/copyright
	cat tools/template/control.template | sed 's|ARG_VERSION|$(MACHINE_EMULATOR_VERSION)|g;s|ARG_ARCH|$(DEB_ARCH)|g' > $(DESTDIR)/DEBIAN/control
	dpkg-deb -Zxz --root-owner-group --build $(DESTDIR) $(DEB_FILENAME)

.SECONDARY: $(DOWNLOADDIR) $(COREPROTO)

.PHONY: help all submodules doc clean distclean downloads checksum src test luacartesi grpc hash uarch \
	$(SUBDIRS) $(SUBCLEAN) $(DEPCLEAN)

wasm-package:
	$(MAKE) install-wasm TARGET_OS=Emscripten TARGET_ARCH=wasm DESTDIR=$(WASM_DIRNAME) PREFIX=/
	tar -cf $(WASM_TARGZ_FILENAME) $(WASM_DIRNAME)

build-wasm-package:
	$(MAKE) dep TARGET_OS=Emscripten TARGET_ARCH=wasm
	$(MAKE) libcartesi TARGET_OS=Emscripten TARGET_ARCH=wasm
	rm -rf $(WASM_DIRNAME) $(WASM_TARGZ_FILENAME)
	$(MAKE) wasm-package
