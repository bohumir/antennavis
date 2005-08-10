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

#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ParseArgs.h"
#include "MyTypes.h"


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                                ParseArgs                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


GLint PA_ParseArgs(Tcl_Interp  *interp, 
                        GLint   argc, 
                         char **argv,
             struct PA_Config  *cfg, 
                         void  *data) {

  GLint    i;       /**  Loop counter           **/
  GLint    n;       /**  Loop counter           **/
  GLfloat  f;       /**  Floating point number  **/
  bool     b;       /**  Flow control           **/
  char buffer[40];  /**  Input buffer           **/
  
  i = 0;
  if(argv[i] != NULL) {
    /**  Search for an entry with the same name  **/
    while(cfg->type != PA_END) {
      if(strcmp( argv[i], cfg->name) == 0) {
        i++;
        switch(cfg->type) {
          case PA_FLOAT:
            if(argv[i] != NULL) {
              f = atof(argv[i]);
              (*((GLfloat *)((char *) data + cfg->offset))) = f;
              i++;
            } else {
	      sprintf(buffer, "%f", 
		(double) (*((GLfloat *)( (char *) data + cfg->offset))));
    	      Tcl_SetResult( interp, buffer, TCL_VOLATILE);
            }  /**  Floating point  **/
            break;
  
          case PA_BOOL:
            if(argv[i] != NULL) {
              if(strcmp(argv[i], "0") == 0 
	        || strcmp(argv[i], "f") == 0 
                || strcmp(argv[i], "false") == 0 
		|| strcmp(argv[i], "False") == 0 
                || strcmp(argv[i], "FALSE") == 0 
		|| strcmp(argv[i], "F") == 0 )
                b = false;
              else
                b = true;
 
              (*((bool *)((char *) data + cfg->offset))) = b;
              i++;
            } else {
    	      Tcl_SetResult(interp, 
		  (bool)(*((bool *)((char *)data+cfg->offset))) ? 
                  "true" : "false",TCL_STATIC);
	    }  /**  Boolean  **/
            break;

          case PA_INT:
            if(argv[i] != NULL) {
              n = atoi(argv[i] );
              (*((GLint *)((char *)data + cfg->offset))) = n;
              i++;
            } else {
	      sprintf(buffer, "%d", 
		(int) (*((GLint *)((char *) data + cfg->offset))));
    	      Tcl_SetResult(interp, buffer, TCL_VOLATILE);
	    }  /**  Integer  **/
            break;
  
          case PA_RGB:
            if(argv[i] != NULL) {
              if(argc - i < 3) {
      	        Tcl_SetResult(interp,
                  "PA_ParseArgs ERROR: need three values for RGB", TCL_STATIC);
                return TCL_ERROR;
              }  /**  Error  **/
	      
              f = atof(argv[i]);
              (*((GLfloat *)((char *)data + cfg->offset))) = f;
              i++;
              f = atof(argv[i]);
              (*((GLfloat *)((char *)data + cfg->offset + sizeof(GLfloat))))=f;
              i++;
              f = atof(argv[i]);
              (*((GLfloat *)((char *)data + cfg->offset+2*sizeof(GLfloat))))=f;
              i++;
            } else {
	      sprintf(buffer, "%f %f %f", 
		(double)(*((GLfloat *)((char *)data+cfg->offset))),
		(double)(*((GLfloat *)((char *)data+cfg->offset+sizeof(GLfloat)))),
		(double)(*((GLfloat *)((char *)data+cfg->offset+2*sizeof(GLfloat)))));
    	      Tcl_SetResult(interp, buffer, TCL_VOLATILE);
	    }  /**  RGB  **/
            break;
  
          default:
  	    Tcl_SetResult(interp,
              "PA_ParseArgs ERROR: Illegal type!", TCL_STATIC);
            return TCL_ERROR;
        }  /**  Switch  **/
        
        if(argv[i] == NULL) {
          return PA_CHANGED;
	} else {
	  Tcl_SetResult(interp,
            "PA_ParseArgs ERROR: Too many arguments!", TCL_STATIC);
          return TCL_ERROR;
	}  /**  Changed  **/
      }  /**  Name OK  **/
      cfg++;
    }  /**  While not done  **/
  } else {
    while(cfg->type != PA_END) {
      Tcl_AppendElement(interp, cfg->name);
      cfg++;
    }  /**  Try all possible options  **/
    return TCL_OK;
  }  /**  Try all possible options  **/

  Tcl_SetResult(interp, "PA_ParseArgs ERROR: No option matches!", TCL_STATIC);
  return TCL_ERROR;

}  /**  End of ParseArgs  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                           End of ParseArgs.c                            **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/
