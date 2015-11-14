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
#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <vector>
using namespace std;

class File
{
public:
    FILE * f;
    File();
    File(FILE *aFileHandle);
    File(const char *aFilename, const char *aFileOpenTypes);
    virtual ~File();
    virtual int readbyte();
    virtual int  readword();
    virtual int  readint();
    virtual void readchars(char * dst, int count);
    virtual void writebyte(char data);
    virtual void writeword(short data);
    virtual void writeint(int data);
    virtual void writechars(const char * src, int count);
    virtual int tell();
    virtual void seek(int pos);
};

class MemoryFile : public File
{
public:
    vector<char> mData;
    int mDataIdx;
    MemoryFile();
    ~MemoryFile();
    virtual int readbyte();
    virtual int readword();
    virtual int readint();
    virtual void writebyte(char data);
    virtual void writeword(short data);
    virtual void writeint(int data);
    virtual int tell();
    virtual void seek(int pos);
};
#endif
