#ifndef _BLEND_MODES_H
#define _BLEND_MODES_H

/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 * or LGPL v2+
 * your choice
 */
 
 #include <colorspace.h>


//http://www.linuxtopia.org/online_books/graphics_tools/gimp_advanced_guide/gimp_guide_node55_002.html
#define MULTIPLY(f,b) ((f) * (b))
#define SCREEN(fo,b) ( 1.0f-(1.0f-(fo)) * (1.0f- (b)) )
#define OVERLAY(fo,b) ( (b) * SCREEN((fo),(b)) + (1.0f - (b)) * MULTIPLY((fo),(b)) )

#define NOOP(fo,b) ( (fo) )

//http://docs.gimp.org/en/gimp-concepts-layer-modes.html
#define SOFTLIGHT(fo,b) ( ((1.0f-(b))*(fo)*(b)) + ((b)*SCREEN((fo),(b))) )
#define HARDLIGHT(fo,b) ((fo>0.5f)?  (1.0f- (1.0f-2*((fo)-0.5f))*(1.0f-(b)))  :  (2.0f*(fo)*(b))   )



#define SHADOW_RECOVERY(img,q) SOFTLIGHT(1.0f-(img),(img))*(q)+(1.0f-(q))*(img);

#define BLEND(fo,b,o) ((fo)*(o)+(b)*(1.0f-(o)))

#define GAMMA(y,g) (pow((y),(g)))
#define DEGAMMA(y,g) (pow((y),1.0f/(g)))

#define MIX(fo,b) ((b)+((fo)-0.5f))


//16 bit routines

#define MULTIPLY16(f,b) ((((unsigned int)f) * (b))>>16)
#define SCREEN16(fo,b) ( WHITEi-MULTIPLY16((WHITEi-(fo)),(WHITEi- (b))) )
#define OVERLAY16(fo,b) ( MULTIPLY16((b) , SCREEN16((fo),(b))) + MULTIPLY16((WHITEi - (b)) , MULTIPLY16((fo),(b))) )
#define BLEND16(fo,b,o) (MULTIPLY16((fo),(o)) + MULTIPLY16((b),(0xFFFF-(o))))

#define SOFTLIGHT16(fo,b) ( MULTIPLY16((WHITEi-(b)),MULTIPLY16((fo),(b))) + MULTIPLY16((b),SCREEN16((fo),(b))) )
#define HARDLIGHT16(fo,b) ((fo>0x8000)?  (WHITEi- MULTIPLY16((WHITEi-2*((fo)-0x8000)),(WHITEi-(b))))  :  (2*MULTIPLY16((fo),(b)))   )

#define SHADOW_RECOVERY16(img,q) (clampI(MULTIPLY16(SOFTLIGHT16(WHITEi-(img),(img)),(q))+MULTIPLY16((WHITEi-(q)),(img))));

#define MIX16(fo,b) (  clampI(((unsigned int)b)+(fo)-0x8000)  )

#endif
