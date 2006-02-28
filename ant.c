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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
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

double    SCALE_FACTOR;               /**  Scale factor for antenna         **/
double    DEFAULT_BOOMHEIGHT;         /**  In case no ground specd          **/
double    POINT_DIST_SCALE;           /**  For point clouds, in dBi         **/
double    POINT_SIZE_SCALE;           /**    ..also in terms of dBi         **/
double    STEP_SIZE;                  /**  Degrees between control points   **/
double    NULL_THRESHOLD;             /**  As a percentage of max dBi       **/
double    NULL_DISTANCE;              /**  So null maps float above         **/
double    ALPHA;                      /**  Alpha transparency factor        **/
double    TUBE_WIDTH_SCALE=2;         /**  Tube width scale                 **/
double    curr_step_size;             /**  Updated when the NEC called      **/
int       WireDrawMode;               /**  Mode to draw the wires in        **/
int       MultipleAntMode;            /**  Current antenna or all in phase  **/
int       ShowRadPat;                 /**  Show radiation pattern?          **/
int       ShowPolSense;               /**  Show polarization sense?         **/
int       ShowPolTilt;                /**  Show polarization tilt?          **/
int       ShowAxialRatio;             /**  Show axial ratios?               **/
int       ShowNulls;                  /**  Show nulls in pattern?           **/
int       DrawMode = 0;               /**  Mode to draw output in           **/
int       FreqSteps;                  /**  Frequency steps                  **/
AntArray  TheAnts;                    /**  The antennas' geometries         **/
bool      FieldDataComputed = false;  /**  Do we need to compute field?     **/
bool      RFPowerDensityOn = false;   /**  Draw RF Power Density?           **/
bool      AntennasInScene = false;    /**  Are there antennas yet?          **/
Point     Center;                     /**  Center of scene                  **/


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
/**                                DrawAxes                                 **/
/**                                                                         **/
/**  Debugging function.                                                    **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DrawAxes() {

  GLfloat  fcolor[4];  /**  Color  **/

  fcolor[0] = 1.0; fcolor[1] = 0.0; fcolor[2] = 0.0; fcolor[3] = 1.0; 
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fcolor);
  glLineWidth(2.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
  glEnd();
  fcolor[0] = 0.0; fcolor[1] = 1.0; fcolor[2] = 0.0; fcolor[3] = 1.0; 
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fcolor);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
  glEnd();
  fcolor[0] = 0.0; fcolor[1] = 0.0; fcolor[2] = 1.0; fcolor[3] = 1.0; 
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fcolor);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
  glEnd();

} /**  End of DrawAxes  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                               InsertTube                                **/
/**                                                                         **/
/**  Add an element to the antenna.                                         **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void InsertTubeR(Tube *root, Tube *the_tube) {

  if(root == NULL)
    return;
  else if(root->next == NULL) {
    root->next = (Tube*)malloc(sizeof(Tube));  
    *root->next = *the_tube;
    root->next->next = NULL;
  }
  else
    InsertTubeR(root->next, the_tube);
    
}  /**  End of InsertTubeR **/


void InsertTube(Ant *the_ant, Tube *the_tube) {

  if(the_ant->first_tube == NULL) {
    the_ant->first_tube = (Tube*)malloc(sizeof(Tube));  
    *the_ant->first_tube = *the_tube;
    the_ant->first_tube->next = NULL;
  } else
    InsertTubeR(the_ant->first_tube, the_tube);

  the_ant->tube_count++;

}  /**  End of InsertTube  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                SetPoint                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void SetPoint(Point *p, double x, double y, double z) {

  p->x = x;
  p->y = y;
  p->z = z;

}  /**  End of SetPoint  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                               Square double                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


double sqr(double x) {

  return(x*x);

}  /**  End of sqr  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                PointDist                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


double PointDist(Point p1, Point p2) {

  double  result;  /**  Distance  **/

  result = (sqr(p2.x-p1.x) + sqr(p2.y-p1.y) + sqr(p2.z-p1.z));
  return(sqrt(result));

}  /**  End of PointDist  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              ToggleDrawMode                             **/
/**                                                                         **/
/**  This is called by the Tk widget code and toggles the variable that     **/
/**  determines which mode to draw the antenna pattern in.                  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ToggleDrawMode(int mode) {

  DrawMode = mode;    

}  /**  End of ToggleDrawMode  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              ChangeFrequency                            **/
/**                                                                         **/
/**  This changes the frequency in MHz that the selected antenna gets       **/
/**  computed at by the NEC code.  This should be a positive value.         **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ChangeFrequency(double MHz) {

  if (MHz < 0)
    TheAnts.ants[TheAnts.curr_ant].frequency = 0.0;
  else
    TheAnts.ants[TheAnts.curr_ant].frequency = MHz;

}  /**  End of ToggleDrawMode  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                InitDisplay                              **/
/**                                                                         **/
/**  An inelegant mechanism for initializing some globals.  Oh well, what   **/
/**  can you do?                                                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void InitDisplay(void) {

  static int   here_before;  /**  Have we been here before  **/

  if(here_before == 0) {    
    if (AntennasInScene == false) 
    TheAnts.ant_count = 0;
    curr_step_size = 5;
    ShowRadPat     = 0;
    ShowPolSense   = 1;
    ShowPolTilt    = 0;
    ShowNulls      = 0;
    NULL_THRESHOLD = 0.85;
    NULL_DISTANCE  = 5.0;
    here_before    = 1;
    SetPoint(&Center, 0, 0, 0);
  }  /**  Thing we do once ever  **/

}  /**  End of InitDisplay  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                DisplayField                             **/
/**                                                                         **/
/**  Displays the radiation field of the antenna.  This is basically a      **/
/**  giant switch.                                                          **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DisplayField(Ant *antData) {

  if(RFPowerDensityOn == true) {

    glPushMatrix();
    if (DrawMode == 0) {
      if(ShowRadPat)
        DrawRFPowerDensityPoints(antData);
      if(ShowPolSense)
        DrawPolarizationSensePoints(antData);
      if(ShowPolTilt)
        DrawPolarizationTiltPoints(antData);
      if(ShowAxialRatio)
        DrawAxialRatioPoints(antData);
      if(ShowNulls)
        DrawShowNullsPoints(antData);
    }  /**  Points  **/

    else if (DrawMode == 1) {
      if(ShowRadPat)
        DrawRFPowerDensitySurface(antData);
      if(ShowPolSense)
        DrawPolarizationSenseSurface(antData);
      if(ShowPolTilt)
        DrawPolarizationTiltSurface(antData);
      if(ShowAxialRatio)
        DrawAxialRatioSurface(antData);
      if(ShowNulls)
        DrawShowNullsSurface(antData);
    }  /**  Surface  **/

    else if (DrawMode == 2) {
      if(ShowRadPat)
        DrawRFPowerDensitySphere(antData);
      if(ShowPolSense)
        DrawPolarizationSenseSphere(antData);
      if(ShowPolTilt)
        DrawPolarizationTiltSphere(antData);
      if(ShowAxialRatio)
        DrawAxialRatioSphere(antData);
      if(ShowNulls)
        DrawShowNullsSphere(antData);
    }  /**  Sphere  **/

    glPopMatrix();

  }  /**  Draw Power Density  **/

}  /**  End of DisplayField  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             DisplaySelectedAnt                          **/
/**                                                                         **/
/**  This will render the antenna on the screen with a boom and mast if     **/
/**  applicable.                                                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DisplaySelectedAnt(Ant *ant, GLint slices, GLint rings, bool selected) {

  double   boomlength;         /**  Length of the boom           **/
  double   boomcenter;         /**  Distance to boom center      **/
  double   boomheight;         /**  Height of boom above ground  **/
  double   boomwidth;          /**  Width of boom                **/
  double   boomshift;          /**  How far we need to center    **/
  GLfloat  selected_color[4];  /**  The selected color           **/

  if (selected == true)
    selected_color[0] = 1.0;
  else
    selected_color[0] = 0.5;
  selected_color[1] = 0.5;
  selected_color[2] = 0.5;
  selected_color[3] = 1.0;

  glPushMatrix();
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, selected_color);

  if (ant->tube_count != 0) {  /**  Display only with antenna  **/
    glScalef(ant->visual_scale, ant->visual_scale, ant->visual_scale);
    glTranslatef(ant->dx, ant->dy, ant->dz);

    /**  Antenna support structure  **/
    if ((ant->type == YAGI) || (ant->type == QUAD) ||(ant->type == DIPOLE)) {
      boomheight = GetBoomHeight(ant->first_tube);
    }  /**  Determine boomheight  **/
    if (ant->type == ELEVATED_VERTICAL) {
      boomheight = GetVerticalHeight(ant->first_tube);
    }  /**  Determine boomheight  **/
    
    if (ant->ground_specified == false) {
      if ((ant->type == YAGI) || 
          (ant->type == QUAD) ||
          (ant->type == DIPOLE) ||
          (ant->type == ELEVATED_VERTICAL)) {
        boomheight = DEFAULT_BOOMHEIGHT;
        boomheight = boomheight / SCALE_FACTOR;
      }  /**  Common gain antennas  **/
      if (ant->type == VERTICAL) {
        boomheight = 0.0;
      }  /**  Verticals  **/
    } else {
      boomheight = boomheight / SCALE_FACTOR;
    }  /**  Boomheight given  **/
    boomwidth = GetBoomWidth(ant->first_tube);
    if ((ant->type == YAGI) || 
        (ant->type == QUAD) ||
        (ant->type == DIPOLE) ||
        (ant->type == ELEVATED_VERTICAL)) {
      glPushMatrix();
      glRotatef(-90.0, 1.0 , 0.0, 0.0 ); 
      if (boomheight != 0.0)
        TKA_Cylinder(boomwidth * 1.5, boomheight, slices, rings);
      glPopMatrix(); 
    }  /**  Only for elevated antennas  **/

    /**  Antenna Boom  **/
    boomlength = GetBoomLength(ant->first_tube);
    boomlength = boomlength / SCALE_FACTOR;
    boomshift = GetBoomShift(ant->first_tube);
    boomshift = boomshift / SCALE_FACTOR;
    boomcenter = (boomlength / 2.0) + boomshift;
    if ((ant->type == YAGI) || 
        (ant->type == QUAD)) {
      glPushMatrix();
      glTranslatef((boomlength / -2.0), boomheight , 0.0);
      glRotatef(90.0, 0.0, 1.0, 0.0); 
      if (boomlength != 0.0)
        TKA_Cylinder(boomwidth, boomlength, slices, rings);
      glPopMatrix(); 
    }  /**  Only for antennas with booms  **/
 
  }  /**  Draw only with antenna  **/

  /**  Antenna elements  **/
  glPushMatrix();
  if (ant->ground_specified == false) {
    glTranslatef((boomcenter * -1.0), boomheight*1, 0.0);
  } else {
    glTranslatef((boomcenter * -1.0), boomheight*0, 0.0);
  }  /**  Move into position  **/

  if (ant->fieldComputed == false) {
    DrawTubeList(ant->first_tube, ant->current_tube, slices, rings);
  } else {
    if (WireDrawMode == 0) {
      DrawTubeList(ant->first_tube, ant->current_tube, slices, rings);
    } else if (WireDrawMode == 1) {
      DrawWireCurrentMagnitudeList(ant->first_tube, 
                                   ant->current_tube, 
                                   ant->max_current_mag, 
                                   ant->min_current_mag,
                                   slices, 
                                   rings);
    } else if (WireDrawMode == 2) {
      DrawWireCurrentPhaseList(ant->first_tube, 
                               ant->current_tube, 
                               ant->max_current_phase, 
                               ant->min_current_phase,
                               slices, 
                               rings);
      /*
      DrawTubeList(ant->first_tube, ant->current_tube, slices, rings);
      */
    }  /**  How to draw wires  **/
  }  /**  Only if field computed  **/

  glPopMatrix();

  /**  Radiation field  **/
  glPushMatrix();
  if ((ant->fieldComputed == true) && (MultipleAntMode == 0)) {
    glTranslatef(0.0, boomheight, 0.0);
    glScalef((1.0/ant->visual_scale), 
             (1.0/ant->visual_scale), 
             (1.0/ant->visual_scale));
    DisplayField(ant);
  }  /**  Only if field is computed  **/
  glPopMatrix(); 

  glPopMatrix(); 

}  /**  End of DisplaySelectedAnt  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                DisplayAnt                               **/
/**                                                                         **/
/**  This should be DisplayAnts plural.                                     **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DisplayAnt(GLint slices, GLint rings) {

  double  median_x;    /**  Median X value               **/
  double  median_y;    /**  Median Y value               **/
  double  median_z;    /**  Median Z value               **/
  double  boomheight;  /**  Height of boom above ground  **/
  int     i;           /**  Loop counter                 **/
  int     j;           /**  Loop counter                 **/

  InitDisplay();

  if (MultipleAntMode == 1) {

    /**  Antenna support structure  **/
    if ((TheAnts.ants[TheAnts.curr_ant].type == YAGI) || 
        (TheAnts.ants[TheAnts.curr_ant].type == QUAD) ||
        (TheAnts.ants[TheAnts.curr_ant].type == DIPOLE)) {
      boomheight = GetBoomHeight(TheAnts.ants[TheAnts.curr_ant].first_tube);
    }  /**  Determine boomheight  **/
    if (TheAnts.ants[TheAnts.curr_ant].type == ELEVATED_VERTICAL) {
      boomheight=GetVerticalHeight(TheAnts.ants[TheAnts.curr_ant].first_tube);
    }  /**  Determine boomheight  **/
    
    if (TheAnts.ants[TheAnts.curr_ant].ground_specified == false) {
      if ((TheAnts.ants[TheAnts.curr_ant].type == YAGI) || 
          (TheAnts.ants[TheAnts.curr_ant].type == QUAD) ||
          (TheAnts.ants[TheAnts.curr_ant].type == DIPOLE) ||
          (TheAnts.ants[TheAnts.curr_ant].type == ELEVATED_VERTICAL)) {
        boomheight = DEFAULT_BOOMHEIGHT;
        boomheight = boomheight / SCALE_FACTOR;
      }  /**  Common gain antennas  **/
      if (TheAnts.ants[TheAnts.curr_ant].type == VERTICAL) {
        boomheight = 0.0;
      }  /**  Verticals  **/
    } else {
      boomheight = boomheight / SCALE_FACTOR;
    }  /**  Boomheight given  **/

    median_x = 0.0;
    median_y = 0.0;
    median_z = 0.0;
    for (j=0;j<TheAnts.ant_count;j++) {
      median_x = median_x + TheAnts.ants[j].dx;      
      median_y = median_y + TheAnts.ants[j].dy;      
      median_z = median_z + TheAnts.ants[j].dz;      
    }  /**  For each antenna  **/
    if (median_x != 0.0) 
      median_x = median_x / TheAnts.ant_count;
    if (median_y != 0.0) 
      median_y = median_y / TheAnts.ant_count;
    if (median_z != 0.0) 
      median_z = median_z / TheAnts.ant_count;
    median_y = median_y + boomheight;
  }  /**  Display field in center of all antennas  **/

  for(i=0; i < TheAnts.ant_count; i++) {
    if (i == TheAnts.curr_ant) {
      DisplaySelectedAnt(&TheAnts.ants[i], slices, rings, true);
    } else {
      DisplaySelectedAnt(&TheAnts.ants[i], slices, rings, false);
    }  /**  Normal antenna  **/
  }  /**  For each antenna  **/

  /**  Radiation field  **/
  glPushMatrix();
  if (MultipleAntMode == 1) {
    glTranslatef(median_x, median_y, median_z);
    glScalef((1.0/TheAnts.ants[TheAnts.curr_ant].visual_scale), 
             (1.0/TheAnts.ants[TheAnts.curr_ant].visual_scale), 
             (1.0/TheAnts.ants[TheAnts.curr_ant].visual_scale));
    DisplayField(&TheAnts.ants[TheAnts.curr_ant]);
  }  /**  Only if field is computed  **/
  glPopMatrix(); 


}  /**  End of DisplayAnt  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  InitAnt                                **/
/**                                                                         **/
/**  An inelegant mechanism for intializing some variables in the antenna   **/
/**  data structure.                                                        **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void InitAnt(Ant *ant) {

  ant->first_tube = NULL;
  ant->tube_count = 0;
  ant->fieldData = NULL;
  ant->dx = 0.0;
  ant->dy = 0.0;
  ant->dz = 0.0;

}  /**  End of InitAnt  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 ReadFile                                **/
/**                                                                         **/
/**  This is the callback function that loads an antenna into memory.       **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ReadFile(char *file_name) {

  if (AntennasInScene == false) {
    TheAnts.ant_count = 0;
    AntennasInScene = true;
  }  /**  Initialize antenna count  **/    
  TheAnts.ant_count++;
  ChangeCurrentAnt(1);
  InitAnt(&TheAnts.ants[TheAnts.curr_ant]);
  TheAnts.ants[TheAnts.curr_ant].tube_count = 0;
  TheAnts.ants[TheAnts.curr_ant].fieldComputed = false;
  ReadCardFile(file_name, &TheAnts.ants[TheAnts.curr_ant]);
  RFPowerDensityOn = false;
  TheAnts.ants[TheAnts.curr_ant].current_tube = 
    TheAnts.ants[TheAnts.curr_ant].first_tube;

}  /**  End of ReadFile  **/



/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            DeleteCurrentAnt                             **/
/**                                                                         **/
/**  There may be a memory leak here.                                       **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void DeleteCurrentAnt(void) {

  int i;  /**  Array index  **/

  for(i = TheAnts.curr_ant; i < TheAnts.ant_count-1; i++) {
    TheAnts.ants[i] = TheAnts.ants[i+1];
  }  /**  Locate current antenna  **/

  if(TheAnts.ant_count > 0)
    TheAnts.ant_count--;

  if(TheAnts.curr_ant >= TheAnts.ant_count)
    TheAnts.curr_ant--;

}  /**  End of DeleteCurrentTube  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              MoveCurrentAnt                             **/
/**                                                                         **/
/**  Moves the current antenna position.                                    **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void MoveCurrentAnt(double dx, double dy, double dz) {

  if(TheAnts.ants[TheAnts.curr_ant].current_tube != NULL) {
    TheAnts.ants[TheAnts.curr_ant].dx += dx;
    TheAnts.ants[TheAnts.curr_ant].dy += dy;
    TheAnts.ants[TheAnts.curr_ant].dz += dz;
  }  /**  While more elements  **/

}  /**  End of MoveCurrentAnt  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              MoveCurrentTube                            **/
/**                                                                         **/
/**  Moves the current antenna element.                                     **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void MoveCurrentTube(double dx, double dy, double dz) {

  Tube *current;  /**  Current tube  **/
 
  if(TheAnts.ants[TheAnts.curr_ant].current_tube != NULL) {
    current = TheAnts.ants[TheAnts.curr_ant].current_tube;
    if (current->type == IS_TUBE) {
      TheAnts.ants[TheAnts.curr_ant].current_tube->e1.x += dx;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e1.y += dy;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e1.z += dz;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e2.x += dx;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e2.y += dy;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e2.z += dz;
    }  /**  Only for elements  **/
  }  /**  While more elements  **/

}  /**  End of MoveCurrentTube  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              MoveCurrentWall                            **/
/**                                                                         **/
/**  Moves the current wall.                                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void MoveCurrentWall(double dx, double dy, double dz) {

  Tube *current;  /**  Current tube  **/
 
  if(TheAnts.ants[TheAnts.curr_ant].current_tube != NULL) {
    current = TheAnts.ants[TheAnts.curr_ant].current_tube;
    if (current->type == IS_WALL) {
      TheAnts.ants[TheAnts.curr_ant].current_tube->e1.x += dx;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e1.y += dy;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e1.z += dz;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e2.x += dx;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e2.y += dy;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e2.z += dz;
    }  /**  Only for walls  **/
  }  /**  While more elements  **/

}  /**  End of MoveCurrentWall  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              MoveCenter                                 **/
/**                                                                         **/
/**  Moves the viewing center.                                              **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void TransformPoint(Point *old_point, double azimuth, double elevation, 
                    double dist, Point *new_point)
{
  Point save_old;
  save_old = *old_point;

  new_point->y =  -dist*sin(elevation/57.3);
  new_point->z = dist*cos(elevation/57.3);

  new_point->x = new_point->z*sin(azimuth/57.3);
  new_point->z = new_point->z*cos(azimuth/57.3);

 /* new_point->x = dist*sin(azimuth/57.3);
  new_point->z = dist*cos(azimuth/57.3);

  new_point->y =  -new_point->z*sin(elevation/57.3);
  new_point->z = new_point->z*cos(elevation/57.3);*/

  new_point->x += save_old.x;
  new_point->y += save_old.y;
  new_point->z += save_old.z;
}/*TransformPoint*/



void DoMoveCenter(double x, double y, double azimuth, double elevation, 
  double dist) {


  TransformPoint(&Center, -azimuth - 90, 0, -x/35.0, &Center);
  TransformPoint(&Center, -azimuth, -elevation -90, -y/35.0, &Center);

  /*Center.x += x;
  Center.y += y; */


}


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            RotateCurrentTube                            **/
/**                                                                         **/
/**  Causes the current antenna element to rotate about its center.         **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void RotateCurrentTube(double rx, double ry, double rz) {

  Point   e1;      /**  Left endpoint   **/
  Point   e2;      /**  Right endpoint  **/
  Point   center;  /**  Pivot point     **/
  double  x;       /**  Computed value  **/
  double  y;       /**  Computed value  **/

  if(TheAnts.ants[TheAnts.curr_ant].current_tube != NULL) {
    if(TheAnts.ants[TheAnts.curr_ant].current_tube->type == IS_TUBE) {

      e1 = TheAnts.ants[TheAnts.curr_ant].current_tube->e1;
      e2 = TheAnts.ants[TheAnts.curr_ant].current_tube->e2;
      center.x = (e1.x + e2.x)/2;
      center.y = (e1.y + e2.y)/2;
      center.z = (e1.z + e2.z)/2;
      e1.x -= center.x;
      e1.y -= center.y;
      e1.z -= center.z;
      e2.x -= center.x;
      e2.y -= center.y;
      e2.z -= center.z;

      y = e1.y*cos(rx/57.3) - e1.z*sin(rx/57.3);
      e1.z = e1.y*sin(rx/57.3) + e1.z*cos(rx/57.3);
      e1.y = y;

      y = e2.y*cos(rx/57.3) - e2.z*sin(rx/57.3);
      e2.z = e2.y*sin(rx/57.3) + e2.z*cos(rx/57.3);
      e2.y = y;

      x = e1.x*cos(ry/57.3) + e1.z*sin(ry/57.3);
      e1.z = -e1.x*sin(ry/57.3) + e1.z*cos(ry/57.3);
      e1.x = x;

      x = e2.x*cos(ry/57.3) + e2.z*sin(ry/57.3);
      e2.z = -e2.x*sin(ry/57.3) + e2.z*cos(ry/57.3);
      e2.x = x;

      x = e1.x*cos(rz/57.3) - e1.y*sin(rz/57.3);
      e1.y = e1.x*sin(rz/57.3) + e1.y*cos(rz/57.3);
      e1.x = x;

      x = e2.x*cos(rz/57.3) - e2.y*sin(rz/57.3);
      e2.y = e2.x*sin(rz/57.3) + e2.y*cos(rz/57.3);
      e2.x = x;

      e1.x += center.x;
      e1.y += center.y;
      e1.z += center.z;
      e2.x += center.x;
      e2.y += center.y;
      e2.z += center.z;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e1 = e1;
      TheAnts.ants[TheAnts.curr_ant].current_tube->e2 = e2;
    }  /**  Only for elements  **/
  }  /**  While more elements  **/

}  /**  End of RotateCurrentTube  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          RotateCurrentAntenna                           **/
/**                                                                         **/
/**  Causes the current antenna to rotate about its mast.                   **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void RotateCurrentAntenna(double rx, double ry, double rz) {

  Point   e1;      /**  Left endpoint   **/
  Point   e2;      /**  Right endpoint  **/
  Point   center;  /**  Pivot point     **/
  double  x;       /**  Computed value  **/
  double  y;       /**  Computed value  **/

  if(TheAnts.ants[TheAnts.curr_ant].current_tube != NULL) {

    e1 = TheAnts.ants[TheAnts.curr_ant].current_tube->e1;
    e2 = TheAnts.ants[TheAnts.curr_ant].current_tube->e2;
    center.x = (e1.x + e2.x)/2;
    center.y = (e1.y + e2.y)/2;
    center.z = (e1.z + e2.z)/2;
    e1.x -= center.x;
    e1.y -= center.y;
    e1.z -= center.z;
    e2.x -= center.x;
    e2.y -= center.y;
    e2.z -= center.z;

    y = e1.y*cos(rx/57.3) - e1.z*sin(rx/57.3);
    e1.z = e1.y*sin(rx/57.3) + e1.z*cos(rx/57.3);
    e1.y = y;

    y = e2.y*cos(rx/57.3) - e2.z*sin(rx/57.3);
    e2.z = e2.y*sin(rx/57.3) + e2.z*cos(rx/57.3);
    e2.y = y;

    x = e1.x*cos(ry/57.3) + e1.z*sin(ry/57.3);
    e1.z = -e1.x*sin(ry/57.3) + e1.z*cos(ry/57.3);
    e1.x = x;

    x = e2.x*cos(ry/57.3) + e2.z*sin(ry/57.3);
    e2.z = -e2.x*sin(ry/57.3) + e2.z*cos(ry/57.3);
    e2.x = x;

    x = e1.x*cos(rz/57.3) - e1.y*sin(rz/57.3);
    e1.y = e1.x*sin(rz/57.3) + e1.y*cos(rz/57.3);
    e1.x = x;

    x = e2.x*cos(rz/57.3) - e2.y*sin(rz/57.3);
    e2.y = e2.x*sin(rz/57.3) + e2.y*cos(rz/57.3);
    e2.x = x;

    e1.x += center.x;
    e1.y += center.y;
    e1.z += center.z;
    e2.x += center.x;
    e2.y += center.y;
    e2.z += center.z;
    TheAnts.ants[TheAnts.curr_ant].current_tube->e1 = e1;
    TheAnts.ants[TheAnts.curr_ant].current_tube->e2 = e2;

  }  /**  While more elements  **/

}  /**  End of RotateCurrentAntenna  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            ScaleCurrentWall                             **/
/**                                                                         **/
/**  Changes the size of the obstruction.                                   **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ScaleCurrentWall(double sx, double sy, double sz) {

  Tube *current;  /**  The element  **/
 
  current = TheAnts.ants[TheAnts.curr_ant].current_tube;
  if (current->type == IS_WALL) {
    TheAnts.ants[TheAnts.curr_ant].current_tube->e2.x += sx;
    TheAnts.ants[TheAnts.curr_ant].current_tube->e2.y += sy;
    TheAnts.ants[TheAnts.curr_ant].current_tube->e2.z += sz;
  }  /**  Only affect walls  **/

}  /**  End of ScaleCurrentWall  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            ScaleCurrentTube                             **/
/**                                                                         **/
/**  Changes the physical length and diameter of the current antenna        **/
/**  element.                                                               **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ScaleCurrentTube(double scale, double width) {

  double  cx;       /**  Point c value X  **/
  double  cy;       /**  Point c value Y  **/
  double  cz;       /**  Point c value Z  **/
  double  dx;       /**  Point d value X  **/
  double  dy;       /**  Point d value Y  **/
  double  dz;       /**  Point d value Z  **/
  Tube   *current;  /**  The element      **/
  Ant     TheAnt;   /**  The antenna      **/
 
  current = TheAnts.ants[TheAnts.curr_ant].current_tube;
  if (current->type == IS_TUBE) {

    TheAnt = TheAnts.ants[TheAnts.curr_ant];
 
    if(TheAnt.current_tube != NULL) {

      cx = (TheAnt.current_tube->e2.x + TheAnt.current_tube->e1.x) / 2.0;
      cy = (TheAnt.current_tube->e2.y + TheAnt.current_tube->e1.y) / 2.0;
      cz = (TheAnt.current_tube->e2.z + TheAnt.current_tube->e1.z) / 2.0;
      dx = TheAnt.current_tube->e1.x - cx;
      dy = TheAnt.current_tube->e1.y - cy;
      dz = TheAnt.current_tube->e1.z - cz;
      if((dx*dx + dy*dy + dz*dz > 0.01) || (scale > 0)) {
        TheAnt.current_tube->e1.x = cx + dx*(1 + scale);
        TheAnt.current_tube->e1.y = cy + dy*(1 + scale);
        TheAnt.current_tube->e1.z = cz + dz*(1 + scale);
        TheAnt.current_tube->e2.x = cx - dx*(1 + scale);
        TheAnt.current_tube->e2.y = cy - dy*(1 + scale);
        TheAnt.current_tube->e2.z = cz - dz*(1 + scale);
      }  /**  Assuming things are visible at all  **/
      TheAnt.current_tube->width *= (width + 1);
      if(TheAnt.current_tube->width < 0.001)
        TheAnt.current_tube->width = 0.001;
    }  /**  While there are still elements  **/
  }  /**  Only if it's not a wall  **/

}  /**  End of MoveCurrentTube  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                           ChangeCurrentAnt                              **/
/**                                                                         **/
/**  Causes the next antenna in the scene to be selected.                   **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ChangeCurrentAnt(int count) {

  if(TheAnts.ant_count != 0) {
    TheAnts.curr_ant++;
    if(TheAnts.curr_ant >= TheAnts.ant_count)
      TheAnts.curr_ant = 0;
  }  /**  Increment current pointer  **/
  
}  /**  End of ChangeCurrentAnt  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                           ChangeCurrentTube                             **/
/**                                                                         **/
/**  Causes the next element in the antenna to be selected.                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ChangeCurrentTube(int count) {

  Ant  TheAnt;  /**  Temporary storage  **/

  TheAnt = TheAnts.ants[TheAnts.curr_ant];

  while(count > 0) {
    if(TheAnt.current_tube == NULL)
      TheAnt.current_tube = TheAnt.first_tube;
    else {
      TheAnt.current_tube = TheAnt.current_tube->next;
      if(TheAnt.current_tube == NULL)
        TheAnt.current_tube = TheAnt.first_tube;
    }  /**  While still elements  **/
    count--;
  }  /**  Traverse element list  **/

  TheAnts.ants[TheAnts.curr_ant] = TheAnt;

}  /**  End of ChangeCurrentTube **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            GenerateNECFile                              **/
/**                                                                         **/
/**  This function traverses the details of the antenna we have stored in   **/
/**  memory and output a .nec file that NEC2 will load by default.          **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void GenerateNECFile(char *file_name) {

  curr_step_size = STEP_SIZE;

  if (MultipleAntMode == 0) {
    WriteCardFile(file_name, 
                 &TheAnts.ants[TheAnts.curr_ant], 
                  STEP_SIZE, 
                  TheAnts.ants[TheAnts.curr_ant].frequency);
  } else if (MultipleAntMode == 1) {
    WriteMultAntsFile(file_name, 
                      STEP_SIZE, 
                      TheAnts.ants[TheAnts.curr_ant].frequency);
  }  /**  Single or all antennas  **/

}  /**  End of GenerateNECFile  **/


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  AddWall                                **/
/**                                                                         **/
/**  The callback function for adding an obstruction into the scene.        **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void AddWall(void) {

  Tube a_tube;  /**  Wall element to be added   **/

  a_tube.width = 1;
  a_tube.type = IS_WALL;
  SetPoint(&a_tube.e1, -2, 0, 0);
  SetPoint(&a_tube.e2, -1, 2, 4);
  InsertTube(&TheAnts.ants[TheAnts.curr_ant], &a_tube);

}  /**  End of AddWall  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             ComputeField                                **/
/**                                                                         **/
/**  This function calls the NEC2 code to recompute the field of radiation  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ComputeField(bool changed) {

  FILE *fin;        /**  Input file               **/
  long  parentpid;  /**  Process ID of parent     **/
  long  childpid;   /**  Process ID of child      **/
  int   status;     /**  Status of child process  **/
  int   ferror;     /**  File access error        **/

  /**  Check to see if antennas exist  **/
  if (AntennasInScene == true) {
  
    if ((TheAnts.ants[TheAnts.curr_ant].fieldComputed == false) ||
        (changed == true)){

      ferror = remove("output.nec");
      ferror = remove("input.nec");

      /**  Output our current antenna to disk  **/
      GenerateNECFile("input.nec"); 
  
      /**  Run NEC code on that file  **/
      parentpid = getpid();
      if ((childpid = fork()) < 0) {
        fprintf(stderr,"Can't fork\n");
        exit(-2);
      }  /**  Error state  **/
      if (childpid == 0) {  /**  We are child  **/
        printf("Running NEC2 code...  please stand by...\n");
        execlp("nec2", "nec2", "input.nec", "output.nec", NULL);  
        exit(-1);  /**  We should never reach here  **/
      }  /**  We are child  **/
      else {  /**  We are parent  **/
        if (wait(&status) != childpid) {
          fprintf(stderr,"Problem with child\n");
          exit(-3);
        }  /**  Error state  **/
      }  /**  We are parent  **/
  
      /**  Read in results from disk  **/
      fin  = fopen("output.nec", "rt");
      if(fin != NULL) {
        ParseFieldData(fin, &TheAnts.ants[TheAnts.curr_ant], true, true);
        fclose(fin);
      } 
      else {
        fprintf(stderr, "Could Not Open File output.nec!!!\n");
        return;
      }
      /*
      ferror = remove("output.nec");
      ferror = remove("input.nec"); 
      */
      printf("Field computation complete.\n");
 
      RFPowerDensityOn = true;
      FieldDataComputed = true;

    }  /**  Need to compute field  **/

  }  /**  Antennas are in scene  **/

}  /**  End of ComputeField  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              End of ant.c                               **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/






