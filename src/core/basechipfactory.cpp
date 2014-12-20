/*
Atanua Real-Time Logic Simulator
Copyright (c) 2008-2014 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#include "atanua.h"
#include "atanua_internal.h"

#include "label.h"
#include "basechipfactory.h"
#include "extrapin.h"
#include "staticlevelchip.h"
#include "buttonchip.h"
#include "switchchip.h"
#include "ledchip.h"
#include "sedchip.h"
#include "ledgrid.h"
#include "7segchip.h"
#include "16segchip.h"
#include "chip309.h"
#include "clockchip.h"
#include "and3chip.h"
#include "or3chip.h"
#include "nand3chip.h"
#include "nor3chip.h"
#include "and8chip.h"
#include "or8chip.h"
#include "nand8chip.h"
#include "nor8chip.h"
#include "andchip.h"
#include "orchip.h"
#include "nandchip.h"
#include "norchip.h"
#include "xorchip.h"
#include "notchip.h"
#include "srchip.h"
#include "srnegchip.h"
#include "serchip.h"
#include "dchip.h"
#include "tchip.h"
#include "jkchip.h"
#include "muxchip.h"
#include "dxchip.h"
#include "logicprobe.h"
#include "stepper.h"

#include "jkflipflop.h"
#include "srflipflop.h"
#include "dflipflop.h"
#include "tflipflop.h"

#include "chip7400.h"
#include "chip7402.h"
#include "chip7404.h"
#include "chip7408.h"
#include "chip7410.h"
#include "chip7420.h"
#include "chip7432.h"
#include "chip7447.h"
#include "chip7473.h"
#include "chip7474.h"
#include "chip7485.h"
#include "chip7486.h"
#include "chip7490.h"
#include "chip74138.h"
#include "chip74139.h"
#include "chip74151.h"
#include "chip74154.h"
#include "chip74163.h"
#include "chip74164.h"
#include "chip74165.h"
#include "chip74181.h"
#include "chip74191.h"
#include "chip74192.h"
#include "chip74193.h"
#include "chip74195.h"
#include "chip74240.h"
#include "chip74241.h"
#include "chip74244.h"
#include "chip74245.h"
#include "chip74283.h"
#include "chip74574.h"

#include "chip2051.h"
#include "audiochip.h"
#include "chip27xx.h"
#include "chip7489.h"

#include "box.h"
#include "extpin.h"

static const char *led_red = "LED (red)";
static const char *led_green = "LED (green)";
static const char *led_blue = "LED (blue)";
static const char *led_brblue = "LED (br.blue)";
static const char *led_cyan = "LED (cyan)";
static const char *led_magenta = "LED (magenta)";
static const char *led_yellow = "LED (yellow)";
static const char *led_white = "LED (white)";
static const char *sed_white = "SED";

static const char *iled_red = "LED (red) inverted";
static const char *iled_green = "LED (green) inverted";
static const char *iled_blue = "LED (blue) inverted";
static const char *iled_brblue = "LED (br.blue) inverted";
static const char *iled_cyan = "LED (cyan) inverted";
static const char *iled_magenta = "LED (magenta) inverted";
static const char *iled_yellow = "LED (yellow) inverted";
static const char *iled_white = "LED (white) inverted";

static const char *ssg_red = "7-seg (red)";
static const char *ssg_green = "7-seg (green)";
static const char *ssg_blue = "7-seg (blue)";
static const char *ssg_brblue = "7-seg (br.blue)";
static const char *ssg_cyan = "7-seg (cyan)";
static const char *ssg_magenta = "7-seg (magenta)";
static const char *ssg_yellow = "7-seg (yellow)";
static const char *ssg_white = "7-seg (white)";

static const char *issg_red = "7-seg (red) inverted";
static const char *issg_green = "7-seg (green) inverted";
static const char *issg_blue = "7-seg (blue) inverted";
static const char *issg_brblue = "7-seg (br.blue) inverted";
static const char *issg_cyan = "7-seg (cyan) inverted";
static const char *issg_magenta = "7-seg (magenta) inverted";
static const char *issg_yellow = "7-seg (yellow) inverted";
static const char *issg_white = "7-seg (white) inverted";

static const char *stsg_red = "16-seg (red)";
static const char *stsg_green = "16-seg (green)";
static const char *stsg_blue = "16-seg (blue)";
static const char *stsg_brblue = "16-seg (br.blue)";
static const char *stsg_cyan = "16-seg (cyan)";
static const char *stsg_magenta = "16-seg (magenta)";
static const char *stsg_yellow = "16-seg (yellow)";
static const char *stsg_white = "16-seg (white)";

static const char *istsg_red = "16-seg (red) inverted";
static const char *istsg_green = "16-seg (green) inverted";
static const char *istsg_blue = "16-seg (blue) inverted";
static const char *istsg_brblue = "16-seg (br.blue) inverted";
static const char *istsg_cyan = "16-seg (cyan) inverted";
static const char *istsg_magenta = "16-seg (magenta) inverted";
static const char *istsg_yellow = "16-seg (yellow) inverted";
static const char *istsg_white = "16-seg (white) inverted";

static const char *chip309 = "309";

static const char *ledgrid4x4_red = "4x4 led grid (red)";
static const char *ledgrid4x4_green = "4x4 led grid (green)";
static const char *ledgrid4x4_blue = "4x4 led grid (blue)";
static const char *ledgrid4x4_brblue = "4x4 led grid (br.blue)";
static const char *ledgrid4x4_cyan = "4x4 led grid (cyan)";
static const char *ledgrid4x4_magenta = "4x4 led grid (magenta)";
static const char *ledgrid4x4_yellow = "4x4 led grid (yellow)";
static const char *ledgrid4x4_white = "4x4 led grid (white)";

static const char *iledgrid4x4_red = "4x4 led grid (red) inverted";
static const char *iledgrid4x4_green = "4x4 led grid (green) inverted";
static const char *iledgrid4x4_blue = "4x4 led grid (blue) inverted";
static const char *iledgrid4x4_brblue = "4x4 led grid (br.blue) inverted";
static const char *iledgrid4x4_cyan = "4x4 led grid (cyan) inverted";
static const char *iledgrid4x4_magenta = "4x4 led grid (magenta) inverted";
static const char *iledgrid4x4_yellow = "4x4 led grid (yellow) inverted";
static const char *iledgrid4x4_white = "4x4 led grid (white) inverted";

static const char *ledgrid8x8_red = "8x8 led grid (red)";
static const char *ledgrid8x8_green = "8x8 led grid (green)";
static const char *ledgrid8x8_blue = "8x8 led grid (blue)";
static const char *ledgrid8x8_brblue = "8x8 led grid (br.blue)";
static const char *ledgrid8x8_cyan = "8x8 led grid (cyan)";
static const char *ledgrid8x8_magenta = "8x8 led grid (magenta)";
static const char *ledgrid8x8_yellow = "8x8 led grid (yellow)";
static const char *ledgrid8x8_white = "8x8 led grid (white)";

static const char *iledgrid8x8_red = "8x8 led grid (red) inverted";
static const char *iledgrid8x8_green = "8x8 led grid (green) inverted";
static const char *iledgrid8x8_blue = "8x8 led grid (blue) inverted";
static const char *iledgrid8x8_brblue = "8x8 led grid (br.blue) inverted";
static const char *iledgrid8x8_cyan = "8x8 led grid (cyan) inverted";
static const char *iledgrid8x8_magenta = "8x8 led grid (magenta) inverted";
static const char *iledgrid8x8_yellow = "8x8 led grid (yellow) inverted";
static const char *iledgrid8x8_white = "8x8 led grid (white) inverted";

static const char *clock_01hz = "Clock 0.1Hz";
static const char *clock_02hz = "Clock 0.2Hz";
static const char *clock_05hz = "Clock 0.5Hz";
static const char *clock_1hz = "Clock 1Hz";
static const char *clock_2hz = "Clock 2Hz";
static const char *clock_5hz = "Clock 5Hz";
static const char *clock_10hz = "Clock 10Hz";
static const char *clock_20hz = "Clock 20Hz";
static const char *clock_50hz = "Clock 50Hz";
static const char *clock_100hz = "Clock 100Hz";
static const char *clock_200hz = "Clock 200Hz";
static const char *clock_500hz = "Clock 500Hz";

static const char *clock_1khz = "Clock 1kHz";
static const char *clock_2khz = "Clock 2kHz";
static const char *clock_5khz = "Clock 5kHz";

static const char *clock_10khz = "Clock 10kHz";
static const char *clock_20khz = "Clock 20kHz";
static const char *clock_50khz = "Clock 50kHz";

static const char *button_basestring = "button ('%c')";
static const char *switch_basestring = "switch ('%c')";

static const char *button_space = "button ('space')";
static const char *button_shift = "button ('shift')";
static const char *button_return = "button ('return')";

static const char *switch_space = "switch ('space')";
static const char *switch_shift = "switch ('shift')";
static const char *switch_return = "switch ('return')";

static const char *and_chip  = "logic AND";
static const char *or_chip   = "logic OR";
static const char *nand_chip = "logic NAND";
static const char *nor_chip  = "logic NOR";
static const char *xor_chip  = "logic XOR";
static const char *not_chip  = "logic NOT";

static const char *and_chip_fi  = "logic AND (fi)";
static const char *or_chip_fi   = "logic OR (fi)";
static const char *nand_chip_fi = "logic NAND (fi)";
static const char *nor_chip_fi  = "logic NOR (fi)";
static const char *xor_chip_fi  = "logic XOR (fi)";
static const char *not_chip_fi  = "logic NOT (fi)";

static const char *and3_chip     = "3-in logic AND";
static const char *and3_chip_fi  = "3-in logic AND (fi)";
static const char *nand3_chip    = "3-in logic NAND";
static const char *nand3_chip_fi = "3-in logic NAND (fi)";
static const char *nor3_chip     = "3-in logic NOR";
static const char *nor3_chip_fi  = "3-in logic NOR (fi)";
static const char *or3_chip      = "3-in logic OR";
static const char *or3_chip_fi   = "3-in logic OR (fi)";

static const char *and8_chip     = "8-in logic AND";
static const char *and8_chip_fi  = "8-in logic AND (fi)";
static const char *nand8_chip    = "8-in logic NAND";
static const char *nand8_chip_fi = "8-in logic NAND (fi)";
static const char *nor8_chip     = "8-in logic NOR";
static const char *nor8_chip_fi  = "8-in logic NOR (fi)";
static const char *or8_chip      = "8-in logic OR";
static const char *or8_chip_fi   = "8-in logic OR (fi)";

static const char *logic_low_chip   = "logic '0'";
static const char *logic_high_chip  = "logic '1'";

static const char *sr_latch      = "SR-latch (nor)";
static const char *sr_neg_latch  = "SR-latch (nand)";
static const char *ser_latch     = "Synch. SR-latch";
static const char *d_latch       = "D-latch";
static const char *t_latch       = "T-latch";
static const char *jk_latch      = "JK-latch";

static const char *mux           = "2-bit MUX";
static const char *dx            = "3-bit DX";

static const char *sr_flipflop   = "SR-flipflop";
static const char *d_flipflop    = "D-flipflop";
static const char *t_flipflop    = "T-flipflop";
static const char *jk_flipflop   = "JK-flipflop";

static const char *chip7400      = "7400";
static const char *chip7402      = "7402";
static const char *chip7404      = "7404";
static const char *chip7408      = "7408";
static const char *chip7410      = "7410";
static const char *chip7420      = "7420";
static const char *chip7432      = "7432";
static const char *chip7447      = "7447";
static const char *chip7473      = "7473";
static const char *chip7474      = "7474";
static const char *chip7485      = "7485";
static const char *chip7486      = "7486";
static const char *chip7490      = "7490";
static const char *chip74138     = "74138";
static const char *chip74139     = "74139";
static const char *chip74151     = "74151";
static const char *chip74154     = "74154";
static const char *chip74163     = "74163";
static const char *chip74164     = "74164";
static const char *chip74165     = "74165";
static const char *chip74181     = "74181";
static const char *chip74191     = "74191";
static const char *chip74192     = "74192";
static const char *chip74193     = "74193";
static const char *chip74195     = "74195";
static const char *chip74240     = "74240";
static const char *chip74241     = "74241";
static const char *chip74244     = "74244";
static const char *chip74245     = "74245";
static const char *chip74283     = "74283";
static const char *chip74574     = "74574";

static const char *chip7489      = "7489";

static const char *chip2051     = "2051";
static const char *audiochip    = "8bit audio";
static const char *stepper1     = "UP Stepper motor";
static const char *stepper2     = "BP Stepper motor";

static const char *chip2708     = "2708";
static const char *chip2716     = "2716";
static const char *chip2732     = "2732";

static const char *extrapin      = "Connection Pin";
static const char *label_5       = "Label (5pt)";
static const char *label_3       = "Label (3pt)";
static const char *label_2       = "Label (2pt)";
static const char *label_1       = "Label (1pt)";
static const char *label_01      = "Label (0.1pt)";

static const char *extpin        = "External Pin";

BaseChipFactory::~BaseChipFactory()
{
}

Chip * BaseChipFactory::build(const char *aChipId)
{
    if (strcmp(extrapin,    aChipId) == 0) return new ExtraPin();
    if (strcmp(label_5,     aChipId) == 0) return new Label(5);
    if (strcmp(label_3,     aChipId) == 0) return new Label(3);
    if (strcmp(label_2,     aChipId) == 0) return new Label(2);
    if (strcmp(label_1,     aChipId) == 0) return new Label(1);
    if (strcmp(label_01,    aChipId) == 0) return new Label(0.1);

	if (strcmp(extpin,      aChipId) == 0) return new ExtPin(1);

    if (strcmp(sr_latch,    aChipId) == 0) return new SRChip();
    if (strcmp(sr_neg_latch,aChipId) == 0) return new SRNegChip();
    if (strcmp(ser_latch,   aChipId) == 0) return new SERChip();
    if (strcmp(d_latch,     aChipId) == 0) return new DChip();
    if (strcmp(t_latch,     aChipId) == 0) return new TChip();
    if (strcmp(jk_latch,    aChipId) == 0) return new JKChip();

    if (strcmp(mux,         aChipId) == 0) return new MuxChip();
    if (strcmp(dx,          aChipId) == 0) return new DxChip();

    if (strcmp(sr_flipflop, aChipId) == 0) return new SRFlipFlop();
    if (strcmp(d_flipflop,  aChipId) == 0) return new DFlipFlop();
    if (strcmp(t_flipflop,  aChipId) == 0) return new TFlipFlop();
    if (strcmp(jk_flipflop, aChipId) == 0) return new JKFlipFlop();

    if (strcmp(logic_low_chip , aChipId) == 0) return new StaticLevelChip(0);
    if (strcmp(logic_high_chip, aChipId) == 0) return new StaticLevelChip(1);

    if (strcmp(and_chip,    aChipId) == 0) return new ANDChip(1);
    if (strcmp(or_chip,     aChipId) == 0) return new ORChip(1);
    if (strcmp(nand_chip,   aChipId) == 0) return new NANDChip(1);
    if (strcmp(nor_chip,    aChipId) == 0) return new NORChip(1);
    if (strcmp(xor_chip,    aChipId) == 0) return new XORChip(1);
    if (strcmp(not_chip,    aChipId) == 0) return new NOTChip(1);

    if (strcmp(and_chip_fi, aChipId) == 0) return new ANDChip(0);
    if (strcmp(or_chip_fi,  aChipId) == 0) return new ORChip(0);
    if (strcmp(nand_chip_fi,aChipId) == 0) return new NANDChip(0);
    if (strcmp(nor_chip_fi, aChipId) == 0) return new NORChip(0);
    if (strcmp(xor_chip_fi, aChipId) == 0) return new XORChip(0);
    if (strcmp(not_chip_fi, aChipId) == 0) return new NOTChip(0);

    if (strcmp(and3_chip,   aChipId) == 0) return new AND3Chip(1);
    if (strcmp(and3_chip_fi,aChipId) == 0) return new AND3Chip(0);
    if (strcmp(nand3_chip,   aChipId) == 0) return new NAND3Chip(1);
    if (strcmp(nand3_chip_fi,aChipId) == 0) return new NAND3Chip(0);
    if (strcmp(nor3_chip,   aChipId) == 0) return new NOR3Chip(1);
    if (strcmp(nor3_chip_fi,aChipId) == 0) return new NOR3Chip(0);
    if (strcmp(or3_chip,   aChipId) == 0) return new OR3Chip(1);
    if (strcmp(or3_chip_fi,aChipId) == 0) return new OR3Chip(0);

    if (strcmp(and8_chip,   aChipId) == 0) return new AND8Chip(1);
    if (strcmp(and8_chip_fi,aChipId) == 0) return new AND8Chip(0);
    if (strcmp(nand8_chip,   aChipId) == 0) return new NAND8Chip(1);
    if (strcmp(nand8_chip_fi,aChipId) == 0) return new NAND8Chip(0);
    if (strcmp(nor8_chip,   aChipId) == 0) return new NOR8Chip(1);
    if (strcmp(nor8_chip_fi,aChipId) == 0) return new NOR8Chip(0);
    if (strcmp(or8_chip,   aChipId) == 0) return new OR8Chip(1);
    if (strcmp(or8_chip_fi,aChipId) == 0) return new OR8Chip(0);

    if (strcmp(led_red,     aChipId) == 0) return new LEDChip(0xff0000, 0);
    if (strcmp(led_green,   aChipId) == 0) return new LEDChip(0x00ff00, 0);
    if (strcmp(led_blue,    aChipId) == 0) return new LEDChip(0x0000ff, 0);
    if (strcmp(led_brblue,  aChipId) == 0) return new LEDChip(0x7f7fff, 0);
    if (strcmp(led_magenta, aChipId) == 0) return new LEDChip(0xff00ff, 0);
    if (strcmp(led_yellow,  aChipId) == 0) return new LEDChip(0xffff00, 0);
    if (strcmp(led_cyan,    aChipId) == 0) return new LEDChip(0x00ffff, 0);
    if (strcmp(led_white,   aChipId) == 0) return new LEDChip(0xffffff, 0);
    if (strcmp(sed_white,   aChipId) == 0) return new SEDChip();

    if (strcmp(ssg_red,     aChipId) == 0) return new SevenSegChip(0xff0000, 0);
    if (strcmp(ssg_green,   aChipId) == 0) return new SevenSegChip(0x00ff00, 0);
    if (strcmp(ssg_blue,    aChipId) == 0) return new SevenSegChip(0x0000ff, 0);
    if (strcmp(ssg_brblue,  aChipId) == 0) return new SevenSegChip(0x7f7fff, 0);
    if (strcmp(ssg_magenta, aChipId) == 0) return new SevenSegChip(0xff00ff, 0);
    if (strcmp(ssg_yellow,  aChipId) == 0) return new SevenSegChip(0xffff00, 0);
    if (strcmp(ssg_cyan,    aChipId) == 0) return new SevenSegChip(0x00ffff, 0);
    if (strcmp(ssg_white,   aChipId) == 0) return new SevenSegChip(0xffffff, 0);

    if (strcmp(stsg_red,     aChipId) == 0) return new SixteenSegChip(0xff0000, 0);
    if (strcmp(stsg_green,   aChipId) == 0) return new SixteenSegChip(0x00ff00, 0);
    if (strcmp(stsg_blue,    aChipId) == 0) return new SixteenSegChip(0x0000ff, 0);
    if (strcmp(stsg_brblue,  aChipId) == 0) return new SixteenSegChip(0x7f7fff, 0);
    if (strcmp(stsg_magenta, aChipId) == 0) return new SixteenSegChip(0xff00ff, 0);
    if (strcmp(stsg_yellow,  aChipId) == 0) return new SixteenSegChip(0xffff00, 0);
    if (strcmp(stsg_cyan,    aChipId) == 0) return new SixteenSegChip(0x00ffff, 0);
    if (strcmp(stsg_white,   aChipId) == 0) return new SixteenSegChip(0xffffff, 0);

    if (strcmp(iled_red,     aChipId) == 0) return new LEDChip(0xff0000, 1);
    if (strcmp(iled_green,   aChipId) == 0) return new LEDChip(0x00ff00, 1);
    if (strcmp(iled_blue,    aChipId) == 0) return new LEDChip(0x0000ff, 1);
    if (strcmp(iled_brblue,  aChipId) == 0) return new LEDChip(0x7f7fff, 1);
    if (strcmp(iled_magenta, aChipId) == 0) return new LEDChip(0xff00ff, 1);
    if (strcmp(iled_yellow,  aChipId) == 0) return new LEDChip(0xffff00, 1);
    if (strcmp(iled_cyan,    aChipId) == 0) return new LEDChip(0x00ffff, 1);
    if (strcmp(iled_white,   aChipId) == 0) return new LEDChip(0xffffff, 1);

    if (strcmp(issg_red,     aChipId) == 0) return new SevenSegChip(0xff0000, 1);
    if (strcmp(issg_green,   aChipId) == 0) return new SevenSegChip(0x00ff00, 1);
    if (strcmp(issg_blue,    aChipId) == 0) return new SevenSegChip(0x0000ff, 1);
    if (strcmp(issg_brblue,  aChipId) == 0) return new SevenSegChip(0x7f7fff, 1);
    if (strcmp(issg_magenta, aChipId) == 0) return new SevenSegChip(0xff00ff, 1);
    if (strcmp(issg_yellow,  aChipId) == 0) return new SevenSegChip(0xffff00, 1);
    if (strcmp(issg_cyan,    aChipId) == 0) return new SevenSegChip(0x00ffff, 1);
    if (strcmp(issg_white,   aChipId) == 0) return new SevenSegChip(0xffffff, 1);

    if (strcmp(istsg_red,     aChipId) == 0) return new SixteenSegChip(0xff0000, 1);
    if (strcmp(istsg_green,   aChipId) == 0) return new SixteenSegChip(0x00ff00, 1);
    if (strcmp(istsg_blue,    aChipId) == 0) return new SixteenSegChip(0x0000ff, 1);
    if (strcmp(istsg_brblue,  aChipId) == 0) return new SixteenSegChip(0x7f7fff, 1);
    if (strcmp(istsg_magenta, aChipId) == 0) return new SixteenSegChip(0xff00ff, 1);
    if (strcmp(istsg_yellow,  aChipId) == 0) return new SixteenSegChip(0xffff00, 1);
    if (strcmp(istsg_cyan,    aChipId) == 0) return new SixteenSegChip(0x00ffff, 1);
    if (strcmp(istsg_white,   aChipId) == 0) return new SixteenSegChip(0xffffff, 1);

    if (strcmp(ledgrid4x4_red,     aChipId) == 0) return new LedGrid(4, 0xff0000, 0);
    if (strcmp(ledgrid4x4_green,   aChipId) == 0) return new LedGrid(4, 0x00ff00, 0);
    if (strcmp(ledgrid4x4_blue,    aChipId) == 0) return new LedGrid(4, 0x0000ff, 0);
    if (strcmp(ledgrid4x4_brblue,  aChipId) == 0) return new LedGrid(4, 0x7f7fff, 0);
    if (strcmp(ledgrid4x4_magenta, aChipId) == 0) return new LedGrid(4, 0xff00ff, 0);
    if (strcmp(ledgrid4x4_yellow,  aChipId) == 0) return new LedGrid(4, 0xffff00, 0);
    if (strcmp(ledgrid4x4_cyan,    aChipId) == 0) return new LedGrid(4, 0x00ffff, 0);
    if (strcmp(ledgrid4x4_white,   aChipId) == 0) return new LedGrid(4, 0xffffff, 0);

    if (strcmp(iledgrid4x4_red,     aChipId) == 0) return new LedGrid(4, 0xff0000, 1);
    if (strcmp(iledgrid4x4_green,   aChipId) == 0) return new LedGrid(4, 0x00ff00, 1);
    if (strcmp(iledgrid4x4_blue,    aChipId) == 0) return new LedGrid(4, 0x0000ff, 1);
    if (strcmp(iledgrid4x4_brblue,  aChipId) == 0) return new LedGrid(4, 0x7f7fff, 1);
    if (strcmp(iledgrid4x4_magenta, aChipId) == 0) return new LedGrid(4, 0xff00ff, 1);
    if (strcmp(iledgrid4x4_yellow,  aChipId) == 0) return new LedGrid(4, 0xffff00, 1);
    if (strcmp(iledgrid4x4_cyan,    aChipId) == 0) return new LedGrid(4, 0x00ffff, 1);
    if (strcmp(iledgrid4x4_white,   aChipId) == 0) return new LedGrid(4, 0xffffff, 1);

    if (strcmp(ledgrid8x8_red,     aChipId) == 0) return new LedGrid(8, 0xff0000, 0);
    if (strcmp(ledgrid8x8_green,   aChipId) == 0) return new LedGrid(8, 0x00ff00, 0);
    if (strcmp(ledgrid8x8_blue,    aChipId) == 0) return new LedGrid(8, 0x0000ff, 0);
    if (strcmp(ledgrid8x8_brblue,  aChipId) == 0) return new LedGrid(8, 0x7f7fff, 0);
    if (strcmp(ledgrid8x8_magenta, aChipId) == 0) return new LedGrid(8, 0xff00ff, 0);
    if (strcmp(ledgrid8x8_yellow,  aChipId) == 0) return new LedGrid(8, 0xffff00, 0);
    if (strcmp(ledgrid8x8_cyan,    aChipId) == 0) return new LedGrid(8, 0x00ffff, 0);
    if (strcmp(ledgrid8x8_white,   aChipId) == 0) return new LedGrid(8, 0xffffff, 0);

    if (strcmp(iledgrid8x8_red,     aChipId) == 0) return new LedGrid(8, 0xff0000, 1);
    if (strcmp(iledgrid8x8_green,   aChipId) == 0) return new LedGrid(8, 0x00ff00, 1);
    if (strcmp(iledgrid8x8_blue,    aChipId) == 0) return new LedGrid(8, 0x0000ff, 1);
    if (strcmp(iledgrid8x8_brblue,  aChipId) == 0) return new LedGrid(8, 0x7f7fff, 1);
    if (strcmp(iledgrid8x8_magenta, aChipId) == 0) return new LedGrid(8, 0xff00ff, 1);
    if (strcmp(iledgrid8x8_yellow,  aChipId) == 0) return new LedGrid(8, 0xffff00, 1);
    if (strcmp(iledgrid8x8_cyan,    aChipId) == 0) return new LedGrid(8, 0x00ffff, 1);
    if (strcmp(iledgrid8x8_white,   aChipId) == 0) return new LedGrid(8, 0xffffff, 1);

    if (strcmp(chip309,     aChipId) == 0) return new Chip309();

    if (strcmp("LogicProbe", aChipId) == 0) return new LogicProbe();

    if (strcmp(clock_01hz,  aChipId) == 0) return new ClockChip(0.1);
    if (strcmp(clock_02hz,  aChipId) == 0) return new ClockChip(0.2);
    if (strcmp(clock_05hz,  aChipId) == 0) return new ClockChip(0.5);
    if (strcmp(clock_1hz,   aChipId) == 0) return new ClockChip(1);
    if (strcmp(clock_2hz,   aChipId) == 0) return new ClockChip(2);
    if (strcmp(clock_5hz,   aChipId) == 0) return new ClockChip(5);
    if (strcmp(clock_10hz,  aChipId) == 0) return new ClockChip(10);
    if (strcmp(clock_20hz,  aChipId) == 0) return new ClockChip(20);
    if (strcmp(clock_50hz,  aChipId) == 0) return new ClockChip(50);
    if (strcmp(clock_100hz, aChipId) == 0) return new ClockChip(100);
    if (strcmp(clock_200hz, aChipId) == 0) return new ClockChip(200);
    if (strcmp(clock_500hz, aChipId) == 0) return new ClockChip(500);

    if (strcmp(clock_1khz,   aChipId) == 0) return new ClockChip(1000);
    if (strcmp(clock_2khz,   aChipId) == 0) return new ClockChip(2000);
    if (strcmp(clock_5khz,   aChipId) == 0) return new ClockChip(5000);
    if (strcmp(clock_10khz,  aChipId) == 0) return new ClockChip(10000);
    if (strcmp(clock_20khz,  aChipId) == 0) return new ClockChip(20000);
    if (strcmp(clock_50khz,  aChipId) == 0) return new ClockChip(50000);

    if (strcmp(chip7400, aChipId) == 0) return new Chip7400();
    if (strcmp(chip7402, aChipId) == 0) return new Chip7402();
    if (strcmp(chip7404, aChipId) == 0) return new Chip7404();
    if (strcmp(chip7408, aChipId) == 0) return new Chip7408();
    if (strcmp(chip7410, aChipId) == 0) return new Chip7410();
    if (strcmp(chip7420, aChipId) == 0) return new Chip7420();
    if (strcmp(chip7432, aChipId) == 0) return new Chip7432();
    if (strcmp(chip7447, aChipId) == 0) return new Chip7447();
    if (strcmp(chip7473, aChipId) == 0) return new Chip7473();
    if (strcmp(chip7474, aChipId) == 0) return new Chip7474();
    if (strcmp(chip7485, aChipId) == 0) return new Chip7485();
    if (strcmp(chip7486, aChipId) == 0) return new Chip7486();
    if (strcmp(chip7490, aChipId) == 0) return new Chip7490();
    if (strcmp(chip74138, aChipId) == 0) return new Chip74138();
    if (strcmp(chip74139, aChipId) == 0) return new Chip74139();
    if (strcmp(chip74151, aChipId) == 0) return new Chip74151();
    if (strcmp(chip74154, aChipId) == 0) return new Chip74154();
    if (strcmp(chip74163, aChipId) == 0) return new Chip74163();
    if (strcmp(chip74164, aChipId) == 0) return new Chip74164();
    if (strcmp(chip74165, aChipId) == 0) return new Chip74165();
    if (strcmp(chip74181, aChipId) == 0) return new Chip74181();
    if (strcmp(chip74191, aChipId) == 0) return new Chip74191();
    if (strcmp(chip74192, aChipId) == 0) return new Chip74192();
    if (strcmp(chip74193, aChipId) == 0) return new Chip74193();
    if (strcmp(chip74195, aChipId) == 0) return new Chip74195();
    if (strcmp(chip74240, aChipId) == 0) return new Chip74240();
    if (strcmp(chip74241, aChipId) == 0) return new Chip74241();
    if (strcmp(chip74244, aChipId) == 0) return new Chip74244();
    if (strcmp(chip74245, aChipId) == 0) return new Chip74245();
    if (strcmp(chip74283, aChipId) == 0) return new Chip74283();
    if (strcmp(chip74574, aChipId) == 0) return new Chip74574();

	if (strcmp(chip7489, aChipId) == 0) return new Chip7489();

    if (strcmp(chip2051, aChipId) == 0) return new Chip2051();

    if (strcmp(chip2708, aChipId) == 0) return new Chip27xx(8);
    if (strcmp(chip2716, aChipId) == 0) return new Chip27xx(16);
    if (strcmp(chip2732, aChipId) == 0) return new Chip27xx(32);

    if (strcmp(audiochip, aChipId) == 0) return new AudioChip();
    if (strcmp(stepper1, aChipId) == 0) return new StepperMotor(0);
    if (strcmp(stepper2, aChipId) == 0) return new StepperMotor(1);

    int i;
    char temp[64];
    for (i = 0; i < 10; i++)
    {
        sprintf(temp, button_basestring, '0' + i);
        if (strcmp(temp, aChipId) == 0) return new ButtonChip('0' + i);
    }
	for (i = 0; i < 26; i++)
    {
        sprintf(temp, button_basestring, 'a' + i);
        if (strcmp(temp, aChipId) == 0) return new ButtonChip('a' + i);
    }
	if (strcmp(button_space, aChipId) == 0) return new ButtonChip(32);
	if (strcmp(button_shift, aChipId) == 0) return new ButtonChip(SDLK_LSHIFT);
	if (strcmp(button_return, aChipId) == 0) return new ButtonChip(SDLK_RETURN);

    for (i = 0; i < 10; i++)
    {
        sprintf(temp, switch_basestring, '0' + i);
        if (strcmp(temp, aChipId) == 0) return new SwitchChip('0' + i);
    }
    for (i = 0; i < 26; i++)
    {
        sprintf(temp, switch_basestring, 'a' + i);
        if (strcmp(temp, aChipId) == 0) return new SwitchChip('a' + i);
    }
	if (strcmp(switch_space, aChipId) == 0) return new SwitchChip(32);
	if (strcmp(switch_shift, aChipId) == 0) return new SwitchChip(SDLK_LSHIFT);
	if (strcmp(switch_return, aChipId) == 0) return new SwitchChip(SDLK_RETURN);
	
	i = strlen(aChipId);
	if (i > 7 && stricmp(aChipId+i-7,".atanua") == 0)
	{
		if (gActiveBoxes > gConfig.mMaxActiveBoxes && okcancel("Maximum number of active boxes exceeded.\nContinue loading anyway?\n\nIf you really need more boxes, adjust the limit in atanua.xml") == 0)
		{
			return NULL;
		}
		BoxStitchingInformation *d = do_preparse_box(aChipId);
		if (d == NULL)
		{
			return NULL;	
		}
		
		// we have a box
		return new Box(aChipId, d);
	}
    
    //unknown, sorry
    return NULL;
}


void BaseChipFactory::getSupportedChips(vector<char *> aChipList[5])
{
    aChipList[4].push_back(mystrdup(logic_low_chip));
    aChipList[4].push_back(mystrdup(logic_high_chip));

    aChipList[4].push_back(NULL);

    aChipList[0].push_back(mystrdup(and_chip));
    aChipList[0].push_back(mystrdup(or_chip));
    aChipList[0].push_back(mystrdup(nand_chip));
    aChipList[0].push_back(mystrdup(nor_chip));
    aChipList[0].push_back(mystrdup(xor_chip));
    aChipList[0].push_back(mystrdup(not_chip));

    aChipList[0].push_back(NULL);

    aChipList[0].push_back(mystrdup(and_chip_fi));
    aChipList[0].push_back(mystrdup(or_chip_fi));
    aChipList[0].push_back(mystrdup(nand_chip_fi));
    aChipList[0].push_back(mystrdup(nor_chip_fi));
    aChipList[0].push_back(mystrdup(xor_chip_fi));
    aChipList[0].push_back(mystrdup(not_chip_fi));

    aChipList[0].push_back(NULL);

    aChipList[0].push_back(mystrdup(and3_chip));
    aChipList[0].push_back(mystrdup(or3_chip));
    aChipList[0].push_back(mystrdup(nand3_chip));
    aChipList[0].push_back(mystrdup(nor3_chip));

    aChipList[0].push_back(NULL);

    aChipList[0].push_back(mystrdup(and3_chip_fi));
    aChipList[0].push_back(mystrdup(or3_chip_fi));
    aChipList[0].push_back(mystrdup(nand3_chip_fi));
    aChipList[0].push_back(mystrdup(nor3_chip_fi));

    aChipList[0].push_back(NULL);

    aChipList[0].push_back(mystrdup(and8_chip));
    aChipList[0].push_back(mystrdup(or8_chip));
    aChipList[0].push_back(mystrdup(nand8_chip));
    aChipList[0].push_back(mystrdup(nor8_chip));

    aChipList[0].push_back(NULL);

    aChipList[0].push_back(mystrdup(and8_chip_fi));
    aChipList[0].push_back(mystrdup(or8_chip_fi));
    aChipList[0].push_back(mystrdup(nand8_chip_fi));
    aChipList[0].push_back(mystrdup(nor8_chip_fi));

    aChipList[0].push_back(NULL);
    aChipList[0].push_back(mystrdup(sr_latch));
    aChipList[0].push_back(mystrdup(sr_neg_latch));
    aChipList[0].push_back(mystrdup(ser_latch));
    aChipList[0].push_back(mystrdup(d_latch));
    aChipList[0].push_back(mystrdup(t_latch));
    aChipList[0].push_back(mystrdup(jk_latch));

    aChipList[0].push_back(NULL);

    aChipList[0].push_back(mystrdup(sr_flipflop));
    aChipList[0].push_back(mystrdup(d_flipflop));
    aChipList[0].push_back(mystrdup(t_flipflop));
    aChipList[0].push_back(mystrdup(jk_flipflop));

    aChipList[0].push_back(NULL);

    aChipList[0].push_back(mystrdup(mux));
    aChipList[0].push_back(mystrdup(dx));

    aChipList[0].push_back(NULL);

    aChipList[4].push_back(mystrdup(clock_01hz));
    aChipList[4].push_back(mystrdup(clock_02hz));
    aChipList[4].push_back(mystrdup(clock_05hz));
    aChipList[4].push_back(mystrdup(clock_1hz));
    aChipList[4].push_back(mystrdup(clock_2hz));
    aChipList[4].push_back(mystrdup(clock_5hz));
    aChipList[4].push_back(mystrdup(clock_10hz));
    aChipList[4].push_back(mystrdup(clock_20hz));
    aChipList[4].push_back(mystrdup(clock_50hz));
    aChipList[4].push_back(mystrdup(clock_100hz));
    aChipList[4].push_back(mystrdup(clock_200hz));
    aChipList[4].push_back(mystrdup(clock_500hz));
    aChipList[4].push_back(mystrdup(clock_1khz));
    aChipList[4].push_back(mystrdup(clock_2khz));
    aChipList[4].push_back(mystrdup(clock_5khz));
    aChipList[4].push_back(mystrdup(clock_10khz));
    aChipList[4].push_back(mystrdup(clock_20khz));
    aChipList[4].push_back(mystrdup(clock_50khz));

    aChipList[4].push_back(NULL);

    aChipList[4].push_back(mystrdup(label_5));
    aChipList[4].push_back(mystrdup(label_3));
    aChipList[4].push_back(mystrdup(label_2));
    aChipList[4].push_back(mystrdup(label_1));
    aChipList[4].push_back(mystrdup(label_01));

    aChipList[4].push_back(NULL);

    aChipList[4].push_back(mystrdup(extpin));

	aChipList[4].push_back(NULL);

	aChipList[3].push_back(mystrdup(led_red));
    aChipList[3].push_back(mystrdup(led_green));
    aChipList[3].push_back(mystrdup(led_blue));
    aChipList[3].push_back(mystrdup(led_brblue));
    aChipList[3].push_back(mystrdup(led_cyan));
    aChipList[3].push_back(mystrdup(led_magenta));
    aChipList[3].push_back(mystrdup(led_yellow));
    aChipList[3].push_back(mystrdup(led_white));
    aChipList[3].push_back(mystrdup(sed_white));

    aChipList[3].push_back(mystrdup(ssg_red));
    aChipList[3].push_back(mystrdup(ssg_green));
    aChipList[3].push_back(mystrdup(ssg_blue));
    aChipList[3].push_back(mystrdup(ssg_brblue));
    aChipList[3].push_back(mystrdup(ssg_cyan));
    aChipList[3].push_back(mystrdup(ssg_magenta));
    aChipList[3].push_back(mystrdup(ssg_yellow));
    aChipList[3].push_back(mystrdup(ssg_white));

    aChipList[3].push_back(NULL);

    aChipList[3].push_back(mystrdup(stsg_red));
    aChipList[3].push_back(mystrdup(stsg_green));
    aChipList[3].push_back(mystrdup(stsg_blue));
    aChipList[3].push_back(mystrdup(stsg_brblue));
    aChipList[3].push_back(mystrdup(stsg_cyan));
    aChipList[3].push_back(mystrdup(stsg_magenta));
    aChipList[3].push_back(mystrdup(stsg_yellow));
    aChipList[3].push_back(mystrdup(stsg_white));

    aChipList[3].push_back(NULL);

    aChipList[3].push_back(mystrdup(chip309));

    aChipList[3].push_back(NULL);

    aChipList[3].push_back(mystrdup(ledgrid4x4_red));
    aChipList[3].push_back(mystrdup(ledgrid4x4_green));
    aChipList[3].push_back(mystrdup(ledgrid4x4_blue));
    aChipList[3].push_back(mystrdup(ledgrid4x4_brblue));
    aChipList[3].push_back(mystrdup(ledgrid4x4_cyan));
    aChipList[3].push_back(mystrdup(ledgrid4x4_magenta));
    aChipList[3].push_back(mystrdup(ledgrid4x4_yellow));
    aChipList[3].push_back(mystrdup(ledgrid4x4_white));

    aChipList[3].push_back(NULL);

    aChipList[3].push_back(mystrdup(ledgrid8x8_red));
    aChipList[3].push_back(mystrdup(ledgrid8x8_green));
    aChipList[3].push_back(mystrdup(ledgrid8x8_blue));
    aChipList[3].push_back(mystrdup(ledgrid8x8_brblue));
    aChipList[3].push_back(mystrdup(ledgrid8x8_cyan));
    aChipList[3].push_back(mystrdup(ledgrid8x8_magenta));
    aChipList[3].push_back(mystrdup(ledgrid8x8_yellow));
    aChipList[3].push_back(mystrdup(ledgrid8x8_white));

    aChipList[3].push_back(NULL);

	aChipList[3].push_back(mystrdup(iled_red));
    aChipList[3].push_back(mystrdup(iled_green));
    aChipList[3].push_back(mystrdup(iled_blue));
    aChipList[3].push_back(mystrdup(iled_brblue));
    aChipList[3].push_back(mystrdup(iled_cyan));
    aChipList[3].push_back(mystrdup(iled_magenta));
    aChipList[3].push_back(mystrdup(iled_yellow));
    aChipList[3].push_back(mystrdup(iled_white));

    aChipList[3].push_back(NULL);

    aChipList[3].push_back(mystrdup(issg_red));
    aChipList[3].push_back(mystrdup(issg_green));
    aChipList[3].push_back(mystrdup(issg_blue));
    aChipList[3].push_back(mystrdup(issg_brblue));
    aChipList[3].push_back(mystrdup(issg_cyan));
    aChipList[3].push_back(mystrdup(issg_magenta));
    aChipList[3].push_back(mystrdup(issg_yellow));
    aChipList[3].push_back(mystrdup(issg_white));

    aChipList[3].push_back(NULL);

    aChipList[3].push_back(mystrdup(istsg_red));
    aChipList[3].push_back(mystrdup(istsg_green));
    aChipList[3].push_back(mystrdup(istsg_blue));
    aChipList[3].push_back(mystrdup(istsg_brblue));
    aChipList[3].push_back(mystrdup(istsg_cyan));
    aChipList[3].push_back(mystrdup(istsg_magenta));
    aChipList[3].push_back(mystrdup(istsg_yellow));
    aChipList[3].push_back(mystrdup(istsg_white));

    aChipList[3].push_back(NULL);

    aChipList[3].push_back(mystrdup(iledgrid4x4_red));
    aChipList[3].push_back(mystrdup(iledgrid4x4_green));
    aChipList[3].push_back(mystrdup(iledgrid4x4_blue));
    aChipList[3].push_back(mystrdup(iledgrid4x4_brblue));
    aChipList[3].push_back(mystrdup(iledgrid4x4_cyan));
    aChipList[3].push_back(mystrdup(iledgrid4x4_magenta));
    aChipList[3].push_back(mystrdup(iledgrid4x4_yellow));
    aChipList[3].push_back(mystrdup(iledgrid4x4_white));

    aChipList[3].push_back(NULL);

    aChipList[3].push_back(mystrdup(iledgrid8x8_red));
    aChipList[3].push_back(mystrdup(iledgrid8x8_green));
    aChipList[3].push_back(mystrdup(iledgrid8x8_blue));
    aChipList[3].push_back(mystrdup(iledgrid8x8_brblue));
    aChipList[3].push_back(mystrdup(iledgrid8x8_cyan));
    aChipList[3].push_back(mystrdup(iledgrid8x8_magenta));
    aChipList[3].push_back(mystrdup(iledgrid8x8_yellow));
    aChipList[3].push_back(mystrdup(iledgrid8x8_white));

    aChipList[3].push_back(NULL);


    aChipList[1].push_back(mystrdup(chip7400));
    aChipList[1].push_back(mystrdup(chip7402));
    aChipList[1].push_back(mystrdup(chip7404));
    aChipList[1].push_back(mystrdup(chip7408));
    aChipList[1].push_back(mystrdup(chip7410));
    aChipList[1].push_back(mystrdup(chip7420));
    aChipList[1].push_back(mystrdup(chip7432));
    aChipList[1].push_back(mystrdup(chip7447));
    aChipList[1].push_back(mystrdup(chip7473));
    aChipList[1].push_back(mystrdup(chip7474));
    aChipList[1].push_back(mystrdup(chip7485));
    aChipList[1].push_back(mystrdup(chip7486));
	aChipList[1].push_back(mystrdup(chip7489));
    aChipList[1].push_back(mystrdup(chip7490));
    aChipList[1].push_back(mystrdup(chip74138));
    aChipList[1].push_back(mystrdup(chip74139));
    aChipList[1].push_back(mystrdup(chip74151));
    aChipList[1].push_back(mystrdup(chip74154));
    aChipList[1].push_back(mystrdup(chip74163));
    aChipList[1].push_back(mystrdup(chip74164));
    aChipList[1].push_back(mystrdup(chip74165));
    aChipList[1].push_back(mystrdup(chip74181));
    aChipList[1].push_back(mystrdup(chip74191));
    aChipList[1].push_back(mystrdup(chip74192));
    aChipList[1].push_back(mystrdup(chip74193));
    aChipList[1].push_back(mystrdup(chip74195));
    aChipList[1].push_back(mystrdup(chip74240));
    aChipList[1].push_back(mystrdup(chip74241));
    aChipList[1].push_back(mystrdup(chip74244));
    aChipList[1].push_back(mystrdup(chip74245));
    aChipList[1].push_back(mystrdup(chip74283));
    aChipList[1].push_back(mystrdup(chip74574));

    aChipList[1].push_back(NULL);

    aChipList[1].push_back(mystrdup(chip2051));

    aChipList[1].push_back(NULL);

    aChipList[1].push_back(mystrdup(chip2708));
    aChipList[1].push_back(mystrdup(chip2716));
    aChipList[1].push_back(mystrdup(chip2732));

    aChipList[1].push_back(NULL);

    int i;
    char temp[64];
    for (i = 0; i < 10; i++)
    {
        sprintf(temp, button_basestring, '0' + i);
        aChipList[2].push_back(mystrdup(temp));
    }

    aChipList[2].push_back(NULL);

    for (i = 0; i < 26; i++)
    {
        sprintf(temp, button_basestring, 'a' + i);
        aChipList[2].push_back(mystrdup(temp));
    }

    aChipList[2].push_back(NULL);
    
	aChipList[2].push_back(mystrdup(button_space));
    aChipList[2].push_back(mystrdup(button_shift));
    aChipList[2].push_back(mystrdup(button_return));

	aChipList[2].push_back(NULL);

    for (i = 0; i < 10; i++)
    {
        sprintf(temp, switch_basestring, '0' + i);
        aChipList[2].push_back(mystrdup(temp));
    }

    aChipList[2].push_back(NULL);

    for (i = 0; i < 26; i++)
    {
        sprintf(temp, switch_basestring, 'a' + i);
        aChipList[2].push_back(mystrdup(temp));
    }

    aChipList[2].push_back(NULL);
    
	aChipList[2].push_back(mystrdup(switch_space));
    aChipList[2].push_back(mystrdup(switch_shift));
    aChipList[2].push_back(mystrdup(switch_return));

	aChipList[2].push_back(NULL);

	aChipList[4].push_back(mystrdup("LogicProbe"));
    aChipList[4].push_back(NULL);
    aChipList[4].push_back(mystrdup(audiochip));
    aChipList[4].push_back(NULL);
    aChipList[4].push_back(mystrdup(stepper1));
    aChipList[4].push_back(mystrdup(stepper2));
    aChipList[4].push_back(NULL);
}
