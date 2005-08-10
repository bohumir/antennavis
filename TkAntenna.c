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
#include "MyTypes.h"
#include "TkAntenna.h"
#include "togl.h"


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            Global Variables                             **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


char *TKA_PrgName;  /* Name of the Program (= argv[0]) */


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 Init                                    **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


local int Init(Tcl_Interp *interp) {

  /* Initialize Tcl, Tk, Togl and the TkAntenna widget module. */
  if (Tcl_Init(interp) == TCL_ERROR)  return TCL_ERROR;
  if (Tk_Init(interp) == TCL_ERROR)   return TCL_ERROR;
  if (Togl_Init(interp) == TCL_ERROR) return TCL_ERROR;
  if (TKA_Init(interp) == TCL_ERROR)  return TCL_ERROR;
  return TCL_OK;

}  /**  End of Init  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                 Main                                    **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


int  main(int argc, char **argv) {

  TKA_PrgName = argv[0];
  Tk_Main(argc, argv, Init );
  return 0;

}  /**  End of main  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                         End of TkAntenna.c                              **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


