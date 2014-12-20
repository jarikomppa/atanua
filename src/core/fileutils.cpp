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
#include <stdio.h>
#include <stdlib.h>
#include "toolkit.h"
#include "fileutils.h"

File::File() 
{ 
    f = NULL; 
}

File::File(FILE *aFileHandle) 
{ 
    f = aFileHandle;
}

File::File(const char *aFilename, const char *aFileOpenTypes)
{
    f = fopen(aFilename, aFileOpenTypes);
}

File::~File()
{
    if (f) fclose(f);
}

int File::readbyte()
{
    char i = 0;
    fread(&i,1,1,f);
    return i;
}

int File::readword()
{
    short i = 0;
    fread(&i,2,1,f);
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    return i;
#else
    return (signed short)SDL_Swap16(i);
#endif
}

int File::readint()
{
    int i = 0;
    fread(&i,4,1,f);
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    return i;
#else
    return SDL_Swap32(i);
#endif
}

void File::readchars(char * dst, int count)
{
    while (count)
    {
        *dst = readbyte();
        dst++;
        count--;
    }
}

void File::writebyte(char data)
{
    fwrite(&data,1,1,f);
}

void File::writeword(short data)
{
#if SDL_BYTEORDER != SDL_LIL_ENDIAN
    data = SDL_Swap16(data);
#endif
    fwrite(&data,2,1,f);
}

void File::writeint(int data)
{
#if SDL_BYTEORDER != SDL_LIL_ENDIAN
    data = SDL_Swap32(data);
#endif
    fwrite(&data,4,1,f);
}

void File::writechars(const char * src, int count)
{
    while (count)
    {
        writebyte(*src);
        src++;
        count--;
    }
}

int File::tell()
{
    return ftell(f);
}

void File::seek(int pos)
{
    fseek(f,pos,SEEK_SET);
}



MemoryFile::MemoryFile() 
{ 
    mDataIdx = 0;
}

MemoryFile::~MemoryFile()
{
}

int MemoryFile::readbyte()
{
    char i = mData[mDataIdx];
    mDataIdx++;
    return i;
}

int MemoryFile::readword()
{
    unsigned short i = 0;
    i |= (unsigned char)readbyte(); i <<= 8;
    i |= (unsigned char)readbyte();
    return i;
}

int MemoryFile::readint()
{
    unsigned int i = 0;
    i |= (unsigned char)readbyte(); i <<= 8;
    i |= (unsigned char)readbyte(); i <<= 8;
    i |= (unsigned char)readbyte(); i <<= 8;
    i |= (unsigned char)readbyte();
    return i;
}

void MemoryFile::writebyte(char data)
{
    mData.push_back(data);
    mDataIdx++;
}

void MemoryFile::writeword(short data)
{
    unsigned short d = (unsigned short)data;
    mData.push_back((d >> 8) & 0xff);
    mData.push_back((d >> 0) & 0xff);
    mDataIdx += 2;
}

void MemoryFile::writeint(int data)
{
    unsigned int d = (unsigned int)data;
    mData.push_back((d >> 24) & 0xff);
    mData.push_back((d >> 16) & 0xff);
    mData.push_back((d >> 8) & 0xff);
    mData.push_back((d >> 0) & 0xff);
    mDataIdx += 4;
}

int MemoryFile::tell()
{
    return mDataIdx;
}

void MemoryFile::seek(int pos)
{
    mDataIdx = pos;
}
