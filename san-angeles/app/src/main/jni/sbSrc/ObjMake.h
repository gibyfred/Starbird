
/*=====================================

 file: objmake.h

 Owner: Hui Tin Yuen, Fred

 Description:

 - functions making objects into ogl display lists
 - with some reusable global variables like color vectors


 Hui Tin Yuen
 92057645
 cs_fredh@ug.cs.ust.hk

 comp341 project

=====================================*/


//--------------------
// local functions - could be exported if necessary

/*
extern void vMakeLights();
extern void vMakePlane();

extern void vMakeDREAM();
*/
//extern void vMakeCube();
extern void vMakeRingPlane(float v1[3], float v2[3]);
extern void vMakeChannel();
extern void vMakeCube(unsigned short obj);
//extern void vMakeVPlane();
extern void vDrawObj(unsigned short id);

extern void vDrawPolygon(float b[4][3], float n[4][3]);
extern void vDrawPolygon2(float b[3][3]);

/*
extern float wall_mat[];
extern float dr_matl[];
extern float dr_matt[];
extern float dr_mate[];
extern float sha_mat[];
extern float stick_mat[];
extern float lm[];
extern float lm2[];
extern float e_mat1[];
extern float e_mat2[];
*/

//--------------------
//

#define MAT_COUNT		4		// A,D,S and SHINESS
typedef float CGMaterial [MAT_COUNT][4];

extern void vMakeAll(void);
extern const float fBlackVec[], fGreenVec[], fWhiteVec[],
             fYellowVec[], fRedVec[], fBlueVec[], fGrayVec[], fCyanVec[];

extern void vChangeMat(CGMaterial mat);
