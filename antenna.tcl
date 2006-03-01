#!/bin/sh
# the next line restarts using TkAnt \
exec TkAnt "$0" "$@"

###############################################################################
###############################################################################
##                                                                           ##
##  Antenna Visualization Toolkit                                            ##
##                                                                           ##
##  Copyright (C) 1998 Adrian Agogino, Ken Harker                            ##
##  Copyright (C) 2005 Joop Stakenborg                                       ##
##                                                                           ##
###############################################################################
###############################################################################

## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU Library General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

###############################################################################
###############################################################################
##                                                                           ##
##                              Global Variables                             ##
##                                                                           ##
###############################################################################
###############################################################################


# set out mouse control
set MouseControl "View"

# set our antenna
set WAntenna ""

# set our material
set Material "Antenna"

# set our shading type
set Shade "Smooth"

# set our basic light
set Light "View"

# set our type
set Type "Off"

# set our viewer
set Viewer "Global"

# set our antenna type
set AntType "TV"

# material update list
set MaterialLs {}

# light update list
set LightLs {}

# global update list
set GlobalLs {}

# general enable and disable list
set AbleLs {}

# light enable and disable list
set LightAbleLs {}

# our font
set font "-*-helvetica-medium-r-*-*-*-*-*-*-*-*-*"

# our bold font
set boldfont "-*-helvetica-bold-r-*-*-*-*-*-*-*-*-*"

# our relief
set relief groove

# our borderwidth
set borderwidth 1

#our pad
set pad 1

# our enable color
set enablecolor ""

# our disable color
set disablecolor gray60

# mouse position for user interaction
set MouseX 0
set MouseY 0

# real eye position (not a view point from a light)
set EyeLongitude 0
set EyeLatitude 0
set EyeDistance 0

# default distance for directional light
set EyeDefaultDistance 0

# names of the mouse widgets
set WMouseLeft   ""
set WMouseMiddle ""
set WMouseRight  ""


###############################################################################
###############################################################################
##                                                                           ##
##                          Window Initialization                            ##
##                                                                           ##
##  Create the basic frames and positions them.                              ##
##                                                                           ##
###############################################################################
###############################################################################


proc CreateUserInterface {} {

  # define global variables
  global WAntenna font relief borderwidth pad MaterialLs LightLs \
      EyeLongitude EyeLatitude EyeDistance EyeDefaultDistance GlobalLs \
      WMouseLeft WMouseMiddle WMouseRight boldfont AbleLs LightAbleLs \
      enablecolor disablecolor AntType

  # Give it a title
  wm title . "Antenna Visualization Toolkit: Antennavis"

  # the basics
  set WTopFrame .top
  frame $WTopFrame
  set WBottomFrame .bottom
  frame $WBottomFrame
  set enablecolor [lindex [$WBottomFrame configure -background] 4]

  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                        Antenna Display Frame                          ##
  ##                                                                       ##
  ##  This is the region of the screen where the antenna is displayed.     ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################

  set WAntennaFrame $WTopFrame.antennaFrame
  frame $WAntennaFrame -borderwidth $borderwidth -relief $relief
  set WAntenna $WAntennaFrame.antenna
  togl $WAntenna -rgba true -double true -depth true -height 390
  pack $WAntenna -fill both -fill both -expand 1 
  pack $WAntennaFrame -side left -fill both -expand 1 -padx $pad -pady $pad

  # remember values for the eye-point
  set EyeLongitude [eval $WAntenna eye longitude]
  set EyeLatitude  [eval $WAntenna eye latitude]
  set EyeDistance  [eval $WAntenna eye distance]
  set EyeDefaultDistance $EyeDistance


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                           File Control Frame                          ##
  ##                                                                       ##
  ##  Initializes the buttons in the File Control frame.                   ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WFileControlFrame $WBottomFrame.fileControlFrame
  frame $WFileControlFrame -borderwidth $borderwidth -relief $relief
  label $WFileControlFrame.label -text "File Control" -font $boldfont
  pack $WFileControlFrame.label -side top

  set WloadButton $WFileControlFrame.loadButton
  button $WloadButton -relief $relief -text "Load Antenna File" \
         -command "LoadAntFile" \
         -font $font 
  pack $WloadButton -side top -pady $pad

  set WdeleteButton $WFileControlFrame.deleteButton
  button $WdeleteButton -relief $relief -text "Unload Current Antenna" \
         -command "DeleteCurrentAnt" \
         -font $font 
  pack $WdeleteButton -side top -pady $pad

  set WsaveFileButton $WFileControlFrame.saveFileButton
  button $WsaveFileButton -relief $relief -text "Save Antenna File" \
         -command "SaveFile $WAntenna" \
         -font $font 
  pack $WsaveFileButton -side top -pady $pad

  set WsaveImageButton $WFileControlFrame.saveImageButton
  button $WsaveImageButton -relief $relief -text "Save As EPS Image" \
         -command "SaveRGBImage $WAntenna" \
         -font $font 
  pack $WsaveImageButton -side top -pady $pad

  pack $WFileControlFrame -side left -padx $pad -pady $pad -fill y


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                             Controls Frame                            ##
  ##                                                                       ##
  ##  Parent frame for Antenna and File control frames.                    ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WControlsFrame $WBottomFrame.controlsFrame
  frame $WControlsFrame
  pack $WControlsFrame -side left -fill y


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                           Vis Control Frame                           ##
  ##                                                                       ##
  ##  Initializes the buttons in the Visualization Control frame.          ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WVisControlFrame $WControlsFrame.visControlFrame
  frame $WVisControlFrame -borderwidth $borderwidth -relief $relief
  label $WVisControlFrame.label -text "Visualization Control" -font $boldfont
  pack $WVisControlFrame.label -side top

  set Wdraw_RFPowerDensityButton \
         $WVisControlFrame.draw_RFPowerDensityButton
  button $Wdraw_RFPowerDensityButton -relief $relief -text "Compute RF Field" \
         -command {$WAntenna draw_RFPowerDensity} \
         -font $font 
  pack $Wdraw_RFPowerDensityButton -side top -pady $pad

  set MultAntsVariable 0
  set SingleAntButton $WVisControlFrame.single_ant_button
  radiobutton $SingleAntButton -text "Current Antenna" -font $font \
              -variable MultAntsVariable -value 0 \
              -command {$WAntenna change_ant_mode "SingleAnt" \
                        $MultAntsVariable}
  $SingleAntButton select
  pack $SingleAntButton -side top

  set MultipleAntButton $WVisControlFrame.multiple_ant_button
  radiobutton $MultipleAntButton -text "All Antennas" -font $font \
              -variable MultAntsVariable -value 1 \
              -command {$WAntenna change_ant_mode "MultipleAnt" \
                        $MultAntsVariable}
  pack $MultipleAntButton -side top


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                         Antenna Control Frame                         ##
  ##                                                                       ##
  ##  Initializes the buttons in the Antenna Control frame.                ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WAntennaControlFrame $WControlsFrame.antennaControlFrame
  frame $WAntennaControlFrame -borderwidth $borderwidth -relief $relief
  label $WAntennaControlFrame.label -text "Antenna Control" -font $boldfont
  pack $WAntennaControlFrame.label -side top

  set Wnext_antButton $WAntennaControlFrame.next_antButton
  button $Wnext_antButton -relief $relief -text "Select Next Antenna" \
         -command {$WAntenna change_current_ant 1} \
         -font $font 
  pack $Wnext_antButton -side top -pady $pad

  set Wnext_tubeButton $WAntennaControlFrame.next_tubeButton
  button $Wnext_tubeButton -relief $relief -text "Select Next Element" \
         -command {$WAntenna change_current_tube 1} \
         -font $font 
  pack $Wnext_tubeButton -side top -pady $pad

  set Winsert_wallButton $WAntennaControlFrame.insert_wallButton
  button $Winsert_wallButton -relief $relief -text "Insert Wall" \
         -command {InsertAnt "Wall"} \
         -font $font 
  pack $Winsert_wallButton -side top -pady $pad

  set WFreqButton $WAntennaControlFrame.freq_button
  button $WFreqButton -relief $relief -text "Set Frequency" \
         -command {SetFrequency} \
         -font $font 
  pack $WFreqButton -side top -pady $pad
  UpdateAntennaControl


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                           Mouse Control Frame                         ##
  ##                                                                       ##
  ##  Initializes the mouse control frame.                                 ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WMouseControlFrame $WControlsFrame.mouseControlFrame
  frame $WMouseControlFrame -borderwidth $borderwidth -relief $relief
  label $WMouseControlFrame.label -text "Mouse Control" -font $boldfont
  pack $WMouseControlFrame.label -side top

  set WMMouseControl $WMouseControlFrame.mousecontrol
  chooser $WMMouseControl MouseControl \
  {"View" "Antenna Position" "Element Position" \
           "Element Rotation" "Element Size" "Wall Position" "Wall Size"} {}
  pack $WMMouseControl -side top -pady $pad

  pack $WMouseControlFrame -side top -padx $pad -pady $pad -fill y

  pack $WAntennaControlFrame -side top -padx $pad -pady $pad -fill y

  pack $WVisControlFrame -side top -padx $pad -pady $pad -fill y


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                             Displays Frame                            ##
  ##                                                                       ##
  ##  Parent frame for Field and Wire displays.                            ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WDisplayFrame $WBottomFrame.displayFrame
  frame $WDisplayFrame
  pack $WDisplayFrame -side left -fill y


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                        Field Display Mode Frame                       ##
  ##                                                                       ##
  ##  Initializes the frame where the user chooses the options for         ##
  ##  visualizing the RF field.                                            ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WFieldDisplayControlFrame $WDisplayFrame.fieldDisplayFrame
  frame $WFieldDisplayControlFrame -borderwidth $borderwidth -relief $relief
  label $WFieldDisplayControlFrame.label \
              -text "Field Display Mode" -font $boldfont
  pack $WFieldDisplayControlFrame.label -side top

  set SurfChooser $WFieldDisplayControlFrame.surf_chooser
  chooser $SurfChooser SurfType \
  {"Dots" "Surface" "Sphere"} {$WAntenna change_mode "DrawMode" $SurfType}
  pack $SurfChooser -side top -pady $pad

  set RadiationButton $WFieldDisplayControlFrame.radiation_button
  checkbutton $RadiationButton -text "Show Radiation Pattern" -font $font \
              -variable ShowRadPat \
              -command {$WAntenna change_mode "ShowRadPat" $ShowRadPat}
  pack $RadiationButton -side top

  set PolarizationButton $WFieldDisplayControlFrame.polarization_button
  checkbutton $PolarizationButton -text "Show Polarization Sense" -font $font \
              -variable ShowPolSense \
              -command {$WAntenna change_mode "ShowPolSense" $ShowPolSense}
  $PolarizationButton select
  pack $PolarizationButton -side top

  set PolarizationTiltButton \
              $WFieldDisplayControlFrame.polarization_tilt_button
  checkbutton $PolarizationTiltButton -text "Show Polarization Tilt" \
              -font $font \
              -variable ShowPolTilt \
              -command {$WAntenna change_mode "ShowPolTilt" $ShowPolTilt}
  pack $PolarizationTiltButton -side top

  set AxialRatioButton $WFieldDisplayControlFrame.axial_ratio_button
  checkbutton $AxialRatioButton -text "Show Axial Ratio" -font $font \
              -variable ShowAxialRatio \
              -command {$WAntenna change_mode "ShowAxialRatio" $ShowAxialRatio}
  pack $AxialRatioButton -side top

  set DispNullsButton $WFieldDisplayControlFrame.disp_nulls_button
  checkbutton $DispNullsButton -text "Display Nulls in Pattern" -font $font \
              -variable ShowNulls \
              -command {$WAntenna change_mode "ShowNulls" $ShowNulls}
  pack $DispNullsButton -side top

  pack $WFieldDisplayControlFrame -side top -padx $pad -pady $pad -fill y


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                         Wire Display Mode Frame                       ##
  ##                                                                       ##
  ##  Initializes the frame where the user chooses the options for         ##
  ##  visualizing the wire characteristics.                                ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WWireDisplayControlFrame $WDisplayFrame.wireDisplayFrame
  frame $WWireDisplayControlFrame -borderwidth $borderwidth -relief $relief
  label $WWireDisplayControlFrame.label -text "Wire Display Mode" \
              -font $boldfont
  pack $WWireDisplayControlFrame.label -side top

  set WireDisplayVariable 0
  set GeometryButton $WWireDisplayControlFrame.geometry_button
  radiobutton $GeometryButton -text "Show Geometry" -font $font \
              -variable WireDisplayVariable -value 0 \
              -command {$WAntenna change_wire_mode "ShowGeometry" \
                        $WireDisplayVariable}
  $GeometryButton select
  pack $GeometryButton -side top

  set CurrentsButton $WWireDisplayControlFrame.currents_button
  radiobutton $CurrentsButton -text "Show Current Magnitude" -font $font \
              -variable WireDisplayVariable -value 1 \
              -command {$WAntenna change_wire_mode "ShowCurrentMagnitude" \
                        $WireDisplayVariable}
  pack $CurrentsButton -side top

  set PhaseButton $WWireDisplayControlFrame.phase_button
  radiobutton $PhaseButton -text "Show Current Phase" -font $font \
              -variable WireDisplayVariable -value 2 \
              -command {$WAntenna change_wire_mode "ShowCurrentPhase" \
                        $WireDisplayVariable}
  pack $PhaseButton -side top

  pack $WWireDisplayControlFrame -side top -padx $pad -pady $pad -fill y


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                            Frame oh frames oh my!                     ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  # The global frame
  set WGlobalFrame $WBottomFrame.globalFrame
  frame $WGlobalFrame

  set WGRightFrame $WGlobalFrame.rightFrame
  frame $WGRightFrame
  

  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                            Mouse Feedback Frame                       ##
  ##                                                                       ##
  ##  Initializes the frame where the application advises the user on what ##
  ##  the various mouse buttons do in the display window.                  ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WGMouseFrame $WGRightFrame.mouseFrame
  frame $WGMouseFrame -borderwidth $borderwidth -relief $relief
  set WGMouseLeftLabel $WGMouseFrame.leftLabel
  label $WGMouseLeftLabel -text "Left Mouse Button" -font $boldfont
  pack $WGMouseLeftLabel -side top 
  set WMouseLeft $WGMouseFrame.leftText
  label $WMouseLeft -text "-" -font $font -height 1
  pack $WMouseLeft -side top 
  set WGMouseMiddleLabel $WGMouseFrame.middleLabel
  label $WGMouseMiddleLabel -text "Middle Mouse Button" -font $boldfont
  pack $WGMouseMiddleLabel -side top 
  set WMouseMiddle $WGMouseFrame.middleText
  label $WMouseMiddle -text "-" -font $font -height 1
  pack $WMouseMiddle -side top 
  set WGMouseRightLabel $WGMouseFrame.rightLabel
  label $WGMouseRightLabel -text "Right Mouse Button" -font $boldfont
  pack $WGMouseRightLabel -side top 
  set WMouseRight $WGMouseFrame.rightText
  label $WMouseRight -text "-" -font $font -height 1
  pack $WMouseRight -side top 
  pack $WGMouseFrame -side top -fill both -expand 1 \
      -padx $pad -pady $pad -ipadx $pad -ipady $pad


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                          Slider Devices Frame                         ##
  ##                                                                       ##
  ##  Creates the slider bars in the bottom right corner of the screen     ##
  ##  that control the many different variables that affect the display    ##
  ##  of the output.                                                       ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WGScalesFrame $WGRightFrame.scalesFrame
  frame $WGScalesFrame -borderwidth $borderwidth -relief $relief

  set WGRings $WGScalesFrame.rings
  lscale $WGRings "Rings " "SetGlobal rings" horizontal 1 64
  pack $WGRings -side bottom -fill x
  lappend GlobalLs "lscale_set $WGRings \[$WAntenna global rings\]"

  set WGSlices $WGScalesFrame.slices
  lscale $WGSlices "Slices" "SetGlobal slices" horizontal 3 64
  pack $WGSlices -side bottom -fill x
  lappend GlobalLs "lscale_set $WGSlices \[$WAntenna global slices\]"

  set WGDBHeight $WGScalesFrame.db_height
  lscale2 $WGDBHeight "Default Boom Height" "DBHeight" \
  horizontal 5 60 
  $WGDBHeight.slider set 30
  pack $WGDBHeight -side bottom -fill x

  set WGScale $WGScalesFrame.scale
  lscale2 $WGScale "Antenna Scale" "Scale" \
  horizontal 1 16 
  $WGScale.slider set 5 
  pack $WGScale -side bottom -fill x

  set WGPDScale $WGScalesFrame.pd_scale
  lscale2 $WGPDScale "Field Scale" "PDScale" \
  horizontal 1 20 
  $WGPDScale.slider set 5
  pack $WGPDScale -side bottom -fill x

  set WGPSScale $WGScalesFrame.ps_scale
  lscale2 $WGPSScale "Dot Size" "PSScale" \
  horizontal 1 100 
  $WGPSScale.slider set 10
  pack $WGPSScale -side bottom -fill x

  set WGStepSize $WGScalesFrame.step_size
  lscale2 $WGStepSize "Resolution" "StepSize" \
  horizontal 1 45 
  $WGStepSize.slider set 5
  pack $WGStepSize -side bottom -fill x

  set WGNullThreshold $WGScalesFrame.null_threshold
  lscale2 $WGNullThreshold "Null Threshold" "NullThreshold" \
  horizontal 1 11 
  $WGNullThreshold.slider set 8.5
  pack $WGNullThreshold -side bottom -fill x

  set WGAlpha $WGScalesFrame.alpha
  lscale2 $WGAlpha "Transparency" "Alpha" \
  horizontal 10 0 
  $WGAlpha.slider set 5
  pack $WGAlpha -side bottom -fill x

  set WGFreqSteps $WGScalesFrame.freq_steps
  lscale2 $WGFreqSteps "Frequency Steps" "FreqSteps" \
  horizontal 0 30 
  $WGFreqSteps.slider set 5
  pack $WGFreqSteps -side bottom -fill x

  pack $WGScalesFrame -side top -fill x \
      -padx $pad -pady $pad -ipadx $pad -ipady $pad


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                        Control Buttons Frame                          ##
  ##                                                                       ##
  ##  Sets the control buttons in the bottom right corner of the screen.   ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  set WGControlFrame $WDisplayFrame.controlFrame
  frame $WGControlFrame
  label $WGControlFrame.label -font $font
  pack $WGControlFrame.label -side left -padx $pad -pady $pad

  set WexitButton $WGControlFrame.exitButton
  button $WexitButton -relief $relief -text "Exit" -command "destroy ." \
      -font $font 
  pack $WexitButton -side right -pady $pad

  set WresetButton $WGControlFrame.resetButton
  button $WresetButton -relief $relief -text "Reset" \
      -command "$WAntenna reset; UpdateAntennaControl; UpdateLight; \
                UpdateGlobal" -font $font
  pack $WresetButton -side right -pady $pad
  pack $WGControlFrame -side bottom -fill x

  pack $WGRightFrame -side right -fill both -expand 1
  pack $WGlobalFrame -side left -fill x -expand 1
  UpdateGlobal
  pack $WTopFrame -side top -fill both -expand 1
  pack $WBottomFrame -side bottom -fill x -expand 0


  ###########################################################################
  ###########################################################################
  ##                                                                       ##
  ##                         Binding Things Together                       ##
  ##                                                                       ##
  ###########################################################################
  ###########################################################################


  bind $WAntenna <Enter> {MouseText 1}
  bind $WAntenna <Leave> {MouseText 0}
  bind $WAntenna <ButtonPress-1> {MouseButtonAntenna %x %y 1 %W}
  bind $WAntenna <Button1-Motion> {MouseMotionAntenna %x %y 1 %W}
  bind $WAntenna <ButtonPress-2> {MouseButtonAntenna %x %y 2 %W}
  bind $WAntenna <Button2-Motion> {MouseMotionAntenna %x %y 2 %W}
  bind $WAntenna <ButtonPress-3> {MouseButtonAntenna %x %y 3 %W}
  bind $WAntenna <Button3-Motion> {MouseMotionAntenna %x %y 3 %W}

}


###############################################################################
###############################################################################
##                                                                           ##
##                                 Updates                                   ##
##                                                                           ##
##  Update values of the antenna widget.                                     ##
##                                                                           ##
##  newpos  = new positon of the mouse in pixels                             ##
##  oldpos  = old (last) positon of the mouse in pixels                      ##
##  valpp   = amount the value should change per pixel                       ##
##  minval  = minimum possible for value                                     ##
##  maxval  = maximum possible for value                                     ##
##  cyclic  = 1 if value exits minval or maxval it should wrap around        ##
##  command = command to read/set the value                                  ##
##                                                                           ##
###############################################################################
###############################################################################


proc ValueUpdate {newpos oldpos valpp minval maxval cyclic command} {

  #read the old value
  set value [eval $command]
  
  # calculate a new value
  set value [expr $value + ($oldpos - $newpos) * $valpp ]

  # make sure the new value is in range
  if {$cyclic} {
    while {$value > $maxval} {set value [expr $value - ($maxval - $minval)]}
    while {$value < $minval} {set value [expr $value + ($maxval - $minval)]}
  } else {
    if {$value > $maxval} {set value $maxval}
    if {$value < $minval} {set value $minval}
  }

  # finally set the new value
  eval $command $value

}


###############################################################################
###############################################################################
##                                                                           ##
##                           MouseButtonAntenna                              ##
##                                                                           ##
##  Handle mouse button presses in the antenna's widget.                     ##
##                                                                           ##
###############################################################################
###############################################################################


proc MouseButtonAntenna {x y button window} {

  global MouseX MouseY

  set MouseX $x
  set MouseY $y

}


###############################################################################
###############################################################################
##                                                                           ##
##                          Handle Mouse Movement                            ##
##                                                                           ##
##  Handle the motion of the mouse in the antenna's widget.                  ##
##                                                                           ##
###############################################################################
###############################################################################


proc MouseMotionAntenna {x y button window} {

  global MouseControl MouseX MouseY EyeLongitude EyeLatitude EyeDistance

  if {$MouseControl == "Element Position"} {
    if {$button == 1} {
      $window control_tube 1 [expr $x - $MouseX]
    }
    if {$button == 2} {
      $window control_tube 2 [expr $x - $MouseX]
    }
    if {$button == 3} {
      $window control_tube 3 [expr $x - $MouseX]
    }
  } elseif {$MouseControl == "Antenna Position"} {
    if {$button == 1} {
      $window control_ant 1 [expr $x - $MouseX]
    }
    if {$button == 2} {
      $window control_ant 2 [expr $x - $MouseX]
    }
    if {$button == 3} {
      $window control_ant 3 [expr $x - $MouseX]
    }

  } elseif {$MouseControl == "Element Rotation"} {
    if {$button == 1} {
      $window control_tube 13 [expr $x - $MouseX]
    }
    if {$button == 2} {
      $window control_tube 14 [expr $x - $MouseX]
    }
    if {$button == 3} {
      $window control_tube 15 [expr $x - $MouseX]
    }

  } elseif {$MouseControl == "Element Size"} {
    if {$button == 1} {
      $window control_tube 4 [expr $x - $MouseX]
    }
    if {$button == 2} {
      $window control_tube 5 [expr $x - $MouseX]
    }
    if {$button == 3} {
      $window control_tube 6 [expr $x - $MouseX]
    }

  } elseif {$MouseControl == "Wall Size"} {
    if {$button == 1} {
      $window control_tube 7 [expr $x - $MouseX] [expr $y - $MouseY]
    }
    if {$button == 2} {
      $window control_tube 8 [expr $x - $MouseX] [expr $y - $MouseY]
    }
    if {$button == 3} {
      $window control_tube 9 [expr $x - $MouseX] [expr $y - $MouseY]
    }

  } elseif {$MouseControl == "Wall Position"} {
    if {$button == 1} {
      $window control_tube 10 [expr $x - $MouseX]
    }
    if {$button == 2} {
      $window control_tube 11 [expr $x - $MouseX]
    }
    if {$button == 3} {
      $window control_tube 12 [expr $x - $MouseX]
    }

  } elseif {$MouseControl == "View"} {
    if {$button == 1} {
      ValueUpdate $x $MouseX -1 0 360 1 "$window eye longitude"
      ValueUpdate $y $MouseY -0.5 0 90 0 "$window eye latitude"
      set EyeLongitude [eval $window eye longitude]
      set EyeLatitude [eval $window eye latitude]
    }

    if {$button == 2} {
      ValueUpdate $y $MouseY 0.002 0 1 0 "$window eye distance"
      set EyeDistance [eval $window eye distance]
    }
    if {$button == 3} {
      $window move_center [expr $x - $MouseX] [expr $y - $MouseY]
      #puts [expr $x - $MouseX]
    }
  } 

  #store mouse pos as oldpos
  set MouseX $x
  set MouseY $y

}


###############################################################################
###############################################################################
##                                                                           ##
##                              Chooser Button                               ##
##                                                                           ##
##  Creates a chooser button.                                                ##
##                                                                           ##
##  path     = name of window path                                           ##
##  variable = global variable name that holds the current value             ##
##  texts    = list of text. One for each menu entry                         ##
##  command  = command that will be executed after the user chooses          ##
##  default  = index that will be the active one at start                    ##
##                                                                           ##
###############################################################################
###############################################################################


proc chooser {path variable texts {command {}} {default 0}} {

  global font relief AbleLs disablecolor enablecolor

  # create menu Button and menu
  menubutton $path -text [lindex "$texts" $default] \
    -relief $relief -menu $path.menu -indicatoron true -font $font
  menu $path.menu

  # create menu entries
  foreach text $texts {
    if {$command != {}} {
      set c "$path configure -text \"$text\";eval $command"
    } else {
      set c "$path configure -text \"$text\""
    }

    $path.menu add radiobutton -label $text -indicatoron false \
        -variable $variable -value $text -font $font -command $c
  }

  lappend AbleLs [list \
    "$path configure -state disabled -background $disablecolor" \
    "$path configure -state normal -background $enablecolor"]

}


###############################################################################
###############################################################################
##                                                                           ##
##                                Chooser Set                                ##
##                                                                           ##
##  Set the value of a chooser button.                                       ##
##                                                                           ##
##  path = name of window path                                               ##
##  index = index of the menu that should be set                             ##
##                                                                           ##
###############################################################################
###############################################################################


proc chooser_set {path index} {

  eval $path.menu invoke [expr $index +1]

}


###############################################################################
###############################################################################
##                                                                           ##
##                           Set Up Scale Device                             ##
##                                                                           ##
##  Creates a scale with a label.                                            ##
##                                                                           ##
##  path       = name of window path                                         ##
##  orient     = horizontal | vertical                                       ##
##  from/to    = range of the value                                          ##
##  resolution = slider can only have certain values (or resolution = 0)     ##
##  range      = 1 print number at the beginning and at the end              ##
##  command    = command to be executed when the slider has been moved       ##
##               command pathname value                                      ##
##                                                                           ##
###############################################################################
###############################################################################


proc lscale {path {text {}} {command {}} {orient vertical} {from 1.0} \
    {to 0.0} {resolution 0.0} {range 0}} {

  global font relief borderwidth AbleLs disablecolor enablecolor WAntenna

  set r {}
  if {$range} {
    set r "-tickinterval [expr $to - $from]"
  }

  frame $path
  label $path.label -text $text -font $font
  eval scale $path.slider -orient $orient -from $from -to $to $r -width 10 \
    -resolution $resolution -showvalue false \
    -command \"$command $path\" \
    -font $font

  if {$orient == "horizontal"} {
    pack $path.label -side left
    pack $path.slider -side right -fill x -expand 1
  } else {
    pack $path.label -side top
    pack $path.slider -side bottom -fill y -expand 1
  }

  lappend AbleLs [list \
    "$path.slider configure -state disabled" \
    "$path.slider configure -state normal" ]
  lappend AbleLs [ list \
    "$path configure -background $disablecolor" \
    "$path configure -background $enablecolor" ]
  lappend AbleLs [ list \
    "$path.slider configure -background $disablecolor \
    -troughcolor $disablecolor -highlightbackground $disablecolor" \
    "$path.slider configure -background $enablecolor \
    -troughcolor $enablecolor -highlightbackground  $enablecolor" ]
  lappend AbleLs [ list \
    "$path.label configure -background $disablecolor" \
    "$path.label configure -background $enablecolor"]
}


proc lscale2 {path {text {}} {command {}} {orient vertical} {from 1.0} \
    {to 0.0} {resolution 0.0} {range 0}} {

  global font relief borderwidth AbleLs disablecolor enablecolor WAntenna

  set r {}
  if {$range} {
    set r "-tickinterval [expr $to - $from]"
  }

  frame $path
  label $path.label -text $text -font $font
  scale $path.slider -orient $orient -from $from -to $to -width 10 \
    -command "$WAntenna change_mode $command" \
    -resolution $resolution -showvalue false \
    -font $font

  if {$orient == "horizontal"} {
    pack $path.label -side left
    pack $path.slider -side right -fill x -expand 1
  } else {
    pack $path.label -side top
    pack $path.slider -side bottom -fill y -expand 1
  }

  lappend AbleLs [list \
    "$path.slider configure -state disabled" \
    "$path.slider configure -state normal" ]
  lappend AbleLs [ list \
    "$path configure -background $disablecolor" \
    "$path configure -background $enablecolor" ]
  lappend AbleLs [ list \
    "$path.slider configure -background $disablecolor \
    -troughcolor $disablecolor -highlightbackground $disablecolor" \
    "$path.slider configure -background $enablecolor \
    -troughcolor $enablecolor -highlightbackground  $enablecolor" ]
  lappend AbleLs [ list \
    "$path.label configure -background $disablecolor" \
    "$path.label configure -background $enablecolor"]

}

###############################################################################
###############################################################################
##                                                                           ##
##                        Set the Value of a Scale                           ##
##                                                                           ##
##  path  = name of window path                                              ##
##  value = new value for scale                                              ##
##                                                                           ##
###############################################################################
###############################################################################


proc lscale_set {path value} {

  set state [$path.slider configure -state]
  $path.slider configure -state normal
  eval $path.slider set $value
  $path.slider configure -state [lindex $state end]

}


###############################################################################
###############################################################################
##                                                                           ##
##                           Set Up Three Scales                             ##
##                                                                           ##
##  path       = name of window path                                         ##
##  text       = name for the whole thing                                    ##
##  labels     = list for three names for each of the three scales           ##
##  orient     = horizontal | vertical                                       ##
##  from/to    = range of the value                                          ##
##  resolution = slider can only have certain values (or resolution = 0)     ##
##  range      = 1 print number at the beginning and at the end              ##
##  command    = command to be executed when the slider has been moved       ##
##               command pathname Rvalue Gvalue Bvalue                       ##
##                                                                           ##
###############################################################################
###############################################################################


proc rgbscale {path text {command {}} {labels {R G B}} {orient vertical} \
    {from {1.0 1.0 1.0}} {to {0.0 0.0 0.0}} {resolution 0.0} {range {0 0 0}}} {

  global font borderwidth relief disablecolor enablecolor AbleLs

  set com {}
  if {$command != {}} {
    set com "rgbscale_exec \{$command\} $path" 
  }

  frame $path -relief $relief -borderwidth $borderwidth
  frame $path.scales
  label $path.label -text $text -font $font

  foreach {c n} {r 0 g 1 b 2} {
    lscale $path.scales.$c [lindex $labels $n] $com $orient \
        [lindex $from $n] [lindex $to $n] $resolution [lindex $range $n]
    if {$orient == "vertical"} {
      pack $path.scales.$c -side left -fill y
    } else {
      pack $path.scales.$c -side top -fill x
    }
  }
    
  if {$orient == "vertical"} {
    pack $path.label -side top
    pack $path.scales -side bottom -fill y -expand 1
  } else {
    pack $path.label -side top
    pack $path.scales -side bottom -fill x -expand 1
  }

  lappend AbleLs [list \
    "$path configure -background $disablecolor" \
    "$path configure -background $enablecolor"]
  lappend AbleLs [list \
    "$path.scales configure -background $disablecolor" \
    "$path.scales configure -background $enablecolor"]
  lappend AbleLs [list \
    "$path.label configure -background $disablecolor" \
    "$path.label configure -background $enablecolor"]

}


proc rgbscale_exec {command path p v} {

  eval $command $path [eval $path.scales.r.slider get] \
      [eval $path.scales.g.slider get] [eval $path.scales.b.slider get]

}


###############################################################################
###############################################################################
##                                                                           ##
##                              Set Up Scales                                ##
##                                                                           ##
##  path   = name of window path                                             ##
##  valueR = new value for red scale                                         ##
##  valueG = new value for green scale                                       ##
##  valueB = new value for blue scale                                        ##
##                                                                           ##
###############################################################################
###############################################################################


proc rgbscale_set {path valueR valueG valueB} {

  lscale_set $path.scales.r $valueR
  lscale_set $path.scales.g $valueG
  lscale_set $path.scales.b $valueB

}


###############################################################################
###############################################################################
##                                                                           ##
##                         Set Material Properties                           ##
##                                                                           ##
###############################################################################
###############################################################################


proc SetMaterial {prop {path .} {r 0.0} {g 0.0} {b 0.0}} {

  global Material WAntenna Shade

  set m 0

  if {$Material == "Table"} {set m 1}
  if {$Material == "Cube"}  {set m 2}
  
  if {$prop == "shininess"} {
    $WAntenna material $m $prop $r
  } elseif {$prop == "smooth"} {
    if {$Shade == "Flat"} {
      $WAntenna material $m $prop 0
    } else {
      $WAntenna material $m $prop 1
    }
  } else {
    $WAntenna material $m $prop $r $g $b
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                       Update Material Properties                          ##
##                                                                           ##
###############################################################################
###############################################################################


proc UpdateAntennaControl {} {

  global Material MaterialLs

  set m 0

  if {$Material == "Table"} {set m 1}
  if {$Material == "Cube"}  {set m 2}
  
  foreach e $MaterialLs {
    eval eval $e
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                          Create a Set of Scales                           ##
##                                                                           ##
##  frame      = pathname of the supporting widget                           ##
##  scalenames = list of names for the scales                                ##
##               each entry my by one name or two names                      ##
##               in the later case is the first name used for the            ##
##               commands and the last one as text at the widget             ##
##  setcommand = command that is used to send the values to the antenna      ##
##  type       = type of scales for update command (material|light)          ##
##  updatelist = listname for the updatecommand                              ##
##                                                                           ##
###############################################################################
###############################################################################


proc rgbscales {frame scalenames setcommand type updatelist} {

  global $updatelist pad WAntenna

  foreach el $scalenames {
    if {[llength $el] == 1} {
      set name $el
      set Name "[string toupper [string index $name 0]]\
        [string range $name 1 end ]"
    } else {
      set name [lindex $el 0]
      set Name [lindex $el 1]
    }
    set Widget $frame.$name
    rgbscale $Widget $Name "$setcommand $name"
    pack $Widget -side left -fill y -padx $pad -pady $pad
    lappend $updatelist "rgbscale_set $Widget \[$WAntenna $type $name\]"
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                          Update Light Properties                          ##
##                                                                           ##
###############################################################################
###############################################################################


proc UpdateLight {} {

  global Light LightLs WAntenna EyeLongitude EyeLatitude EyeDistance \
      EyeDefaultDistance LightAbleLs

  if {$Light == "Antenna"} { 
    eval $WAntenna eye longitude $EyeLongitude
    eval $WAntenna eye latitude  $EyeLatitude
    eval $WAntenna eye distance  $EyeDistance
    LightAble 
  } elseif {$Light == "View"} {
    eval $WAntenna eye longitude $EyeLongitude
    eval $WAntenna eye latitude  $EyeLatitude
    eval $WAntenna eye distance  $EyeDistance
    LightAble 
  } else {
    set m [string index $Light [expr [string length $Light] -1]]
    foreach e $LightLs {
      eval eval $e
    }
    eval $WAntenna eye longitude [eval $WAntenna light $m longitude]
    eval $WAntenna eye latitude  [eval $WAntenna light $m latitude]
    if {[eval $WAntenna light $m type] > 1} {
      eval $WAntenna eye distance [eval $WAntenna light $m distance]
    } else {
      eval $WAntenna eye distance $EyeDefaultDistance
    }
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                            Set Light Properties                           ##
##                                                                           ##
###############################################################################
###############################################################################


proc SetLight {prop {path .} {r 0.0} {g ""} {b ""}} {

  global Light WAntenna Type EyeDefaultDistance

  if {$Light == "Antenna"} { 
  } elseif {$Light == "View"} {
  } else {
    set m [string index $Light [expr [string length $Light] -1]]
    if {$prop == "type"} {
      if {$Type == "Off"} {
        $WAntenna light $m $prop 0
        eval $WAntenna eye distance $EyeDefaultDistance
      } elseif {$Type == "Direct."} {
        $WAntenna light $m $prop 1
        eval $WAntenna eye distance $EyeDefaultDistance
      } elseif {$Type == "Posit."} {
        $WAntenna light $m $prop 2
        eval $WAntenna eye distance [eval $WAntenna light $m distance]
      } else {
        $WAntenna light $m $prop 3
        eval $WAntenna eye distance [eval $WAntenna light $m distance]
      }
    } else {
      eval $WAntenna light $m $prop $r $g $b
    }
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                   Update Light Attenuation Properties                     ##
##                                                                           ##
###############################################################################
###############################################################################


proc SetAttenuation {path constant linear quadratic} {

  global Light WAntenna

  if {$Light != "Antenna" && $Light != "View"} { 
    set m [string index $Light [expr [string length $Light] -1]]
    eval $WAntenna light $m constant $constant
    eval $WAntenna light $m linear $linear
    eval $WAntenna light $m quadratic $quadratic
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                          Update Light Properties                          ##
##                                                                           ##
###############################################################################
###############################################################################


proc Attenuation_Set {path} {

  global Light WAntenna

  if {$Light != "Antenna" && $Light != "View"} { 
    set m [string index $Light [expr [string length $Light] -1]]
    rgbscale_set $path [ $WAntenna light $m constant ] \
      [$WAntenna light $m linear] \
      [$WAntenna light $m quadratic]
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                          Update Global Properties                         ##
##                                                                           ##
###############################################################################
###############################################################################


proc UpdateGlobal {} {

  global GlobalLs WAntenna

  foreach e $GlobalLs {
    eval eval $e
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                           Set Global Properties                           ##
##                                                                           ##
###############################################################################
###############################################################################


proc SetGlobal {prop {path ""} {r ""} {g ""} {b ""}} {

  global WAntenna Viewer

  if {$prop == "viewer"} {
    if {$Viewer == "Global"} {
      $WAntenna global $prop 1
    } else {
      $WAntenna global $prop 0
    }
  } else {
    eval $WAntenna global $prop $r $g $b
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                                  Help Text                                ##
##                                                                           ##
##  enter = 1 if the mouse is in the window, 0 else                          ##
##                                                                           ##
###############################################################################
###############################################################################


proc MouseText {enter} {

  global Light MouseControl WMouseLeft WMouseMiddle WMouseRight Type

  if {$enter} {
    if {$MouseControl == "View"} {
      set l "Rotate and move antenna"
      set m "Change distance from antenna"
      set r "Translate antenna position"
    } elseif {$MouseControl == "Antenna Position"} {
      set l "Move forward and backward"
      set m "Move left and right"
      set r "Move up and down"
    } elseif {$MouseControl == "Element Position"} {
      set l "Move forward and backward"
      set m "Move left and right"
      set r "Move up and down"
    } elseif {$MouseControl == "Element Size"} {
      set l "Change length"
      set m "Change diameter"
      set r ""
    } elseif {$MouseControl == "Wall Position"} {
      set l "Move forward and backward"
      set m "Move left and right"
      set r "Move up and down"
    } elseif {$MouseControl == "Wall Size"} {
      set l "Change depth"
      set m "Change width"
      set r "Change height"
    } else {
      set l ""
      set m ""
      set r ""
    }   

    $WMouseLeft   configure -text $l
    $WMouseMiddle configure -text $m
    $WMouseRight  configure -text $r
  } else {
    $WMouseLeft   configure -text "-"
    $WMouseMiddle configure -text "-"
    $WMouseRight  configure -text "-"
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                                 Light Frame                               ##
##                                                                           ##
##  Enables or disables the different parts of the light frame               ##
##  LightAbleLs contains five elements.                                      ##
##  Each element is a pair. Each element of that pair is a list              ##
##  of commands. The first element of the pair disables widgets and          ##
##  the secound enables them.                                                ##
##                                                                           ##
###############################################################################
###############################################################################


proc LightAble {} {

  global WAntenna Light LightAbleLs LightLs

  if {$Light == "Antenna" || $Light == "View"} {
    set n 0
  } else {
    set m [ string index $Light [ expr [ string length $Light ] -1 ] ]
    set n [ expr [ $WAntenna light $m type ] + 1 ]
  }

  foreach ls $LightAbleLs {
    foreach com $ls {
      eval [ lindex $com [ expr $n > 0 ] ]
    }
    incr n -1
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                                InsertAnt                                  ##
##                                                                           ##
##  Inserts and antenna into the scene.                                      ##
##                                                                           ##
###############################################################################
###############################################################################


proc InsertAnt {ant_type} {

  global WAntenna

  $WAntenna std_ant "$ant_type"

}


###############################################################################
###############################################################################
##                                                                           ##
##                               LoadAntFile                                 ##
##                                                                           ##
##  Puts up a file selection dialog box.                                     ##
##                                                                           ##
###############################################################################
###############################################################################


proc LoadAntFile {} {

  global WAntenna

  set file_name [fileselect "File Selection" "*.nec"]
  
  if {[string length $file_name] > 0} {
    $WAntenna load_ant $file_name
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                                ScaleAction                                ##
##                                                                           ##
##  Performs scaling of values.                                              ##
##                                                                           ##
###############################################################################
###############################################################################


proc ScaleAction {value} {

  puts "value = $value"

}


###############################################################################
###############################################################################
##                                                                           ##
##                                  SaveFile                                 ##
##                                                                           ##
##  Saves the antenna as a .nec file.                                        ##
##                                                                           ##
###############################################################################
###############################################################################


proc SaveFile {WAntenna} {

  set file_name [GetValue "Please Enter File Name"]

  if {[string length $file_name] > 0} {
    $WAntenna save_file $file_name
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                                 SaveRGBImage                              ##
##                                                                           ##
##  Saves the image in the main antenna vis window as a color EPS file.      ##
##                                                                           ##
###############################################################################
###############################################################################


proc SaveRGBImage {WAntenna} {

  set file_name [GetValue "Please Enter File Name"]

  if {[string length $file_name] > 0} {
    $WAntenna save_rgb_image $file_name
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                                DeleteCurrentAnt                           ##
##                                                                           ##
##  Presently does not ask for confirmation.                                 ##
##                                                                           ##
###############################################################################
###############################################################################


proc DeleteCurrentAnt {} {

  global WAntenna

  $WAntenna delete_ant 

}


###############################################################################
###############################################################################
##                                                                           ##
##                                  SetFrequency                             ##
##                                                                           ##
##  Pops up a dialog box to ask for the frequency value.                     ##
##                                                                           ##
###############################################################################
###############################################################################


proc SetFrequency {} {

  global WAntenna

  set Frequency [GetValue "Please Enter Frequency"]
  if {[string length $Frequency] > 0} {
    $WAntenna change_mode "Freq" $Frequency
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                                  GetValue                                 ##
##                                                                           ##
##  Used by functions that require a popup dialog box.                       ##
##                                                                           ##
###############################################################################
###############################################################################


proc GetValue { string } {

  global prompt font

  set f [toplevel .prompt -borderwidth 10]
  message $f.msg -text $string -font $font
  entry $f.entry -textvariable prompt(result)
  set b [frame $f.buttons -bd 10]
  pack $f.msg $f.entry $f.buttons -side top -fill x

  button $b.ok -text OK -command {set prompt(ok) 1} \
          -underline 0
  button $b.cancel -text Cancel -command {set prompt(ok) 0} \
          -underline 0
  pack $b.ok -side left
  pack $b.cancel -side right

  foreach w [list $f.entry $b.ok $b.cancel] {
    bindtags $w [list .prompt [winfo class $w] $w all]
  }

  bind .prompt <Alt-o> "focus $b.ok ; break"
  bind .prompt <Alt-c> "focus $b.cancel ; break"
  bind .prompt <Alt-Key> break
  bind .prompt <Return> {set prompt(ok) 1}
  bind .prompt <Control-c> {set prompt(ok) 0}

  focus $f.entry
  grab $f
  tkwait variable prompt(ok)
  grab release $f
  destroy $f
  if {$prompt(ok)} {
    return $prompt(result)
  } else {
    return {}
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                            fileselectResources                            ##
##                                                                           ##
##  Used by functions that require a popup dialog box for traversing the     ##
##  directory structure and selecting a file.                                ##
##                                                                           ##
###############################################################################
###############################################################################


proc fileselectResources {} {

  # path is used to enter the file name
  option add *Fileselect*path.relief		sunken	startup
  option add *Fileselect*path.background	white	startup
  option add *Fileselect*path.foreground	black	startup

  # Text for the label on pathname entry
  option add *Fileselect*l.text		File:	startup

  # Text for the OK and Cancel buttons
  option add *Fileselect*ok*text		OK	startup
  option add *Fileselect*ok*underline		0	startup
  option add *Fileselect*cancel.text		Cancel	startup
  option add *Fileselect*cancel.underline 	0	startup

  # Size of the listbox
  option add *Fileselect*list.width		20	startup
  option add *Fileselect*list.height		10	startup

}


###############################################################################
###############################################################################
##                                                                           ##
##                                fileselect                                 ##
##                                                                           ##
##  Used by functions that require a popup dialog box for traversing the     ##
##  directory structure and selecting a file.                                ##
##                                                                           ##
##  Returns the selected pathname, or {}                                     ##
##                                                                           ##
###############################################################################
###############################################################################


proc fileselect {{why "File Selection"} {default {}} {mustExist 1} } {

  global fileselect

  set t [toplevel .fileselect -bd 4 -class Fileselect]
  fileselectResources  
  message $t.msg -aspect 1000 -text $why
  pack $t.msg -side top -fill x
    
  # Create a read-only entry for the current directory
  set fileselect(dirEnt) [entry $t.dir -width 15 \
        -relief flat -state disabled]
  pack $t.dir -side top -fill x
    
  # Create an entry for the pathname
  # The value is kept in fileselect(path)
  frame $t.top
  label $t.top.l -padx 0
  set e [entry $t.top.path -textvariable fileselect(path)]
  pack $t.top -side top -fill x
  pack $t.top.l -side left
  pack $t.top.path -side right -fill x -expand true

    
  # Create a listbox to hold the directory contents
  set lb [listbox $t.list \
        -yscrollcommand [list $t.scroll set]]
  scrollbar $t.scroll -command [list $lb yview]

  # Create the OK and Cancel buttons
  # The OK button has a rim to indicate it is the default
  frame $t.buttons -bd 10
  frame $t.buttons.ok -bd 2 -relief sunken
  set ok [button $t.buttons.ok.b \
        -command fileselectOK]
  set can [button $t.buttons.cancel \
        -command fileselectCancel]

  # Pack the list, scrollbar, and button box
  # in a horizontal stack below the upper widgets
  pack $t.list -side left -fill both -expand true
  pack $t.scroll -side left -fill y
  pack $t.buttons -side left -fill both
  pack $t.buttons.ok $t.buttons.cancel \
        -side top -padx 10 -pady 5
  pack $t.buttons.ok.b -padx 4 -pady 4
  fileselectBindings $t $e $lb $ok $can

  # Initialize variables and list the directory
  if {[string length $default] == 0} {
    set fileselect(path) {}
    set dir [pwd]
  } else {
    set fileselect(path) [file tail $default]
    set dir [file dirname $default]
  }
  set fileselect(dir) {}
  set fileselect(done) 0
  set fileselect(mustExist) $mustExist

  # Wait for the listbox to be visible so
  # we can provide feedback during the listing 
  tkwait visibility .fileselect.list
  fileselectList $dir

  tkwait variable fileselect(done)
  destroy $t
  return $fileselect(path)

}


###############################################################################
###############################################################################
##                                                                           ##
##                            fileselectBindings                             ##
##                                                                           ##
##  t - toplevel                                                             ##
##  e - name entry                                                           ##
##  lb - listbox                                                             ##
##  ok - OK button                                                           ##
##  can - Cancel button                                                      ##
##                                                                           ##
###############################################################################
###############################################################################


proc fileselectBindings { t e lb ok can } {

  # Elimate the all binding tag because we
  # do our own focus management
  foreach w [list $e $lb $ok $can] {
    bindtags $w [list $t [winfo class $w] $w]
  }

  # Dialog-global cancel binding
  bind $t <Control-c> fileselectCancel

  # Entry bindings
  bind $e <Return> fileselectOK
  bind $e <space> fileselectComplete

  # A single click, or <space>, puts the name in the entry
  # A double-click, or <Return>, selects the name
  bind $lb <space> "fileselectTake $%W ; focus $e"
  bind $lb <Button-1> \
        "fileselectClick %W %y ; focus $e"
  bind $lb <Return> "fileselectTake %W ; fileselectOK"
  bind $lb <Double-Button-1> \
        "fileselectClick %W %y ; fileselectOK"

  # Focus management.  	
  # <Return> or <space> selects the name.
  bind $e <Tab> "focus $lb ; $lb select set 0"
  bind $lb <Tab> "focus $e"

  # Button focus.  Extract the underlined letter
  # from the button label to use as the focus key.
  foreach but [list $ok $can] {
    set char [string tolower [string index  \
          [$but cget -text] [$but cget -underline]]]
    bind $t <Alt-$char> "focus $but ; break"
  }
  bind $ok <Tab> "focus $can"
  bind $can <Tab> "focus $ok"

  # Set up for type in
  focus $e

}


###############################################################################
###############################################################################
##                                                                           ##
##                              fileselectList                               ##
##                                                                           ##
##  Gets the list of files to select from.                                   ##
##                                                                           ##
###############################################################################
###############################################################################


proc fileselectList { dir {files {}} } {

  global fileselect

  # Update the directory display
  set e $fileselect(dirEnt)
  $e config -state normal
  $e delete 0 end
  $e insert 0 $dir
  $e config -state disabled

  # scroll to view the tail end
  $e xview moveto 1

  .fileselect.list delete 0 end
  set fileselect(dir) $dir
  if ![file isdirectory $dir] {
    .fileselect.list insert 0 "Bad Directory"
    return
  }
	
  .fileselect.list insert 0 Listing...
  update idletasks
  .fileselect.list delete 0
  if {[string length $files] == 0} {
    # List the directory and add an
    # entry for the parent directory
    set files [glob -nocomplain $fileselect(dir)/*]
    .fileselect.list insert end ../
  }

  # Sort the directories to the front
  set dirs {}
  set others {}
  foreach f [lsort $files] {
    if [file isdirectory $f] {
      lappend dirs [file tail $f]/
    } else {
      lappend others [file tail $f]
    }
  }
  foreach f [concat $dirs $others] {
    .fileselect.list insert end $f
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                               fileselectOK                                ##
##                                                                           ##
###############################################################################
###############################################################################


proc fileselectOK {} {

  global fileselect

  # Handle the parent directory specially
  if {[regsub {^\.\./?} $fileselect(path) {} newpath] != 0} {
    set fileselect(path) $newpath
    set fileselect(dir) [file dirname $fileselect(dir)]
    fileselectOK
    return
  }
    
  set path [string trimright $fileselect(dir)/$fileselect(path) /]
    
  if [file isdirectory $path] {
    set fileselect(path) {}
    fileselectList $path
    return
  }

  if [file exists $path] {
    set fileselect(path) $path
    set fileselect(done) 1
    return
  }

  # Neither a file or a directory.
  # See if glob will find something
  if [catch {glob $path} files] {

    # No, perhaps the user typed a new
    # absolute pathname
    if [catch {glob $fileselect(path)} path] {

      # Nothing good
      if {$fileselect(mustExist)} {

        # Attempt completion
        fileselectComplete

      } elseif [file isdirectory \
        [file dirname $fileselect(path)]] {

        # Allow new name
        set fileselect(done) 1
      }
      return

    } else {

      # OK - try again
      set fileselect(dir) [file dirname $fileselect(path)]
      set fileselect(path) [file tail $fileselect(path)]
      fileselectOK
      return
    }
  } else {

    # Ok - current directory is ok,
    # either select the file or list them.
    if {[llength [split $files]] == 1} {
      set fileselect(path) $files
      fileselectOK
    } else {
      set fileselect(dir) [file dirname [lindex $files 0]]
      fileselectList $fileselect(dir) $files
    }
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                             fileselectCancel                              ##
##                                                                           ##
###############################################################################
###############################################################################


proc fileselectCancel {} {

  global fileselect
  
  set fileselect(done) 1
  set fileselect(path) {}

}


###############################################################################
###############################################################################
##                                                                           ##
##                              fileselectClick                              ##
##                                                                           ##
##  Take the item the user clicked on.                                       ##
##                                                                           ##
###############################################################################
###############################################################################


proc fileselectClick { lb y } {

  global fileselect

  set fileselect(path) [$lb get [$lb nearest $y]]

}


###############################################################################
###############################################################################
##                                                                           ##
##                              fileselectTake                               ##
##                                                                           ##
##  Take the currently selected list item.                                   ##
##                                                                           ##
###############################################################################
###############################################################################


proc fileselectTake { lb } {

  global fileselect

  set fileselect(path) [$lb get [$lb curselection]]

}


###############################################################################
###############################################################################
##                                                                           ##
##                            fileselectComplete                             ##
##                                                                           ##
##  Complete the file selection list.                                        ##
##                                                                           ##
###############################################################################
###############################################################################


proc fileselectComplete {} {

  global fileselect

  # Do file name completion
  # Nuke the space that triggered this call
  set fileselect(path) [string trim $fileselect(path) \t\ ]

  # Figure out what directory we are looking at
  # dir is the directory
  # tail is the partial name
  if {[string match /* $fileselect(path)]} {
    set dir [file dirname $fileselect(path)]
    set tail [file tail $fileselect(path)]
  } elseif [string match ~* $fileselect(path)] {
    if [catch {file dirname $fileselect(path)} dir] {
      return	;# Bad user
    }
    set tail [file tail $fileselect(path)]
  } else {
    set path $fileselect(dir)/$fileselect(path)
    set dir [file dirname $path]
    set tail [file tail $path]
  }

  # See what files are there
  set files [glob -nocomplain $dir/$tail*]
  if {[llength [split $files]] == 1} {
    
    # Matched a single file
    set fileselect(dir) $dir
    set fileselect(path) [file tail $files]
  } else {
    if {[llength [split $files]] > 1} {
      # Find the longest common prefix
      set l [expr [string length $tail]-1]
      set miss 0
      # Remember that files has absolute paths
      set file1 [file tail [lindex $files 0]]
      while {!$miss} {
        incr l
        if {$l == [string length $file1]} {
          # file1 is a prefix of all others
          break
        }
        set new [string range $file1 0 $l]
        foreach f $files {
          if ![string match $new* [file tail $f]] {
            set miss 1
            incr l -1
            break
          }
        }
      }
      set fileselect(path) [string range $file1 0 $l]
    }
    fileselectList $dir $files
  }

}


###############################################################################
###############################################################################
##                                                                           ##
##                                   Main                                    ##
##                                                                           ##
###############################################################################
###############################################################################


CreateUserInterface

tkwait window .


###############################################################################
###############################################################################
##                                                                           ##
##                             End of antenna.tcl                            ##
##                                                                           ##
###############################################################################
###############################################################################






