#
#  For more information, please see: http://software.sci.utah.edu
# 
#  The MIT License
# 
#  Copyright (c) 2004 Scientific Computing and Imaging Institute,
#  University of Utah.
# 
#  License for the specific language governing rights and limitations under
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
# 
#  The above copyright notice and this permission notice shall be included
#  in all copies or substantial portions of the Software.
# 
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#


# GUI for IsoValueController module
# by Allen R. Sanderson
# SCI Institute
# University of Utah
# September 2005

catch {rename Fusion_Fields_StreamlineAnalyzer ""}

itcl_class Fusion_Fields_StreamlineAnalyzer {
    inherit Module
    constructor {config} {
        set name StreamlineAnalyzer
        set_defaults
    }

    method set_defaults {} {

	global $this-planes-list
	set $this-planes-list "0.0"

	global $this-planes-quantity
	set $this-planes-quantity 0

	global $this-color
	set $this-color 1

	global $this-maxWindings
	set $this-maxWindings 30

	global $this-override
	set $this-override 0

	global $this-curve-mesh
	set $this-curve-mesh 1

	global $this-island-centroids
	set $this-island-centroids 0

	global $this-scalar-field
	set $this-scalar-field 1

	global $this-show-islands
	set $this-show-islands 0

	global $this-overlaps
	set $this-overlaps 0
    }

    method ui {} {
        set w .ui[modname]
        if {[winfo exists $w]} {
            raise $w
            return
        }

	toplevel $w

	frame $w.plane_list
	label $w.plane_list.l -text "List of Planes:"
	entry $w.plane_list.e -width 40 -text $this-planes-list
	bind $w.plane_list.e <Return> "$this-c needexecute"
	pack $w.plane_list.l $w.plane_list.e -side left -fill both -expand 1

	
	###### Save the plane-quantity since the iwidget resets it
	frame $w.plane_quant
	global $this-planes-quantity
	set quantity [set $this-planes-quantity]
	iwidgets::spinint $w.plane_quant.q \
	    -labeltext "Number of evenly-spaced planes: " \
	    -range {0 100} -step 1 \
	    -textvariable $this-planes-quantity \
	    -width 10 -fixed 10 -justify right
	
	$w.plane_quant.q delete 0 end
	$w.plane_quant.q insert 0 $quantity

# 	label $w.plane_quant.l -text "List of Planes:"
# 	entry $w.plane_quant.e -width 40 -text $this-quantity-list -state disabled
# 	pack $w.plane_quant.l $w.plane_quant.e -side left -fill both -expand 1

	pack $w.plane_quant.q -side top -expand 1 -fill x -pady 5


	pack $w.plane_list $w.plane_quant -fill x


	frame $w.color -relief groove -borderwidth 2
	label $w.color.label -text "Color Style"

	frame $w.color.left
	radiobutton $w.color.left.orig     -text "Original Value" \
	    -variable $this-color -value 0
	radiobutton $w.color.left.input    -text "Input Order" \
	    -variable $this-color -value 1
	radiobutton $w.color.left.index    -text "Point Index" \
	    -variable $this-color -value 2

	frame $w.color.middle
	radiobutton $w.color.middle.plane   -text "Plane" \
	    -variable $this-color -value 3
	radiobutton $w.color.middle.winding -text "Winding Group" \
	    -variable $this-color -value 4
	radiobutton $w.color.middle.order   -text "Winding Group Order" \
	    -variable $this-color -value 5

	frame $w.color.right
	radiobutton $w.color.right.winding -text "Number of Windings" \
	    -variable $this-color -value 6
	radiobutton $w.color.right.twist   -text "Number of Twists" \
	    -variable $this-color -value 7
	radiobutton $w.color.right.safety  -text "Saftey Factor" \
	    -variable $this-color -value 8

	pack $w.color.left.orig $w.color.left.input \
	    $w.color.left.index -side top -anchor w
	pack $w.color.middle.plane $w.color.middle.winding \
	    $w.color.middle.order -side top -anchor w
	pack $w.color.right.winding $w.color.right.twist \
	    $w.color.right.safety -side top -anchor w

	pack $w.color.label -side top -fill both
	pack $w.color.left $w.color.middle $w.color.right \
	    -side left -anchor w


	frame $w.windings -relief groove -borderwidth 2

#	checkbutton $w.windings.check -variable $this-override-check

	global $this-override
	set override [set $this-override]
	iwidgets::spinint $w.windings.override -labeltext "Override winding: " \
	    -range {0 1000} -step 1 \
	    -textvariable $this-override \
	    -width 10 -fixed 10 -justify right
	
	$w.windings.override delete 0 end
	$w.windings.override insert 0 $override


	global $this-maxWindings
	set max [set $this-maxWindings]
	iwidgets::spinint $w.windings.max -labeltext "Max windings: " \
	    -range {0 1000} -step 1 \
	    -textvariable $this-maxWindings \
	    -width 10 -fixed 10 -justify right
	
	$w.windings.max delete 0 end
	$w.windings.max insert 0 $max


	pack $w.windings.max $w.windings.override -side left -fill x


	frame $w.mesh -relief groove -borderwidth 2
	radiobutton $w.mesh.crv -text "Curve Mesh" \
	    -variable $this-curve-mesh -value 1

	radiobutton $w.mesh.srf -text "Surface Mesh" \
	    -variable $this-curve-mesh -value 0

	frame $w.mesh.cloud
	checkbutton $w.mesh.cloud.check -variable $this-island-centroids
	label $w.mesh.cloud.label -text "Output Island Centroids" \
	    -width 24 -anchor w -just left
	pack $w.mesh.cloud.check $w.mesh.cloud.label -side left

	pack $w.mesh.crv $w.mesh.srf $w.mesh.cloud -side left -anchor w


	frame $w.field -relief groove -borderwidth 2
	radiobutton $w.field.scalar -text "Scalar Field" \
	    -variable $this-scalar-field -value 1

	radiobutton $w.field.vector -text "Vector Field" \
	    -variable $this-scalar-field -value 0

	pack $w.field.scalar $w.field.vector -side left -anchor w


	frame $w.misc -relief groove -borderwidth 2

	frame $w.misc.islands
	checkbutton $w.misc.islands.check -variable $this-show-islands
	label $w.misc.islands.label -text "Show Only Islands" -width 18 \
	    -anchor w -just left
	pack $w.misc.islands.check $w.misc.islands.label -side left


	frame $w.misc.overlaps
	label $w.misc.overlaps.label -text "Overlaps" \
	    -width 9 -anchor w -just left

	radiobutton $w.misc.overlaps.raw -text "Raw" \
	    -variable $this-overlaps -value 0
	radiobutton $w.misc.overlaps.remove -text "Remove" \
	    -variable $this-overlaps -value 1
	radiobutton $w.misc.overlaps.merge -text "Merge" \
	    -variable $this-overlaps -value 2
	radiobutton $w.misc.overlaps.smooth -text "Smooth" \
	    -variable $this-overlaps -value 3

	pack $w.misc.overlaps.label $w.misc.overlaps.raw \
	    $w.misc.overlaps.remove $w.misc.overlaps.merge \
	    $w.misc.overlaps.smooth -side left -anchor w


	pack $w.misc.islands $w.misc.overlaps \
	    -side left -anchor w


	pack $w.plane_list $w.color $w.windings $w.mesh $w.field $w.misc \
	    -side top -padx 4 -pady 4 -fill both

	makeSciButtonPanel $w $w $this
	moveToCursor $w
    }
}
