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
#include "VisWires.h"


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            Global Variables                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


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
/**                            ComputeColor                                 **/
/**                                                                         **/
/**  Computes a color in a three color range - red green blue               **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ComputeColor(double val, double min, double max, GLfloat *color) {

  GLfloat cval;  /**  Range  **/
        
  if(min != max)
    cval = (val - min) / (max - min);
  else
    cval = 0.5;

  color[0] = 0.0;
  color[1] = 0.0;
  color[2] = 0.0;
  color[3] = 1.0;

  if(cval > 0.5) {
    color[0] = (cval - 0.5)*2;
    color[1] = (1.0 - cval)*2;
  } else {
    color[1] = (cval)*2;
    color[2] = (0.5 - cval)*2;
  }  /**  Left or Right handedness  **/

}  /**  End of ComputeColor  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             GetBoomLength                               **/
/**                                                                         **/
/**  This function computes the probable length of the boom by determining  **/
/**  the max and min coordinates in X.                                      **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


double GetBoomLength(Tube *tube) {

  double  max;    /**  Max x               **/
  double  min;    /**  Min x               **/
  bool    first;  /**  Is first elements?  **/

  first = true;  
  while(tube != NULL) {
    if (first == true) {
      if (tube->e1.x > tube->e2.x)
        max = tube->e1.x;
      else
        max = tube->e2.x;
      if (tube->e1.x < tube->e2.x)      
        min = tube->e2.x;
      else
        min = tube->e1.x;
      first = false;
    } else {
      if (tube->e1.x > max)
        max = tube->e1.x;
      if (tube->e2.x > max)
        max = tube->e2.x;
      if (tube->e1.x < min)
        min = tube->e1.x;
      if (tube->e2.x < min)
        min = tube->e2.x;
    }  /**  Not first element  **/
    tube = tube->next;
  }  /**  Traverse tube list  **/

  return (max - min);

}  /**  End of GetBoomLength  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             GetBoomHeight                               **/
/**                                                                         **/
/**  This computes the probable height above ground of the boom.  It finds  **/
/**  a median Z value for all GW control points.                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


double GetBoomHeight(Tube *tube) {

  double  median;  /**  Our best guess so far  **/
  double  max;     /**  Max x                  **/
  double  min;     /**  Min x                  **/
  bool    first;   /**  Is first elements?     **/

  first = true;    
  while(tube != NULL) {
    if (first == true) {
      if (tube->e1.z > tube->e2.z)
        max = tube->e1.z;
      else
        max = tube->e2.z;
      if (tube->e1.z < tube->e2.z)      
        min = tube->e2.z;
      else
        min = tube->e1.z;
      first = false;
    } else {
      if (tube->e1.z > max)
        max = tube->e1.z;
      if (tube->e2.z > max)
        max = tube->e2.z;
      if (tube->e1.z < min)
        min = tube->e1.z;
      if (tube->e2.z < min)
        min = tube->e2.z;
    }  /**  Not first element  **/
    tube = tube->next;
  }  /**  Traverse tube list  **/

  median = ((max + min) / 2.0);
  return median;

}  /**  End of GetBoomHeight  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                           GetVerticalHeight                             **/
/**                                                                         **/
/**  This computes the probable height above ground for elevated verticals. **/
/**  It finds a min Z value for all GW control points.                      **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


double GetVerticalHeight(Tube *tube) {

  double  min;     /**  Min x                  **/
  bool    first;   /**  Is first elements?     **/

  first = true;    
  while(tube != NULL) {
    if (first == true) {
      if (tube->e1.z < tube->e2.z)      
        min = tube->e2.z;
      else
        min = tube->e1.z;
      first = false;
    } else {
      if (tube->e1.z < min)
        min = tube->e1.z;
      if (tube->e2.z < min)
        min = tube->e2.z;
    }  /**  Not first element  **/
    tube = tube->next;
  }  /**  Traverse tube list  **/

  return min;

}  /**  End of GetVerticalHeigth  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             GetBoomWidth                                **/
/**                                                                         **/
/**  This computes the probable diameter of the boom by finding the median  **/
/**  diameter of the GW elements involved and multiplying that by a         **/
/**  constant factor.                                                       **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


double GetBoomWidth(Tube *tube) {

  double  sum;    /**  Sum of widths       **/
  double  mean;   /**  mean of widths      **/
  int     count;  /**  Number of elements  **/

  count = 0;
  sum = 0.0;
  while(tube != NULL) {
    sum = sum + tube->width;
    count = count + 1;
    tube = tube->next;
  }  /**  Traverse tube list  **/
  mean = sum / count;

  return mean;

}  /**  End of GetBoomWidth  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             GetBoomShift                                **/
/**                                                                         **/
/**  This computes the maximum negative or minimum positive X value of the  **/
/**  antenna and is used in helping to center the antenna visually on the   **/
/**  screen.                                                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


double GetBoomShift(Tube *tube) {

  double  min;    /**  Min x               **/
  bool    first;  /**  Is first elements?  **/

  first = true;    
  while(tube != NULL) {
    if (first == true) {
      if (tube->e1.x < tube->e2.x)
        min = tube->e1.x;
      else
        min = tube->e1.x;
      first = false;
    } else {
      if (tube->e1.x < min)
        min = tube->e1.x;
      if (tube->e2.x < min)
        min = tube->e2.x;
    }  /**  Not first element  **/
    tube = tube->next;
  }  /**  Traverse tube list  **/

  return min;

}  /**  End of GetBoomShift  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                               DrawTubeList                              **/
/**                                                                         **/
/**  Traverses the linked list of antenna elements and draws each one on    **/
/**  the screen.                                                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawTubeList(Tube *root, Tube *current_tube, GLint s, GLint r) {

  GLfloat red_color[] = {0.8, 0.1, 0.1, 1.0};   /**  Rouge  **/
  GLfloat gray_color[] = {0.8, 0.9, 0.9, 0.9};  /**  Gris   **/

  if(root != NULL) {

    glPushMatrix();
    if(current_tube == root)
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_color);
    else
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray_color);
    glRotatef(180,1.0,0.0,0.0);
    glRotatef(90.0, 1.0, 0.0, 0.0); 
    DrawTube(root, s, r);
    glPopMatrix(); 
    DrawTubeList(root->next, current_tube, s, r);

  }  /**  While not at end  **/

}  /**  End of DrawTubeList  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  DrawTube                               **/
/**                                                                         **/
/**  Renders an individual antenna element on the screen.                   **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawTube(Tube *the_tube, GLint s, GLint r) {

  double  length;  /**  Length of the element  **/
  double  angle1;  /**  Angle                  **/
  double  angle2;  /**  Angle                  **/
  double  dx;      /**  Delta in X direction   **/
  double  dy;      /**  Delta in Y direction   **/
  double  dz;      /**  Delta in Z direction   **/
  double  p1x;     /**  Point 1 in X           **/
  double  p1y;     /**  Point 1 in Y           **/
  double  p1z;     /**  Point 1 in Z           **/
  double  p2x;     /**  Point 2 in X           **/
  double  p2y;     /**  Point 2 in Y           **/
  double  p2z;     /**  Point 2 in Z           **/

  p1x = the_tube->e1.x;
  p1y = the_tube->e1.y;
  p1z = the_tube->e1.z;
  if (p1x != 0.0) p1x = p1x / SCALE_FACTOR;
  if (p1y != 0.0) p1y = p1y / SCALE_FACTOR;
  if (p1z != 0.0) p1z = p1z / SCALE_FACTOR;

  p2x = the_tube->e2.x;
  p2y = the_tube->e2.y;
  p2z = the_tube->e2.z;
  if (p2x != 0.0) p2x = p2x / SCALE_FACTOR;
  if (p2y != 0.0) p2y = p2y / SCALE_FACTOR;
  if (p2z != 0.0) p2z = p2z / SCALE_FACTOR;

  dx = p2x - p1x;
  dy = p2y - p1y;
  dz = p2z - p1z;

  if(the_tube->type == IS_TUBE) {

    length = PointDist(the_tube->e1, the_tube->e2);
    length = length / SCALE_FACTOR;

    angle1 = 3.14149/2 - atan2(dz, dx);
    angle2 = atan2(dy, -sin(-angle1)*dx + cos(-angle1)*dz);

    glTranslatef(p1x, p1y, p1z);  
    glRotatef(angle1*57.3, 0.0 , 1.0, 0.0 );
    glRotatef(angle2*57.3, -1.0 , 0.0, 0.0 );
    TKA_Cylinder(the_tube->width*TUBE_WIDTH_SCALE, length, s, r);

  } else { /** Walls  **/
    glTranslatef(p1x, p1y, p1z);  
    glScalef(dx, dy, dz);
    glTranslatef(0.5, 0.5, 0.5);  
    TKA_Cube(1.0);
  }  /**  Not a tube  **/

}  /**  End of DrawTube **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                       DrawWireCurrentMagnitudeList                      **/
/**                                                                         **/
/**  Traverses the linked list of antenna elements and draws each one on    **/
/**  the screen, visualized the current density of the wire.                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawWireCurrentMagnitudeList(Tube *root, 
                                  Tube *current_tube, 
                                double  max_current_mag, 
                                double  min_current_mag, 
                                 GLint  s, 
                                 GLint  r) {

  GLfloat red_color[] = {0.8, 0.1, 0.1, 1.0};   /**  Rouge  **/
  GLfloat gray_color[] = {0.8, 0.9, 0.9, 0.9};  /**  Gris   **/

  if(root != NULL) {
    glPushMatrix();
    if(current_tube == root)
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_color);
    else
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray_color);
    glRotatef(180,1.0,0.0,0.0);
    glRotatef(90.0, 1.0, 0.0, 0.0); 
    DrawWireCurrentMagnitude(root, max_current_mag, min_current_mag, s, r);
    glPopMatrix(); 
    DrawWireCurrentMagnitudeList(root->next,
                                 current_tube,
                                 max_current_mag,
                                 min_current_mag,
                                 s,
                                 r);
  }  /**  While not at end  **/

}  /**  End of DrawWireCurrentMagnitudeList  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         DrawWireCurrentMagnitude                        **/
/**                                                                         **/
/**  Renders an individual antenna element on the screen, color coded for   **/
/**  charge density.                                                        **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawWireCurrentMagnitude(Tube *the_tube, 
                            double  max_current_mag, 
                            double  min_current_mag, 
                             GLint  s, 
                             GLint  r) {

  double       length;         /**  Length of the element  **/
  double       seg_length;     /**  Each segment length    **/
  double       angle1;         /**  Angle                  **/
  double       angle2;         /**  Angle                  **/
  double       dx;             /**  Delta in X direction   **/
  double       dy;             /**  Delta in Y direction   **/
  double       dz;             /**  Delta in Z direction   **/
  double       p1x;            /**  Point 1 in X           **/
  double       p1y;            /**  Point 1 in Y           **/
  double       p1z;            /**  Point 1 in Z           **/
  double       p2x;            /**  Point 2 in X           **/
  double       p2y;            /**  Point 2 in Y           **/
  double       p2z;            /**  Point 2 in Z           **/
  GLfloat      wire_color[4];  /**  Wire color             **/
  SegmentData *segptr;         /**  Segment data           **/
  int          i;              /**  Loop counter           **/

  p1x = the_tube->e1.x;
  p1y = the_tube->e1.y;
  p1z = the_tube->e1.z;
  if (p1x != 0.0) p1x = p1x / SCALE_FACTOR;
  if (p1y != 0.0) p1y = p1y / SCALE_FACTOR;
  if (p1z != 0.0) p1z = p1z / SCALE_FACTOR;

  p2x = the_tube->e2.x;
  p2y = the_tube->e2.y;
  p2z = the_tube->e2.z;
  if (p2x != 0.0) p2x = p2x / SCALE_FACTOR;
  if (p2y != 0.0) p2y = p2y / SCALE_FACTOR;
  if (p2z != 0.0) p2z = p2z / SCALE_FACTOR;

  dx = p2x - p1x;
  dy = p2y - p1y;
  dz = p2z - p1z;

  if(the_tube->type == IS_TUBE) {

    length = PointDist(the_tube->e1, the_tube->e2);
    length = length / SCALE_FACTOR;
    seg_length = length / the_tube->segments;

    angle1 = 3.14149/2 - atan2(dz, dx);
    angle2 = atan2(dy, -sin(-angle1)*dx + cos(-angle1)*dz);

    glTranslatef(p1x, p1y, p1z);  
    glRotatef(angle1*57.3, 0.0 , 1.0, 0.0 );
    glRotatef(angle2*57.3, -1.0 , 0.0, 0.0 );

    segptr = the_tube->currents;
    if (segptr == NULL) {
      fprintf(stderr,"Fatal Current Magnitude\n");
      exit(3);
    }  /**  Current magnitude not available  **/
    for (i=0;i<the_tube->segments;i++) {
      ComputeColor(segptr->currentMagnitude, 
                   min_current_mag, 
                   max_current_mag, 
                   wire_color);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wire_color);
      TKA_Cylinder(the_tube->width*TUBE_WIDTH_SCALE, seg_length, s, r);
      glTranslatef(0.0,0.0,seg_length);
      segptr = segptr->next;
    }  /**  For each segment  **/

  } else {  /**  Draw wall  **/
    glTranslatef(p1x, p1y, p1z);
    glScalef(dx, dy, dz);
    glTranslatef(0.5, 0.5, 0.5);  
    TKA_Cube(1.0);
  }  /**  Not a tube  **/

}  /**  End of DrawWireCurrentMagnitude **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         DrawWireCurrentPhaseList                        **/
/**                                                                         **/
/**  Traverses the linked list of antenna elements and draws each one on    **/
/**  the screen, visualized the current phase of the wire.                  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawWireCurrentPhaseList(Tube *root, 
                              Tube *current_tube, 
                            double  max_current_phase, 
                            double  min_current_phase, 
                             GLint  s, 
                             GLint  r) {

  GLfloat red_color[] = {0.8, 0.1, 0.1, 1.0};   /**  Rouge  **/
  GLfloat gray_color[] = {0.8, 0.9, 0.9, 0.9};  /**  Gris   **/

  if(root != NULL) {
    glPushMatrix();
    if(current_tube == root)
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_color);
    else
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray_color);
    glRotatef(180,1.0,0.0,0.0);
    glRotatef(90.0, 1.0, 0.0, 0.0); 
    DrawWireCurrentPhase(root, max_current_phase, min_current_phase, s, r);
    glPopMatrix(); 
    DrawWireCurrentPhaseList(root->next,
                             current_tube,
                             max_current_phase,
                             min_current_phase,
                             s,
                             r);
  }  /**  While not at end  **/

}  /**  End of DrawWireCurrentPhaseList  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          DrawWireCurrentPhase                           **/
/**                                                                         **/
/**  Renders an individual antenna element on the screen, color coded for   **/
/**  charge phase.                                                          **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawWireCurrentPhase(Tube *the_tube, 
                        double  max_current_phase, 
                        double  min_current_phase, 
                         GLint  s, 
                         GLint  r) {

  double       length;         /**  Length of the element  **/
  double       seg_length;     /**  Each segment length    **/
  double       angle1;         /**  Angle                  **/
  double       angle2;         /**  Angle                  **/
  double       dx;             /**  Delta in X direction   **/
  double       dy;             /**  Delta in Y direction   **/
  double       dz;             /**  Delta in Z direction   **/
  double       p1x;            /**  Point 1 in X           **/
  double       p1y;            /**  Point 1 in Y           **/
  double       p1z;            /**  Point 1 in Z           **/
  double       p2x;            /**  Point 2 in X           **/
  double       p2y;            /**  Point 2 in Y           **/
  double       p2z;            /**  Point 2 in Z           **/
  GLfloat      wire_color[4];  /**  Wire color             **/
  SegmentData *segptr;         /**  Segment data           **/
  int          i;              /**  Loop counter           **/

  p1x = the_tube->e1.x;
  p1y = the_tube->e1.y;
  p1z = the_tube->e1.z;
  if (p1x != 0.0) p1x = p1x / SCALE_FACTOR;
  if (p1y != 0.0) p1y = p1y / SCALE_FACTOR;
  if (p1z != 0.0) p1z = p1z / SCALE_FACTOR;

  p2x = the_tube->e2.x;
  p2y = the_tube->e2.y;
  p2z = the_tube->e2.z;
  if (p2x != 0.0) p2x = p2x / SCALE_FACTOR;
  if (p2y != 0.0) p2y = p2y / SCALE_FACTOR;
  if (p2z != 0.0) p2z = p2z / SCALE_FACTOR;

  dx = p2x - p1x;
  dy = p2y - p1y;
  dz = p2z - p1z;

  if(the_tube->type == IS_TUBE) {

    length = PointDist(the_tube->e1, the_tube->e2);
    length = length / SCALE_FACTOR;
    seg_length = length / the_tube->segments;

    angle1 = 3.14149/2 - atan2(dz, dx);
    angle2 = atan2(dy, -sin(-angle1)*dx + cos(-angle1)*dz);

    glTranslatef(p1x, p1y, p1z);  
    glRotatef(angle1*57.3, 0.0 , 1.0, 0.0 );
    glRotatef(angle2*57.3, -1.0 , 0.0, 0.0 );

    segptr = the_tube->currents;
    if (segptr == NULL) {
      fprintf(stderr,"Fatal Current Magnitude\n");
      exit(3);
    }  /**  Current magnitude not available  **/
    for (i=0;i<the_tube->segments;i++) {
      ComputeColor(segptr->currentPhase, 
                   min_current_phase, 
                   max_current_phase, 
                   wire_color);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wire_color);
      TKA_Cylinder(the_tube->width*TUBE_WIDTH_SCALE, seg_length, s, r);
      glTranslatef(0.0,0.0,seg_length);
      segptr = segptr->next;
    }  /**  For each segment  **/

  } else {  /**  Draw wall  **/
    glTranslatef(p1x, p1y, p1z);
    glScalef(dx, dy, dz);
    glTranslatef(0.5, 0.5, 0.5);  
    TKA_Cube(1.0);
  }  /**  Not a tube  **/

}  /**  End of DrawWireCurrentPhase **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             End of VisWires.c                           **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/






