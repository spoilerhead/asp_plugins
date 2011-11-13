#ifndef EAW_H
#define EAW_H
void EAW_process (float *image,
         int    width,
         int    height,
         float  alpha,
         int    maxband,
         int    inband,
         int    mode);
         
#define EAW_MODE_LINEAR 0
#define EAW_MODE_SUPRESS LOWEST 1
#define EAW_MODE_ONLY_LOWEST 2         

#endif //EAW_H
