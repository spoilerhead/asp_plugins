/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010-2011
 * licensed: GPL v2+, other licenses  available on request
 */
 
#ifndef SPHTILECACHE_H
#define SPHTILECACHE_H

#include "PluginPipeSettings.h"

using namespace std;

#define USETILECACHE    //undef to disable caching
//#define USEVECTOR     //tested to be slower
#define USEQLIST        //st::list has a bug, too. damnit
//make all inline!

#ifdef USETILECACHE
#ifdef USEVECTOR
#include <vector> //can't use QList, as it crashes with symbol lookup errors (qt4.7 vs qt4.6) :-/
#elif defined (USEQLIST)
#include <QList>
#else
#include <list>
#endif
#endif




// One cache entry
typedef struct {
    QRect rect;
    int x;
    int y;
    int rowWords;
    uint16* data16;
} TCData;

//Pixel caching class

class sphPixelCache
{
    public:
	    sphPixelCache(QRect &reqRect, const PipeSettings  &settings, const int srcStep) : m_srcRect(reqRect), m_settings(settings), m_srcStep(srcStep) { 
	        m_pad = m_settings.tilePad();
	        m_srcWidth  = m_srcRect.width();
	        m_srcHeight = m_srcRect.height();
	        m_srcLeft   = m_srcRect.left();
	        m_srcTop    = m_srcRect.top();
	        m_srcRight  = m_srcRect.right();
	        m_srcBottom = m_srcRect.bottom();
	        
	        if((m_srcWidth == 0) || (m_srcHeight == 0) ) {
	            qDebug()<<"WARNING: sphPixelCache configured with invalid ROI";
	        }
	    }
	    
	    uint16* getPixel( int wx,  int wy) ;
	    
	
    protected:
        QRect &m_srcRect;
        
        int m_srcLeft, m_srcTop,m_srcRight,m_srcBottom;
        
        PipeSettings const &m_settings;
        int m_pad;
        int m_srcStep;
        int m_srcWidth;
        int m_srcHeight;
        
        
        #ifdef USETILECACHE
        #ifdef USEVECTOR
        std::vector<TCData> m_cache;
        #elif defined (USEQLIST)
        QList<TCData> m_cache;
        #else
        std::list<TCData> m_cache;
        #endif
        #endif
        
};


//faster version of contains
static inline bool isIn(const int x, const int y, const QRect &rect) {
    return     (x >= rect.left()) && (x <= rect.right()) 
            && (y >= rect.top())  && (y <= rect.bottom());
}

static inline int mirror(const int x, const int lo, const int hi) {
    if(hi == lo) return lo;
    const int mirror = (x-lo)/(hi-lo)+ ( (x<0)?-1:0);
    return (mirror % 2)?/*odd*/( lo+hi-(x-(mirror*(hi-lo))))   : /*even*/ (x-(mirror*(hi-lo)));
}

//get the tile for a certainl world coordinate and extract the adress of that pixel
inline uint16* sphPixelCache::getPixel( int wx,  int wy)  {
    uint16 *pIn = NULL;
    //Pixel is in our prerequisites image
    if(!isIn(wx,wy,m_srcRect)) {      //we haven't requested this pixel, so probably we're close to a border
        if((m_srcWidth <= 0) || (m_srcHeight <= 0) ) return NULL;  //invalid srcROI
        //just repeat the last valid pixel
        //TODO: more intelligent mirroring
        //wx = max(min(wx,m_srcRect.right()) ,m_srcRect.left());
        //wy = max(min(wy,m_srcRect.bottom()),m_srcRect.top());
        //qDebug()<<"prev:  "<<wx<<" "<<wy;
        
        wx = mirror(wx,m_srcLeft,m_srcRight);
        wy = mirror(wy,m_srcTop ,m_srcBottom);
        //qDebug()<<"after: "<<wx<<" "<<wy<<"  Rect: l:"<< m_srcRect.left() <<" r:"<<m_srcRect.right() << " / t:"<< m_srcRect.top()<< " b:"<<m_srcRect.bottom();
        if(!isIn(wx,wy,m_srcRect)) {
            qDebug()<<"sphLayerFilter: ERROR: Invalid coordinate computed";
            qDebug()<<"after: "<<wx<<" "<<wy<<"  Rect: l:"<< m_srcRect.left() <<" r:"<<m_srcRect.right() << " / t:"<< m_srcRect.top()<< " b:"<<m_srcRect.bottom();
        }
    }
    
    
    //always true after above check if(m_srcRect.contains(wx,wy)) {       //check if we requested this pixel in prerequisites
        //TODO: do some actual caching here :D
        /*PluginTile *curTile = (dynamic_cast<PluginTile*>(m_settings.tileAtXY(wx,wy, m_srcStep)));
        int tx = wx - curTile->x();
        int ty = wy - curTile->y();
        pIn = curTile->image()->data16() + (curTile->image()->rowWords() * (m_pad+ty))   +   (m_pad+tx);*/
    //}
    
    
    
    TCData curTile;
    bool found = false;
    #ifdef USETILECACHE
    #ifdef USEVECTOR
    //check if we got that tile in our cache already
    for(int i = 0;i<m_cache.size();i++) {
        if(isIn(wx,wy,m_cache[i].rect))
        {
            curTile = m_cache[i];
            found = true;
            break;
        }
    }
    #else
    for(int i = 0;i<m_cache.size();i++) {
        if(isIn(wx,wy,m_cache.front().rect))
        {
            curTile = m_cache.front();
            found = true;
            break;
        }
        TCData moved = m_cache.front();
        m_cache.pop_front();
        m_cache.push_back(moved);
    }
    #endif
    #endif
    
    if(!found) {//if we haven't cached it yet, add it to the cache
        PluginTile *pTile = (dynamic_cast<PluginTile*>(m_settings.tileAtXY(wx,wy, m_srcStep)));
        if( pTile == NULL ) return NULL;    //bibble 5.2.3++ returns NULL here
        curTile.x        = pTile->x();
        curTile.y        = pTile->y();
        curTile.rect     = QRect(QPoint(curTile.x,curTile.y), m_settings.tileSize());
        curTile.rowWords = pTile->image()->rowWords();
        curTile.data16   = pTile->image()->data16();
        #ifdef USETILECACHE
        #ifdef USEVECTOR
        m_cache.push_back(curTile);
        #else
        m_cache.push_front(curTile);
        #endif
        #endif
    }

    //compute coordinates within the tile and return the address of the pixel data
    int tx = wx - curTile.x;
    int ty = wy - curTile.y;
    pIn = curTile.data16 + (curTile.rowWords* (m_pad+ty))   +   (m_pad+tx);

    return pIn;
}


#endif //SPHTILECACHE_H

