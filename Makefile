#	djnn cpp
#
#	The copyright holders for the contents of this file are:
#		Ecole Nationale de l'Aviation Civile, France (2017-2018)
#	See file "license.terms" for the rights and conditions
#	defined by copyright holders.
#
#
#	Contributors:
#		Mathieu Magnaudet <mathieu.magnaudet@enac.fr>
#		Stéphane Conversy <stephane.conversy@enac.fr>
#		Mathieu Poirier	  <mathieu.poirier@enac.fr>


default: all
.PHONY: default

all: config.mk djnn
.PHONY: all

help:
	@echo "default: djnn ; all: djnn"
	@echo "experiment make -j !!"



config.mk:
	cp config.default.mk config.mk
include config.default.mk
-include config.mk

# remove builtin rules: speed up build process and help debug
MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

ifndef os
os := $(shell uname -s)
endif
ifndef arch
arch := $(shell uname -m)
endif

# cross-platform support
crossp ?=
#crossp := g
#crossp := arm-none-eabi-
#cross_prefix := em

osmingw := MINGW64_NT-10.0
#osmingw := MINGW64_NT-6.1
#osmingw := MINGW32_NT-6.1


src_dir := src

ifndef graphics
graphics := QT
endif

CC := $(crossp)cc
CXX := $(crossp)c++
GPERF ?= gperf
CXXFLAGS ?= $(CFLAGS) -std=c++14

ifeq ($(os),Linux)
lib_suffix=.so
boost_libs = -lboost_thread -lboost_chrono -lboost_system
dynlib=-shared
CFLAGS ?= -fpic -g -MMD -Wall
endif

ifeq ($(os),Darwin)
lib_suffix=.dylib
boost_libs = -lboost_thread-mt -lboost_chrono-mt -lboost_system-mt
dynlib=-dynamiclib
CFLAGS ?= -g -MMD -Wall
endif

# for windows with mingw64 
ifeq ($(os),$(osmingw))
lib_suffix=.dll
boost_libs = -lboost_thread-mt -lboost_chrono-mt -lboost_system-mt
dynlib =-shared
CFLAGS ?= -fpic -g -MMD -Wall
endif

LDFLAGS ?= $(boost_libs) -L$(build_dir)
tidy := /usr/local/Cellar/llvm/5.0.1/bin/clang-tidy
tidy_opts := -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk 

lcov_file ?= $(build_dir)/djnn_cov.info
lcov_output_dir ?= $(build_dir)/coverage_html


ifeq ($(os),$(osmingw))
djnn_libs := core base display gui animation # comms input
else
djnn_libs := core base comms display gui input animation
endif

srcs :=
objs :=
libs :=
deps :=
cov :=

define lib_makerule

lib_srcs :=
lib_srcgens :=
lib_objs :=
lib_cppflags :=
lib_cflags :=
lib_ldflags :=
lib_djnn_deps := 

# possibly override default
-include $$(src_dir)/$1/djnn-lib.mk

# default
$1_gperf_srcs :=
#$$(shell find src/$1 -name "*.gperf")
$1_cpp_srcs := $$(filter %.cpp,$$(lib_srcs))
$1_c_srcs := $$(filter %.c,$$(lib_srcs))
$1_objs := $$($1_cpp_srcs:.cpp=.o) $$($1_c_srcs:.c=.o)
$1_objs := $$(addprefix $(build_dir)/, $$($1_objs))
$1_cov_gcno  := $$($1_objs:.o=.gcno)
$1_cov_gcda  := $$($1_objs:.o=.gcda)

$1_srcgens := $$(lib_srcgens)
$1_objs += $$(lib_objs)

$1_deps := $$($1_objs:.o=.d)
$1_lib := $$(build_dir)/libdjnn-$1$$(lib_suffix)
$1_lib_cppflags := $$(lib_cppflags)
$1_lib_cflags := $$(lib_cflags)
$1_lib_ldflags := $$(lib_ldflags)
$1_lib_all_ldflags := $$($1_lib_ldflags)

ifeq ($(os),$(filter $(os),Darwin MINGW64_NT-10.0))
ifdef lib_djnn_deps
$1_djnn_deps := $$(addsuffix $$(lib_suffix),$$(addprefix $$(build_dir)/libdjnn-,$$(lib_djnn_deps)))
$1_lib_all_ldflags += $$(foreach lib,$$(lib_djnn_deps), $$(value $$(lib)_lib_ldflags))
endif
endif

$$($1_objs): CXXFLAGS+=$$($1_lib_cppflags)
$$($1_objs): CFLAGS+=$$($1_lib_cflags)
$$($1_lib): LDFLAGS+=$$($1_lib_all_ldflags)
$$($1_lib): $$($1_djnn_deps)

$$($1_lib): $$($1_objs)
	@mkdir -p $$(dir $$@)
	$$(CXX) $(dynlib) -o $$@ $$^ $$(LDFLAGS)


$1_tidy_srcs := $$(addsuffix _tidy,$$($1_srcs))
$$($1_tidy_srcs): tidy_opts+=$$($1_lib_cppflags)

$1_tidy: $$($1_tidy_srcs)
.PHONY: $1_tidy

$1_dbg:
	@echo $1_dbg
	@echo $$($1_cpp_srcs)
	@echo $$($1_c_srcs)
	@echo $$($1_objs)
	@echo $$($1_djnn_deps)
	@echo $$($1_lib_ldflags)
	@echo $$($1_lib_all_ldflags)
	@echo $$($1_cov_gcno)
	@echo $$($1_cov_gcda)

srcs += $$($1_srcs)
srcgens += $$($1_srcgens)
objs += $$($1_objs)
deps += $$($1_deps)
libs += $$($1_lib)
cov  += $$($1_cov_gcno) $$($1_cov_gcda) $(lcov_file)

endef




$(foreach a,$(djnn_libs),$(eval $(call lib_makerule,$a)))

#headers := $(foreach a,$(djnn_libs),$a/$a)
headers := $(djnn_libs)
headers := $(addsuffix .h,$(headers)) $(addsuffix -dev.h,$(headers))
headers := $(addprefix $(build_dir)/include/djnn/,$(headers))

headers: $(headers)
.PHONY: headers

djnn: $(libs) $(headers)
.PHONY: djnn

$(build_dir)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(build_dir)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# for generated .cpp
$(build_dir)/%.o: $(build_dir)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(build_dir)/include/djnn/%.h: src/*/%.h
	@mkdir -p $(dir $@)
	cp $< $@

%_tidy: %
	$(tidy) -header-filter="djnn" -checks="*" -extra-arg=-std=c++14 $^ -- $(tidy_opts)
.PHONY: %_tidy

all_tidy := $(addsuffix _tidy,$(srcs))
tidy: $(all_tidy)
.PHONY: tidy

-include $(deps)

pre_cov: CXXFLAGS += --coverage
pre_cov: LDFLAGS += --coverage
pre_cov : djnn
.PHONY: pre_cov

cov:
	lcov -o $(lcov_file) -c -d . -b . --no-external > /dev/null 2>&1
	lcov --remove $(lcov_file) '*/ext/*' -o $(lcov_file)
	genhtml -o $(lcov_output_dir) $(lcov_file)
	cd $(lcov_output_dir) ; open index.html
.PHONY: cov

clean:
	rm -f $(deps) $(objs) $(libs) $(srcgens) $(cov)
	rm -rf $(lcov_output_dir) > /dev/null 2>&1 || true
	rmdir $(build_dir) > /dev/null 2>&1 || true
.PHONY: clean

distclean clear:
	rm -rf $(build_dir)
.PHONY: distclean clear

dbg:
	@echo $(os)
.PHONY: dbg

ifeq ($(os),Linux)
#https://brew.sh/
pkgdeps := libexpat1-dev libcurl4-openssl-dev libudev-dev gperf libboost-thread-dev
pkgdeps += qt5-default
#pkgdeps += freetype sdl2
pkgcmd := apt install -y
endif

ifeq ($(os),Darwin)
#https://brew.sh/
pkgdeps := expat curl boost
pkgdeps += expat qt5
#pkgdeps += freetype sdl2
pkgcmd := brew install
endif

ifeq ($(os),MINGW64_NT-10.0)
#https://www.msys2.org/
#pkgdeps := git make
pkgdeps := pkg-config gcc boost expat curl qt5
#pkgdeps += freetype SDL2
pkgdeps := $(addprefix mingw-w64-x86_64-, $(pkgdeps))
pkgcmd := pacman -S
endif

install-pkgdeps:
	$(pkgcmd) $(pkgdeps)
.PHONY: install-pkgdeps


