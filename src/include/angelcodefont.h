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
// Angelcode bitmap font generator font loader, as of 1.9c (format version 2)

#ifndef ANGELCODEFONT_H
#define ANGELCODEFONT_H

class File;

struct ACCachedString
{
    ACCachedString() { mNext = NULL; mString = NULL; mTexcoords = NULL; mCoords = NULL; mTvbo = 0; mVvbo = 0; mHash = 0; mHt = 0; mCount = 0;}
    ACCachedString *mNext;
    unsigned int mHash;
    char * mString;
    float mHt;
    float *mTexcoords, *mCoords;
    GLuint mTvbo, mVvbo;
    unsigned int mCount;
};

class ACFontInfoBlock
{
public:
    int fontSize;
    int bold;
    int italic;
    int unicode;
    int smooth;
    int charSet;
    int stretchH;
    int aa;
    int paddingUp;
    int paddingRight;
    int paddingDown;
    int paddingLeft;
    int spacingHoriz;
    int spacingVert;
    int outline;
    char * fontName;
    void load(File * f);
    ~ACFontInfoBlock();
    ACFontInfoBlock();
};

class ACFontCommonBlock
{
public:
    int lineHeight;
    int base;
    int scaleW;
    int scaleH;
    int pages;
    int packed;
    int encoded;
    void load(File * f);
    ACFontCommonBlock();
};

class ACFontPagesBlock
{
public:
    int pages;
    int *glhandle;
    char **name;
    void load(File * f);
    ~ACFontPagesBlock();
    ACFontPagesBlock();
};


class ACFontCharBlock
{
public:
    int id;
    int x;
    int y;
    int width;
    int height;
    int xoffset;
    int yoffset;
    int xadvance;
    int page;
    int channel;
    void load(File * f);
    ACFontCharBlock();
};

class ACFontCharsBlock
{
public:
    int charcount;
    ACFontCharBlock *chars;
    void load(File * f);
    ACFontCharsBlock();
    ~ACFontCharsBlock();
};


class ACFontKerningPair
{
public:
    int first;
    int second;
    int amount;
    void load(File * f);
    ACFontKerningPair();
};

class ACFontKerningPairsBlock
{
public:
    int kerningPairs;
    ACFontKerningPair * pair;
    void load(File * f);
    ~ACFontKerningPairsBlock();
    ACFontKerningPairsBlock();
};

class ACFont
{
public:
    ACFont();
    int mFontCacheEntries;
    int mFontCacheTrashed;
    int mFontCacheUse;
    void fontcacheframe();
    ACFontInfoBlock info;
    ACFontCommonBlock common;
    ACFontPagesBlock pages;
    ACFontCharsBlock chars;
    ACFontKerningPairsBlock kerning;
    void load(const char *filename);
    void load(FILE * f);
    void load(File * f);
    void drawstring(const char * string, float x, float y, int color=0xffffffff,float desired_ht = 0.0f);
    void stringmetrics(const char * string, float &w, float &h, float &lastlinew, float desired_ht = 0.0f);
protected:
    int mMRUMode;
    ACCachedString *mCacheRoot;
    ACFontCharBlock *findcharblock(int ch);
    int findkern(int id1, int id2);
};

#endif