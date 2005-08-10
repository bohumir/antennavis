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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <tcl.h>
#include <tk.h>
#include "togl.h"
#include "MyTypes.h"
#include "TkAntenna.h"
#include "ParseArgs.h"
#include "ant.h"


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              Definitions                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


#define PIXEL_PER_MM       3.84  /**  Pixels per millimeter                 **/
#define SCREEN_DISTANCE     500  /**  Distance of the user to the screen    **/
#define SCREEN_OFFSET       0.1  /**  Empty space in front of ground plane  **/
#define WINDOW_SIZE         500  /**  Initial size of the window in pixels  **/
#define WINDOW_OFFSET       200  /**  Initial x/y offset of the window      **/
#define EYE_LONGITUDE_INIT  0.0  /**  Initial location of viewer            **/
#define EYE_LATITUDE_INIT  30.0  /**  Initial location of viewer            **/
#define EYE_DISTANCE_MAX    5    /**  Relative max. distance to antenna     **/
#define EYE_DISTANCE_MIN    0.2  /**  Relative min. distance to antenna     **/
#define GPLANE_DISTANCE     2.0  /**  Distance to ground plane              **/
#define GPLANE_OFFSET       1.0  /**  Ground plane offset                   **/
#define GPLANE_RADIUS      15.0  /**  Radius of the ground plane            **/
#define LIGHTMAX            8    /**  Number of lights                      **/
#define SLICES_INIT         24   /**  Disks and cylinders have many slices  **/
#define RINGS_INIT          1    /**  Disks and cylinders have many rings   **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             Global Variables                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


extern double  SCALE_FACTOR;        /**  Antenna scale factor             **/
extern double  DEFAULT_BOOMHEIGHT;  /**  No height above ground specd     **/
extern double  POINT_DIST_SCALE;    /**  For point clouds, mult of dBi    **/
extern double  POINT_SIZE_SCALE;    /**    ..also in terms of dBi         **/
extern double  STEP_SIZE;           /**  Degrees between control points   **/
extern double  NULL_THRESHOLD;      /**  As a percentage of max dBi       **/
extern double  NULL_DISTANCE;       /**  So null maps float above         **/
extern double  ALPHA;               /**  Alpha transparency factor        **/
extern int     WireDrawMode;        /**  Mode to draw the wires in        **/
extern int     MultipleAntMode;     /**  Current antenna or all in phase  **/
extern int     ShowRadPat;          /**  Do we show radiation pattern     **/
extern int     ShowPolSense;        /**  Do we show polarization sense?   **/
extern int     ShowPolTilt;         /**  Do we show polarization tilt?    **/
extern int     ShowAxialRatio;      /**  Show axial ratios?               **/
extern int     ShowNulls;           /**  Do we show nulls in pattern?     **/
extern int     FreqSteps;           /**  Number of frequencies            **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                           Structs and Types                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


enum MaterialType {Antenna, Gplane, Cube, SizeOfMaterialType};


enum LightType {off, directional, positional, spot};


struct Material {
  bool     SmoothShade;  /**  SmoothShade  **/
  GLfloat  Ambient[4];   /**  Ambient      **/
  GLfloat  Diffuse[4];   /**  Diffuse      **/
  GLfloat  Specular[4];  /**  Specular     **/
  GLfloat  Emission[4];  /**  Emmission    **/
  GLfloat  Shininess;    /**  Shininess    **/
};  /**  End of Material  **/


struct Light {
  GLenum          Number;                 /**  Number of light  **/
  enum LightType  Type;	                  /**  Type of Light    **/
  GLfloat         Ambient[4];             /**  Ambient          **/
  GLfloat         Diffuse[4];             /**  Diffuse          **/
  GLfloat         Specular[4];            /**  Specular         **/
  GLfloat         Longitude;              /**  Longitude        **/
  GLfloat         Latitude;               /**  Latitude         **/
  GLfloat         Distance;               /**  Distance         **/
  GLfloat         Position[4];            /**  Position         **/
  GLint           Spot_x;                 /**  Spotlight        **/
  GLint           Spot_y;                 /**  Spotlight        **/
  GLfloat         Spot_Pos[4];            /**  Spotlight        **/
  GLfloat         Spot_Exponent;          /**  Spotlight        **/
  GLfloat         Spot_Cutoff;            /**  Spotlight        **/
  GLfloat         Attenuation_Constant;   /**  Attenuation      **/
  GLfloat         Attenuation_Linear;     /**  Attenuation      **/
  GLfloat         Attenuation_Quadratic;  /**  Attenuation      **/
};  /**  End of Light  **/


struct Antenna {
  GLfloat          Eye_Longitude;                 /**  Eye position      **/
  GLfloat          Eye_Latitude;                  /**  Eye position      **/
  GLfloat          Eye_Distance;                  /**  Eye position      **/
  GLfloat          Eye_Distance_Min;              /**  Eye position      **/
  GLfloat          Eye_Distance_Max;              /**  Eye position      **/
  GLfloat          Global_Ambient[4];             /**  Ambient light     **/
  GLfloat          Global_Background[4];          /**  Background light  **/
  bool             Global_Viewer;                 /**  Global viewer     **/
  GLint            Global_Slices;                 /**  Global slices     **/
  GLint            Global_Rings;                  /**  Global rings      **/
  struct Material  material[SizeOfMaterialType];  /**  Materials         **/
  struct Light     light[LIGHTMAX];               /**  Lights            **/ 
};  /**  End of Antenna  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          Function Prototypes                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void    TKA_Display(struct Togl *togl);
local void    TKA_Reshape(struct Togl *togl);
local void    TKA_Rotate(GLfloat ax, GLfloat ay, GLfloat az);
local GLfloat TKA_Angle(GLfloat size, GLfloat distance);
local void    TKA_Disk(GLfloat radius, GLint slices, GLint rings);
local void    TKA_ErrorCallback(GLenum errorCode);
      void    TKA_Cylinder(GLfloat radius,
                           GLfloat height,
                             GLint slices,
                             GLint rings);
      void    TKA_Cube(GLfloat size);
local void    TKA_Create(struct Togl *togl);
local void    TKA_Destroy(struct Togl *togl);
local GLint   TKA_Reset(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_Eye(struct Togl *togl, GLint argc, char **argv);
local void    TKA_SetMaterial(struct Material *m);
local void    TKA_SetLight(struct Light *l, GLfloat eyemin, GLfloat eyemax);
local GLint   TKA_Global(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_Material(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_Light(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_StdAnt(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_LoadAnt(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_DeleteAnt(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_AddElement(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_ControlTube(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_ControlAnt(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_ChangeCurrentTube(struct Togl *togl, GLint argc,char **argv);
local GLint   TKA_ChangeCurrentAnt(struct Togl *togl, GLint argc,char **argv);
local GLint   TKA_DrawRFPowerDensity(struct Togl  *togl, 
                                           GLint   argc, 
                                            char **argv);
local GLint   TKA_SaveFile(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_SaveRGBImage(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_MoveCenter(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_GetVariable(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_ChangeDrawMode(struct Togl *togl, GLint argc, char **argv);
local GLint   TKA_ChangeWireDrawMode(struct Togl  *togl, 
                                           GLint   argc,
                                            char **argv);
local GLint   TKA_ChangeAntMode(struct Togl *togl, GLint argc, char **argv);


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            Global Variables                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLUquadricObj *Qobj;  /**  Quadratic object  **/

local struct PA_Config CfgEye[] = {
  {PA_FLOAT,"longitude",Tk_Offset(struct Antenna, Eye_Longitude)},
  {PA_FLOAT,"latitude",Tk_Offset(struct Antenna, Eye_Latitude)},
  {PA_FLOAT,"distance",Tk_Offset(struct Antenna, Eye_Distance)},
  {PA_END, NULL, 0}
};  /**  Configuration specification for eye position  **/


local struct PA_Config CfgGlobal[] = {
  {PA_RGB, "ambient", Tk_Offset( struct Antenna, Global_Ambient)},
  {PA_RGB, "background", Tk_Offset( struct Antenna, Global_Background)},
  {PA_BOOL, "viewer", Tk_Offset( struct Antenna, Global_Viewer)},
  {PA_INT, "slices", Tk_Offset( struct Antenna, Global_Slices)},
  {PA_INT, "rings", Tk_Offset( struct Antenna, Global_Rings)},
  {PA_END, NULL, 0}
};  /**  Configuration specification for global  **/


local struct PA_Config CfgMaterial[] = {
  {PA_BOOL, "smooth", Tk_Offset( struct Material, SmoothShade)},
  {PA_RGB, "ambient", Tk_Offset( struct Material, Ambient)},
  {PA_RGB, "diffuse", Tk_Offset( struct Material, Diffuse)},
  {PA_RGB, "specular", Tk_Offset( struct Material, Specular)},
  {PA_RGB, "emission", Tk_Offset( struct Material, Emission)},
  {PA_FLOAT, "shininess", Tk_Offset( struct Material, Shininess)},
  {PA_END, NULL, 0}
};  /**  Configuration specification for material  **/


local struct PA_Config CfgLight[] = {
  {PA_INT, "type", Tk_Offset( struct Light, Type)},
  {PA_RGB, "ambient", Tk_Offset( struct Light, Ambient)},
  {PA_RGB, "diffuse", Tk_Offset( struct Light, Diffuse)},
  {PA_RGB, "specular", Tk_Offset( struct Light, Specular)},
  {PA_FLOAT, "longitude", Tk_Offset( struct Light, Longitude)},
  {PA_FLOAT, "latitude", Tk_Offset( struct Light, Latitude)},
  {PA_FLOAT, "distance", Tk_Offset( struct Light, Distance)},
  {PA_INT, "x", Tk_Offset( struct Light, Spot_x)},
  {PA_INT, "y", Tk_Offset( struct Light, Spot_y)},
  {PA_FLOAT, "exponent", Tk_Offset( struct Light, Spot_Exponent)},
  {PA_FLOAT, "cutoff", Tk_Offset( struct Light, Spot_Cutoff)},
  {PA_FLOAT, "constant", Tk_Offset( struct Light, Attenuation_Constant)},
  {PA_FLOAT, "linear", Tk_Offset( struct Light, Attenuation_Linear)},
  {PA_FLOAT, "quadratic", Tk_Offset( struct Light, Attenuation_Quadratic)},
  {PA_END, NULL, 0}
};  /**  Configuration specification for light  **/


local GLenum LightNum[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
                           GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7}; 

local bool antennaChanged         = false;

/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                   Init                                  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


GLint TKA_Init(Tcl_Interp *interp) {

  srand(time(NULL));

  /**  Init quadratics  **/
  assert(Qobj = gluNewQuadric());
  gluQuadricCallback(Qobj, GLU_ERROR, TKA_ErrorCallback);
  gluQuadricDrawStyle(Qobj, GLU_FILL);
  gluQuadricNormals(Qobj, GLU_FLAT);

  /**  Setup callback functions  **/
  Togl_CreateFunc(TKA_Create);
  Togl_DestroyFunc(TKA_Destroy);
  Togl_DisplayFunc(TKA_Display);
  Togl_ReshapeFunc(TKA_Reshape);

  /**  Setup togl sub-commands  **/
  Togl_CreateCommand("reset", TKA_Reset);
  Togl_CreateCommand("eye", TKA_Eye);
  Togl_CreateCommand("add_element", TKA_AddElement);
  Togl_CreateCommand("global", TKA_Global);
  Togl_CreateCommand("material", TKA_Material);
  Togl_CreateCommand("light", TKA_Light);
  Togl_CreateCommand("std_ant", TKA_StdAnt);
  Togl_CreateCommand("load_ant", TKA_LoadAnt);
  Togl_CreateCommand("delete_ant", TKA_DeleteAnt);
  Togl_CreateCommand("control_tube", TKA_ControlTube);
  Togl_CreateCommand("control_ant", TKA_ControlAnt);
  Togl_CreateCommand("change_current_tube", TKA_ChangeCurrentTube);
  Togl_CreateCommand("change_current_ant", TKA_ChangeCurrentAnt);
  Togl_CreateCommand("draw_RFPowerDensity", TKA_DrawRFPowerDensity);
  Togl_CreateCommand("save_file", TKA_SaveFile);
  Togl_CreateCommand("save_rgb_image", TKA_SaveRGBImage);
  Togl_CreateCommand("move_center", TKA_MoveCenter);
  Togl_CreateCommand("get_var", TKA_GetVariable);
  Togl_CreateCommand("change_mode", TKA_ChangeDrawMode);
  Togl_CreateCommand("change_wire_mode", TKA_ChangeWireDrawMode);
  Togl_CreateCommand("change_ant_mode", TKA_ChangeAntMode);

  return TCL_OK;

}  /**  End of Init  **/

extern Point Center;

/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 Display                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void TKA_Display(struct Togl *togl) {

  struct Antenna *antenna = Togl_GetClientData(togl);  /**  Antenna data   **/
  GLint           s = antenna->Global_Slices;          /**  Slices         **/
  GLint           r = antenna->Global_Rings;           /**  Rings          **/

  glClearColor(antenna->Global_Background[0], 
               antenna->Global_Background[1],
               antenna->Global_Background[2], 
               antenna->Global_Background[3]); 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,
    (antenna->Global_Viewer) ? GL_TRUE : GL_FALSE);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, antenna->Global_Ambient);
  glMatrixMode(GL_MODELVIEW);

  /**  Eye point transformations  **/
  glLoadIdentity();
  glTranslatef(0.0, 
               0.0, 
               -(antenna->Eye_Distance * 
               (antenna->Eye_Distance_Max - antenna->Eye_Distance_Min) 
               + antenna->Eye_Distance_Min));
  TKA_Rotate(antenna->Eye_Latitude, 0.0, 0.0);
  TKA_Rotate(0.0, antenna->Eye_Longitude, 0.0);

  /**  Ground plane  **/
  glTranslatef(Center.x, Center.y, Center.z);
  TKA_SetMaterial(&(antenna->material[Gplane]));
  glPushMatrix();
  TKA_Rotate(-90.0, 0.0, 0.0); 
  TKA_Disk(GPLANE_RADIUS, s, r);
  glPopMatrix(); 

  DisplayAnt(s, r);

  Togl_SwapBuffers(togl);

}  /**  End of Display  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 Reshape                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void TKA_Reshape(struct Togl *togl) {

  struct Antenna *antenna = Togl_GetClientData(togl);  /**  Antenna data  **/
  GLint         w = Togl_Width(togl);                  /**  Width window  **/
  GLint         h = Togl_Height(togl);                 /**  Width height  **/

  glViewport(0,0,(GLsizei)w,(GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective((GLdouble)2.0 * TKA_Angle(h/(PIXEL_PER_MM * 2.0), 
    SCREEN_DISTANCE),(GLdouble) w/(GLdouble) h, 
    (GLdouble) antenna->Eye_Distance_Min - GPLANE_RADIUS,
    (GLdouble) antenna->Eye_Distance_Max + antenna->Eye_Distance_Min);

}  /**  End of Reshape  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 Rotate                                  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void TKA_Rotate(GLfloat ax, GLfloat ay, GLfloat az) {

  GLfloat  m[16];                 /**  Matrix       **/
  GLfloat  sx = sin(radian(ax));  /**  Sin of X     **/
  GLfloat  cx = cos(radian(ax));  /**  Cosine of X  **/
  GLfloat  sy = sin(radian(ay));  /**  Sin of Y     **/
  GLfloat  cy = cos(radian(ay));  /**  Cosine of Y  **/
  GLfloat  sz = sin(radian(az));  /**  Sin of Z     **/
  GLfloat  cz = cos(radian(az));  /**  Cosine of Z  **/

  m[ 0] = cy * cz;
  m[ 1] = cy * sz;
  m[ 2] = -sy;
  m[ 3] = 0;

  m[ 4] = sx * sy * cz - cx * sz;
  m[ 5] = sx * sy * sz + cx * cz;
  m[ 6] = sx * cy;
  m[ 7] = 0;

  m[ 8] = cx * sy * cz + sx * sz;
  m[ 9] = cx * sy * sz - sx * cz;
  m[10] = cx * cy;
  m[11] = 0;
  
  m[12] = 0;
  m[13] = 0;
  m[14] = 0;
  m[15] = 1;

  glMultMatrixf(m);

}  /**  End of Rotate  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  Angle                                  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLfloat TKA_Angle(GLfloat size, GLfloat distance) {

  return degree(atan2(size, distance));

}  /**  End of angle  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  Disk                                   **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void TKA_Disk(GLfloat radius, GLint slices, GLint rings) {

  gluDisk(Qobj, 0.0, (GLdouble) radius, slices, rings); 

}  /**  End of Disk  **/



/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              ErrorCallback                              **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void TKA_ErrorCallback(GLenum errorCode) {

  const  GLubyte  *estring;  /**  Error string  **/
  
  estring = gluErrorString(errorCode);
  fprintf(stderr, "%s FATAL QUADRATIC ERROR: %s\n", TKA_PrgName, estring);
  exit(EXIT_FAILURE);

}  /**  End of ErrorCallBack  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  Cylinder                               **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void TKA_Cylinder(GLfloat radius, GLfloat height, GLint slices, GLint rings) {

  glPushMatrix();
  glPushMatrix();
  glTranslatef(0.0, 0.0, height);
  TKA_Disk(radius, slices, rings);
  glPopMatrix();
  gluCylinder(Qobj, radius, radius, height, slices, rings);
  TKA_Rotate(180.0, 0.0, 180.0);
  TKA_Disk(radius, slices, rings);
  glPopMatrix();

}  /**  End of Cylinder  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                    Cube                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void TKA_Cube(GLfloat size) {

  glPushMatrix();  
  glTranslatef(0.0, 0.0, -size / 2.0);
  TKA_Rotate(0.0, 0.0, 45.0);
  glPushMatrix();
  glTranslatef(0.0, 0.0, size);
  gluDisk(Qobj, 0.0, (GLdouble) size / 1.414, 4, 1); 
  glPopMatrix();
  gluCylinder(Qobj, size / 1.414, size / 1.414, size, 4, 1);
  TKA_Rotate(180.0, 0.0, 0.0);
  gluDisk(Qobj, 0.0, (GLdouble) size / 1.414, 4, 1); 
  glPopMatrix();

}  /**  End of cube  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                   Create                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void TKA_Create(struct Togl *togl) {

  struct Antenna *antenna;  /**  Antenna            **/
  GLint           i;        /**  Loop counter       **/
  GLint           wm;       /**  Window size in mm  **/

  wm = WINDOW_SIZE*(1-SCREEN_OFFSET)/PIXEL_PER_MM;
  antenna = Togl_GetClientData( togl );
  if(antenna == NULL) {
    assert(antenna = calloc(1,sizeof(struct Antenna)));
    antenna->Eye_Longitude = EYE_LONGITUDE_INIT;
    antenna->Eye_Latitude = EYE_LATITUDE_INIT;
    antenna->Eye_Distance = GPLANE_RADIUS * SCREEN_DISTANCE / (wm / 2.0);
    antenna->Eye_Distance_Min = antenna->Eye_Distance * EYE_DISTANCE_MIN;
    antenna->Eye_Distance_Max = antenna->Eye_Distance * EYE_DISTANCE_MAX;
    antenna->Eye_Distance = (antenna->Eye_Distance - antenna->Eye_Distance_Min)
      / (antenna->Eye_Distance_Max - antenna->Eye_Distance_Min);
  }  /**  Set global values  **/

  /**  Color of background  **/
  /*
  antenna->Global_Background[0] = 0.4;
  antenna->Global_Background[1] = 0.6;
  antenna->Global_Background[2] = 0.8;
  */
  antenna->Global_Background[0] = 0.0;
  antenna->Global_Background[1] = 0.0;
  antenna->Global_Background[2] = 0.05;

  antenna->Global_Viewer = true;
  antenna->Global_Slices = SLICES_INIT;
  antenna->Global_Rings  = RINGS_INIT;
  antenna->material[Antenna].Ambient[0] = 0.1;
  antenna->material[Antenna].Ambient[1] = 0.2;
  antenna->material[Antenna].Ambient[2] = 0.3;
  antenna->material[Antenna].Diffuse[0] = 0.1;
  antenna->material[Antenna].Diffuse[1] = 0.4;
  antenna->material[Antenna].Diffuse[2] = 0.7;
  antenna->material[Gplane].Ambient[0] = 0.2;
  antenna->material[Gplane].Ambient[1] = 0.2;
  antenna->material[Gplane].Ambient[2] = 0.2;
  antenna->material[Gplane].Diffuse[0] = 0.5;
  antenna->material[Gplane].Diffuse[1] = 0.5;
  antenna->material[Gplane].Diffuse[2] = 0.5;

  /** light  **/

  for(i = 0; i < LIGHTMAX; i++) {
    antenna->light[i].Number = LightNum[i];
    if(i == 0) {
      antenna->light[i].Type = directional;
      antenna->light[i].Ambient[0] = 0.7;
      antenna->light[i].Ambient[1] = 0.7;
      antenna->light[i].Ambient[2] = 0.7;
      antenna->light[i].Diffuse[0] = 1.0;
      antenna->light[i].Diffuse[1] = 1.0;
      antenna->light[i].Diffuse[2] = 1.0;
      antenna->light[i].Longitude = 315.0;
      antenna->light[i].Latitude = 45.0;
    } else {
      antenna->light[i].Type = off;
      antenna->light[i].Longitude = 360.0 / LIGHTMAX * i;
      antenna->light[i].Latitude = 90.0 / LIGHTMAX * i;
    }  /**  First light  **/

    antenna->light[i].Distance = 1.0 / LIGHTMAX * i;
    antenna->light[i].Spot_Cutoff = 10.0;
    antenna->light[i].Attenuation_Constant = 1.0;

    TKA_SetLight(&(antenna->light[i]), 
                 antenna->Eye_Distance_Min, 
                 antenna->Eye_Distance_Max );
  }  /**  For each light  **/

  Togl_SetClientData(togl, antenna);

  /**  Initialize openGL environment  **/
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

}  /**  End of Create  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  Destroy                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void TKA_Destroy(struct Togl *togl) {

  struct  Antenna *antenna;  /**  The antenna  **/
  
  antenna = Togl_GetClientData(togl);
  free(antenna);
  Togl_SetClientData(togl, NULL);  

}  /**  End of Destroy  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                   Reset                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_Reset(struct Togl *togl, GLint argc, char **argv) {

  TKA_Create(togl);
  Togl_PostRedisplay(togl);

  if(argc > 2) {
    Tcl_SetResult(Togl_Interp(togl),
      "PA_ParseArgs ERROR: Too many arguments for reset!", TCL_STATIC);
    return TCL_ERROR;
  }  /**  End of error  **/

  return TCL_OK;

}  /**  End of reset  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                    Eye                                  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_Eye(struct Togl *togl, GLint argc, char **argv) {

  struct Antenna *antenna = Togl_GetClientData(togl);  /**  Antenna data  **/
  GLint           result;                              /**  Result        **/

  result = PA_ParseArgs(Togl_Interp(togl), argc-2, argv+2, CfgEye, antenna);

  if(result == PA_CHANGED) {
    Togl_PostRedisplay(togl);
    result = TCL_OK;
  }  /**  Result  **/

  return result;

}  /**  End of eye  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 AddElement                              **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_AddElement(struct Togl *togl, GLint argc, char **argv) {

  struct Antenna *antenna = Togl_GetClientData(togl);  /**  Set up antenna  **/
  GLint           result;                              /**  Result          **/

  result = PA_ParseArgs(Togl_Interp(togl), argc-2, argv+2, CfgEye, antenna);
  if(result == PA_CHANGED) {
    Togl_PostRedisplay(togl);
    result = TCL_OK;
  }  /**  If changed  **/

  return result;

}  /**  End of AddElement  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  StdAnt                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_StdAnt(struct Togl *togl, GLint argc, char **argv) {

  char   file_name[256];  /**  Name of file  **/
  GLint  result;          /**  Result        **/

  
  if(strcmp(argv[2], "Load Yagi") == 0) {
    ReadFile("yagi.nec");
  }  /**  Yagi  **/
  else if(strcmp(argv[2], "FromFile") == 0) {
    printf("Enter name of file:");
    scanf("%s", file_name);
    ReadFile(file_name);
  }  /**  From a file  **/
  else if(strcmp(argv[2], "Wall") == 0) {
    AddWall();
  }  /**  Add a wall  **/

  Togl_PostRedisplay(togl);
  result = TCL_OK;
  antennaChanged = true;

  return result;

}  /**  End of StdAnt  **/



/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  TKA_LoadAnt                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_LoadAnt(struct Togl *togl, GLint argc, char **argv) {

  GLint  result;          /**  Result        **/

  fprintf(stderr, "File Name = %s\n", argv[2]);
  ReadFile(argv[2]);

  Togl_PostRedisplay(togl);
  result = TCL_OK;
  antennaChanged = true;

  return result;

}  /**  End of LoadAnt  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                TKA_DeleteAnt                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_DeleteAnt(struct Togl *togl, GLint argc, char **argv) {

  GLint  result;          /**  Result        **/

  DeleteCurrentAnt();

  Togl_PostRedisplay(togl);
  result = TCL_OK;
  antennaChanged = true;

  return result;

}  /**  End of LoadAnt  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                ControlAnt                               **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_ControlAnt(struct Togl *togl, GLint argc, char **argv) {

  GLint   result;  /**  Result    **/
  int     func;    /**  Function  **/

  if(argc >= 4) {
    func = atoi(argv[2]);
    /*printf("func = %d argv[3]=%lf\n", func, argv[3]);*/
    if(func == 1)
      MoveCurrentAnt(atof(argv[3])/20, 0, 0);
    else if(func == 2)
      MoveCurrentAnt(0, 0, atof(argv[3])/20);
    else if(func == 3)
      MoveCurrentAnt(0, atof(argv[3])/20, 0);
  }  /**  Moving tubes around  **/
  Togl_PostRedisplay(togl);
  result = TCL_OK;
  antennaChanged = true;

  return result;

}  /**  End of ControlAnt  **/

/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                ControlTube                              **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_ControlTube(struct Togl *togl, GLint argc, char **argv) {

  GLint   result;  /**  Result    **/
  int     func;    /**  Function  **/


  if(argc >= 4) {
    func = atoi(argv[2]);

    /**  Element position  **/
    if(func == 1)
      MoveCurrentTube(atof(argv[3])/20, 0, 0);
    else if(func == 2)
      MoveCurrentTube(0, atof(argv[3])/20, 0);
    else if(func == 3)
      MoveCurrentTube(0, 0, atof(argv[3])/20);

    /**  Element Size  **/
    else if(func == 4)
      ScaleCurrentTube(atof(argv[3])/50, 0);
    else if(func == 5)
      ScaleCurrentTube(0, atof(argv[3])/50);

    /**  Wall Size  **/
    else if(func == 7)
      ScaleCurrentWall(atof(argv[3])/50, 0, 0);
    else if(func == 8)
      ScaleCurrentWall(0, atof(argv[3])/50, 0);
    else if(func == 9)
      ScaleCurrentWall(0, 0, atof(argv[3])/50);

    /**  Wall Position  **/
    else if(func == 10)
      MoveCurrentWall(atof(argv[3])/20, 0, 0);
    else if(func == 11)
      MoveCurrentWall(0, atof(argv[3])/20, 0);
    else if(func == 12)
      MoveCurrentWall(0, 0, atof(argv[3])/20);

    /**  Rotate Tube  **/
    else if(func == 13)
      RotateCurrentTube(atof(argv[3]), 0, 0);
    else if(func == 14)
      RotateCurrentTube(0, atof(argv[3]), 0);
    else if(func == 15)
      RotateCurrentTube(0, 0, atof(argv[3]));

  }  /**  Moving tubes around  **/

  Togl_PostRedisplay(togl);
  result = TCL_OK;
  antennaChanged = true;

  return result;

}  /**  End of Control_Tube  **/

/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             ChangeCurrentAnt                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_ChangeCurrentAnt(struct Togl *togl, GLint argc, char **argv) {

  GLint  result;  /**  Result  **/

  if(argc >= 3) {
    ChangeCurrentAnt(atoi(argv[2]));
  }  /**  Change the tube  **/
  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of ChangeCurrentTube  **/

/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             ChangeCurrentTube                           **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_ChangeCurrentTube(struct Togl *togl, GLint argc, char **argv) {

  GLint  result;  /**  Result  **/

  if(argc >= 3) {
    ChangeCurrentTube(atoi(argv[2]));
  }  /**  Change the tube  **/
  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of ChangeCurrentTube  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              ChangeDrawMode                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_ChangeDrawMode(struct Togl *togl, GLint argc, char **argv) {

  GLint  result;  /**  Result  **/


  if (strcmp(argv[2],"DrawMode") == 0) {
    if (strcmp(argv[3],"Dots") == 0) {
      ToggleDrawMode(0);
    }  /**  Dots  **/
  
    if (strcmp(argv[3],"Surface") == 0) {
      ToggleDrawMode(1);
    }  /**  Polygons  **/
    
    if (strcmp(argv[3],"Sphere") == 0) {
      ToggleDrawMode(2);
    }  /**  Polygons  **/
  }  /**  DrawMode  **/

  else if(strcmp(argv[2], "Scale") == 0) {
    SCALE_FACTOR = 15.0/atof(argv[3]);
  }  /**  Antenna scale  **/

  else if(strcmp(argv[2], "StepSize") == 0) {
    antennaChanged = true;
    STEP_SIZE = atof(argv[3]);
  }  /**  Step size  **/

  else if(strcmp(argv[2], "DBHeight") == 0) {
    DEFAULT_BOOMHEIGHT = atof(argv[3]);
  }  /**  Default boom height  **/

  else if(strcmp(argv[2], "PDScale") == 0) {
    POINT_DIST_SCALE = atof(argv[3]);
  }  /**  Field scale  **/

  else if(strcmp(argv[2], "PSScale") == 0) {
    POINT_SIZE_SCALE = atof(argv[3])/100.0;
  }  /**  Dot size  **/

  else if(strcmp(argv[2], "NullThreshold") == 0) {
    NULL_THRESHOLD = atof(argv[3])/10.0;
  }  /**  Null threshold value  **/

  else if(strcmp(argv[2], "Alpha") == 0) {
    ALPHA = atof(argv[3])/10.0;
  }  /**  Alpha value  **/

  else if(strcmp(argv[2], "Freq") == 0) {
    ChangeFrequency(atof(argv[3]));
    antennaChanged = true;
  }  /**  Frequency  **/

  else if(strcmp(argv[2], "FreqSteps") == 0) {
    FreqSteps = atoi(argv[3]) ;
  }  /**  Frequency  **/

  else if(strcmp(argv[2], "ShowRadPat") == 0) {
    ShowRadPat = atoi(argv[3]);
  }  /**  Radiation pattern checkbox  **/

  else if(strcmp(argv[2], "ShowPolSense") == 0) {
    ShowPolSense = atoi(argv[3]);
  }  /**  Polarization sense checkbox  **/
  
  else if(strcmp(argv[2], "ShowPolTilt") == 0) {
    ShowPolTilt = atoi(argv[3]);
  }  /**  Polarization tilt checkbox  **/
  
  else if(strcmp(argv[2], "ShowAxialRatio") == 0) {
    ShowAxialRatio = atoi(argv[3]);
  }  /**  Axial ratio checkbox  **/
  
  else if(strcmp(argv[2], "ShowNulls") == 0) {
    ShowNulls = atoi(argv[3]);
  }  /**  Nulls in pattern checkbox  **/
  
  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of ChangeDrawMode  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            ChangeWireDrawMode                           **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_ChangeWireDrawMode(struct Togl *togl,GLint argc,char **argv) {

  GLint  result;  /**  Result  **/

  if(strcmp(argv[2], "ShowCurrentMagnitude") == 0) {
    WireDrawMode = atoi(argv[3]);
  }  /**  Current magnitude  **/

  else if(strcmp(argv[2], "ShowCurrentPhase") == 0) {
    WireDrawMode = atoi(argv[3]);
  }  /**  Current phase  **/
  
  else if(strcmp(argv[2], "ShowGeometry") == 0) {
    WireDrawMode = atoi(argv[3]);
  }  /**  No wire visualization  **/
  
  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of ChangeWireDrawMode  **/



/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                               ChangeAntMode                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_ChangeAntMode(struct Togl *togl,GLint argc,char **argv) {

  GLint  result;  /**  Result  **/

  if(strcmp(argv[2], "SingleAnt") == 0) {
    MultipleAntMode = atoi(argv[3]);
  }  /**  Single antenna  **/

  else if(strcmp(argv[2], "MultipleAnt") == 0) {
    MultipleAntMode = atoi(argv[3]);
  }  /**  All antennas  **/
  
  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of ChangeAntMode  **/



/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                           DrawRFPowerDensity                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_DrawRFPowerDensity(struct Togl  *togl, 
                                         GLint   argc, 
                                          char **argv) {

  GLint   result;  /**  Result   **/

  if(argc >= 2) {
    ComputeField(antennaChanged);
    if (antennaChanged == true)
      antennaChanged=false;
  }  /**  Draw the field  **/
  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of DrawRFPowerDensity  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 SaveFile                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_SaveFile(struct Togl *togl, GLint argc, char **argv) {

  char    file_name[255];  /**  File name  **/
  GLint   result;          /**  Result     **/

  if(argc >= 2) {
    if (argc >= 3)
      GenerateNECFile(argv[2]);
    else
    {
      printf("Enter name of file:");
      scanf("%s", file_name);
      GenerateNECFile(file_name);
    }
    
  }  /**  Generate file  **/
  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of SaveFile  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                SaveRGBImage                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_SaveRGBImage(struct Togl *togl, GLint argc, char **argv) {

  char    file_name[255];  /**  File name  **/
  GLint   result;          /**  Result     **/

  if(argc >= 2) {
    if (argc >= 3)
      Togl_DumpToEpsFile(togl,argv[2],2,(void *)TKA_Display);
    else {
      printf("Enter name of file:");
      scanf("%s", file_name);
      Togl_DumpToEpsFile(togl,file_name,2,(void *)TKA_Display);
    }  /**  Generate image  **/
  }  /**  Args  **/

  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of SaveRGBImage  **/



/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                MoveCenter                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_MoveCenter(struct Togl *togl, GLint argc, char **argv) {
  struct Antenna *antenna = Togl_GetClientData(togl);  /**  Antenna data   **/
  GLint   result;          /**  Result     **/
  double dist, elevation, azimuth;

  if(argc >= 2) {
    if(argc >= 4) {
      dist =  -(antenna->Eye_Distance *
               (antenna->Eye_Distance_Max - antenna->Eye_Distance_Min)
               + antenna->Eye_Distance_Min);
      elevation = antenna->Eye_Latitude;
      azimuth = antenna->Eye_Longitude;
      DoMoveCenter(atof(argv[2]), atof(argv[3]), azimuth, elevation, dist);
    }
  }  /**  Args  **/

  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of SaveRGBImage  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 GetVariable                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_GetVariable(struct Togl *togl, GLint argc, char **argv) {

  GLint   result;          /**  Result     **/

  if(argc >= 2) {
    #if 0
    if(strcmp(argv[2], "SCALE_FACTOR") == 0)
      sprintf(togl->Interp->result, "%lf", SCALE_FACTOR);
    else
      sprintf(togl->Interp->result, "NO_VARIABLE");
    #endif
    
  }  /**  Generate file  **/
  Togl_PostRedisplay(togl);
  result = TCL_OK;

  return result;

}  /**  End of SaveFile  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                               SetMaterial                               **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void TKA_SetMaterial(struct Material *m) {

  glShadeModel(GL_SMOOTH);
  glMaterialfv(GL_FRONT, GL_AMBIENT, m->Ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, m->Diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, m->Specular);
  glMaterialf(GL_FRONT, GL_SHININESS, m->Shininess);
  glMaterialfv(GL_FRONT, GL_EMISSION, m->Emission);

}  /**  End of SetMaterial  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                SetLight                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local void TKA_SetLight(struct Light *l, GLfloat eyemin, GLfloat eyemax) {

  GLfloat  dist;  /**  Distance  **/
  GLfloat  la;    /**  La        **/
  GLfloat  lo;    /**  Lo        **/

  if(l->Type == off) {
    glDisable( l->Number);
  } else {
    l->Position[3] = (l->Type == directional) ? 0.0 : 1.0;
    dist = l->Distance * (eyemax - eyemin) + eyemin;
    l->Position[0] = -dist*sin(radian(l->Longitude))*cos(radian(l->Latitude));
    l->Position[1] = dist * sin(radian(l->Latitude));
    l->Position[2] = dist * cos(radian(l->Longitude))*cos(radian(l->Latitude));
    glLightfv(l->Number, GL_AMBIENT, l->Ambient);
    glLightfv(l->Number, GL_DIFFUSE, l->Diffuse);
    glLightfv(l->Number, GL_SPECULAR, l->Specular);
    glLightf(l->Number, GL_CONSTANT_ATTENUATION, l->Attenuation_Constant);
    glLightf(l->Number, GL_LINEAR_ATTENUATION, l->Attenuation_Linear);
    glLightf(l->Number, GL_QUADRATIC_ATTENUATION, l->Attenuation_Quadratic);

    if(l->Type == spot) {
      glLightf(l->Number, GL_SPOT_EXPONENT, l->Spot_Exponent);
      glLightf(l->Number, GL_SPOT_CUTOFF, l->Spot_Cutoff);
      la = l->Latitude+TKA_Angle(l->Spot_y/PIXEL_PER_MM,SCREEN_DISTANCE)-180;
      lo = TKA_Angle(l->Spot_x / PIXEL_PER_MM, SCREEN_DISTANCE);
      l->Spot_Pos[0] = -sin(radian(l->Longitude))*cos(radian(la))+
        cos(radian(l->Longitude))*sin(radian(lo));
      l->Spot_Pos[1] = sin(radian(la));
      l->Spot_Pos[2] = cos(radian(l->Longitude))*cos(radian(la))+ 
          sin(radian(l->Longitude))*sin(radian(lo));
      l->Spot_Pos[3] = 1.0;
    } else {
      glLightf(l->Number,GL_SPOT_CUTOFF,(GLfloat) 180.0);
    }  /**  Spotlight off  **/

    glEnable(l->Number);
  }  /**  Is spotlight on  **/

}  /**  End of setlight  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 Global                                  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_Global(struct Togl *togl, GLint argc, char **argv) {

  struct Antenna *antenna = Togl_GetClientData(togl);  /**  Antenna Data  **/
  GLint           result;                              /**  Result        **/

  result = PA_ParseArgs(Togl_Interp(togl), argc-2, argv+2, CfgGlobal, antenna);
  if(result == PA_CHANGED) {
    Togl_PostRedisplay(togl);
    result = TCL_OK;
  }  /**  Redisplay  **/
  return result;

}  /**  End of global  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                Material                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_Material(struct Togl *togl, GLint argc, char **argv) {

  struct Antenna *antenna = Togl_GetClientData(togl);  /**  Antenna data   **/
  GLint           result;                              /**  Result         **/
  GLint           type;                                /**  Type material  **/

  if(argv[2] == NULL) {
    Tcl_SetResult(Togl_Interp(togl), "0 1 2", TCL_STATIC);
    return TCL_OK;
  }  /**  Material OK  **/

  type = atoi(argv[2]);
  if(type < 0 || type >= SizeOfMaterialType) {
    Tcl_SetResult( Togl_Interp(togl),
      "TKA_Material ERROR: Material type must be 0, 1 or 2!", TCL_STATIC );
    return TCL_ERROR;
  }  /**  Bad material  **/

  result = PA_ParseArgs(Togl_Interp(togl), argc-3, argv+3,
    CfgMaterial, &(antenna->material[type]));

  if(result == PA_CHANGED) {
    Togl_PostRedisplay(togl);
    result = TCL_OK;
  }  /**  Redraw  **/

  return result;

}  /**  End of Material  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                  Light                                  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local GLint TKA_Light(struct Togl *togl, GLint argc, char **argv) {

  struct Antenna *antenna = Togl_GetClientData(togl);  /**  Antenna data   **/ 
  GLint           result;                              /**  Result         **/
  GLint           n;                                   /**  Number lights  **/

  if(argv[2] == NULL) {
    Tcl_SetResult( Togl_Interp(togl), "0 1 2 3 4 5 6 7", TCL_STATIC);
    return TCL_OK;
  }  /**  Lights OK  **/

  n = atoi(argv[2]);
  if(n < 0 || n >= LIGHTMAX) {
    Tcl_SetResult(Togl_Interp(togl),
      "TKA_Light ERROR: Light number must be 0, 1, 2, 3, 4, 5, 6 or 7!",
      TCL_STATIC);
    return TCL_ERROR;
  }  /**  Lights error  **/

  result = PA_ParseArgs(Togl_Interp(togl), argc-3, argv+3, 
    CfgLight, &(antenna->light[n]));

  if(result == PA_CHANGED) {
    TKA_SetLight(&(antenna->light[n]), 
      antenna->Eye_Distance_Min, antenna->Eye_Distance_Max);
    Togl_PostRedisplay(togl);
    result = TCL_OK;
  }  /**  Redraw  **/

  return result;

}  /**  End of lights  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         End of AntennaWidget.c                          **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/





