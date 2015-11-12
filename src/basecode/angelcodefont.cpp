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
#include "atanua.h"
#include "toolkit.h"
#include "fileutils.h"

void ACFontInfoBlock::load(File * f)
{
    int blocksize = f->readint() - 4;
    fontSize = f->readword();
    int db = f->readbyte();
    smooth  = !!(db & (1 << 7));
    unicode = !!(db & (1 << 6));
    italic  = !!(db & (1 << 5));
    bold    = !!(db & (1 << 4));
    charSet = f->readbyte();
    stretchH = f->readword();
    aa = f->readbyte();
    paddingUp = f->readbyte();
    paddingRight = f->readbyte();
    paddingDown = f->readbyte();
    paddingLeft = f->readbyte();
    spacingHoriz = f->readbyte();
    spacingVert = f->readbyte();
    outline = f->readbyte();
    int fontnamelen = blocksize - (2 + 1 + 1 + 2 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1);
    fontName = new char[fontnamelen];
    f->readchars(fontName, fontnamelen);    
}
ACFontInfoBlock::~ACFontInfoBlock()
{
    delete[] fontName;
}
ACFontInfoBlock::ACFontInfoBlock()
{
    fontSize = bold = italic = unicode = smooth = charSet = stretchH = aa = paddingUp = 
    paddingLeft = paddingRight = paddingDown = spacingHoriz = spacingVert = outline = 0;
    fontName = 0;
}    

void ACFontCommonBlock::load(File * f)
{
    int blockSize = f->readint() - 4;
    lineHeight = f->readword();
    base = f->readword();
    scaleW = f->readword();
    scaleH = f->readword();
    pages = f->readword();
    int db = f->readbyte();
    packed  = !!(db & (1 << 0));
    encoded = !!(db & (1 << 1));
}
ACFontCommonBlock::ACFontCommonBlock()
{
    lineHeight = base = scaleW = scaleH = pages = packed = encoded = 0;
}

void ACFontPagesBlock::load(File * f)
{
    int blocksize = f->readint() - 4;
    int p = f->tell();
    int l = 1;
    while (f->readbyte()) l++;
    f->seek(p);
    pages = blocksize / l;
    int i;
    name = new char*[pages];
    glhandle = new int[pages];
    for (i = 0; i < pages; i++)
    {
        name[i] = new char[l];
        f->readchars(name[i], l);
        char temp[256];
        // should actually take the directory from the .fnt filename string..
        sprintf(temp,"data/%s",name[i]); 
        glhandle[i] = load_texture(temp, 1);
    }
}
ACFontPagesBlock::~ACFontPagesBlock()
{
    int i;
    for (i = 0; i < pages; i++)
        delete[] name[i];
    delete[] name;
    delete[] glhandle;
}
ACFontPagesBlock::ACFontPagesBlock()
{
	  glhandle = 0;
    pages = 0;
    name = 0;
}

void ACFontCharBlock::load(File * f)
{
    id = f->readword();
    x = f->readword();
    y = f->readword();
    width = f->readword();
    height = f->readword();
    xoffset = f->readword();
    yoffset = f->readword();
    xadvance = f->readword();
    page = f->readbyte();
    channel = f->readbyte();
}

ACFontCharBlock::ACFontCharBlock()
{
    id = x = y = width = height = xoffset = yoffset = xadvance = page = channel = 0;
}

void ACFontCharsBlock::load(File * f)
{
    int blocksize = f->readint() - 4;
    charcount = blocksize / (2 * 8 + 1 + 1);
    chars = new ACFontCharBlock[charcount];
    int i;
    for (i = 0; i < charcount; i++)
        chars[i].load(f);
}

ACFontCharsBlock::ACFontCharsBlock()
{
    charcount = 0;
    chars = 0;
}

ACFontCharsBlock::~ACFontCharsBlock()
{
    delete[] chars;
}


void ACFontKerningPair::load(File * f)
{
    first = f->readword();
    second = f->readword();
    amount = f->readword();
}

ACFontKerningPair::ACFontKerningPair()
{
    first = second = amount = 0;
}    

void ACFontKerningPairsBlock::load(File * f)
{
    int blocksize = f->readint() - 4;
    kerningPairs = blocksize / (2 + 2 + 2);
    pair = new ACFontKerningPair[kerningPairs];
    int i;
    for (i = 0; i < kerningPairs; i++)
        pair[i].load(f);
}

ACFontKerningPairsBlock::~ACFontKerningPairsBlock()
{
    delete[] pair;
}

ACFontKerningPairsBlock::ACFontKerningPairsBlock()
{
    kerningPairs = 0;
    pair = 0;
}

void ACFont::load(FILE *handle)
{
    File * f = new File(handle);
    if (f)
    {
        load(f);
        delete f;
    }
}

void ACFont::load(const char *filename)
{
    File * f = new File(fopen(filename, "rb"));
    if (f)
    {
        load(f);
        delete f;
    }
}

void ACFont::load(File * f)
{
    if (f->f == NULL) return;
    if (f->readbyte() != 0x42) return; // B
    if (f->readbyte() != 0x4d) return; // M
    if (f->readbyte() != 0x46) return; // F
    if (f->readbyte() != 2) return;    // ver 2
    if (f->readbyte() != 1) return;
    info.load(f);
    if (f->readbyte() != 2) return;
    common.load(f);
    if (f->readbyte() != 3) return;
    pages.load(f);
    if (f->readbyte() != 4) return;
    chars.load(f);
    if (f->readbyte() != 5) return; // this is ok to fail
    kerning.load(f);         
}

ACFont::ACFont()
{
    mCacheRoot = NULL;
    mFontCacheEntries = 0;
    mFontCacheTrashed = 0;
    mFontCacheUse = 0;
    mMRUMode = 0;
}

ACFontCharBlock * ACFont::findcharblock(int ch)
{
    int i;
    for (i = 0; i < chars.charcount; i++)
    {
        if (chars.chars[i].id == ch)
            return &(chars.chars[i]);
    }
    return chars.chars;
}

int ACFont::findkern(int id1, int id2)
{
    int i;
    for (i = 0; i < kerning.kerningPairs; i++)
        if (kerning.pair[i].first == id1 && kerning.pair[i].second == id2)
            return kerning.pair[i].amount;
    return 0;
}



volatile int crap = 0;

void ACFont::drawstring(const char * string, float x, float y, int color, float desired_ht)
{
    float scalefactor;
	if (stricmp(string,"Home")==0)
	{
		crap = 1;
	}
    if (desired_ht == 0.0f)
        scalefactor = 1.0f;
    else
        scalefactor = desired_ht / common.lineHeight;

    int l;
    unsigned int h;
    l = 0; h = 0; 
	int len = 0;
    while (string[l])
    {
        h ^= string[l];
        h = (h << 5) | (h >> (32-5));
        if (string[l] != '\n')
			len++;   
		l++;
    }
    h ^= *(unsigned int*)&scalefactor;

    //int len = l;

    if (len == 0) return;

    int currentpage = 0;
    if (pages.pages == 0)
        return;

    float xofs, yofs;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, pages.glhandle[0]);
    if (gConfig.mUseBlending)
    {
        glColor4f(((color >> 16) & 0xff) / 256.0f,
                ((color >> 8) & 0xff) / 256.0f,
                ((color >> 0) & 0xff) / 256.0f,
                ((color >> 24) & 0xff) / 256.0f);
    }
    else
    {
        glColor4f(1,1,1,1);
    }


    if (!gConfig.mUseOldFontSystem)
    {
        ACCachedString *str = NULL;
        ACCachedString *lru = NULL;
        ACCachedString *prev = NULL;
        ACCachedString *walker = mCacheRoot;

        while (walker != NULL && str == NULL)
        {
            if (walker->mHash == h && 
                walker->mHt == scalefactor && 
                strcmp(walker->mString, string) == 0)
            {
                str = walker;
            }
            prev = lru;
            lru = walker;
            walker = walker->mNext;
        }

        if (str == NULL && mFontCacheEntries < gConfig.mFontCacheMax)
        {
            if (mCacheRoot)
            {
                prev = lru;
                lru = new ACCachedString;
                prev->mNext = lru;
            }
            else
            {
                mCacheRoot = lru = new ACCachedString;
            }
            mFontCacheEntries++;
        }

        if (str == NULL)
        {
            int p = 0;
            mFontCacheTrashed++;

            if (mMRUMode == 0)
            {
                if (prev != NULL)
                {
                    prev->mNext = prev->mNext->mNext;
                    lru->mNext = mCacheRoot;
                    mCacheRoot = lru;
                }
            }
            else
            {
                mMRUMode--;
                lru = mCacheRoot;
            }

            delete[] lru->mCoords;
            delete[] lru->mTexcoords;
            delete[] lru->mString;

            if (gConfig.mUseVBOs && GLEE_ARB_vertex_buffer_object)
            {
                glDeleteBuffers(1, &lru->mVvbo);
                glDeleteBuffers(1, &lru->mTvbo);
            }

            lru->mCount = len * 8 - 2;
            lru->mCoords = new float[lru->mCount * 2];
            lru->mTexcoords = new float[lru->mCount * 2];
            lru->mHash = h;
            lru->mString = mystrdup(string);
            lru->mHt = scalefactor;

            xofs = 0;
            yofs = 0;
            int lastid = 0;
            while (*string)
            {

                xofs += findkern(lastid, *string) * scalefactor;
                lastid = *string;
                if (*string == '\n')
                {
                    xofs = 0;
                    yofs += common.lineHeight * scalefactor;					
                }
                else
                {
                    ACFontCharBlock *curr = findcharblock(*string);
                    if (curr->page != currentpage)
                    {
                        currentpage = curr->page;
                        glBindTexture(GL_TEXTURE_2D, pages.glhandle[currentpage]);
                    }

                    lru->mTexcoords[p] = curr->x / (float)common.scaleW;
                    lru->mCoords[p] = xofs + curr->xoffset * scalefactor;
                    if (p)
                    {
                        p++;
                        lru->mTexcoords[p] = lru->mTexcoords[p-2];
                        lru->mCoords[p] = lru->mCoords[p-2];
                        p++;
                        lru->mTexcoords[p] = lru->mTexcoords[p-2];
                        lru->mCoords[p] = lru->mCoords[p-2];

                        p++;
                        lru->mTexcoords[p] = lru->mTexcoords[p-2];
                        lru->mCoords[p] = lru->mCoords[p-2];
                        p++;
                        lru->mTexcoords[p] = lru->mTexcoords[p-2];
                        lru->mCoords[p] = lru->mCoords[p-2];
                    }
                    p++;
                    lru->mTexcoords[p] = curr->y / (float)common.scaleH;
                    lru->mCoords[p] = yofs + curr->yoffset * scalefactor;
                    p++;
                    lru->mTexcoords[p] = curr->x / (float)common.scaleW;
                    lru->mCoords[p] = xofs + curr->xoffset * scalefactor;
                    p++;
                    lru->mTexcoords[p] = (curr->y + curr->height) / (float)common.scaleH;
                    lru->mCoords[p] = yofs + (curr->yoffset + curr->height) * scalefactor;
                    p++;
                    lru->mTexcoords[p] = (curr->x + curr->width) / (float)common.scaleW;
                    lru->mCoords[p] = xofs + (curr->xoffset + curr->width) * scalefactor;
                    p++;
                    lru->mTexcoords[p] = curr->y / (float)common.scaleH;
                    lru->mCoords[p] = yofs + curr->yoffset * scalefactor;
                    p++;
                    lru->mTexcoords[p] = (curr->x + curr->width) / (float)common.scaleW;
                    lru->mCoords[p] = xofs + (curr->xoffset + curr->width) * scalefactor;
                    p++;
                    lru->mTexcoords[p] = (curr->y + curr->height) / (float)common.scaleH;
                    lru->mCoords[p] = yofs + (curr->yoffset + curr->height) * scalefactor;
                    p++;

                    lru->mTexcoords[p] = lru->mTexcoords[p-2];
                    lru->mCoords[p] = lru->mCoords[p-2];
                    p++;
                    lru->mTexcoords[p] = lru->mTexcoords[p-2];
                    lru->mCoords[p] = lru->mCoords[p-2];
                    p++;

                    lru->mTexcoords[p] = lru->mTexcoords[p-2];
                    lru->mCoords[p] = lru->mCoords[p-2];
                    p++;
                    lru->mTexcoords[p] = lru->mTexcoords[p-2];
                    lru->mCoords[p] = lru->mCoords[p-2];
                    p++;

                    xofs += curr->xadvance * scalefactor;
                }
                string++;
            }
            str = lru;

            if (gConfig.mUseVBOs && GLEE_ARB_vertex_buffer_object)
            {                  
                glGenBuffers(1, &lru->mVvbo);   
                glBindBuffer(GL_ARRAY_BUFFER, lru->mVvbo);
                glBufferData(GL_ARRAY_BUFFER, p * sizeof(float), lru->mCoords, GL_STATIC_DRAW);
                delete[] lru->mCoords;
                lru->mCoords = NULL;
                glGenBuffers(1, &lru->mTvbo);   
                glBindBuffer(GL_ARRAY_BUFFER, lru->mTvbo);
                glBufferData(GL_ARRAY_BUFFER, p * sizeof(float), lru->mTexcoords, GL_STATIC_DRAW);
                delete[] lru->mTexcoords;
                lru->mTexcoords = NULL;
            }
        }
        else
        {
            if (str != mCacheRoot)
            {
                prev->mNext = prev->mNext->mNext;
                str->mNext = mCacheRoot;
                mCacheRoot = str;
            }
        }

        mFontCacheUse++;

        glPushMatrix();
        glTranslatef(x,y,0);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        if (gConfig.mUseVBOs && GLEE_ARB_vertex_buffer_object)
        {
            glBindBuffer(GL_ARRAY_BUFFER, str->mVvbo);
            glVertexPointer(2, GL_FLOAT, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, str->mTvbo);
            glTexCoordPointer(2, GL_FLOAT, 0, 0);

            glDrawArrays(GL_TRIANGLE_STRIP,0,str->mCount);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else
        {
            glVertexPointer(2, GL_FLOAT, 0, str->mCoords);
            glTexCoordPointer(2, GL_FLOAT, 0, str->mTexcoords);

            glDrawArrays(GL_TRIANGLE_STRIP,0,str->mCount);
        }

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glPopMatrix();
    }
    else
    {
        // fallback old glbegin/glend system
        xofs = x;
        yofs = y;
        int lastid = 0;
        while (*string)
        {

            xofs += findkern(lastid, *string) * scalefactor;
            lastid = *string;
            if (*string == '\n')
            {
                xofs = x;
                yofs += common.lineHeight * scalefactor;
            }
            else
            {
                ACFontCharBlock *curr = findcharblock(*string);
                if (curr->page != currentpage)
                {
                    currentpage = curr->page;
                    glBindTexture(GL_TEXTURE_2D, pages.glhandle[currentpage]);
                }

                glBegin(GL_TRIANGLE_STRIP);
                glTexCoord2f(curr->x / (float)common.scaleW, curr->y / (float)common.scaleH);
                glVertex2f(xofs + curr->xoffset * scalefactor, yofs + curr->yoffset * scalefactor);

                glTexCoord2f(curr->x / (float)common.scaleW, (curr->y + curr->height) / (float)common.scaleH);
                glVertex2f(xofs + curr->xoffset * scalefactor, yofs + (curr->yoffset + curr->height) * scalefactor);

                glTexCoord2f((curr->x + curr->width) / (float)common.scaleW, curr->y / (float)common.scaleH);
                glVertex2f(xofs + (curr->xoffset + curr->width) * scalefactor, yofs + curr->yoffset * scalefactor);

                glTexCoord2f((curr->x + curr->width) / (float)common.scaleW, (curr->y + curr->height) / (float)common.scaleH);
                glVertex2f(xofs + (curr->xoffset + curr->width) * scalefactor, yofs + (curr->yoffset + curr->height) * scalefactor);
                glEnd();

                xofs += curr->xadvance * scalefactor;
            }
            string++;
        }
    }
    glDisable(GL_TEXTURE_2D);
}

void ACFont::fontcacheframe()
{
    if (mFontCacheTrashed >= gConfig.mFontCacheMax)
    {
        mMRUMode = mFontCacheTrashed - 1;
    }

    mFontCacheTrashed = 0;
    mFontCacheUse = 0;
}

void ACFont::stringmetrics(const char * string, float &w, float &h, float &lastlinew, float desired_ht)
{
    float scalefactor;
    if (desired_ht == 0.0f)
        scalefactor = 1.0f;
    else
        scalefactor = desired_ht / common.lineHeight;

    if (pages.pages == 0)
    {
        w = h = 0;
        return;
    }
    float maxx = 0;
    int len = strlen(string);
    float xofs, yofs;
    xofs = 0;
    yofs = 0;
    int lastid = 0;
    while (*string)
    {
        xofs += findkern(lastid, *string) * scalefactor;
        lastid = *string;
        if (*string == '\n')
        {
            xofs = 0;
            yofs += common.lineHeight * scalefactor;
        }
        else
        {
            ACFontCharBlock *curr = findcharblock(*string);
            xofs += curr->xadvance * scalefactor;
        }
        if (maxx < xofs) maxx = xofs;
        string++;
    }
    lastlinew = xofs;
    w = maxx;
    h = yofs + common.lineHeight * scalefactor;
}
