/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**  Antenna Visualization Toolkit                                          **/
/**                                                                         **/
/**  Copyright (C) 1998 Adrian Agogino, Ken Harker                          **/
/**  Copyright (C) 2005 Joop Stakenborg                                     **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "MyTypes.h"
#include "TkAntenna.h"
#include "togl.h"
#include "ant.h"
#include "pcard.h"
#include "VisField.h"
#include "VisWires.h"


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            Global Variables                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


extern double  POINT_DIST_SCALE;  /**  For point clouds, in dBi     **/
extern double  POINT_SIZE_SCALE;  /**    ..also in terms of dBi     **/
extern double  ALPHA;             /**  Alpha transparency factor    **/
extern double  curr_step_size;    /**  Updated when the NEC called  **/
extern double  NULL_THRESHOLD;    /**  As a percentage of max dBi   **/
extern double  NULL_DISTANCE;     /**  So null maps float above     **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         Function Prototypes                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void TKA_Cylinder(GLfloat radius, GLfloat height, GLint slices, GLint rings);
void TKA_Cube(GLfloat size);


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             AntNormalize                                **/
/**                                                                         **/
/**  Normalizes a vector by reducing it to unit length.                     **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void AntNormalize(double v[3]) {

  double  d;  /**  Temporary value  **/

  d = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
  if (d == 0.0) {
    fprintf(stderr,"Zero length vector\n");
    return;
  }  /**  Error  **/
  v[0] = v[0] / d;
  v[1] = v[1] / d;
  v[2] = v[2] / d;
    
}  /**  End of AntNormalize **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                           AntNormCrossProd                              **/
/**                                                                         **/
/**  Performs a cross product and normalizes the resulting vector.          **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void AntNormCrossProd(double v1[3], double v2[3], double out[3]) {

  out[0] = v1[1]*v2[2] - v1[2]*v2[1];
  out[1] = v1[2]*v2[0] - v1[0]*v2[2];
  out[2] = v1[0]*v2[1] - v1[1]*v2[0];
  AntNormalize(out);
    
}  /**  End of AntNormCrossProd **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                PlotPoint                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void PlotPoint(GLfloat azimuth, GLfloat elevation, GLfloat dist) {

   glPushMatrix();
   glRotatef(elevation, 0.0, 1.0, 0.0);
   glRotatef(azimuth, 0.0, 0.0, -1.0);
   glTranslatef(0.0, dist * POINT_DIST_SCALE, 0.0);
   TKA_Cube(dist * POINT_SIZE_SCALE);
   glPopMatrix();

}  /**  End of PlotPoint  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                       DrawRFPowerDensityPoints                          **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  using PlotPoint.                                                       **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawRFPowerDensityPoints(Ant *antData) {

  GLfloat  green_color[] = {0.1, 0.8, 0.1, 1.0};  /**  Color of points  **/
  int      i;                                     /**  Loop counter     **/

  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green_color);
  for(i = 0; i < antData->fieldData->count; i++) {
    PlotPoint(antData->fieldData->vals[i].theta, 
              antData->fieldData->vals[i].phi,
              exp(antData->fieldData->vals[i].total_gain/10.0));
  }  /**  For each point  **/

}  /**  End of DrawRFPowerDensityPoints  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                     DrawPolarizationSensePoints                         **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  using PlotPoint.   Each control point is color coded depending on the  **/
/**  polarization sense.                                                    **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawPolarizationSensePoints(Ant *antData) {

  GLfloat  point_color[4];  /**  Color of points  **/
  int      i;               /**  Loop counter     **/

  for(i = 0; i < antData->fieldData->count; i++) {
    if (antData->fieldData->vals[i].sense == LINEAR) {
      point_color[0] = 0.0;
      point_color[1] = 1.0;
      point_color[2] = 0.0;
    }  /**  Linear  **/
    if (antData->fieldData->vals[i].sense == RIGHT) {
      point_color[0] = 1.0;
      point_color[1] = 1.0;
      point_color[2] = 1.0;
    }  /**  Linear  **/
    if (antData->fieldData->vals[i].sense == LEFT) {
      point_color[0] = 0.0;
      point_color[1] = 0.0;
      point_color[2] = 1.0;
    }  /**  Linear  **/
    point_color[3] = ALPHA;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color); 
    PlotPoint(antData->fieldData->vals[i].theta, 
              antData->fieldData->vals[i].phi,
              exp(antData->fieldData->vals[i].total_gain/10.0));
  }  /**  For each point  **/

}  /**  End of DrawPolarizationSensePoints  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                     DrawPolarizationTiltPoints                          **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  using PlotPoint.   Each control point is color coded depending on the  **/
/**  polarization tilt.                                                     **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawPolarizationTiltPoints(Ant *antData) {

  GLfloat  point_color[4];  /**  Color of points      **/
  int      i;               /**  Loop counter         **/
  double   range;           /**  Range of tilt        **/
  double   current_value;   /**  Current color value  **/

  range = antData->fieldData->maxtilt - antData->fieldData->mintilt;
  for(i = 0; i < antData->fieldData->count; i++) {
    current_value = (antData->fieldData->vals[i].tilt + 
                    (-1 * antData->fieldData->mintilt)) / range;
    point_color[0] = current_value;
    point_color[1] = 1.0;
    point_color[2] = current_value;
    point_color[3] = ALPHA;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color); 
    PlotPoint(antData->fieldData->vals[i].theta, 
              antData->fieldData->vals[i].phi,
              exp(antData->fieldData->vals[i].total_gain/10.0));
  }  /**  For each point  **/

}  /**  End of DrawPolarizationTiltPoints  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         DrawAxialRatioPoints                            **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  using PlotPoint.   Each control point is color coded depending on the  **/
/**  axial ratio.                                                           **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawAxialRatioPoints(Ant *antData) {

  GLfloat  point_color[4];  /**  Color of points      **/
  int      i;               /**  Loop counter         **/
  double   range;           /**  Range of tilt        **/
  double   current_value;   /**  Current color value  **/

  range=antData->fieldData->maxaxialratio - antData->fieldData->minaxialratio;
  for(i = 0; i < antData->fieldData->count; i++) {
    current_value = (antData->fieldData->vals[i].axial_ratio + 
                    (-1 * antData->fieldData->minaxialratio)) / range;
    ComputeColor(current_value, 
                 antData->fieldData->minaxialratio,
                 antData->fieldData->maxaxialratio,
                 point_color);
    point_color[3] = ALPHA;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color); 
    PlotPoint(antData->fieldData->vals[i].theta, 
              antData->fieldData->vals[i].phi,
              exp(antData->fieldData->vals[i].total_gain/10.0));
  }  /**  For each point  **/

}  /**  End of DrawAxialRatioPoints  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         DrawShowNullsPoints                             **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  using PlotPoint.   Each control point is color coded if it is in the   **/
/**  null of the pattern.                                                   **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawShowNullsPoints(Ant *antData) {

  GLfloat  point_color[4];  /**  Color of points      **/
  int      i;               /**  Loop counter         **/
  double   range;           /**  Range of tilt        **/
  double   current_value;   /**  Current color value  **/

  range = antData->fieldData->maxgain + (-1*antData->fieldData->mingain);
  for(i = 0; i < antData->fieldData->count; i++) {
    current_value = ((antData->fieldData->vals[i].total_gain + 
                    (-1*antData->fieldData->mingain))
                    / range);
    point_color[0] = 1.0;
    point_color[1] = 0.0;
    point_color[2] = 0.0;
    if (current_value < NULL_THRESHOLD)
      point_color[3] = ALPHA;
    else
      point_color[3] = 0.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color); 
    if (point_color[3] > 0.0) {
      PlotPoint(antData->fieldData->vals[i].theta, 
                antData->fieldData->vals[i].phi,
                exp(antData->fieldData->vals[i].total_gain/10.0) +
                (NULL_DISTANCE / 5));
    }  /**  Plot a point  **/
  }  /**  For each point  **/

}  /**  End of DrawShowNullsPoints  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                        DrawRFPowerDensitySurface                        **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  as a triangular mesh rendered smooth shaded.                           **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawRFPowerDensitySurface(Ant *antData) {

  int      increments;       /**  Number of incs   **/
  int      az;               /**  Loop counter     **/
  int      el;               /**  Loop counter     **/
  int      latitude;         /**  Loop counter     **/
  int      longitude;        /**  Loop counter     **/
  int      i;                /**  Loop counter     **/
  double   tx;               /**  Temporary x      **/
  double   ty;               /**  Temporary y      **/
  double   tz;               /**  Temporary z      **/
  double   ttheta;           /**  Temporary angle  **/
  double   tphi;             /**  Temporary angle  **/
  double   tdist;            /**  Temp distance    **/
  GLfloat  green_color[4];   /**  Color surface    **/
  GLfloat  red_color[4];     /**  Color surface    **/
  GLfloat  blue_color[4];    /**  Color surface    **/
  GLfloat  yellow_color[4];  /**  Color surface    **/
  GLfloat  white_color[4];   /**  Color surface    **/

  green_color[0] = 0.1;
  green_color[1] = 0.8;
  green_color[2] = 0.1;
  green_color[3] = ALPHA;

  red_color[0] = 0.8;
  red_color[1] = 0.1;
  red_color[2] = 0.1;
  red_color[3] = ALPHA;

  blue_color[0] = 0.1;
  blue_color[1] = 0.1;
  blue_color[2] = 0.8;
  blue_color[3] = ALPHA;

  yellow_color[0] = 1.0;
  yellow_color[1] = 1.0;
  yellow_color[2] = 0.0;
  yellow_color[3] = ALPHA;

  white_color[0] = 1.0;
  white_color[1] = 1.0;
  white_color[2] = 1.0;
  white_color[3] = ALPHA;

  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = exp(antData->fieldData->vals[i].total_gain / 10.0) * 
          POINT_DIST_SCALE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;      
	/*
        if (el >= increments/4 && el < increments *3 /4) 
          surfaceMesh[el][az].posy = surfaceMesh[el][az].posy * -1;
	  */
        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green_color);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      while (longitude <= increments/2) {

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawRFPowerDensitySurface  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                      DrawPolarizationSenseSurface                       **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  as a triangular mesh rendered smooth shaded.  Each control point is    **/
/**  color coded depending on the polarization sense.                       **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawPolarizationSenseSurface(Ant *antData) {

  int      increments;       /**  Number of incs   **/
  int      az;               /**  Loop counter     **/
  int      el;               /**  Loop counter     **/
  int      latitude;         /**  Loop counter     **/
  int      longitude;        /**  Loop counter     **/
  int      i;                /**  Loop counter     **/
  double   tx;               /**  Temporary x      **/
  double   ty;               /**  Temporary y      **/
  double   tz;               /**  Temporary z      **/
  double   ttheta;           /**  Temporary angle  **/
  double   tphi;             /**  Temporary angle  **/
  double   tdist;            /**  Temp distance    **/
  GLfloat  green_color[4];   /**  Color surface    **/
  GLfloat  red_color[4];     /**  Color surface    **/
  GLfloat  blue_color[4];    /**  Color surface    **/
  GLfloat  yellow_color[4];  /**  Color surface    **/
  GLfloat  white_color[4];   /**  Color surface    **/
  GLfloat  point_color[4];   /**  Color surface    **/

  green_color[0] = 0.1;
  green_color[1] = 0.8;
  green_color[2] = 0.1;
  green_color[3] = ALPHA;

  red_color[0] = 0.8;
  red_color[1] = 0.1;
  red_color[2] = 0.1;
  red_color[3] = ALPHA;

  blue_color[0] = 0.1;
  blue_color[1] = 0.1;
  blue_color[2] = 0.8;
  blue_color[3] = ALPHA;

  yellow_color[0] = 1.0;
  yellow_color[1] = 1.0;
  yellow_color[2] = 0.0;
  yellow_color[3] = ALPHA;

  white_color[0] = 1.0;
  white_color[1] = 1.0;
  white_color[2] = 1.0;
  white_color[3] = ALPHA;

  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = exp(antData->fieldData->vals[i].total_gain / 10.0) * 
          POINT_DIST_SCALE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;
	/*
        if (el >= increments/4 && el < increments *3 /4) 
          surfaceMesh[el][az].posy = surfaceMesh[el][az].posy * -1;
	  */

        if (antData->fieldData->vals[i].sense == LINEAR) {
          antData->surfaceMesh[el][az].r = 0.0;
          antData->surfaceMesh[el][az].g = 1.0;
          antData->surfaceMesh[el][az].b = 0.0;
        }  /**  Linear  **/

        if (antData->fieldData->vals[i].sense == RIGHT) {
          antData->surfaceMesh[el][az].r = 1.0;
          antData->surfaceMesh[el][az].g = 1.0;
          antData->surfaceMesh[el][az].b = 1.0;
        }  /**  Right  **/

        if (antData->fieldData->vals[i].sense == LEFT) {
          antData->surfaceMesh[el][az].r = 0.0;
          antData->surfaceMesh[el][az].g = 0.0;
          antData->surfaceMesh[el][az].b = 1.0;
        }  /**  Left  **/
          
        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
      antData->surfaceMesh[el][increments].r = antData->surfaceMesh[el][0].r;
      antData->surfaceMesh[el][increments].g = antData->surfaceMesh[el][0].g;
      antData->surfaceMesh[el][increments].b = antData->surfaceMesh[el][0].b;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
      antData->surfaceMesh[increments][az].r = antData->surfaceMesh[0][az].r;
      antData->surfaceMesh[increments][az].g = antData->surfaceMesh[0][az].g;
      antData->surfaceMesh[increments][az].b = antData->surfaceMesh[0][az].b;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      while (longitude <= increments/2) {

        point_color[0] = antData->surfaceMesh[latitude][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        point_color[0] = antData->surfaceMesh[latitude+1][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude+1][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude+1][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawPolarizationSenseSurface  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                      DrawPolarizationTiltSurface                        **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  as a triangular mesh rendered smooth shaded.  Each control point is    **/
/**  color coded depending on the polarization tilt.                        **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawPolarizationTiltSurface(Ant *antData) {

  int      increments;      /**  Number of incs       **/
  int      az;              /**  Loop counter         **/
  int      el;              /**  Loop counter         **/
  int      latitude;        /**  Loop counter         **/
  int      longitude;       /**  Loop counter         **/
  int      i;               /**  Loop counter         **/
  double   tx;              /**  Temporary x          **/
  double   ty;              /**  Temporary y          **/
  double   tz;              /**  Temporary z          **/
  double   ttheta;          /**  Temporary angle      **/
  double   tphi;            /**  Temporary angle      **/
  double   tdist;           /**  Temp distance        **/
  GLfloat  point_color[4];  /**  Color surface        **/
  double   range;           /**  Range of tilt        **/
  double   current_value;   /**  Current color value  **/

  range = antData->fieldData->maxtilt - antData->fieldData->mintilt;


  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = exp(antData->fieldData->vals[i].total_gain / 10.0) * 
          POINT_DIST_SCALE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;

        current_value = (antData->fieldData->vals[i].tilt + 
                        (-1 * antData->fieldData->mintilt)) / range;
        antData->surfaceMesh[el][az].r = current_value;
        antData->surfaceMesh[el][az].g = 1.0;
        antData->surfaceMesh[el][az].b = current_value;

        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
      antData->surfaceMesh[el][increments].r = antData->surfaceMesh[el][0].r;
      antData->surfaceMesh[el][increments].g = antData->surfaceMesh[el][0].g;
      antData->surfaceMesh[el][increments].b = antData->surfaceMesh[el][0].b;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
      antData->surfaceMesh[increments][az].r = antData->surfaceMesh[0][az].r;
      antData->surfaceMesh[increments][az].g = antData->surfaceMesh[0][az].g;
      antData->surfaceMesh[increments][az].b = antData->surfaceMesh[0][az].b;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      while (longitude <= increments/2) {

        point_color[0] = antData->surfaceMesh[latitude][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        point_color[0] = antData->surfaceMesh[latitude+1][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude+1][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude+1][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawPolarizationTiltSurface  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         DrawAxialRatioSurface                           **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  as a triangular mesh rendered smooth shaded.  Each control point is    **/
/**  color coded depending on the axial ratio.                              **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawAxialRatioSurface(Ant *antData) {

  int      increments;      /**  Number of incs       **/
  int      az;              /**  Loop counter         **/
  int      el;              /**  Loop counter         **/
  int      latitude;        /**  Loop counter         **/
  int      longitude;       /**  Loop counter         **/
  int      i;               /**  Loop counter         **/
  double   tx;              /**  Temporary x          **/
  double   ty;              /**  Temporary y          **/
  double   tz;              /**  Temporary z          **/
  double   ttheta;          /**  Temporary angle      **/
  double   tphi;            /**  Temporary angle      **/
  double   tdist;           /**  Temp distance        **/
  GLfloat  point_color[4];  /**  Color surface        **/
  double   range;           /**  Range of tilt        **/
  double   current_value;   /**  Current color value  **/

  range= antData->fieldData->maxaxialratio - antData->fieldData->minaxialratio;

  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = exp(antData->fieldData->vals[i].total_gain / 10.0) * 
          POINT_DIST_SCALE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;

        current_value = (antData->fieldData->vals[i].axial_ratio + 
                        (-1 * antData->fieldData->minaxialratio)) / range;
        ComputeColor(current_value, 
                     antData->fieldData->minaxialratio,
                     antData->fieldData->maxaxialratio,
                     point_color);
        antData->surfaceMesh[el][az].r = point_color[0];
        antData->surfaceMesh[el][az].g = point_color[1];
        antData->surfaceMesh[el][az].b = point_color[2];

        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
      antData->surfaceMesh[el][increments].r = antData->surfaceMesh[el][0].r;
      antData->surfaceMesh[el][increments].g = antData->surfaceMesh[el][0].g;
      antData->surfaceMesh[el][increments].b = antData->surfaceMesh[el][0].b;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
      antData->surfaceMesh[increments][az].r = antData->surfaceMesh[0][az].r;
      antData->surfaceMesh[increments][az].g = antData->surfaceMesh[0][az].g;
      antData->surfaceMesh[increments][az].b = antData->surfaceMesh[0][az].b;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      while (longitude <= increments/2) {

        point_color[0] = antData->surfaceMesh[latitude][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        point_color[0] = antData->surfaceMesh[latitude+1][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude+1][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude+1][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawAxialRatioSurface  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          DrawShowNullsSurface                           **/
/**                                                                         **/
/**  This function traverses each of the points and draws it on the screen  **/
/**  as a triangular mesh rendered smooth shaded.  Each control point is    **/
/**  colored only if it is in a NULL                                        **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawShowNullsSurface(Ant *antData) {

  int      increments;      /**  Number of incs       **/
  int      az;              /**  Loop counter         **/
  int      el;              /**  Loop counter         **/
  int      latitude;        /**  Loop counter         **/
  int      longitude;       /**  Loop counter         **/
  int      i;               /**  Loop counter         **/
  double   tx;              /**  Temporary x          **/
  double   ty;              /**  Temporary y          **/
  double   tz;              /**  Temporary z          **/
  double   ttheta;          /**  Temporary angle      **/
  double   tphi;            /**  Temporary angle      **/
  double   tdist;           /**  Temp distance        **/
  GLfloat  point_color[4];  /**  Color surface        **/
  double   range;           /**  Range of tilt        **/
  double   current_value;   /**  Current color value  **/

  range = antData->fieldData->maxtilt - antData->fieldData->mintilt;


  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = (exp(antData->fieldData->vals[i].total_gain / 10.0) * 
          POINT_DIST_SCALE) + NULL_DISTANCE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;

        range = antData->fieldData->maxgain + (-1*antData->fieldData->mingain);
        current_value = ((antData->fieldData->vals[i].total_gain + 
                         (-1*antData->fieldData->mingain))
                         / range);
        if (current_value < NULL_THRESHOLD) {
          antData->surfaceMesh[el][az].r = 1.0;
          antData->surfaceMesh[el][az].g = 0.0;
          antData->surfaceMesh[el][az].b = 0.0;
        } else {
          antData->surfaceMesh[el][az].r = 0.0;
          antData->surfaceMesh[el][az].g = 0.0;
          antData->surfaceMesh[el][az].b = 0.0;
        }  /**  Not a null  **/

        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
      antData->surfaceMesh[el][increments].r = antData->surfaceMesh[el][0].r;
      antData->surfaceMesh[el][increments].g = antData->surfaceMesh[el][0].g;
      antData->surfaceMesh[el][increments].b = antData->surfaceMesh[el][0].b;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
      antData->surfaceMesh[increments][az].r = antData->surfaceMesh[0][az].r;
      antData->surfaceMesh[increments][az].g = antData->surfaceMesh[0][az].g;
      antData->surfaceMesh[increments][az].b = antData->surfaceMesh[0][az].b;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      while (longitude <= increments/2) {

        point_color[0] = antData->surfaceMesh[latitude][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude][longitude].b;
        if (point_color[0] == 1.0)
          point_color[3] = ALPHA;
        else
          point_color[3] = 0.0;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        point_color[0] = antData->surfaceMesh[latitude+1][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude+1][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude+1][longitude].b;
        if (point_color[0] == 1.0)
          point_color[3] = ALPHA;
        else
          point_color[3] = 0.0;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawShowNullsSurface  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         DrawRFPowerDensitySphere                        **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawRFPowerDensitySphere(Ant *antData) {

  int      increments;       /**  Number of incs   **/
  int      az;               /**  Loop counter     **/
  int      el;               /**  Loop counter     **/
  int      latitude;         /**  Loop counter     **/
  int      longitude;        /**  Loop counter     **/
  int      i;                /**  Loop counter     **/
  double   tx;               /**  Temporary x      **/
  double   ty;               /**  Temporary y      **/
  double   tz;               /**  Temporary z      **/
  double   ttheta;           /**  Temporary angle  **/
  double   tphi;             /**  Temporary angle  **/
  double   tdist;            /**  Temp distance    **/
  GLfloat  green_color[4];   /**  Color surface    **/

  green_color[0] = 0.1;
  green_color[1] = 0.8;
  green_color[2] = 0.1;
  green_color[3] = ALPHA;

  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = 1.0 * POINT_DIST_SCALE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;
        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green_color);
      while (longitude <= increments/2) {

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawRFPowerDensitySphere  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                       DrawPolarizationSenseSphere                       **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawPolarizationSenseSphere(Ant *antData) {

  int      increments;       /**  Number of incs   **/
  int      az;               /**  Loop counter     **/
  int      el;               /**  Loop counter     **/
  int      latitude;         /**  Loop counter     **/
  int      longitude;        /**  Loop counter     **/
  int      i;                /**  Loop counter     **/
  double   tx;               /**  Temporary x      **/
  double   ty;               /**  Temporary y      **/
  double   tz;               /**  Temporary z      **/
  double   ttheta;           /**  Temporary angle  **/
  double   tphi;             /**  Temporary angle  **/
  double   tdist;            /**  Temp distance    **/
  GLfloat  green_color[4];   /**  Color surface    **/
  GLfloat  red_color[4];     /**  Color surface    **/
  GLfloat  blue_color[4];    /**  Color surface    **/
  GLfloat  yellow_color[4];  /**  Color surface    **/
  GLfloat  white_color[4];   /**  Color surface    **/
  GLfloat  point_color[4];   /**  Color surface    **/

  green_color[0] = 0.1;
  green_color[1] = 0.8;
  green_color[2] = 0.1;
  green_color[3] = ALPHA;

  red_color[0] = 0.8;
  red_color[1] = 0.1;
  red_color[2] = 0.1;
  red_color[3] = ALPHA;

  blue_color[0] = 0.1;
  blue_color[1] = 0.1;
  blue_color[2] = 0.8;
  blue_color[3] = ALPHA;

  yellow_color[0] = 1.0;
  yellow_color[1] = 1.0;
  yellow_color[2] = 0.0;
  yellow_color[3] = ALPHA;

  white_color[0] = 1.0;
  white_color[1] = 1.0;
  white_color[2] = 1.0;
  white_color[3] = ALPHA;

  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = 1.0 * POINT_DIST_SCALE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;

        if (antData->fieldData->vals[i].sense == LINEAR) {
          antData->surfaceMesh[el][az].r = 0.0;
          antData->surfaceMesh[el][az].g = 1.0;
          antData->surfaceMesh[el][az].b = 0.0;
        }  /**  Linear  **/

        if (antData->fieldData->vals[i].sense == RIGHT) {
          antData->surfaceMesh[el][az].r = 1.0;
          antData->surfaceMesh[el][az].g = 1.0;
          antData->surfaceMesh[el][az].b = 1.0;
        }  /**  Right  **/

        if (antData->fieldData->vals[i].sense == LEFT) {
          antData->surfaceMesh[el][az].r = 0.0;
          antData->surfaceMesh[el][az].g = 0.0;
          antData->surfaceMesh[el][az].b = 1.0;
        }  /**  Left  **/
          
        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
      antData->surfaceMesh[el][increments].r = antData->surfaceMesh[el][0].r;
      antData->surfaceMesh[el][increments].g = antData->surfaceMesh[el][0].g;
      antData->surfaceMesh[el][increments].b = antData->surfaceMesh[el][0].b;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
      antData->surfaceMesh[increments][az].r = antData->surfaceMesh[0][az].r;
      antData->surfaceMesh[increments][az].g = antData->surfaceMesh[0][az].g;
      antData->surfaceMesh[increments][az].b = antData->surfaceMesh[0][az].b;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      while (longitude <= increments/2) {

        point_color[0] = antData->surfaceMesh[latitude][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        point_color[0] = antData->surfaceMesh[latitude+1][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude+1][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude+1][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawPolarizationSenseSphere  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                       DrawPolarizationTiltSphere                        **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawPolarizationTiltSphere(Ant *antData) {

  int      increments;       /**  Number of incs   **/
  int      az;               /**  Loop counter     **/
  int      el;               /**  Loop counter     **/
  int      latitude;         /**  Loop counter     **/
  int      longitude;        /**  Loop counter     **/
  int      i;                /**  Loop counter     **/
  double   tx;               /**  Temporary x      **/
  double   ty;               /**  Temporary y      **/
  double   tz;               /**  Temporary z      **/
  double   ttheta;           /**  Temporary angle  **/
  double   tphi;             /**  Temporary angle  **/
  double   tdist;            /**  Temp distance    **/
  GLfloat  point_color[4];   /**  Color surface    **/
  double   range;           /**  Range of tilt        **/
  double   current_value;   /**  Current color value  **/

  range = antData->fieldData->maxtilt - antData->fieldData->mintilt;
  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = 1.0 * POINT_DIST_SCALE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;

        current_value = (antData->fieldData->vals[i].tilt + 
                        (-1 * antData->fieldData->mintilt)) / range;
        antData->surfaceMesh[el][az].r = current_value;
        antData->surfaceMesh[el][az].g = 1.0;
        antData->surfaceMesh[el][az].b = current_value;
          
        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
      antData->surfaceMesh[el][increments].r = antData->surfaceMesh[el][0].r;
      antData->surfaceMesh[el][increments].g = antData->surfaceMesh[el][0].g;
      antData->surfaceMesh[el][increments].b = antData->surfaceMesh[el][0].b;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
      antData->surfaceMesh[increments][az].r = antData->surfaceMesh[0][az].r;
      antData->surfaceMesh[increments][az].g = antData->surfaceMesh[0][az].g;
      antData->surfaceMesh[increments][az].b = antData->surfaceMesh[0][az].b;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      while (longitude <= increments/2) {

        point_color[0] = antData->surfaceMesh[latitude][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        point_color[0] = antData->surfaceMesh[latitude+1][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude+1][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude+1][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawPolarizationTiltSphere  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          DrawAxialRatioSphere                           **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawAxialRatioSphere(Ant *antData) {

  int      increments;       /**  Number of incs   **/
  int      az;               /**  Loop counter     **/
  int      el;               /**  Loop counter     **/
  int      latitude;         /**  Loop counter     **/
  int      longitude;        /**  Loop counter     **/
  int      i;                /**  Loop counter     **/
  double   tx;               /**  Temporary x      **/
  double   ty;               /**  Temporary y      **/
  double   tz;               /**  Temporary z      **/
  double   ttheta;           /**  Temporary angle  **/
  double   tphi;             /**  Temporary angle  **/
  double   tdist;            /**  Temp distance    **/
  GLfloat  point_color[4];   /**  Color surface    **/
  double   range;           /**  Range of tilt        **/
  double   current_value;   /**  Current color value  **/

  range= antData->fieldData->maxaxialratio - antData->fieldData->minaxialratio;
  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = 1.0 * POINT_DIST_SCALE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;

        current_value = (antData->fieldData->vals[i].axial_ratio + 
                        (-1 * antData->fieldData->minaxialratio)) / range;
        ComputeColor(current_value, 
                     antData->fieldData->minaxialratio,
                     antData->fieldData->maxaxialratio,
                     point_color);
        antData->surfaceMesh[el][az].r = point_color[0];
        antData->surfaceMesh[el][az].g = point_color[1];
        antData->surfaceMesh[el][az].b = point_color[2];
          
        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
      antData->surfaceMesh[el][increments].r = antData->surfaceMesh[el][0].r;
      antData->surfaceMesh[el][increments].g = antData->surfaceMesh[el][0].g;
      antData->surfaceMesh[el][increments].b = antData->surfaceMesh[el][0].b;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
      antData->surfaceMesh[increments][az].r = antData->surfaceMesh[0][az].r;
      antData->surfaceMesh[increments][az].g = antData->surfaceMesh[0][az].g;
      antData->surfaceMesh[increments][az].b = antData->surfaceMesh[0][az].b;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      while (longitude <= increments/2) {

        point_color[0] = antData->surfaceMesh[latitude][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        point_color[0] = antData->surfaceMesh[latitude+1][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude+1][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude+1][longitude].b;
        point_color[3] = ALPHA;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawAxialRatioSphere  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          DrawShowNullsSphere                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawShowNullsSphere(Ant *antData) {

  int      increments;       /**  Number of incs   **/
  int      az;               /**  Loop counter     **/
  int      el;               /**  Loop counter     **/
  int      latitude;         /**  Loop counter     **/
  int      longitude;        /**  Loop counter     **/
  int      i;                /**  Loop counter     **/
  double   tx;               /**  Temporary x      **/
  double   ty;               /**  Temporary y      **/
  double   tz;               /**  Temporary z      **/
  double   ttheta;           /**  Temporary angle  **/
  double   tphi;             /**  Temporary angle  **/
  double   tdist;            /**  Temp distance    **/
  GLfloat  point_color[4];   /**  Color surface    **/
  double   range;           /**  Range of tilt        **/
  double   current_value;   /**  Current color value  **/

  range = antData->fieldData->maxtilt - antData->fieldData->mintilt;
  if (antData->fieldData->count != 0) {
  
    /**  Build data structure  **/
    increments = 360 / curr_step_size;
    i = 0;
    for (el = 0; el < increments; el++) {
      for (az = 0; az < increments; az++) {
        ttheta = antData->fieldData->vals[i].theta * 2.0 * PI / 360.0;
        tphi = antData->fieldData->vals[i].phi * 2.0 * PI / 360.0;
        tdist = 1.0 * POINT_DIST_SCALE + NULL_DISTANCE;
        ty = cos(ttheta) * cos(tphi) * tdist;
        tx = sin(ttheta) * cos(tphi) * tdist;
        tz = sin(tphi)               * tdist;
        antData->surfaceMesh[el][az].posx = tx;
        antData->surfaceMesh[el][az].posy = ty;
        antData->surfaceMesh[el][az].posz = tz;

        range = antData->fieldData->maxgain + (-1*antData->fieldData->mingain);
        current_value = ((antData->fieldData->vals[i].total_gain + 
                         (-1*antData->fieldData->mingain))
                         / range);
        if (current_value < NULL_THRESHOLD) {
          antData->surfaceMesh[el][az].r = 1.0;
          antData->surfaceMesh[el][az].g = 0.0;
          antData->surfaceMesh[el][az].b = 0.0;
        } else {
          antData->surfaceMesh[el][az].r = 0.0;
          antData->surfaceMesh[el][az].g = 0.0;
          antData->surfaceMesh[el][az].b = 0.0;
        }  /**  Not a null  **/
          
        i++;
      }  /**  By elevation  **/
    }  /**  By azimuth  **/

    for (el = 0; el < increments; el++) {
      antData->surfaceMesh[el][increments].posx = 
        antData->surfaceMesh[el][0].posx;
      antData->surfaceMesh[el][increments].posy = 
        antData->surfaceMesh[el][0].posy;
      antData->surfaceMesh[el][increments].posz = 
        antData->surfaceMesh[el][0].posz;
      antData->surfaceMesh[el][increments].r = antData->surfaceMesh[el][0].r;
      antData->surfaceMesh[el][increments].g = antData->surfaceMesh[el][0].g;
      antData->surfaceMesh[el][increments].b = antData->surfaceMesh[el][0].b;
    }  /**  Wrap around buffer  **/
    for (az = 0; az < increments; az++) {
      antData->surfaceMesh[increments][az].posx = 
        antData->surfaceMesh[0][az].posx;
      antData->surfaceMesh[increments][az].posy = 
        antData->surfaceMesh[0][az].posy;
      antData->surfaceMesh[increments][az].posz = 
        antData->surfaceMesh[0][az].posz;
      antData->surfaceMesh[increments][az].r = antData->surfaceMesh[0][az].r;
      antData->surfaceMesh[increments][az].g = antData->surfaceMesh[0][az].g;
      antData->surfaceMesh[increments][az].b = antData->surfaceMesh[0][az].b;
    }  /**  Wrap around buffer  **/

    glPushMatrix();
    glEnable(GL_BLEND);

    glShadeModel(GL_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (latitude=0; latitude < increments; latitude++) {
      longitude = 0;
      glBegin(GL_TRIANGLE_STRIP);
      while (longitude <= increments/2) {

        point_color[0] = antData->surfaceMesh[latitude][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude][longitude].b;
        if (point_color[0] == 1.0)
          point_color[3] = ALPHA;
        else
          point_color[3] = 0.0;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude][longitude].posx,
                   antData->surfaceMesh[latitude][longitude].posy,
                   antData->surfaceMesh[latitude][longitude].posz);

        point_color[0] = antData->surfaceMesh[latitude+1][longitude].r;
        point_color[1] = antData->surfaceMesh[latitude+1][longitude].g;
        point_color[2] = antData->surfaceMesh[latitude+1][longitude].b;
        if (point_color[0] == 1.0)
          point_color[3] = ALPHA;
        else
          point_color[3] = 0.0;
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, point_color);

        glNormal3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);
        glVertex3f(antData->surfaceMesh[latitude+1][longitude].posx,
                   antData->surfaceMesh[latitude+1][longitude].posy,
                   antData->surfaceMesh[latitude+1][longitude].posz);

        longitude++;
      }  /**  For all latitudes control points  **/
      glEnd();
    }  /**  Triangle strips  **/
    glDisable(GL_BLEND);
    glPopMatrix();
  }  /**  Not empty  **/

}  /**  End of DrawShowNullsSphere  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            End of VisField.c                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/






