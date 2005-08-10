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

#ifndef PCARD_H
#define PCARD_H

#include <stdio.h>
#include "ant.h"


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          Function Prototypes                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void  PrintTube(FILE *, Tube *, int);
void  PrintTubeOffset(FILE *, Tube *, int, double, double, double);
void  WriteCardFile(char *, Ant *, int, double);
void  WriteMultAntsFile(char *, int, double);
bool  CardToTube(char *, Tube *);
void  ReadCardFile(char *, Ant *);
void  ParseFieldData(FILE *, Ant *, bool, bool);

#endif


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            End of pcard.h                               **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/



