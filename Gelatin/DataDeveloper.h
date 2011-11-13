#ifndef DATADEVELOPER_H
#define DATADEVELOPER_H
#include "CFilter.h"


#define NUM_DEV 9
static const contrastType developers[NUM_DEV] = {
    {
        {0.f,.025f,.08f,.17f,.3f,.42f,.54f,.64f,.73f,.82f},
        0.73f,
        0.01f,
        0.0f,//1.33f, //factor, EV calibrated for about RGB = 95
        "trix-400, tmax developer, 6 min pull1"
    },
    {
        {0.f,.03f,.1f,.22f,.36f,.48f,.6f,.7f,.8f,.9f},
        0.8f,
        0.01f,
        0.f,//1.f, //factor, EV
        "trix-400, tmax developer, 7 min std"
    },
    {
        {0.f,.05f,.15f,.29f,.46f,.60f,.73f,.85f,.95f,1.05f},
        0.95f,
        0.01f,
        0.f,//.25f, //factor, EV
        "trix-400, tmax developer, 9 min push1"
    },
    {
        {0.f,.06f,.18f,.41f,.57f,.70f,.83f,.95f,1.05f,1.15f},
        1.05f,
        0.01f,
        0.f,//-.5f, //factor, EV
        "trix-400, tmax developer, 11 min, push2"
    },
    //neopan 100, D-76, 4min is pull1, 7 is std, 10min is push1
    {
    #define F 2.2f
        {0.f,.03f/F,.09f/F,.30f/F,.56f/F,.83f/F,1.10f/F,1.38f/F,1.65f/F,1.90f/F},
        1.65f/F,
        0.01f,
        0.25f,//-.5f, //factor, EV
        "Neopan 100, D-76, 4 min pull1" //auf auge fuji-FilmNBDataGuide.pdf
    },
    
    {
        {0.f,.05f/F,.22f/F,.44f/F,.74f/F,1.10f/F,1.41f/F,1.76f/F,2.1f/F,2.45f/F},
        2.1f/F,
        0.01f,
        0.25f,//-.5f, //factor, EV
        "Neopan 100, D-76, 7 min std" //auf auge fuji-FilmNBDataGuide.pdf
    },
    {
        {0.f,.06f/F,.32f/F,.65f/F,1.00f/F,1.33f/F,1.70f/F,2.1f/F,2.45f/F,2.8f/F},
        2.45f/F,
        0.01f,
        0.25f,//-.5f, //factor, EV
        "Neopan 100, D-76, 10 min push1" //auf auge fuji-FilmNBDataGuide.pdf
    },
    #undef F
    {
        {0.f,.01f/4.f,.06f/4.f,.34f/4.f,1.78f/4.f,3.28f/4.f,4.11f/4.f,4.21f/4.f,4.25f/4.f,4.26f/4.f}, //div by 4 for normalization
        4.25f/4.f, //max
        0.01f,
        0.f,//0.1667f, //exposure add
        "TPAN, Dektol, 3 min (3,6)"
    },
    #define F 5.0f
    {   //-3,5 ...+1
       {0.f,.005f/F,
        .025f/F,0.40f/F,
        4.10f/F,4.85f/F,
        5.00f/F,5.04f/F,
        5.08f/F,5.10f/F},
        5.08f/F,
        0.002f,
        0.0f,//-.5f, //factor, EV
        "Kodalith, Kodalith 2.75 min"
    }
    #undef F


}; //end developers

#endif
