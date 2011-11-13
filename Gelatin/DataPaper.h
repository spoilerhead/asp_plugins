#ifndef DATAPAPER_H
#define DATAPAPER_H
#include "CFilter.h"


#define NUM_PAPERS 7
static const contrastType papers[NUM_PAPERS] = {
    {   //0     0.25    0.5     0.75    1       1.25    1.5     1.75    2
        {0.1f,  .26f,   .48f,   .72f,    0.99f,  1.29f,   1.58f,  1.82f,  2.04f,  2.1f},
        2.1f,
        0.1f,
        0.05f/*1.f*/, //exposure multiplier + = darker, - is brighter calibrated for about RGB = 95
        "Fomaspeed Variant III, Filter 00 - special soft" //only extrapolated
    },
    {   //0     0.25    0.5     0.75    1       1.25    1.5     1.75    2
        {0.1f,  .24f,   .45f,   .7f,    1.03f,  1.33f,   1.65f,  1.90f,  2.06f,  2.1f},
        2.1f,
        0.1f,
        0.05f/*1.f*/, //exposure multiplier + = darker, - is brighter calibrated for about RGB = 95
        "Fomaspeed Variant III, Filter 0 - extra soft"
    },
    {
        {0.1f,  .22f,   .41f,   .72f,   1.08f,  1.41f,  1.77f,  2.01f,  2.07f,  2.1f},
        2.1f,
        0.1f,
        -0.10f/*1.f*/, //exposure multiplier
        "Fomaspeed Variant III, Filter 1 - soft"
    },
    {
        {0.1f,  .18f,   .33f,   .76f,   1.15f,  1.56f,  1.88f,  2.05f,  2.07f,  2.1f},
        2.1f,
        0.10f,
        -0.3f/*1.f*/, //exposure multiplier
        "Fomaspeed Variant III, Filter 2 - special"
    },
    {
        {0.1f,  .13f, .24f,    .78f    ,1.27f, 1.72f,   2.01f,  2.06f,   2.08f, 2.1f},
        2.1f,
        0.1f,
        -0.45f/*1.f*/, //exposure multiplier
        "Fomaspeed Variant III, Filter 3 - normal"
    },
    {
        {0.1f,  .115f,    .13f,   .30f,   .72f,   1.30f,  1.78f,  2.05f,  2.09f,  2.1f},
        2.1f,
        0.1f,
        -0.f/*1.26f*/,//1.5f, //exposure multiplier //fomaspeed 4 and 5 needs prolonged exposure (1 stop in lab)
        "Fomaspeed Variant III, Filter 4 - hard"
    },
    {
        {0.1f,  .11f,    .12f,   .26f,   .73f,   1.35f,  1.90f,  2.07f,  2.09f,  2.1f},
        2.1f,
        0.1f,
        -0.f/*1.26f*/,//1.5f, //exposure multiplier (1 stop in lab)
        "Fomaspeed Variant III, Filter 5 - ultra hard"
    }

}; //end papers
#endif
