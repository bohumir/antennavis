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

#ifndef VIS_WIRES_H
#define VIS_WIRES_H

#include "MyTypes.h"
#include "ant.h"
#include <GL/gl.h>
#include <GL/glu.h>


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             Definitions                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


extern double  SCALE_FACTOR;      /**  Scale factor for antenna  **/
extern double  TUBE_WIDTH_SCALE;  /**  Tube width scale          **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         Function Prototypes                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


double GetVerticalHeight(Tube *tube);
void    ComputeColor(double, double, double, GLfloat *);
double  GetBoomLength(Tube *);
double  GetBoomHeight(Tube *);
double  GetBoomWidth(Tube *);
double  GetBoomShift(Tube *);
void    DrawTubeList(Tube *, Tube *, GLint, GLint);
void    DrawTube(Tube *, GLint, GLint);
void    DrawWireCurrentMagnitudeList(Tube *, 
                                     Tube *, 
                                     double, 
                                     double, 
                                     GLint, 
                                     GLint);
void    DrawWireCurrentMagnitude(Tube *, double, double, GLint, GLint);
void    DrawWireCurrentPhaseList(Tube *, Tube *, double, double, GLint, GLint);
void    DrawWireCurrentPhase(Tube *, double, double, GLint, GLint);


#endif


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          End of VisWires.h                              **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/
