all: atanua

atanua-cpp-src = \
src/core/fileio.cpp \
src/core/BoxStitchingInformation.cpp \
src/chip/and8chip.cpp \
src/chip/or8chip.cpp \
src/chip/nand8chip.cpp \
src/chip/nor8chip.cpp \
src/core/slidingaverage.cpp \
src/chip/switchchip.cpp \
src/chip/16segchip.cpp \
src/chip/ledgrid.cpp \
src/chip/box.cpp \
src/chip/extpin.cpp \
src/chip/chip7489.cpp \
src/chip/audiochip.cpp \
src/chip/7segchip.cpp \
src/chip/and3chip.cpp \
src/chip/andchip.cpp \
src/basecode/angelcodefont.cpp \
src/core/basechipfactory.cpp \
src/chip/buttonchip.cpp \
src/chip/stepper.cpp \
src/chip/chip309.cpp \
src/chip/chip27xx.cpp \
src/chip/chip74193.cpp \
src/chip/chip74165.cpp \
src/chip/chip74192.cpp \
src/core/chip.cpp \
src/chip/chip2051.cpp \
src/chip/chip7400.cpp \
src/chip/chip7402.cpp \
src/chip/chip7404.cpp \
src/chip/chip7408.cpp \
src/chip/chip7410.cpp \
src/chip/chip74138.cpp \
src/chip/chip74139.cpp \
src/chip/chip74151.cpp \
src/chip/chip74154.cpp \
src/chip/chip74163.cpp \
src/chip/chip74164.cpp \
src/chip/chip74181.cpp \
src/chip/chip74191.cpp \
src/chip/chip74195.cpp \
src/chip/chip7420.cpp \
src/chip/chip74240.cpp \
src/chip/chip74241.cpp \
src/chip/chip74244.cpp \
src/chip/chip74245.cpp \
src/chip/chip74283.cpp \
src/chip/chip7432.cpp \
src/chip/chip7447.cpp \
src/chip/chip74574.cpp \
src/chip/chip7473.cpp \
src/chip/chip7474.cpp \
src/chip/chip7485.cpp \
src/chip/chip7486.cpp \
src/chip/chip7490.cpp \
src/chip/clockchip.cpp \
src/chip/dchip.cpp \
src/chip/dflipflop.cpp \
src/chip/dxchip.cpp \
src/chip/extrapin.cpp \
src/core/fileutils.cpp \
src/chip/jkchip.cpp \
src/chip/jkflipflop.cpp \
src/chip/label.cpp \
src/chip/ledchip.cpp \
src/chip/logicprobe.cpp \
src/core/main.cpp \
src/basecode/mersennetwister.cpp \
src/chip/muxchip.cpp \
src/chip/nand3chip.cpp \
src/chip/nandchip.cpp \
src/core/nativefunctions.cpp \
src/core/net.cpp \
src/chip/nor3chip.cpp \
src/chip/norchip.cpp \
src/chip/notchip.cpp \
src/chip/or3chip.cpp \
src/chip/orchip.cpp \
src/core/pin.cpp \
src/core/pluginchip.cpp \
src/core/pluginchipfactory.cpp \
src/chip/sedchip.cpp \
src/chip/serchip.cpp \
src/core/simutils.cpp \
src/chip/srchip.cpp \
src/chip/srflipflop.cpp \
src/chip/srnegchip.cpp \
src/chip/staticlevelchip.cpp \
src/chip/tchip.cpp \
src/chip/tflipflop.cpp \
src/basecode/toolkit.cpp \
src/core/wire.cpp \
src/chip/xorchip.cpp \
src/core/AtanuaConfig.cpp

atanua-c-src = \
src/8051/core.c \
src/8051/disasm.c \
src/8051/opcodes.c

atanua-obj = $(atanua-cpp-src:.cpp=.o) $(atanua-c-src:.c=.o)

tinyxml-cpp-src = \
src/tinyxml_2_5_3/tinyxml/tinystr.cpp \
src/tinyxml_2_5_3/tinyxml/tinyxml.cpp \
src/tinyxml_2_5_3/tinyxml/tinyxmlerror.cpp \
src/tinyxml_2_5_3/tinyxml/tinyxmlparser.cpp \

tinyxml-obj = $(tinyxml-cpp-src:.cpp=.o)

stb-c-src = \
src/stb/stb_image.c \
src/stb/stb_image_write.c

stb-obj = $(stb-c-src:.c=.o)

glee-c-src = \
src/glee/GLee.c

glee-obj = $(glee-c-src:.c=.o)

CXX = clang
CC = clang
CFLAGS = -O3

glib_CFLAGS = $(shell pkg-config --cflags glib-2.0)
glib_LDFLAGS = $(shell pkg-config --libs glib-2.0)

gtk_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
gtk_LDFLAGS = $(shell pkg-config --libs gtk+-3.0)

sdl_CFLAGS = $(shell pkg-config --cflags sdl)
sdl_LDFLAGS = $(shell pkg-config --libs sdl)

ifdef UNBUNDLE

# Link against the system libraries
tinyxml_LDFLAGS += -ltinyxml
stb_LDFLAGS += -lstbi
glee_LDFLAGS += -lGLee

else

# Build bundled copies
tinyxml_CFLAGS = -Isrc/tinyxml_2_5_3/tinyxml
stb_CFLAGS = -Isrc/stb
glee_CFLAGS = -Isrc/glee
atanua-obj += $(tinyxml-obj) $(stb-obj) $(glee-obj)

endif

override CFLAGS += $(glib_CFLAGS)
override CFLAGS += $(gtk_CFLAGS)
override CFLAGS += $(sdl_CFLAGS)
override CFLAGS += $(stb_CFLAGS)
override CFLAGS += $(tinyxml_CFLAGS)
override CFLAGS += $(glee_CFLAGS)
override CFLAGS += -Isrc -Isrc/include -I/usr/include/GL -I/usr/include/OpenGL
override CXXFLAGS += $(CFLAGS)

override LDFLAGS += $(glib_LDFLAGS)
override LDFLAGS += $(gtk_LDFLAGS)
override LDFLAGS += $(sdl_LDFLAGS)
override LDFLAGS += $(stb_LDFLAGS)
override LDFLAGS += $(tinyxml_LDFLAGS)
override LDFLAGS += $(glee_LDFLAGS)
override LDFLAGS += -lGL -lGLU -lm -ldl -lstdc++

atanua: $(atanua-obj)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm $(atanua-obj) atanua

