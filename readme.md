# Atanua Real-Time Logic Simulator

This is a real-time logic simulator designed for educational use. It was originally built as a bachelor of engineering project, and it was commercially available from 2008 to 2014. Since it never really "took off" commercially, I decided to release it as open source.

If you want to support the development financially, you can still buy the commercial licenses (even though you don't need the license key for any builds based on the open source release) at http://atanua.org

Binaries can be found at http://atanua.org - I'll probably only make windows builds from now on, though.

## Overview

The simulation itself is based on having little snippets of code called "chips" connected by "wires" which produce "nets". When simulating, all "chips" are told to update, reading their input values from the input "pins" and writing the result in the output "pins". Once all chips are updated, all the nets are updated; each net reads the output pins of the chips in their net and write the result to the input pins. This process is repeated (by default) at about 1kHz.

Some more details regarding simulation can be read from my bachelor's thesis (such as why there's 7 boolean states, none of which is FILE_NOT_FOUND).

The user interface is basically an OpenGL 1.x rendered IMGUI widget.

Some source files of interest -

- atanua.h is the "windows.h" of atanua, with a lot of class definitions.
- atanua_internal.h is the "simulator internal" header, with data not exposed to the chip code.
- main.cpp hosts the user interface code in the ~1200 line draw_screen() function. This function is relatively fragile, as the IMGUI nature of the interface depends a lot on the order of operations.
- nativefunctions.cpp contains code to access native file io and popup dialogs on windows, linux and osx. (osx additionally has an .m file).
- simutils.cpp has all sorts of code related to the manipulation of simulation state, like net building and complex deletions, etc.
- basechipfactory.cpp links all the separate chip modules to the system. It was more tolerable when there were less than 50 varieties, the way I built this doesn't scale too well.
- atanuaconfig.cpp loads (and possibly creates) the atanua.xml, which contains the application preferences.
- fileutils.cpp contains 'file' and 'memoryfile' class implementations, switching between these makes it possible to serialize into memory instead of file, for instance..
- fileio.cpp contains the project file saving and loading functions. Originally the file was binary, but I opted for xml later on. The binary format is still in use, as the undo/redo stack is simply storing the whole project in memory.
- toolkit.cpp, mersennetwister.cpp and angelcodefont.cpp are pretty directly from my "2d opengl toolkit" that I've released separately long ago.
- core.c, disasm.c and opcodes.c, the 8051 support files, are directly from the 8051 simulator I released separately long ago.
- The chips themselves are relatively simple, and creating new ones is pretty simple.

One of the base design principles with Atanua was that all of the chips must have real-life counterparts, so chips such as "pulse this input and it plays a song" or "really easy graphics interface chips" were not implemented even though they were requested. If you feel you want such chips (which could just be microcontrollers in disguise these days), feel free.

## Known issues

There's fairly few comments in there. Sorry about that.

There's dependency to a couple of rather old third party code bases, namely tinyxml and GLee. At some point I considered replacing tinyxml with tinyxml2, but the APIs have changed so much that I didn't go through with it.

Atanua has been known to crash, especially with very complex designs. There's an autosave system, but it's not in use by default. The crashes may have something to do with the undo/redo stack, not sure.

Loading of some very, very, VERY complex designs can take a lot of time. Working with said designs can be painful because Atanua serializes things to the undo stack rather often. Some optimizations have been done (like lazy rebuilding of nets instead of the brute-force algorithm that worked fine up to ~100 chips or something).

There may be some partial optimization left in place which may cause more issues.

## Stuff I've Considered, But Not Done

Atanua was designed to do electronics engineering lab experiments, which means that a design probably never has more than a dozen chips in it. What actually happened was that some students have created designs with thousands of chips in them.

Some people have asked whether Atanua could do analog components, which practically would require SPICE or something.

To make the desings really fly at pretty high frequencies it could be possible to just build AST out of the design and run it through LLVM or something (this would probably rule out the most complex chips like 8051).

I had looked at and considered adding support for other microcontrollers like AVR, but never got around to it.

Several game designs were pondered on top of Atanua, but never built (as they probably would have required more work than has gone into Atanua as is).