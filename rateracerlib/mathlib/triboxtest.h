/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap();                       */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*   2001-06-18: Modified by Göran Wallgren, using gm.h */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/

#include "Vec3.h"

int planeBoxOverlap(Vec3 normal, float d, Vec3 maxbox);
int triBoxOverlap(Vec3 boxcenter, Vec3 boxhalfsize, Vec3 triV0, Vec3 triV1, Vec3 triV2);
