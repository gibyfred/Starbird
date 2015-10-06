#if 0

/*
 * Name      : void vCopyMatrix(Matrix mSource, Matrix mDestination)
 *
 * Parameters: Matrix mSource      - The source matrix.
 *             Matrix mDestination - The destination matrix.
 *
 * Returns   : void
 *
 * Purpose   : Copies matrix mSource to matrix mDestination.
 *             the result in mDestination.
 */
void vCopyMatrix(Matrix mSource, Matrix mDestination) {

  int i, j;

  for(i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
        mDestination[i][j] = mSource[i][j] ;
}

/*
 * Name      : void vRightMultiply(Matrix mMat1, Matrix mMat2)
 *
 * Parameters: Matrix mMat1 - The first and destination matrix.
 *             Matrix mMat2 - The second matrix.
 *
 * Returns   : void
 *
 * Purpose   : Right multiplies matrix mMat1 by matrix mMat2 and stores
 *             the result in mMat1.
 */
void vRightMultiply(Matrix mMat1, Matrix mMat2) {

   int    i, j;
   Matrix mMat3;

   for(i = 0; i < 4; i++)
      for(j = 0; j < 4; j++)
         mMat3[i][j] = mMat1[i][0] * mMat2[0][j] + mMat1[i][1] * mMat2[1][j] +
                       mMat1[i][2] * mMat2[2][j] + mMat1[i][3] * mMat2[3][j];
   for(i = 0; i < 4; i++)
      for(j = 0; j < 4; j++)
         mMat1[i][j] = mMat3[i][j] ;
}

/*
 * Name      : void vglTranslatef(float fTrans, char cAxis, Matrix mMat)
 *
 * Parameters: float  fAngle - The distance of translation.
 *             char   cAxis  - The axis of rotation.
 *             Matrix mMat   - The matrix to store the result in.
 *
 * Returns   : void
 *
 * Purpose   : Computes the translation along the given axis and stores
 *             the result in mMat.
 */
void vglTranslatef(float fTrans, char cAxis, Matrix mMat) {

   vCopyMatrix(mIdentity, mMat);
   switch(cAxis) {
      case 'x':
         mMat[3][0] = fTrans;
         break;

      case 'y':
         mMat[3][1] = fTrans;
         break;

      case 'z':
         mMat[3][2] = fTrans;
         break;
   }
}

/*
 * Purpose   : Toggles the direction in the direction vector on 
 *             and off.
 */
void vToggleDir(int nDir) {

   nCurrentDir ^= nDir;
}

/*
 * Name      : void vPerformTransfo(float fOldX, float fNewX,
 *                                  float fOldY, float fNewY)
 *
 * Parameters: float fOldX - The previous X value.
 *             float fNewX - The current X value.
 *             float fOldY - The previous Y value.
 *             float fNewY - The current Y value.
 *
 * Returns   : void
 *
 * Purpose   : Updates the various transformation matrices.
 */
void vPerformTransfo(float fOldX, float fNewX, float fOldY, float fNewY) {

   float  fRotVecX, fRotVecY, fRotVecZ;
   Matrix mNewMat;
  
   /*
    * Track ball rotations are being used.
    */
   if (nCurrentDir & DIR_Z) {
      vGenerateRotVec(fNewX, fNewY, &fRotVecX, &fRotVecY, &fRotVecZ);
      vRotateScene(fRotVecX, fRotVecY, fRotVecZ, mNewMat);
      vRightMultiply(mRotations, mNewMat);
   }

   /*
    * Pan translations are being used.
    */
   if (nCurrentDir & DIR_X) {
      vglTranslatef((fNewX - fOldX) / ((float)nWinWidth / SENS_PANX), 'x', 
                 mNewMat);
      vRightMultiply(mTranslations, mNewMat);
      vglTranslatef((fNewY - fOldY) / ((float)nWinHeight / SENS_PANY), 'y', 
                 mNewMat);
      vRightMultiply(mTranslations, mNewMat);
   }
   
   /*
    * Zoom translations are being used.
    */
   if (nCurrentDir & DIR_Y) {
      vglTranslatef(-(fNewY - fOldY) / ((float)nWinHeight / SENS_ZOOM), 'z', 
                 mNewMat);
      vRightMultiply(mTranslations, mNewMat);
   }
}

void vEventLoop(void (*scene)())
{

   short     sVal, sXCurrent, sYCurrent;
   long      lDevice;
   float     fDummyX, fDummyY, fDummyZ;
   Matrix    mNewMat;

   nCurrentDir = DIR_NONE;
   while (TRUE) {

      if (qtest()) {
         lDevice = qread(&sVal);


         switch (lDevice) {
			case INPUTCHANGE: 
				#ifdef DOFF
					printf(" %d, %d, %d\n", Win_ids[0], Win_ids[1], sVal);
				#endif
				if ( sVal != 0 )
					Input_win = sVal;
				else
					Input_win = -1;
				break;

            case REDRAW:
				if ( sVal == Win_ids[1] && Win_ids[1] != -1 )
					vDrawHelpScene();
				else if ( sVal == Win_ids[0] )
				{
               		reshapeviewport();
               		getsize(&nWinWidth, &nWinHeight);
               		getorigin(&nXWinOrigin, &nYWinOrigin);
               		vDrawScene();
				}
               break;

            case LEFTMOUSE:
            case RIGHTMOUSE:
            case MIDDLEMOUSE:
               if (lDevice == LEFTMOUSE) {
                  vToggleDir(DIR_X);
               }
               if (lDevice == MIDDLEMOUSE) {
                  vToggleDir(DIR_Y);
               }
               if (lDevice == RIGHTMOUSE) {
                  vToggleDir(DIR_Z);
               }

               /*
                * Get the reference point and if the button is being
                * pressed, initialize the rotation vector.
                */
               qread(&sXReference);
               qread(&sYReference);
               if (sVal != 0) {
                  vGenerateRotVec((float)sXReference, (float)sYReference,
                                  &fDummyX, &fDummyY, &fDummyZ);
               } else {
                  (*scene)();
               }
               break;

			case ESCKEY:
			    #ifdef WIN_NOTICE
					fprintf(stderr, "<EXIT_TRACKBALL>\n");
	 		    #endif
				return;
				break;

            case QKEY:
				if ( Input_win == Win_ids[1] ) 
				{
					winclose(Input_win);
					Win_ids[1] != -1;
				}
				else if ( Input_win == Win_ids[0] )
				{
					winclose(Win_ids[0]);
					if ( Win_ids[1] != -1 )
						winclose(Win_ids[1]);
	               exit(0);
				}
                	break;

             default:
               break;
         }
      }
	  else
	  {
		  (*scene)();
      }

		getdev(DR_NUM_BUT, Devs, Last_Actions);

      if (nCurrentDir) {
         sXCurrent     = getvaluator(MOUSEX);
         sYCurrent     = getvaluator(MOUSEY);
         vPerformTransfo((float)sXReference, (float)sXCurrent,
                         (float)sYReference, (float)sYCurrent);
         sXReference   = sXCurrent;
         sYReference   = sYCurrent;
         (*scene)();
      }
		#ifdef DTEST
			Time++;
		#endif
   }
}
#endif