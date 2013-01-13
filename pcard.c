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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define __USE_XOPEN_EXTENDED
#include <string.h>
#include "ant.h"
#include "pcard.h"


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          Global Variables                               **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


extern double    curr_step_size;  /**  Current step size         **/
extern int       FreqSteps;       /**  Frequency steps           **/
extern AntArray  TheAnts;         /**  The antennas' geometries  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                               PrintTube                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void PrintTube(FILE *f, Tube *tube, int tube_num) {

  double  y;         /**  Y coordinate of current segment  **/
  double  z;         /**  Z coordinate of current segment  **/
  int     i;         /**  Loop counter                     **/
  double  width;     /**  Width of tubes                   **/
  int     segments;  /**  Number of segments per side      **/

  segments = 10;
  if(tube->type == IS_TUBE) {
    fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num, 
          tube->segments,
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0,
          tube->e2.x*100.0, tube->e2.y*100.0, 
          tube->e2.z*100.0, tube->width*100.0);
  } else if(tube->type == IS_WALL) {

    #if 0
    /*left wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0,
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);

    /*right wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0,
          tube->e2.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);

    /*front wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0,
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);

    /*back wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e1.z*100.0,
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0);

    /*top wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0,
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);

    /*bottom wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e1.z*100.0,
          tube->e2.x*100.0, tube->e2.y*100.0, tube->e1.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0);
    #endif


    fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num, 5,
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0,
          tube->e1.x*100.0, tube->e1.y*100.0, 
          tube->e2.z*100.0, 5.0);
    /*fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num+1, 5,
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0,
          tube->e1.x*100.0, tube->e2.y*100.0, 
          tube->e2.z*100.0, 5.0);*/
    fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num+2, 5,
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0,
          tube->e1.x*100.0, tube->e2.y*100.0, 
          tube->e1.z*100.0, 5.0);
    /*fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num+3, 5,
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e1.z*100.0,
          tube->e1.x*100.0, tube->e1.y*100.0, 
          tube->e1.z*100.0, 5.0);*/
    for(i=0; i <= segments; i++) {
      width = 0.5*fabs(tube->e2.z - tube->e1.z)/segments;
      z = 100*(tube->e1.z + (tube->e2.z - tube->e1.z)*i/segments);
      fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num, 5, tube->e1.x*100.0,tube->e1.y*100, z, 
          tube->e1.x*100.0, tube->e2.y*100, z, width);

      y = 100*(tube->e1.y + (tube->e2.y - tube->e1.y)*i/segments);
      fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num, 5, tube->e1.x*100.0, y, tube->e1.z*100, 
          tube->e1.x*100.0, y, tube->e2.z*100, width);
    }  /**  Put in multiple wires  **/

  }  /**  Walls  **/

}  /**  End of PrintTube  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            PrintTubeOffset                              **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void PrintTubeOffset(FILE *f, 
                     Tube *tube, 
                      int  tube_num, 
                   double  dx, 
                   double  dy, 
                   double  dz) {

  double  y;         /**  Y coordinate of current segment  **/
  double  z;         /**  Z coordinate of current segment  **/
  int     i;         /**  Loop counter                     **/
  double  width;     /**  Width of tubes                   **/
  int     segments;  /**  Number of segments per side      **/

  segments = 10;
  if(tube->type == IS_TUBE) {
    fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num, 
          tube->segments,
          (tube->e1.x + dx) * 100.0, 
          (tube->e1.y + dy) * 100.0, 
          (tube->e1.z + dz) * 100.0,
          (tube->e2.x + dx) * 100.0, 
          (tube->e2.y + dy) * 100.0, 
          (tube->e2.z + dz) * 100.0, 
          tube->width * 100.0);
  } else if(tube->type == IS_WALL) {

    #if 0
    /*left wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0,
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);

    /*right wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0,
          tube->e2.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);

    /*front wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0,
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);

    /*back wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e1.z*100.0,
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0);

    /*top wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0,
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0);

    /*bottom wall*/
    fprintf(f, "SP  0 1 %lf %lf %lf %lf %lf %lf \n", 
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e1.z*100.0,
          tube->e2.x*100.0, tube->e2.y*100.0, tube->e1.z*100.0);
    fprintf(f, "SC  0 1 %lf %lf %lf 0 0 0\n", 
          tube->e2.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0);
    #endif


    fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num, 5,
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e1.z*100.0,
          tube->e1.x*100.0, tube->e1.y*100.0, 
          tube->e2.z*100.0, 5.0);
    /*fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num+1, 5,
          tube->e1.x*100.0, tube->e1.y*100.0, tube->e2.z*100.0,
          tube->e1.x*100.0, tube->e2.y*100.0, 
          tube->e2.z*100.0, 5.0);*/
    fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num+2, 5,
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e2.z*100.0,
          tube->e1.x*100.0, tube->e2.y*100.0, 
          tube->e1.z*100.0, 5.0);
    /*fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num+3, 5,
          tube->e1.x*100.0, tube->e2.y*100.0, tube->e1.z*100.0,
          tube->e1.x*100.0, tube->e1.y*100.0, 
          tube->e1.z*100.0, 5.0);*/
    for(i=0; i <= segments; i++) {
      width = 0.5*fabs(tube->e2.z - tube->e1.z)/segments;
      z = 100*(tube->e1.z + (tube->e2.z - tube->e1.z)*i/segments);
      fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num, 5, tube->e1.x*100.0,tube->e1.y*100, z, 
          tube->e1.x*100.0, tube->e2.y*100, z, width);

      y = 100*(tube->e1.y + (tube->e2.y - tube->e1.y)*i/segments);
      fprintf(f, "GW %d %d %lf %lf %lf %lf %lf %lf %lf\n", 
          tube_num, 5, tube->e1.x*100.0, y, tube->e1.z*100, 
          tube->e1.x*100.0, y, tube->e2.z*100, width);
    }  /**  Put in multiple wires  **/

  }  /**  Walls  **/

}  /**  End of PrintTubeOffset  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                            WriteCardFile                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void WriteCardFile(CONST84 char *file_name, Ant *the_ant, int step_size, double freq) {

  FILE *fout;            /**  Output file                 **/
  char *card;            /**  Ouput buffer                **/
  Tube *the_tube;        /**  Current tube                **/
  int   i;               /**  Loop counter                **/
  int   curr_tube;       /**  Current tube                **/
  int   increment;       /**  Increment in theta and phi  **/
  bool  seen_rp;         /**  Seen the RP card            **/
  bool  finished_tubes;  /**  Done drawing tubes          **/

  finished_tubes = false;
  curr_tube = 1;
  fout = fopen(file_name, "wt");
  seen_rp = false;

  if(fout == NULL)
    fprintf(stderr, "Could not open file %s for writing\n", file_name);
  else {
    the_tube = the_ant->first_tube;
    for(i=0; i < the_ant->card_count; i++) {
      card = the_ant->cards[i];
      if((card[0] == 'G') && (card[1] == 'W')) {
        while((the_tube != NULL) && (!finished_tubes)) {
          PrintTube(fout, the_tube, curr_tube++);
          the_tube = the_tube->next;
        }
        finished_tubes = true;
      } else if ((card[0] == 'R') && (card[1] == 'P')) {
        if (seen_rp == false) {
          increment = 361 / step_size;
          fprintf(fout,"RP  0   %d   %d    1001   0   0   %d   %d     0   0\n",
            increment, increment, step_size, step_size);
            seen_rp = true;
        } else {
        }  /**  Have we already output RP?  **/
      } else if ((card[0] == 'F') && (card[1] == 'R')) {
        fprintf(fout,"FR  0    1    0   0   %f     .0000     .0000     .0000    .0000    .0000\n",freq);
      } else if ((card[0] == 'G') && (card[1] == 'N')) {
        /**  Do nothing  **/
      } else {
        fprintf(fout, "%s", card);
      }  /**  Just output all other lines  **/
    }  /**  For each card  **/ 
    fclose(fout);
  }  /**  Antenna exists in memory  **/

}  /**  End of WriteCardFile  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          WriteMultAntsFile                              **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void WriteMultAntsFile(CONST84 char *file_name, int step_size, double freq) {

  FILE *fout;            /**  Output file                 **/
  char *card;            /**  Ouput buffer                **/
  Tube *the_tube;        /**  Current tube                **/
  int   i;               /**  Loop counter                **/
  int   k;               /**  Loop counter                **/
  int   tag;             /**  Tag number                  **/
  int   curr_tube;       /**  Current tube                **/
  int   increment;       /**  Increment in theta and phi  **/
  bool  seen_rp;         /**  Seen the RP card            **/
  bool  finished_tubes;  /**  Are we done yet             **/
  Ant  *the_ant;         /**  Current antenna             **/

  finished_tubes = false;
  fout = fopen(file_name, "wt");
  seen_rp = false;
  tag = 1;

  if(fout == NULL)
    fprintf(stderr, "Could not open file %s for writing\n", file_name);
  else {

    for(k=0; k < TheAnts.ant_count-1; k++) {

      curr_tube = 1;
      the_ant = &TheAnts.ants[k];
 
      the_tube = the_ant->first_tube;
      for(i=0; i < the_ant->card_count; i++) {
        card = the_ant->cards[i];
        if ((card[0] == 'C') && (card[1] == 'M') && (k==0)) {
          fprintf(fout, "%s", card);
        } else if ((card[0] == 'C') && (card[1] == 'E') && (k==0)) {
          fprintf(fout, "%s", card);
        } else if ((card[0] == 'G') && (card[1] == 'W')) {
          while((the_tube != NULL) && (!finished_tubes)) {
            PrintTubeOffset(fout,
                            the_tube, 
                            tag++, 
                            the_ant->dx,
                            the_ant->dy, 
                            the_ant->dz);
            the_tube = the_tube->next;
          }
          finished_tubes = true;
        }  /**  Print out only geometry cards  **/
      }  /**  For each card  **/ 
      finished_tubes = false;
    }  /**  For all but last antenna  **/

    curr_tube = 1;
    the_ant = &TheAnts.ants[TheAnts.ant_count-1];
    finished_tubes = false;

    the_tube = the_ant->first_tube;
    for(i=0; i < the_ant->card_count; i++) {
      card = the_ant->cards[i];
      if ((card[0] == 'C') && (card[1] == 'M')) {
        if (TheAnts.ant_count == 1)
          fprintf(fout, "%s", card);
      } else if ((card[0] == 'C') && (card[1] == 'E')) {
        if (TheAnts.ant_count == 1)
          fprintf(fout, "%s", card);
      } else if((card[0] == 'G') && (card[1] == 'W')) {
        while((the_tube != NULL) && (!finished_tubes)) {
          PrintTubeOffset(fout,
                          the_tube, 
                          tag++, 
                          the_ant->dx,
                          the_ant->dy, 
                          the_ant->dz);
          the_tube = the_tube->next;
        }  /**  Output tubes  **/
        finished_tubes = true;
      } else if ((card[0] == 'R') && (card[1] == 'P')) {
        if (seen_rp == false) {
          increment = 361 / step_size;
          fprintf(fout,"RP  0   %d   %d    1001   0   0   %d   %d   0   0\n",
            increment, increment, step_size, step_size);
            seen_rp = true;
        } else {
        }  /**  Have we already output RP?  **/
      } else if ((card[0] == 'F') && (card[1] == 'R')) {
        fprintf(fout,"FR  0    1    0   0   %f     .0000     .0000     .0000    .0000    .0000\n",freq);
      } else if ((card[0] == 'G') && (card[1] == 'N')) {
        /**  Do nothing  **/
      } else {
        fprintf(fout, "%s", card);
      }  /**  Just output all other lines  **/
    }  /**  For each card  **/ 
    finished_tubes = false;

  }  /**  Antenna exists in memory  **/
  fclose(fout);

}  /**  End of WriteMultAntFile  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                              CardToTube                                 **/
/**                                                                         **/
/**  Returns true if the antenna is specified above ground.                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


bool CardToTube(char *card, Tube *tube) {

  int     card_num;   /**  Current card number    **/ 
  int     seg_num;    /**  Number of segments     **/
  double  x1;         /**  First X value          **/
  double  y1;         /**  First Y value          **/
  double  z1;         /**  First Z value          **/
  double  x2;         /**  Second X value         **/
  double  y2;         /**  Second Y value         **/
  double  z2;         /**  Second Z value         **/
  bool    returnval;  /**  Did we see positive Z  **/

  returnval = false;
  sscanf(card+2, "%d%d%lf%lf%lf%lf%lf%lf%lf", &card_num, &seg_num, 
         &x1, &y1, &z1, &x2, &y2, &z2, &tube->width);

  SetPoint(&tube->e1, x1/100.0, y1/100.0, z1/100.0);
  SetPoint(&tube->e2, x2/100.0, y2/100.0, z2/100.0);
  tube->width /= 100.0;
  tube->segments = seg_num;
  tube->type = IS_TUBE;
  tube->currents = NULL;
  if ((z1 > 0.0) || (z2 > 0.0))
    returnval = true;

  return returnval;

}  /**  End of CardToTube  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                             ReadCardFile                                **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ReadCardFile(CONST84 char *file_name, Ant *antData) {

  FILE   *fin;          /**  Input file             **/
  char    line[256];    /**  Input buffer           **/
  Tube    new_tube;     /**  Current tube           **/
  char   *result;       /**  Result of operation    **/
  bool    seen_ground;  /**  Seen a ground yet      **/
  int     dummy;        /**  Fields we ignore       **/
  double  max_x;        /**  Dynamic range of X     **/
  double  max_y;        /**  Dynamic range of Y     **/
  double  max_z;        /**  Dynamic range of Z     **/
  double  min_x;        /**  Dynamic range of X     **/
  double  min_y;        /**  Dynamic range of Y     **/
  double  min_z;        /**  Dynamic range of Z     **/
  double  range_x;      /**  Dynamic range of X     **/
  double  range_y;      /**  Dynamic range of Y     **/
  double  range_z;      /**  Dynamic range of Z     **/
  double  max_range;    /**  Maximum dynamic range  **/
  Tube   *currentTube;  /**  For traversal          **/

  seen_ground = false;
  fin = fopen(file_name, "rt");
  antData->card_count=0;
  antData->ground_specified = false;
  antData->total_segments = 0;
  if(fin == NULL)
    fprintf(stderr, "Could not open file %s\n", file_name);
  else {
    result = fgets(line, 255, fin);
    while(result != NULL) {

      if((line[0] == 'C') && (line[1] == 'M')) {
        if (strncmp(line+3,"ELEVATED_VERTICAL",17) == 0)
          antData->type = ELEVATED_VERTICAL;
        else if (strncmp(line+3,"VERTICAL",8) == 0)
          antData->type = VERTICAL;
        else if (strncmp(line+3,"QUAD",4) == 0)
          antData->type = QUAD;
        else if (strncmp(line+3,"YAGI",4) == 0)
          antData->type = YAGI;
        else if (strncmp(line+3,"DIPOLE",6) == 0)
          antData->type = DIPOLE;
        else
          antData->type = UNKNOWN;
      }  /**  Geometry cards  **/

     if((line[0] == 'G') && (line[1] == 'W')) {
        seen_ground = CardToTube(line, &new_tube);
        if (seen_ground == true) 
          antData->ground_specified = true;
        antData->total_segments = antData->total_segments + new_tube.segments;
        InsertTube(antData, &new_tube);
      }  /**  Geometry cards  **/

      if((line[0] == 'G') && (line[1] == 'N')) {
        antData->ground_specified = true;
      }  /**  Ground cards  **/

      if((line[0] == 'F') && (line[1] == 'R')) {
        sscanf(line+2, "%d%d%d%d%lf%d%d%d%d", &dummy, &dummy, &dummy, &dummy, 
          &antData->frequency, &dummy, &dummy, &dummy, &dummy);
	/*
        antData->ground_specified = false;        
	*/
      }  /**  Ground cards  **/
      antData->cards[antData->card_count++] = strdup(line);
      result = fgets(line, 255, fin);
    }  /**  For each card  **/
    fclose(fin);

    /** Check whether we read any tubes at all before proceeding. **/

    if( antData->first_tube == NULL) {
	    fprintf(stderr,"No geometry tubes (GW lines) specified in file %s\n",file_name);
	    return;
    }


    /**  Traverse list of elements to find dynamic range  **/
    /**  We will need to scale things to accomodate all   **/
    /**  sizes of antennas.                               **/

    currentTube = antData->first_tube;
    max_x = currentTube->e1.x;
    max_y = currentTube->e1.y;
    max_z = currentTube->e1.z;
    min_x = currentTube->e1.x;
    min_y = currentTube->e1.y;
    min_z = currentTube->e1.z;
    while (currentTube != NULL) {
      if (currentTube->e1.x > max_x)
        max_x = currentTube->e1.x;
      if (currentTube->e2.x > max_x)
        max_x = currentTube->e2.x;
      if (currentTube->e1.y > max_y)
        max_y = currentTube->e1.x;
      if (currentTube->e2.y > max_y)
        max_y = currentTube->e2.x;
      if (currentTube->e1.z > max_z)
        max_z = currentTube->e1.x;
      if (currentTube->e2.z > max_z)
        max_z = currentTube->e2.x;
      if (currentTube->e1.x < min_x)
        min_x = currentTube->e1.x;
      if (currentTube->e2.x < min_x)
        min_x = currentTube->e2.x;
      if (currentTube->e1.y < min_y)
        min_y = currentTube->e1.x;
      if (currentTube->e2.y < min_y)
        min_y = currentTube->e2.x;
      if (currentTube->e1.z < min_z)
        min_z = currentTube->e1.x;
      if (currentTube->e2.z < min_z)
        min_z = currentTube->e2.x;
      currentTube = currentTube->next;
    }  /**  Traverse tube list  **/
    range_x = max_x - min_x;
    range_y = max_y - min_y;
    range_z = max_z - min_z;
    if (range_x > range_y) {
      if (range_x > range_z)
        max_range = range_x;
      else
        max_range = range_z;
    } else {
      if (range_y > range_z)
        max_range = range_y;
      else
        max_range = range_z;
    }  /**  Checking range  **/
    
    antData->visual_scale = 50 / max_range;

  }  /**  File exists  **/

}  /**  End of ReadCardFile  **/


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          ParseFieldData                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void ParseFieldData(FILE *fin, 
                     Ant *currAnt, 
                    bool  compField, 
                    bool  compCurrents) {

  char         line[256];        /**  A line in the nec_out file         **/
  char        *ptr;              /**  Pointer                            **/
  FieldVal     val;              /**  Value of the radiation             **/
  char         sense[10];        /**  Buffer                             **/
  int          count;            /**  Running count                      **/
  int          max_data;         /**  Data limit                         **/
  int          i;                /**  Loop counter                       **/
  bool         done_first_line;  /**  Have we processed the first line?  **/
  bool         end_of_file;      /**  Checks for end of file             **/
  SegmentData *segptr;           /**  Current data                       **/
  int          seg_num;          /**  Segment number                     **/
  int          tag_num;          /**  Wire tag                           **/
  int          card_num;         /**  Current card number                **/
  double       dummyf;           /**  Data we don't care about           **/
  double       mag;              /**  Magnitude of the current           **/
  double       phase;            /**  Phase of the current               **/

  end_of_file = false;
  fprintf(stdout,"Parsing NEC2 output...\n");
  if (currAnt->fieldData == NULL)
    currAnt->fieldData = calloc(1,sizeof(FieldData));
  max_data = 1024;
  currAnt->fieldData->count   = 0;
  currAnt->fieldData->maxgain = 0.0;
  currAnt->fieldData->mingain = 0.0;
  currAnt->fieldData->maxtilt = 0.0;
  currAnt->fieldData->mintilt = 0.0;
  currAnt->fieldData->vals    = (FieldVal*)malloc(max_data*sizeof(FieldVal));
  count = 0;
  done_first_line = false;


  if (compCurrents == true) {
    do {  /**  Ignore everything up to currents  **/
      ptr = fgets(line, 256, fin);
      if(strstr(line, "CURRENTS AND LOCATION") != NULL)
        count++;
      if(ptr == NULL) {
        end_of_file = true;
      }  /**  End of file  **/
    } while((count < 1) && (end_of_file == false));
    if (end_of_file == false) {  /**  Advance to first line  **/
      for(i=0; i < 7; i++) {
        ptr = fgets(line, 255, fin);
      }  /**  Skip ahead  **/
      if (ptr == NULL)
        end_of_file = true;
    }  /**  End of file  **/

    currAnt->current_tube = currAnt->first_tube;
    count = 0;
    card_num = 1;
    segptr = NULL;
    while((end_of_file == false) && (count < currAnt->total_segments)) {
      sscanf(line, "%d%d%lf%lf%lf%lf%lf%lf%lf%lf", 
                    &seg_num, &tag_num, &dummyf, &dummyf, 
                    &dummyf, &dummyf, &dummyf, &dummyf,
                    &mag, &phase);

      if (tag_num > card_num) {
        currAnt->current_tube = currAnt->current_tube->next;
        card_num = tag_num;
        segptr = NULL;
      }  /**  Advance to next card  **/

      if (done_first_line == false) {
        currAnt->max_current_mag = mag;
        currAnt->min_current_mag = mag;
        currAnt->max_current_phase = phase;
        currAnt->min_current_phase = phase;
        done_first_line = true;
      } else {
        if (mag > currAnt->max_current_mag)
          currAnt->max_current_mag = mag;
        if (mag < currAnt->min_current_mag)
          currAnt->min_current_mag = mag;
        if (phase > currAnt->max_current_phase)
          currAnt->max_current_phase = phase;
        if (phase < currAnt->min_current_phase)
          currAnt->min_current_phase = phase;
      }  /**  Updated max min  **/

      if (segptr == NULL) {  /**  First segment data  **/
        currAnt->current_tube->currents = 
          (SegmentData *)calloc(1,sizeof(SegmentData));
        currAnt->current_tube->currents->currentMagnitude = mag;
        currAnt->current_tube->currents->currentPhase = phase;
        currAnt->current_tube->currents->next = NULL;
        segptr = currAnt->current_tube->currents;
      } else {
        segptr->next = 
          (SegmentData *)calloc(1,sizeof(SegmentData));
        segptr = segptr->next;
        segptr->currentMagnitude = mag;
        segptr->currentPhase = phase;
        segptr->next = NULL;
      }  /**  New segment data  **/

      fgets(line, 255, fin);
      count++;

    }  /**  Processing loop  **/
    currAnt->current_tube = currAnt->first_tube;
  }  /**  Do we compute currents  **/


  if (compField == true) {
    count = 0;
    do {  /**  Ignore everything but RP  **/
      ptr = fgets(line, 256, fin);
      if(strstr(line, "RADIATION PATTERNS") != NULL)
        count++;
      if(ptr == NULL) {
        end_of_file = true;
      }  /**  End of file  **/
    } while((count < 1) && (end_of_file == false));
   
    if (end_of_file == false) {
      for(i=0; i < 5; i++) {
        ptr = fgets(line, 255, fin);
      }  /**  Skip ahead  **/
      if (ptr == NULL)
        end_of_file = true;
    }  /**  End of file  **/

    count = 0;
    while((end_of_file == false) && (strlen(line) > 1)) {
      sscanf(line, "%lf%lf%lf%lf%lf%lf%lf%s%lf%lf%lf%lf", 
                    &val.theta, &val.phi, &val.vert_gain, &val.hor_gain, 
                    &val.total_gain, &val.axial_ratio, &val.tilt,
                    sense, &val.theta_mag, &val.theta_phase, &val.phi_mag,
                    &val.phi_phase);
      if (done_first_line == false) {
        currAnt->fieldData->maxgain = val.total_gain;
        currAnt->fieldData->mingain = val.total_gain;
        currAnt->fieldData->maxtilt = val.tilt;
        currAnt->fieldData->mintilt = val.tilt;
        currAnt->fieldData->maxaxialratio = val.axial_ratio;
        currAnt->fieldData->minaxialratio = val.axial_ratio;
        done_first_line = true;
      } else {
        if (val.total_gain > currAnt->fieldData->maxgain) {
          currAnt->fieldData->maxgain = val.total_gain;
        }  /**  Increase max gain  **/
        if (val.total_gain < currAnt->fieldData->mingain) {
          currAnt->fieldData->mingain = val.total_gain;
        }  /**  Increase max gain  **/
 
        if (val.tilt > currAnt->fieldData->maxtilt) {
          currAnt->fieldData->maxtilt = val.tilt;
        }  /**  Increase max tilt  **/
        if (val.tilt < currAnt->fieldData->mintilt) {
          currAnt->fieldData->mintilt = val.tilt;
        }  /**  Increase Max gain  **/

        if (val.axial_ratio > currAnt->fieldData->maxaxialratio) {
          currAnt->fieldData->maxaxialratio = val.axial_ratio;
        }  /**  Increase max tilt  **/
        if (val.axial_ratio < currAnt->fieldData->minaxialratio) {
          currAnt->fieldData->minaxialratio = val.axial_ratio;
        }  /**  Increase Max gain  **/
      }  /**  Updated max min  **/
 
      if (strncmp(sense,"LINEAR",6) == 0) 
        val.sense = LINEAR;
      if(strncmp(sense,"RIGHT",5) == 0) 
        val.sense = RIGHT;
      if (strncmp(sense,"LEFT",4) == 0) 
        val.sense = LEFT;

      currAnt->fieldData->vals[count++] = val;

      fgets(line, 255, fin);

      if(count >= max_data) {
        max_data *= 2;
        currAnt->fieldData->vals = (FieldVal*)realloc(currAnt->fieldData->vals,
                                    max_data*sizeof(FieldVal));
      }  /**  Store  **/

    }  /**  Processing loop  **/
    currAnt->fieldData->count = count;
    currAnt->fieldComputed = true;
  }  /**  Compute field  **/

  if (end_of_file == true)
    printf("NEC RP failed!\n");

}  /**  End of ParseFieldData  **/




/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          GetNextPattern                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/


void GetNextPattern(FILE *fin, FieldData *field_data) {
  char         line[256];        /**  A line in the nec_out file         **/
  char        *ptr;              /**  Pointer                            **/
  FieldVal     val;              /**  Value of the radiation             **/
  char         sense[10];        /**  Buffer                             **/
  int          count;            /**  Running count                      **/
  int          max_data;         /**  Data limit                         **/
  int          i;                /**  Loop counter                       **/
  bool         done_first_line;  /**  Have we processed the first line?  **/
  bool         end_of_file;      /**  Checks for end of file             **/

  end_of_file = false;
  fprintf(stdout,"Parsing NEC2 output...\n");
  if (field_data == NULL)
    field_data = calloc(1,sizeof(FieldData));
  max_data = 1024;
  field_data->count   = 0;
  field_data->maxgain = 0.0;
  field_data->mingain = 0.0;
  field_data->maxtilt = 0.0;
  field_data->mintilt = 0.0;
  field_data->vals    = (FieldVal*)malloc(max_data*sizeof(FieldVal));
  count = 0;
  done_first_line = false;

    do {  /**  Ignore everything but RP  **/
      ptr = fgets(line, 256, fin);
      if(strstr(line, "RADIATION PATTERNS") != NULL)
        count++;
      if(ptr == NULL) {
        end_of_file = true;
      }  /**  End of file  **/
    } while((count < 1) && (end_of_file == false));
   
    if (end_of_file == false) {
      for(i=0; i < 5; i++) {
        ptr = fgets(line, 255, fin);
      }  /**  Skip ahead  **/
      if (ptr == NULL)
        end_of_file = true;
    }  /**  End of file  **/

    count = 0;
    while((end_of_file == false) && (strlen(line) > 1)) {
      sscanf(line, "%lf%lf%lf%lf%lf%lf%lf%s%lf%lf%lf%lf", 
                    &val.theta, &val.phi, &val.vert_gain, &val.hor_gain, 
                    &val.total_gain, &val.axial_ratio, &val.tilt,
                    sense, &val.theta_mag, &val.theta_phase, &val.phi_mag,
                    &val.phi_phase);
      if (done_first_line == false) {
        field_data->maxgain = val.total_gain;
        field_data->mingain = val.total_gain;
        field_data->maxtilt = val.tilt;
        field_data->mintilt = val.tilt;
        done_first_line = true;
      } else {
        if (val.total_gain > field_data->maxgain) {
          field_data->maxgain = val.total_gain;
        }  /**  Increase max gain  **/
        if (val.total_gain < field_data->mingain) {
          field_data->mingain = val.total_gain;
        }  /**  Increase max gain  **/
 
        if (val.tilt > field_data->maxtilt) {
          field_data->maxtilt = val.tilt;
        }  /**  Increase max tilt  **/
        if (val.tilt < field_data->mintilt) {
          field_data->mintilt = val.tilt;
        }  /**  Increase Max gain  **/
      }  /**  Updated max min  **/
 
      if (strncmp(sense,"LINEAR",6) == 0) 
        val.sense = LINEAR;
      if(strncmp(sense,"RIGHT",5) == 0) 
        val.sense = RIGHT;
      if (strncmp(sense,"LEFT",4) == 0) 
        val.sense = LEFT;

      field_data->vals[count++] = val;

      fgets(line, 255, fin);

      if(count >= max_data) {
        max_data *= 2;
        field_data->vals = (FieldVal*)realloc(field_data->vals,
                                    max_data*sizeof(FieldVal));
      }  /**  Store  **/

    }  /**  Processing loop  **/
    field_data->count = count;
  if (end_of_file == true)
    printf("NEC RP failed!\n");

}

void WritePattern(FieldData *field_data, FILE *f) {
  int i;
  float gain;


  fprintf(stderr, "Count = %d\n", field_data->count);
  for(i=0; i < field_data->count; i++) {
    gain = exp(field_data->vals[i].total_gain / 10.0);
    gain = field_data->vals[i].total_gain;
    fwrite(&gain, sizeof(float), 1, f);
  } 

}


void WriteHeader(int xd, int yd, int zd, FILE *f)
{
   float xo, yo, zo;
   float xs, ys, zs;
   float min[3], max[3];
   unsigned int nv, nc;


   xo = 1.0;
   yo = 1.0;
   zo = 1.0;

   xs = 1.0;
   ys = 1.0;
   zs = 1.0;

   min[0] = xo;
   min[1] = yo;
   min[2] = zo;
   max[0] = xo + xs*(xd-1);
   max[1] = yo + ys*(yd-1);
   max[2] = zo + zs*(zd-1);
   fwrite(min, sizeof(float), 3, f);
   fwrite(max, sizeof(float), 3, f);

   nv = xd*yd*zd;
   nc = (xd-1)*(yd-1)*(zd-1);
   fwrite(&nv, sizeof(int), 1, f);
   fwrite(&nc, sizeof(int), 1, f);

   fwrite(&xd, sizeof(int), 1, f);
   fwrite(&yd, sizeof(int), 1, f);
   fwrite(&zd, sizeof(int), 1, f);

   fwrite(&xo, sizeof(float), 1, f);
   fwrite(&yo, sizeof(float), 1, f);
   fwrite(&zo, sizeof(float), 1, f);

   fwrite(&xs, sizeof(float), 1, f);
   fwrite(&ys, sizeof(float), 1, f);
   fwrite(&zs, sizeof(float), 1, f);
}


/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/**                          End of pcard.c                                 **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/








