all: atanua

atanua-cpp-src = \
fileio.cpp \
BoxStitchingInformation.cpp \
and8chip.cpp \
or8chip.cpp \
nand8chip.cpp \
nor8chip.cpp \
slidingaverage.cpp \
switchchip.cpp \
16segchip.cpp \
ledgrid.cpp \
box.cpp \
extpin.cpp \
chip7489.cpp \
audiochip.cpp \
7segchip.cpp \
and3chip.cpp \
andchip.cpp \
angelcodefont.cpp \
atanuaconfig.cpp \
basechipfactory.cpp \
buttonchip.cpp \
stepper.cpp \
chip309.cpp \
chip27xx.cpp \
chip74193.cpp \
chip74165.cpp \
chip74192.cpp \
chip.cpp \
chip2051.cpp \
chip7400.cpp \
chip7402.cpp \
chip7404.cpp \
chip7408.cpp \
chip7410.cpp \
chip74138.cpp \
chip74139.cpp \
chip74151.cpp \
chip74154.cpp \
chip74163.cpp \
chip74164.cpp \
chip74181.cpp \
chip74191.cpp \
chip74195.cpp \
chip7420.cpp \
chip74240.cpp \
chip74241.cpp \
chip74244.cpp \
chip74245.cpp \
chip74283.cpp \
chip7432.cpp \
chip7447.cpp \
chip74574.cpp \
chip7473.cpp \
chip7474.cpp \
chip7485.cpp \
chip7486.cpp \
chip7490.cpp \
clockchip.cpp \
dchip.cpp \
dflipflop.cpp \
dxchip.cpp \
extrapin.cpp \
fileutils.cpp \
jkchip.cpp \
jkflipflop.cpp \
label.cpp \
ledchip.cpp \
logicprobe.cpp \
main.cpp \
mersennetwister.cpp \
muxchip.cpp \
nand3chip.cpp \
nandchip.cpp \
nativefunctions.cpp \
net.cpp \
nor3chip.cpp \
norchip.cpp \
notchip.cpp \
or3chip.cpp \
orchip.cpp \
pin.cpp \
pluginchip.cpp \
pluginchipfactory.cpp \
sedchip.cpp \
serchip.cpp \
simutils.cpp \
srchip.cpp \
srflipflop.cpp \
srnegchip.cpp \
staticlevelchip.cpp \
tchip.cpp \
tflipflop.cpp \
toolkit.cpp \
wire.cpp \
xorchip.cpp \
tinyxml/tinystr.cpp \
tinyxml/tinyxml.cpp \
tinyxml/tinyxmlerror.cpp \
tinyxml/tinyxmlparser.cpp \


atanua-c-src = \
8051/core.c \
8051/disasm.c \
8051/opcodes.c \
stb/stb_image.c \
stb/stb_image_write.c \
glee/GLee.c

atanua-obj = $(atanua-cpp-src:.cpp=.o) $(atanua-c-src:.c=.o)

CXX = clang
CC = clang

CXXFLAGS = \
-O3 \
-I/usr/include/c++/4.5 \
-I/usr/include/c++/4.5/i686-linux-gnu \
-I/usr/X11R6/include \
-Itinyxml \
-I/usr/include/gtk-3.0 \
-I/usr/include/cairo \
-I/usr/include/glib-2.0 \
-I/usr/lib/glib-2.0/include \
-I/usr/include/pango-1.0 \
-I/usr/lib/gtk-3.0/include \
-I/usr/include/atk-1.0 \
-I/usr/lib/i386-linux-gnu/glib-2.0/include \
-I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
-I/usr/include/gdk-pixbuf-2.0 



atanua: $(atanua-obj)
	$(CXX) `pkg-config --cflags gtk+-3.0` `pkg-config --cflags glib-2.0` -o $@ $(atanua-obj) -L. -lSDLmain -lSDL -lGL -lGLU `pkg-config --libs gtk+-3.0` `pkg-config --libs glib-2.0` -O3 $(CXXFLAGS)

clean:
	rm $(atanua-obj) atanua

