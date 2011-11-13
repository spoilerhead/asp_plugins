#include "dePepper.h"
void dePepperF (float *fimg[3], const unsigned int width, const unsigned int height) {

  int col,row,i;
  int size = width * height;
// Remove Salt and Pepper Noise
 
      for(col = 1; col < width-1; col++) {
        for(row = 1; row < height-1; row++) {
          float cur = fimg[0][(row) *width+ (col)];

          float davg = fimg[0][(row-1) *width+ (col-1)]
                     + fimg[0][(row-1) *width+ (col+1)]
                     + fimg[0][(row+1) *width+ (col-1)]
                     + fimg[0][(row+1) *width+ (col+1)];    //Diagonal Average X
          davg /= 4.0f;
          float cavg = fimg[0][(row+1) *width+ (col)]
                     + fimg[0][(row-1) *width+ (col)]
                     + fimg[0][(row) *width+ (col-1)]
                     + fimg[0][(row) *width+ (col+1)];      //Cross average +
          cavg /= 4.0f;
          
          static const float scale = 1.07f;
          static const float invscale = 1/scale;
          float cratio = (cur+1.0f)/(cavg+1.0f);                          //Cross Ratio, is in the region 0.5..2
          float dratio = (cur+1.0f)/(davg+1.0f);                          //Diagonal Ratio
          
          if( ((cratio > scale) && (dratio > scale)) || ((cratio < invscale) && (dratio < invscale))) //only replace if both ratios are off, meaning the pixel is different than his neighborhoods
            fimg[1][(row) *width+ (col)] = (2.0f*cavg+davg)/3.0f; 
          else
            fimg[1][(row) *width+ (col)] = cur;     //else, keep pixel
        }
      } 
      //Copyback
      for(i = 0; i < size; i++) 
        fimg[0][i] = fimg[1][i];
   //----------------------------------------------------
}
