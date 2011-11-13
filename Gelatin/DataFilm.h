#ifndef DATAFILM_H
#define DATAFILM_H
#include "CFilter.h"

#define NUM_FILMS 33
static const filterType films[NUM_FILMS] = { //=========================================================
    { //None
        {
            1.f,1.f,1.f,1.f,1.f,1.f,
            1.f,1.f,1.f,1.f,1.f,1.f,
            1.f,1.f,1.f,1.f,1.f,1.f
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "None"
    },
    //===============================
    { 
         {
            0.93f,          //1  505   495-515 bluegreen
            0.87f,          //2  495   very narrow
            0.86f,          //3  490    "", cyan
            0.82f,          //4  485  ""
            0.79f,          //5  480  ""
            0.82f,          //6  465  455-475 blue
            0.90f,          //7  440  400-170
            0.72f,          //8  400  390-410 violet, magenta
            0.35f,          //9  380 magenta
            (0.25f+0.2f)/2.f,//10 <380 & >650
            0.55f,           //11 650 600-700 red
            1.08f,          //12 600 590-610 red/orange
            1.26f,          //13 585 580-595 orange/yrellow
            1.31f,          //14 580 575-585 yellow
            1.35f,          //15 575 565-580 yellow/lime
            1.26f,          //16 565 555-575 lime/green
            1.13f,          //17 545 525-565 green
            1.10f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Kodak BW400CN"
    },
    //===============================
    { 
         {
            1.17f,          //1  505   495-515 bluegreen
            1.20f,          //2  495   very narrow
            1.22f,          //3  490    "", cyan
            1.24f,          //4  485  ""
            1.28f,          //5  480  ""
            1.46f,          //6  465  455-475 blue
            1.70f,          //7  440  400-170
            1.72f,          //8  400  390-410 violet, magenta
            0.50f,          //9  380 magenta
            (0.4f+0.5f)/2.f,//10 <380 & >650
            0.55f,           //11 650 600-700 red
            1.20f,          //12 600 590-610 red/orange
            1.25f,          //13 585 580-595 orange/yrellow
            1.27f,          //14 580 575-585 yellow
            1.29f,          //15 575 565-580 yellow/lime
            1.25f,          //16 565 555-575 lime/green
            1.18f,          //17 545 525-565 green
            1.09f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Kodak Plus-X 125"
    },
    //===============================
    { 
         {
            1.01f,          //1  505   495-515 bluegreen
            1.01f,          //2  495   very narrow
            1.03f,          //3  490    "", cyan
            1.06f,          //4  485  ""
            1.09f,          //5  480  ""
            1.21f,          //6  465  455-475 blue
            1.35f,          //7  440  400-170
            1.48f,          //8  400  390-410 violet, magenta
            1.45f,          //9  380 magenta
            (1.4f+0.5f)/2.f,//10 <380 & >650
            0.5f,           //11 650 600-700 red
            1.01f,          //12 600 590-610 red/orange
            1.03f,          //13 585 580-595 orange/yrellow
            1.01f,          //14 580 575-585 yellow
            1.01f,          //15 575 565-580 yellow/lime
            1.04f,          //16 565 555-575 lime/green
            1.11f,          //17 545 525-565 green
            1.05f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Kodak TMAX 100"
    },

    //===============================
    { 
         {
            1.70f,          //1  505   495-515 bluegreen
            1.65f,          //2  495   very narrow
            1.63f,          //3  490    "", cyan
            1.62f,          //4  485  ""
            1.7f,          //5  480  ""
            1.82f,          //6  465  455-475 blue
            1.86f,          //7  440  400-170
            1.82f,          //8  400  390-410 violet, magenta
            1.65f,          //9  380 magenta
            (1.6f+1.f)/2.f,//10 <380 & >650
            1.1f,           //11 650 600-700 red
            1.52f,          //12 600 590-610 red/orange
            1.9f,          //13 585 580-595 orange/yrellow
            1.89f,          //14 580 575-585 yellow
            1.87f,          //15 575 565-580 yellow/lime
            1.84f,          //16 565 555-575 lime/green
            1.92f,          //17 545 525-565 green
            1.85f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Kodak TMAX 400 new (2007)"
    },
    //===============================
    { 
         {
            0.96f,          //1  505   495-515 bluegreen
            0.97f,          //2  495   very narrow
            0.99f,          //3  490    "", cyan
            1.03f,          //4  485  ""
            1.07f,          //5  480  ""
            1.15f,          //6  465  455-475 blue
            1.20f,          //7  440  400-170
            1.40f,          //8  400  390-410 violet, magenta
            1.45f,          //9  380 magenta
            (1.4f+0.4f)/2.f,//10 <380 & >650
            0.6f,           //11 650 600-700 red
            0.89f,          //12 600 590-610 red/orange
            0.89f,          //13 585 580-595 orange/yrellow
            0.85f,          //14 580 575-585 yellow
            0.86f,          //15 575 565-580 yellow/lime
            0.92f,          //16 565 555-575 lime/green
            0.99f,          //17 545 525-565 green
            1.01f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Kodak TMAX 400 old (?)"
    },
    //===============================
    { 
         {
            1.02f,          //1  505   495-515 bluegreen
            1.02f,          //2  495   very narrow
            1.04f,          //3  490    "", cyan
            1.06f,          //4  485  ""
            1.10f,          //5  480  ""
            1.18f,          //6  465  455-475 blue
            1.20f,          //7  440  400-170
            1.15f,          //8  400  390-410 violet, magenta
            1.10f,          //9  380 magenta
            (1.05f+0.3f)/2.f,//10 <380 & >650
            0.42f,           //11 650 600-700 red
            0.89f,          //12 600 590-610 red/orange
            0.91f,          //13 585 580-595 orange/yrellow
            0.92f,          //14 580 575-585 yellow
            0.93f,          //15 575 565-580 yellow/lime
            0.97f,          //16 565 555-575 lime/green
            1.02f,          //17 545 525-565 green
            1.05f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Kodak TMAX P3200"
    },
    //===============================
    { //TRI-X
         {//sph tri x mk2
            1.17f,          //1  505   495-515 bluegreen
            1.18f,          //2  495   very narrow
            1.20f,          //3  490    "", cyan
            1.24f,          //4  485  ""
            1.27f,          //5  480  ""
            1.32f,          //6  465  455-475 blue
            1.49f,          //7  440  400-170
            1.55f,          //8  400  390-410 violet, magenta
            1.58f,          //9  380 magenta
            (1.5f+0.5f)/2.f,//10 <380 & >650
            0.75f,          //11 650 600-700 red
            1.38f,          //12 600 590-610 red/orange
            1.39f,          //13 585 580-595 orange/yrellow
            1.41f,          //14 580 575-585 yellow
            1.42f,          //15 575 565-580 yellow/lime
            1.42f,          //16 565 555-575 lime/green
            1.37f,          //17 545 525-565 green
            1.27f           //18 520 500-545
        },
        {-0.0000f,0.0000f},  //A (dark, bright), green/red
        {-0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Kodak Tri-X 400 New"
    },
    //===============================
    { //TRI-X
         {//sph tri x mk2
            0.85f,          //1  505   495-515 bluegreen
            0.87f,          //2  495   very narrow
            0.89f,          //3  490    "", cyan
            0.94f,          //4  485  ""
            1.00f,          //5  480  ""
            1.12f,          //6  465  455-475 blue
            1.25f,          //7  440  400-170
            1.37f,          //8  400  390-410 violet, magenta
            1.40f,          //9  380 magenta
            (1.4f+0.6f)/2.f,//10 <380 & >650
            0.7f,          //11 650 600-700 red
            0.94f,          //12 600 590-610 red/orange
            1.00f,          //13 585 580-595 orange/yrellow
            1.02f,          //14 580 575-585 yellow
            1.03f,          //15 575 565-580 yellow/lime
            1.00f,          //16 565 555-575 lime/green
            0.95f,          //17 545 525-565 green
            0.89f           //18 520 500-545
        },
        {-0.0000f,0.0000f},  //A (dark, bright), green/red
        {-0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Kodak Tri-X 400 Old"
    },
    //===============================
    { 
         {
            0.50f,          //1  505   495-515 bluegreen
            0.50f,          //2  495   very narrow
            0.50f,          //3  490    "", cyan
            0.51f,          //4  485  ""
            0.52f,          //5  480  ""
            0.57f,          //6  465  455-475 blue
            0.78f,          //7  440  400-170
            0.97f,          //8  400  390-410 violet, magenta
            1.05f,          //9  380 magenta
            (1.1f+0.7f)/2.f,//10 <380 & >650
            0.84f,          //11 650 600-700 red
            0.65f,          //12 600 590-610 red/orange
            0.59f,          //13 585 580-595 orange/yrellow
            0.57f,          //14 580 575-585 yellow
            0.56f,          //15 575 565-580 yellow/lime
            0.55f,          //16 565 555-575 lime/green
            0.57f,          //17 545 525-565 green
            0.52f           //18 520 500-545
        },
        {-0.0000f,0.0000f},  //A (dark, bright), green/red
        {-0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Kodak Technical Pan"
    },
    //===============================
    { 
         {
            0.60f,          //1  505   495-515 bluegreen
            0.53f,          //2  495   very narrow
            0.50f,          //3  490    "", cyan
            0.53f,          //4  485  ""
            0.60f,          //5  480  ""
            0.79f,          //6  465  455-475 blue
            0.96f,          //7  440  400-170
            1.22f,          //8  400  390-410 violet, magenta
            1.35f,          //9  380 magenta
            (1.4f+0.5)/2.f,//10 <380 & >650
            0.57f,          //11 650 600-700 red
            0.87f,          //12 600 590-610 red/orange
            1.12f,          //13 585 580-595 orange/yrellow
            1.26f,          //14 580 575-585 yellow
            1.34f,          //15 575 565-580 yellow/lime
            1.20f,          //16 565 555-575 lime/green
            0.88f,          //17 545 525-565 green
            0.77f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Agfapan APX 25"
    },
    //===============================
    { 
         {
            0.57f,          //1  505   495-515 bluegreen
            0.47f,          //2  495   very narrow
            0.49f,          //3  490    "", cyan
            0.52f,          //4  485  ""
            0.53f,          //5  480  ""
            0.71f,          //6  465  455-475 blue
            0.82f,          //7  440  400-170
            1.06f,          //8  400  390-410 violet, magenta
            1.15f,          //9  380 magenta
            (1.1f+0.1f)/2.f,//10 <380 & >650
            0.30f,          //11 650 600-700 red
            0.58f,          //12 600 590-610 red/orange
            0.61f,          //13 585 580-595 orange/yrellow
            0.71f,          //14 580 575-585 yellow
            0.89f,          //15 575 565-580 yellow/lime
            1.03f,          //16 565 555-575 lime/green
            0.83f,          //17 545 525-565 green
            0.68f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Agfapan APX 100/Rollei Retro 100"
    },
    //===============================
    { 
         {
            0.91f,          //1  505   495-515 bluegreen
            0.82f,          //2  495   very narrow
            0.81f,          //3  490    "", cyan
            0.79f,          //4  485  ""
            0.78f,          //5  480  ""
            0.85f,          //6  465  455-475 blue
            0.99f,          //7  440  400-170
            1.46f,          //8  400  390-410 violet, magenta
            1.66f,          //9  380 magenta
            (1.6f+0.3f)/2.f,//10 <380 & >650
            0.59f,          //11 650 600-700 red
            0.97f,          //12 600 590-610 red/orange
            1.02f,          //13 585 580-595 orange/yrellow
            1.04f,          //14 580 575-585 yellow
            1.06f,          //15 575 565-580 yellow/lime
            1.08f,          //16 565 555-575 lime/green
            1.09f,          //17 545 525-565 green
            1.00f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Agfapan APX 400/Rollei Retro 400"
    },
    //===============================
    { 
         {
            0.82f,          //1  505   495-515 bluegreen
            0.80f,          //2  495   very narrow
            0.78f,          //3  490    "", cyan
            0.77f,          //4  485  ""
            0.76f,          //5  480  ""
            0.80f,          //6  465  455-475 blue
            0.91f,          //7  440  400-170
            1.21f,          //8  400  390-410 violet, magenta
            1.45f,          //9  380 magenta
            (1.3f+0.00f)/2.f,//10 <380 & >650
            0.005f,         //11 650 600-700 red
            0.02f,          //12 600 590-610 red/orange
            0.28f,          //13 585 580-595 orange/yrellow
            0.62f,          //14 580 575-585 yellow
            0.88f,          //15 575 565-580 yellow/lime
            1.03f,          //16 565 555-575 lime/green
            1.00f,          //17 545 525-565 green
            0.84f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Agfa Ortho 25" //very rough, just by eye
    },
    //===============================
    { 
         {
            0.90f,          //1  505   495-515 bluegreen
            0.85f,          //2  495   very narrow
            0.84f,          //3  490    "", cyan
            0.83f,          //4  485  ""
            0.83f,          //5  480  ""
            0.86f,          //6  465  455-475 blue
            0.76f,          //7  440  400-170
            0.50f,          //8  400  390-410 violet, magenta
            0.40f,          //9  380 magenta
            (0.4f+0.40f)/2.f,//10 <380 & >650
            1.30f,         //11 650 600-700 red
            1.40f,          //12 600 590-610 red/orange
            1.36f,          //13 585 580-595 orange/yrellow
            1.38f,          //14 580 575-585 yellow
            1.45f,          //15 575 565-580 yellow/lime
            1.24f,          //16 565 555-575 lime/green
            1.23f,          //17 545 525-565 green
            1.05f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Foma Fomapan 100 Classic" 
    },
    //===============================
    { 
         {
            1.05f,          //1  505   495-515 bluegreen
            0.95f,          //2  495   very narrow
            0.90f,          //3  490    "", cyan
            0.85f,          //4  485  ""
            0.80f,          //5  480  ""
            0.85f,          //6  465  455-475 blue
            0.80f,          //7  440  400-170
            0.50f,          //8  400  390-410 violet, magenta
            0.40f,          //9  380 magenta
            (0.6f+1.55f)/2.f,//10 <380 & >650
            1.6f,         //11 650 600-700 red
            1.34f,          //12 600 590-610 red/orange
            1.28f,          //13 585 580-595 orange/yrellow
            1.32f,          //14 580 575-585 yellow
            1.35f,          //15 575 565-580 yellow/lime
            1.21f,          //16 565 555-575 lime/green
            1.15f,          //17 545 525-565 green
            1.20f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Foma Fomapan 400 Action" 
    },
    //===============================
    { 
         {
            0.80f,          //1  505   495-515 bluegreen
            0.74f,          //2  495   very narrow
            0.76f,          //3  490    "", cyan
            0.78f,          //4  485  ""
            0.81f,          //5  480  ""
            0.93f,          //6  465  455-475 blue
            1.04f,          //7  440  400-170
            1.05f,          //8  400  390-410 violet, magenta
            1.07f,          //9  380 magenta
            (1.1f+0.20f)/2.f,//10 <380 & >650
            0.55f,         //11 650 600-700 red
            1.07f,          //12 600 590-610 red/orange
            1.14f,          //13 585 580-595 orange/yrellow
            1.17f,          //14 580 575-585 yellow
            1.16f,          //15 575 565-580 yellow/lime
            1.12f,          //16 565 555-575 lime/green
            1.07f,          //17 545 525-565 green
            0.91f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Fujifilm Neopan 100 Acros" 
    },
    //===============================
    { 
         {
            0.86f,          //1  505   495-515 bluegreen
            0.87f,          //2  495   very narrow
            0.88f,          //3  490    "", cyan
            0.90f,          //4  485  ""
            0.94f,          //5  480  ""
            1.07f,          //6  465  455-475 blue
            1.12f,          //7  440  400-170
            1.10f,          //8  400  390-410 violet, magenta
            0.92f,          //9  380 magenta
            (0.82f+0.10f)/2.f,//10 <380 & >650
            0.55f,         //11 650 600-700 red
            0.99f,          //12 600 590-610 red/orange
            1.03f,          //13 585 580-595 orange/yrellow
            1.02f,          //14 580 575-585 yellow
            1.01f,          //15 575 565-580 yellow/lime
            0.97f,          //16 565 555-575 lime/green
            0.93f,          //17 545 525-565 green
            0.90f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Fujifilm Neopan 400" 
    },
    //===============================
    { 
         {
            0.81f,          //1  505   495-515 bluegreen
            0.82f,          //2  495   very narrow
            0.83f,          //3  490    "", cyan
            0.88f,          //4  485  ""
            0.93f,          //5  480  ""
            1.14f,          //6  465  455-475 blue
            1.26f,          //7  440  400-170
            1.30f,          //8  400  390-410 violet, magenta
            1.25f,          //9  380 magenta
            (1.2f+0.10f)/2.f,//10 <380 & >650
            0.57f,         //11 650 600-700 red
            1.12f,          //12 600 590-610 red/orange
            1.17f,          //13 585 580-595 orange/yrellow
            1.18f,          //14 580 575-585 yellow
            1.18f,          //15 575 565-580 yellow/lime
            1.14f,          //16 565 555-575 lime/green
            0.96f,          //17 545 525-565 green
            0.92f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Fujifilm Neopan 1600" 
    },
    //===============================
    { 
         {
            0.81f,          //1  505   495-515 bluegreen
            0.81f,          //2  495   very narrow
            0.82f,          //3  490    "", cyan
            0.83f,          //4  485  ""
            0.84f,          //5  480  ""
            0.85f,          //6  465  455-475 blue
            0.83f,          //7  440  400-170
            0.7f,          //8  400  390-410 violet, magenta
            0.4f,          //9  380 magenta
            (0.2f+0.2f)/2.f,//10 <380 & >650
            0.45f,           //11 650 600-700 red
            0.93f,          //12 600 590-610 red/orange
            0.98f,          //13 585 580-595 orange/yrellow
            1.00f,          //14 580 575-585 yellow
            1.01f,          //15 575 565-580 yellow/lime
            1.00f,          //16 565 555-575 lime/green
            0.94f,          //17 545 525-565 green
            0.85f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Ilford Delta 100/HP5+"
    },
    //===============================
    { 
         {
            0.82f,          //1  505   495-515 bluegreen
            0.84f,          //2  495   very narrow
            0.85f,          //3  490    "", cyan
            0.87f,          //4  485  ""
            0.88f,          //5  480  ""
            0.91f,          //6  465  455-475 blue
            0.89f,          //7  440  400-170
            0.77f,          //8  400  390-410 violet, magenta
            0.45f,          //9  380 magenta
            (0.3f+0.7f)/2.f,//10 <380 & >650
            0.99f,           //11 650 600-700 red
            0.98f,          //12 600 590-610 red/orange
            1.00f,          //13 585 580-595 orange/yrellow
            1.01f,          //14 580 575-585 yellow
            1.00f,          //15 575 565-580 yellow/lime
            0.99f,          //16 565 555-575 lime/green
            0.95f,          //17 545 525-565 green
            0.84f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Ilford Delta 400/3200"
    },
    //===============================
    { 
         {
            0.70f,          //1  505   495-515 bluegreen
            0.71f,          //2  495   very narrow
            0.72f,          //3  490    "", cyan
            0.73f,          //4  485  ""
            0.73f,          //5  480  ""
            0.73f,          //6  465  455-475 blue
            0.64f,          //7  440  400-170
            0.47f,          //8  400  390-410 violet, magenta
            0.2f,          //9  380 magenta
            (0.15f+0.2f)/2.f,//10 <380 & >650
            0.6f,           //11 650 600-700 red
            1.01f,          //12 600 590-610 red/orange
            0.96f,          //13 585 580-595 orange/yrellow
            0.94f,          //14 580 575-585 yellow
            0.91f,          //15 575 565-580 yellow/lime
            0.86f,          //16 565 555-575 lime/green
            0.79f,          //17 545 525-565 green
            0.75f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Ilford FP4+"
    },
    //===============================
    { 
         {
            0.92f,          //1  505   495-515 bluegreen
            1.00f,          //2  495   very narrow
            1.06f,          //3  490    "", cyan
            1.15f,          //4  485  ""
            1.20f,          //5  480  ""
            1.31f,          //6  465  455-475 blue
            1.28f,          //7  440  400-170
            1.10f,          //8  400  390-410 violet, magenta
            0.9f,          //9  380 magenta
            (0.7f+0.4f)/2.f,//10 <380 & >650
            0.5f,           //11 650 600-700 red
            1.05f,          //12 600 590-610 red/orange
            1.07f,          //13 585 580-595 orange/yrellow
            1.08f,          //14 580 575-585 yellow
            1.11f,          //15 575 565-580 yellow/lime
            1.13f,          //16 565 555-575 lime/green
            1.02f,          //17 545 525-565 green
            0.97f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Ilford XP2 Super"
    },
    //===============================
    { 
         {
            0.68f,          //1  505   495-515 bluegreen
            0.66f,          //2  495   very narrow
            0.68f,          //3  490    "", cyan
            0.70f,          //4  485  ""
            0.76f,          //5  480  ""
            0.83f,          //6  465  455-475 blue
            0.82f,          //7  440  400-170
            0.4f,          //8  400  390-410 violet, magenta
            0.1f,          //9  380 magenta
            (0.1f+0.1f)/2.f,//10 <380 & >650
            0.5f,           //11 650 600-700 red
            0.93f,          //12 600 590-610 red/orange
            0.92f,          //13 585 580-595 orange/yrellow
            0.91f,          //14 580 575-585 yellow
            0.90f,          //15 575 565-580 yellow/lime
            0.88f,          //16 565 555-575 lime/green
            0.82f,          //17 545 525-565 green
            0.82f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Ilford PanF+"
    },
    //===============================
    { 
         {
            0.59f,          //1  505   495-515 bluegreen
            0.57f,          //2  495   very narrow
            0.56f,          //3  490    "", cyan
            0.55f,          //4  485  ""
            0.56f,          //5  480  ""
            0.66f,          //6  465  455-475 blue
            0.75f,          //7  440  400-170
            0.60f,          //8  400  390-410 violet, magenta
            0.36f,          //9  380 magenta
            (0.30f+0.15f)/2.f,//10 <380 & >650
            0.45f,           //11 650 600-700 red
            0.82f,          //12 600 590-610 red/orange
            0.98f,          //13 585 580-595 orange/yrellow
            0.99f,          //14 580 575-585 yellow
            0.96f,          //15 575 565-580 yellow/lime
            0.90f,          //16 565 555-575 lime/green
            0.85f,          //17 545 525-565 green
            0.70f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Efke/Adox CHS 25/50"
    },
    //===============================
    { 
         {
            0.61f,          //1  505   495-515 bluegreen
            0.61f,          //2  495   very narrow
            0.64f,          //3  490    "", cyan
            0.67f,          //4  485  ""
            0.70f,          //5  480  ""
            0.80f,          //6  465  455-475 blue
            0.84f,          //7  440  400-170
            0.63f,          //8  400  390-410 violet, magenta
            0.34f,          //9  380 magenta
            (0.15f+0.6f)/2.f,//10 <380 & >650
            0.84f,           //11 650 600-700 red
            0.79f,          //12 600 590-610 red/orange
            0.88f,          //13 585 580-595 orange/yrellow
            0.94f,          //14 580 575-585 yellow
            0.98f,          //15 575 565-580 yellow/lime
            0.95f,          //16 565 555-575 lime/green
            0.84f,          //17 545 525-565 green
            0.67f           //18 520 500-545
        },
        { 0.0000f, 0.0000f}, //A (dark, bright), green/red
        { 0.0000f, 0.0000f}, //B (dark, bright), blue/yellow
        "Efke/Adox CHS 100"
    },
    //===============================
    { 
         {
            1.14f,          //1  505   495-515 bluegreen
            1.12f,          //2  495   very narrow
            1.18f,          //3  490    "", cyan
            1.23f,          //4  485  ""
            1.29f,          //5  480  ""
            1.41f,          //6  465  455-475 blue
            1.50f,          //7  440  400-170
            1.48f,          //8  400  390-410 violet, magenta
            1.30f,          //9  380 magenta
            (1.1f+0.1f)/2.f,//10 <380 & >650
            0.59f,           //11 650 600-700 red
            1.27f,          //12 600 590-610 red/orange
            1.30f,          //13 585 580-595 orange/yrellow
            1.33f,          //14 580 575-585 yellow
            1.37f,          //15 575 565-580 yellow/lime
            1.35f,          //16 565 555-575 lime/green
            1.33f,          //17 545 525-565 green
            1.18f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "ORWO N 74 plus"
    },
    //===============================
    { 
         {
            0.81f,          //1  505   495-515 bluegreen
            0.79f,          //2  495   very narrow
            0.77f,          //3  490    "", cyan
            0.80f,          //4  485  ""
            0.82f,          //5  480  ""
            1.10f,          //6  465  455-475 blue
            1.25f,          //7  440  400-170
            1.27f,          //8  400  390-410 violet, magenta
            1.20f,          //9  380 magenta
            (1.1f+0.1f)/2.f,//10 <380 & >650
            0.47f,           //11 650 600-700 red
            1.13f,          //12 600 590-610 red/orange
            1.19f,          //13 585 580-595 orange/yrellow
            1.20f,          //14 580 575-585 yellow
            1.22f,          //15 575 565-580 yellow/lime
            1.23f,          //16 565 555-575 lime/green
            1.17f,          //17 545 525-565 green
            1.00f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "ORWO UN 54"
    },
    //===============================
    { 
         {
            1.25f,          //1  505   495-515 bluegreen
            1.23f,          //2  495   very narrow
            1.23f,          //3  490    "", cyan
            1.24f,          //4  485  ""
            1.25f,          //5  480  ""
            1.30f,          //6  465  455-475 blue
            1.33f,          //7  440  400-170
            1.24f,          //8  400  390-410 violet, magenta
            1.05f,          //9  380 magenta
            (0.90f+1.5f)/2.f,//10 <380 & >650
            1.42f,           //11 650 600-700 red
            1.61f,          //12 600 590-610 red/orange
            1.57f,          //13 585 580-595 orange/yrellow
            1.54f,          //14 580 575-585 yellow
            1.51f,          //15 575 565-580 yellow/lime
            1.48f,          //16 565 555-575 lime/green
            1.45f,          //17 545 525-565 green
            1.37f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Rollei Retro 80S"
    },
    //===============================
    { 
         {
            0.97f,          //1  505   495-515 bluegreen
            1.00f,          //2  495   very narrow
            1.02f,          //3  490    "", cyan
            1.03f,          //4  485  ""
            1.03f,          //5  480  ""
            1.02f,          //6  465  455-475 blue
            0.97f,          //7  440  400-170
            0.70f,          //8  400  390-410 violet, magenta
            0.55f,          //9  380 magenta
            (0.42f+0.40f)/2.f,//10 <380 & >650
            0.42f,           //11 650 600-700 red
            0.81f,          //12 600 590-610 red/orange
            1.05f,          //13 585 580-595 orange/yrellow
            1.04f,          //14 580 575-585 yellow
            1.03f,          //15 575 565-580 yellow/lime
            1.01f,          //16 565 555-575 lime/green
            0.98f,          //17 545 525-565 green
            0.92f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Rollei Retro 100 Tonal"
    },
    //===============================
    { 
         {
            0.94f,          //1  505   495-515 bluegreen
            0.94f,          //2  495   very narrow
            0.97f,          //3  490    "", cyan
            0.99f,          //4  485  ""
            1.00f,          //5  480  ""
            1.11f,          //6  465  455-475 blue
            1.27f,          //7  440  400-170
            1.33f,          //8  400  390-410 violet, magenta
            1.35f,          //9  380 magenta
            (1.35f+1.1f)/2.f,//10 <380 & >650
            1.27f,           //11 650 600-700 red
            1.21f,          //12 600 590-610 red/orange
            1.25f,          //13 585 580-595 orange/yrellow
            1.23f,          //14 580 575-585 yellow
            1.19f,          //15 575 565-580 yellow/lime
            1.15f,          //16 565 555-575 lime/green
            1.09f,          //17 545 525-565 green
            1.00f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Rollei Retro 400S"
    },
    //===============================
    { 
         {
            1.29f,          //1  505   495-515 bluegreen
            1.30f,          //2  495   very narrow
            1.32f,          //3  490    "", cyan
            1.37f,          //4  485  ""
            1.45f,          //5  480  ""
            1.70f,          //6  465  455-475 blue
            1.80f,          //7  440  400-170
            1.89f,          //8  400  390-410 violet, magenta
            1.85f,          //9  380 magenta
            (1.8f+1.0f)/2.f,//10 <380 & >650
            1.30f,           //11 650 600-700 red
            1.40f,          //12 600 590-610 red/orange
            1.42f,          //13 585 580-595 orange/yrellow
            1.43f,          //14 580 575-585 yellow
            1.49f,          //15 575 565-580 yellow/lime
            1.46f,          //16 565 555-575 lime/green
            1.43f,          //17 545 525-565 green
            1.36f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Rollei R^3"
    },
    //===============================
    { 
         {
            1.30f,          //1  505   495-515 bluegreen
            1.30f,          //2  495   very narrow
            1.32f,          //3  490    "", cyan
            1.33f,          //4  485  ""
            1.34f,          //5  480  ""
            1.36f,          //6  465  455-475 blue
            1.40f,          //7  440  400-170
            1.28f,          //8  400  390-410 violet, magenta
            1.20f,          //9  380 magenta
            (0.7f+1.5f)/2.f,//10 <380 & >650
            1.60f,           //11 650 600-700 red
            1.65f,          //12 600 590-610 red/orange
            1.70f,          //13 585 580-595 orange/yrellow
            1.71f,          //14 580 575-585 yellow
            1.69f,          //15 575 565-580 yellow/lime
            1.60f,          //16 565 555-575 lime/green
            1.52f,          //17 545 525-565 green
            1.37f           //18 520 500-545
        },
        {0.0000f,0.0000f},  //A (dark, bright), green/red
        {0.0000f,0.0000f}, //B (dark, bright), blue/yellow
        "Rollei Superpan 200"
    }/*,
    //===============================
    { 
        {
            0.7f+0.8f,          //1  505   495-515 bluegreen
            0.8f+0.7f,          //2  495   very narrow
            0.85f+0.7f,          //3  490    "", cyan
            1.2f+0.5f,          //4  485  ""
            1.25f+0.3f,          //5  480  ""
            1.70f,          //6  465  455-475 blue
            1.80f,          //7  440  400-170
            1.30f,          //8  400  390-410 violet, magenta
            1.00f,          //9  380 magenta
            (0.7f+1.2f)/2.f,//10 <380 & >650
            1.82f,           //11 650 600-700 red
            1.30f,          //12 600 590-610 red/orange
            0.70f+0.7f,          //13 585 580-595 orange/yrellow
            0.90f+0.55f,          //14 580 575-585 yellow
            1.20f+0.45f,          //15 575 565-580 yellow/lime
            1.55f+0.1f,          //16 565 555-575 lime/green
            1.70f,          //17 545 525-565 green
            1.10f+0.4f           //18 520 500-545
        },
        {-0.0250f,0.0200f},  //A (dark, bright), green/red
        {-0.0250f,0.0100f}, //B (dark, bright), blue/yellow
        "DBG: Kodak Kodachrome 64 - qick'n'dirty, spectral sensitivity"
    },
    //===============================
    { 
         {
            1.2f,          //1  505   495-515 bluegreen
            0.95f,          //2  495   very narrow
            0.75f,          //3  490    "", cyan
            0.65f,          //4  485  ""
            0.55f,          //5  480  ""
            0.65f,          //6  465  455-475 blue
            0.70f,          //7  440  400-170
            0.85f,          //8  400  390-410 violet, magenta
            0.95f,          //9  380 magenta
            (0.9f+0.9f)/2.f,//10 <380 & >650
            0.7f,           //11 650 600-700 red
            0.8f,          //12 600 590-610 red/orange
            0.9f,          //13 585 580-595 orange/yrellow
            0.85f,          //14 580 575-585 yellow
            0.95f,          //15 575 565-580 yellow/lime
            1.05f,          //16 565 555-575 lime/green
            1.35f,          //17 545 525-565 green
            1.3f           //18 520 500-545
        },
        { 0.0100f, 0.0300f},  //A (dark, bright), green/red
        {-0.0200f, 0.0200f}, //B (dark, bright), blue/yellow
        "DBG: Imaginary Portrait SPH1"
    }*/
//===============================  


}; //end films===========================================================================

#endif
