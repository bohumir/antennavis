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

#ifndef ANT_H
#define ANT_H

#include "MyTypes.h"
#include "togl.h"
#include <GL/gl.h>
#include <GL/glu.h>


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             Definitions                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


#define  MAX_ANTENNAS  5
#define  LINEAR        0
#define  RIGHT         1
#define  LEFT          2

 
/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                               Typedefs                                  **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


typedef enum {IS_TUBE, IS_WALL} tube_type_enum;
typedef enum {VERTICAL, YAGI, QUAD, DIPOLE, ELEVATED_VERTICAL, UNKNOWN} antenna_type_enum;

typedef struct Point {
  double  x;  /**  X coordinate cartesian value  **/
  double  y;  /**  Y coordinate cartesian value  **/
  double  z;  /**  Z coordinate cartesian value  **/
} Point;

typedef struct {
  double   posx;   /**  Position in x       **/
  double   posy;   /**  Position in y       **/
  double   posz;   /**  Position in z       **/
  GLfloat  r;      /**  Red color value     **/
  GLfloat  g;      /**  Green color value   **/
  GLfloat  b;      /**  Blue color value    **/
  GLfloat  a;      /**  Transparency value  **/
} MeshPoint;

typedef struct SegmentData {
  float               currentMagnitude;  /**  Magnitude of current in amps  **/
  float               currentPhase;      /**  Phase of the current wave     **/
  struct SegmentData *next;              /**  Next pointer                  **/
} SegmentData;

typedef struct Tube {
  Point        e1;        /**  Endpoint 1 of the tube        **/
  Point        e2;        /**  Endpoint 2 of the tube        **/
  int          type;      /**  Is it a tube or a wall        **/
  int          segments;  /**  Number of segment NEC uses    **/
  double       width;     /**  Thickness of the tube         **/
  SegmentData *currents;  /**  Currents                      **/
  struct Tube *next;      /**  Pointer to next tube in list  **/
} Tube;

typedef struct FieldVal {
  double  theta;        /**  Degrees azimuth                           **/
  double  phi;          /**  Degrees elevation                         **/
  double  vert_gain;    /**  Vertical component of gain                **/
  double  hor_gain;     /**  Horizontal component of gain              **/
  double  total_gain;   /**  Gain in dBi in theta, phi direction       **/
  double  axial_ratio;  /**  Axial ratio                               **/
  int     sense;        /**  Polarity: linear, right or left circular  **/
  double  tilt;         /**  Polarization tilt                         **/
  double  theta_mag;    /**  Azimuthal magnitude                       **/
  double  theta_phase;  /**  Azimuthal phase                           **/
  double  phi_mag;      /**  Elevation magnitude                       **/
  double  phi_phase;    /**  Elevation phase                           **/
} FieldVal;

typedef struct FieldData {
  int       count;          /**  Number of lines in field data       **/     
  FieldVal *vals;           /**  One for each theta, phi direction   **/ 
  double    maxgain;        /**  Maximum gain value                  **/
  double    mingain;        /**  Minimum gain value                  **/
  double    maxtilt;        /**  Maximum value of polarization tilt  **/
  double    mintilt;        /**  Minimum value of polarization tilt  **/
  double    maxaxialratio;  /**  Maximum axial ratio                 **/
  double    minaxialratio;  /**  Minimum axial ratio                 **/
} FieldData;

typedef struct Ant {
  int        tube_count;             /**  Number of elements in antenna  **/ 
  int        card_count;             /**  Number of cards in .nec file   **/
  int        total_segments;         /**  Total number of segments       **/
  int        type;                   /**  Vertical?  Yagi?  Quad?        **/
  Tube      *current_tube;           /**  List traversal tool            **/
  Tube      *first_tube;             /**  Origin element                 **/
  char      *cards[1000];            /**  The .nec file                  **/
  bool       ground_specified;       /**  Ground is specified            **/
  double     frequency;              /**  Antenna Offset                 **/
  double     dx;                     /**  Antenna Offset                 **/
  double     dy;                     /**  Antenna Offset                 **/
  double     dz;                     /**  Antenna Offset                 **/
  double     max_current_mag;        /**  Maximum current magnitude      **/
  double     min_current_mag;        /**  Minimum current magnitude      **/
  double     max_current_phase;      /**  Maximum current phase          **/
  double     min_current_phase;      /**  Minimum current phase          **/
  MeshPoint  surfaceMesh[361][361];  /**  Triangular mesh                **/
  FieldData *fieldData;              /**  Field data for this antenna    **/
  bool       fieldComputed;          /**  Field data computed yet        **/
  double     visual_scale;           /**  Visual scale factor            **/
} Ant;

typedef   struct AntArray {
  int  ant_count;           /**  Number of antennas in scene  **/
  int  curr_ant;            /**  The current antenna          **/
  Ant  ants[MAX_ANTENNAS];  /**  Array of antennas            **/
} AntArray;


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         Function Prototypes                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void    InsertTube(Ant *, Tube *);
void    SetPoint(Point *, double, double, double);
double  sqr(double);
double  PointDist(Point, Point);
void    ToggleDrawMode(int);
void    ChangeFrequency(double);
void    DisplaySelectedAnt(Ant *, GLint, GLint, bool);
void    DisplayAnt(GLint, GLint);
void    ReadFile(CONST84 char *);
void    MoveCurrentTube(double, double, double);
void    MoveCurrentWall(double, double, double);
void    DoMoveCenter(double, double, double, double, double);
void    RotateCurrentTube(double, double, double);
void    MoveCurrentAnt(double, double, double);
void    ScaleCurrentWall(double, double, double);
void    ScaleCurrentTube(double, double);
void    ChangeCurrentTube(int);
void    ChangeCurrentAnt(int);
void    GenerateNECFile(CONST84 char *);
void    AddWall(void);
void    ComputeField(bool);
void    DeleteCurrentAnt(void);

#endif


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             End of ant.h                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/
