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
#ifndef MERSENNETWISTER_H_INCLUDED
#define MERSENNETWISTER_H_INCLUDED


class MersenneTwister
{
public:

    // uses init_genrand to initialize the rng
    explicit MersenneTwister(unsigned long s = 5489UL);
    // uses init_by_array to initialize the rng
    explicit MersenneTwister(unsigned long init_key[], int key_length);
    ~MersenneTwister();

    /* initializes state[N] with a seed */
    void init_genrand(unsigned long s);

    /* initialize by an array with array-length */
    /* init_key is the array for initializing keys */
    /* key_length is its length */
    void init_by_array(unsigned long init_key[], int key_length);

    /* generates a random number on [0,0xffffffff]-interval */
    unsigned long genrand_int32();

    /* generates a random number on [0,0x7fffffff]-interval */
    long genrand_int31();

    /* generates a random number on [0,1]-real-interval */
    double genrand_real1();

    /* generates a random number on [0,1)-real-interval */
    double genrand_real2();

    /* generates a random number on (0,1)-real-interval */
    double genrand_real3();

    /* generates a random number on [0,1) with 53-bit resolution*/
    double genrand_res53();

private:

    enum MT_ENUM
    {
        N = 624
    };

    void next_state();

    unsigned long state[N]; /* the array for the state vector */
    int left;
    //int initf;
    unsigned long *next;
};


#endif // !MERSENNETWISTER_H_INCLUDED
