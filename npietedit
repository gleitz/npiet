#!/bin/sh
## -*- tcl -*-
## i am a tcl/tk program - launch me with a wish: \
exec wish "$0" "$@"
## 
## npietedit:						May 2004
## (schoenfr@web.de)
## 
## npietedit is a simple editor for the piet programming language
## this file is part of npiet.
##
## more about the piet programming language and the npiet interpreter see:
##
##	http://www.bertnase.de/npiet/
##
## about the piet programming language see:
##
##	http://www.dangermouse.net/esoteric/piet.html
##
##
## the default image size is about 20x20 (or 32x32) pixel or so.
## larger sizes are not very useful, because its very slow and
## looks ugly - so this is for simple playing with small programs.
##
##
## Copyright (C) 2004 Erik Schoenfelder (schoenfr@web.de)
##
## npiet is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation version 2.
##
## npiet is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with npiet; see the file COPYING.  If not, write to the Free
## Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
##
##

set version "npietedit v0.9d"

##
## howto launch:
##
proc usage { } {
    puts "\nnpietedit $version\n"
    puts "use: npietedit \[ <filename> \]"
    puts "\t<filename>   - file to load / save"
    exit
}

##
## catch broken x11 args: (hmm does not seem to work)
##
if { [catch [wm title . npietedit] err] } {
    puts "unknown window argument: $err"
    exit 
}

##
## here we go:
##
frame .top -borderwidth 10
frame .cpick
frame .cmd
frame .paint

pack .top -side top -fill x

##
## buttons:
##
label .top.version -text $version
button .top.quit -text quit -command exit
button .top.save -text Save -command { save_cells $filename }
button .top.load -text Load -command { load_cells $filename }
button .top.shrink -text Shrink -command { cells_enlarge -1 }
button .top.enlarge -text Enlarge -command { cells_enlarge +1 }

pack .top.shrink .top.enlarge .top.save .top.load .top.quit .top.version \
    -side left

##
## number of cells:
##
set c_maxx 48
set c_maxy 32 

for {set y 0} {$y < $c_maxy} {incr y} {
    for {set x 0} {$x < $c_maxx} {incr x} {
	set cells($x,$y) 19
    }
}

## color pick pixel zoom factor: (fixed)
set c_zc 20

## canvas pixel zoom factor: (may change when a pix is loaded)
set c_zp 20

## default canvas width / height:
set c_width [expr $c_zp * $c_maxx]
set c_height [expr $c_zp * $c_maxy]

## set some default filename:
set filename "npietedit-filename.ppm"


##
## 6 colors and 3 lightness values:
##
set n_hue 6
set n_light 3

##
## 6 light colors, 6 normal colors and 6 dark colors:
##
array set colors {

   0  "#FFC0C0"   1  "#FFFFC0"   2  "#C0FFC0"
   3  "#C0FFFF"   4  "#C0C0FF"   5  "#FFC0FF"

   6  "#FF0000"   7  "#FFFF00"   8  "#00FF00"
   9  "#00FFFF"  10  "#0000FF"  11  "#FF00FF"

  12  "#C00000"  13  "#C0C000"  14  "#00C000"
  15  "#00C0C0"  16  "#0000C0"  17  "#C000C0"
}


##
## the commands based on the color changes:
##
#   Commands
#                            Lightness change
#   Hue change      None    1 Darker   2 Darker
# 
#        None                  push        pop
#     1 Step        add    subtract   multiply
#     2 Steps    divide         mod        not
#     3 Steps   greater     pointer     switch
#     4 Steps duplicate        roll in(number)
#     5 Steps  in(char) out(number)  out(char)

##
## index is (light_change * 6 + hue_change):
##
array set commands {
     0  "nop"        6  "push"      12  "pop"
     1  "add"        7  "sub"       13  "mul"
     2  "div"        8  "mod"       14  "not"
     3  "greater"    9  "pointer"   15  "switch"
     4  "dup"       10  "roll"      16  "in(num)"
     5  "in(char)"  11  "out(num)"  17  "out(char)"
}


##
## cell info: current cell and current color index:
## (default color is white)
##
set cur_x -1
set cur_y -1
set cur_idx 19
## last picked color:
set cpick_x -1
set cpick_y -1


proc idx2col {idx} {
    global colors

    if {$idx == 18} { 
	return "#000000"
    } elseif {$idx == 19} {
	return "#ffffff"
    } elseif {$idx >= 0 && $idx < 18} {
	return $colors($idx)
    } else {
	return -1
    }
}

proc col2idx {r g b} {
    global colors

    set str [string toupper [format "#%02x%02x%02x" $r $g $b]]

    ## special case black and white:
    if {$str == "#000000"} {
	return 18
    }
    if {$str == "#FFFFFF"} {
	return 19
    }

    for {set i 0} {$i < 18} {incr i} {
	if {$colors($i) == $str} {
	    return $i
	}
    }
    ## substitute unknown color by white:
    return 19
}

##
## 
##
proc paint_rect {frame x y idx zoom} {

    set x [expr 2 + ($x * $zoom)]
    set y [expr 2 + ($y * $zoom)]
    
    set col [idx2col $idx]

    $frame create rectangle $x $y [expr $x + $zoom - 2] \
	[expr $y + $zoom - 2] -fill $col
}

proc paint_border {frame x y flag zoom} {

    set x [expr 1 + ($x * $zoom)]
    set y [expr 1 + ($y * $zoom)]

    if {$flag == 0} {
	set fill ""
	set outline "#ffffff"
    } else {
	set fill ""
	set outline "#000000"
    }
    $frame create rectangle $x $y [expr $x + $zoom] \
	    [expr $y + $zoom] -fill $fill -outline $outline
}


##
## canvas click/paint callback:
## 
proc click_canvas {x y button} {

    global .paint.canvas
    global cells cur_x cur_y cur_idx
    global c_maxx c_maxy c_zp
    global count_col_n

    set x [expr $x / $c_zp]
    set y [expr $y / $c_zp]

    if {$x < 0 || $y < 0 || $x >= $c_maxx || $y >= $c_maxy} {
	return
    }

    if {$button != 1} {
	set cur_idx $cells($x,$y)
	pick_color $cur_idx
    } else {
	set cells($x,$y) $cur_idx
	paint_rect .paint.canvas $x $y $cur_idx $c_zp
    }
    
    if {$cur_x >= 0 && $cur_y >= 0} {
	## reset old border:
	paint_border .paint.canvas $cur_x $cur_y 0 $c_zp
    }
    ## paint new border:
    paint_border .paint.canvas $x $y 1 $c_zp

    set cur_x $x
    set cur_y $y

    ## update display:
    draw_conn [count_col]
    draw_pos $x $y
}

##
## canvas motion callback:
## 
proc motion_canvas {x y} {

    global .cpick.c c_zp c_maxx c_maxy
    global cells cur_x cur_y cur_idx

    set x [expr $x / $c_zp]
    set y [expr $y / $c_zp]

    if {$x < 0 || $y < 0 || $x >= $c_maxx || $y >= $c_maxy} {
	return
    }

    draw_pos $x $y
}

proc draw_pos {x y} {
    global .cpick.c cur_x cur_y

    .cpick.c create rectangle 150 22 340 38 -fill white -outline grey
    .cpick.c create text 151 24 -anchor nw -text \
	"px=$cur_x py=$cur_y, x=$x y=$y"
}

proc draw_conn {n} {
    global .cpick.c 

    ## number of currently connected cells:
    set n 
    if {$n >= 32 && $n <= 127} {
	set n [format "$n (char: '%c')" $n]
    }

    .cpick.c create rectangle 150 2 340 18 -fill white -outline grey
    if {$n >= 1} {
	.cpick.c create text 151 4 -anchor nw -text "connected: $n"
    }
}


##
## color pick from color table:
##
canvas .cpick.c -bg white -width $c_width -height [expr $c_zc * 3 + 2]

pack .cpick.c -fill both
pack .cpick -fill both -side top

bind .cpick.c <1> "cpick %x %y"

##
## create color palette:
##
for {set i 0} {$i < 18} {incr i} {
    paint_rect .cpick.c [expr $i % 6] [expr $i / 6] $i $c_zc
}
paint_rect .cpick.c 6 0 18 $c_zc
paint_rect .cpick.c 6 1 19 $c_zc

## highlite default color: (white)
paint_border .cpick.c 6 1 1 $c_zc
set cpick_x 6
set cpick_y 1

##
## mouse callback:
##
proc cpick {x y} {
    global .cpick.c
    global cpick_x cpick_y cur_idx
    global c_zc

    set x [expr ($x / $c_zc) ]
    set y [expr ($y / $c_zc) ]
    
    if {$x == 6 && $y == 0} {
	## black
	set n_idx 18
    } elseif {$x == 6 && $y == 1} {
	## white
	set n_idx 19
    } elseif {$x >= 0 && $x <= 5 && $y >= 0 && $y <=2} {
	set n_idx [expr ($y * 6 + $x)]
    } else {
	## invalid click:
	return
    }

    set cur_idx $n_idx

    # puts "pick: $cur_idx"

    pick_color $cur_idx
}

##
## pick color from color table:
##
proc pick_color {idx} {

    global cpick_x cpick_y
    global c_zc

    if {$idx == 19} {
	set x 6
	set y 1
    } elseif {$idx == 18} {
	set x 6
	set y 0
    } else {
	set x [expr $idx % 6]
	set y [expr $idx / 6]
    }

    if {$cpick_x >= 0 && $cpick_y >= 0} {
	## reset old border:
	paint_border .cpick.c $cpick_x $cpick_y 0 $c_zc
    }
    ## paint new border:
    paint_border .cpick.c $x $y 1 $c_zc

    set cpick_x $x
    set cpick_y $y

    
}


##
## command frame:
##

canvas .cmd.c -bg white -width $c_width -height [expr $c_zc * 3 + 2]

pack .cmd.c -fill both
pack .cmd -fill both 

##
## create command strings:
##
for {set i 0} {$i < 18} {incr i} {
    set x [expr 2 + 56 * ($i % 6)] 
    set y [expr 2 + $c_zc * ($i / 6)]
    .cmd.c create text [expr $x + 2] [expr $y + 2] \
	-text $commands($i) -anchor nw
    .cmd.c create rectangle $x $y \
	[expr $x + 54] [expr $y + $c_zc - 2] -fill "" -outline black
}

bind .cmd.c <1> "cmd %x %y"

##
## mouse callback:
##
proc cmd {x y} {
    global .cmd.c
    global cmd_x cmd_y cmd_idx cur_idx
    global commands colors c_zc

    set x [expr ($x / 56) ]
    set y [expr ($y / $c_zc) ]

    set n_idx [expr $x + $y * 6]
    if {$n_idx < 18 && $x < 6 && $y < 3} {
	set cmd_hue [expr $n_idx % 6]
	set cmd_light [expr (($n_idx / 6) + 3) % 3]

	# set cur_idx [expr ($cur_idx + (6 * $cmd_light) + $cmd_hue) % 18]
	set ncx [expr (($cur_idx % 6) + $cmd_hue) % 6]
	set ncy [expr (($cur_idx / 6) + $cmd_light) % 3]
	set cur_idx [expr $ncx + 6 * $ncy]

	# puts "pick: (cmd: $x $y) -> $n_idx -> $commands($n_idx) hue=$cmd_hue light=$cmd_light -> $cur_idx $colors($cur_idx)"

	pick_color $cur_idx
    }    
}


##
## paint area:
##


set w ""
set c .paint.canvas

if {1} {
    ## stay with a fixed area:
    canvas .paint.canvas -bg white -width [expr $c_width + 1] \
	-height [expr $c_height + 1]
    .paint.canvas create rectangle 0 0 \
	[expr $c_width + 1] [expr $c_height + 1] -fill white -outline ""
} else {
    ##
    ## use a scrollable area: xxx - fix me 
    ## 
    canvas $c -scrollregion {0 0 200p 200p} \
	-xscrollc "$w.hscroll set" \
	-yscrollc "$w.vscroll set"
    
    scrollbar $w.vscroll  -relief sunken \
	-command "$c yview"

    scrollbar $w.hscroll -orient horiz \
	-relief sunken -command "$c xview"
    
    pack $w.vscroll -side right -fill y
    pack $w.hscroll -side bottom -fill x
    pack $c -expand yes -fill both
}

pack .paint.canvas -fill both
pack .paint -fill both -side left

bind .paint.canvas <Motion> "motion_canvas %x %y"
bind .paint.canvas <1> "click_canvas %x %y 1"
bind .paint.canvas <2> "click_canvas %x %y 2"
bind .paint.canvas <B1-Motion> "click_canvas %x %y 1"

for {set y 0} {$y < $c_maxy} {incr y} {
    for {set x 0} {$x < $c_maxx} {incr x} {
	paint_rect .paint.canvas $x $y 19 $c_zp
    }
}

##
##
##

## counter:
set count_col_n -1
array set count_col_arr { }

proc count_col_rec {x y} {

    global cur_x cur_y cur_idx
    global count_col_n count_col_arr
    global c_maxx c_maxy

    if {$x < 0 || $x >= $c_maxx || $y < 0 || $y >= $c_maxy} {
	return
    }
    if {$count_col_arr($x,$y) != $cur_idx} {
	return
    }

    ## avoid tcl limitation:
    if {$count_col_n == 990} {
	return
    }

    ## count, mark and recurse on:
    incr count_col_n
    set count_col_arr($x,$y) 9999

    count_col_rec [expr $x + 1] $y
    count_col_rec $x            [expr $y + 1]
    count_col_rec [expr $x - 1] $y
    count_col_rec $x            [expr $y - 1]
}

proc count_col { } { 

    global cells cur_x cur_y cur_idx
    global count_col_arr count_col_n
    
    ## ignore black/white/other colors:
    if {$cur_idx >= 18} {
	return 0
    }
    
    set count_col_n 0
    array set count_col_arr [array get cells]

    count_col_rec $cur_x $cur_y
    
    # puts "count_col: got $count_col_n"

    if {$count_col_n == 990} {
	## avoid tcl recursion error:
	return "???"
    } else {
	return $count_col_n
    }
}

##
## color index and r/g/b value:
##
array set c_red {
  0 255 1 255 2 192 3 192 4 192 5 255 6 255 7 255 8 0 9 0 10 0 11 255 12 192
  13 192 14 0 15 0 16 0 17 192 18 0 19 255
}

array set c_green {
   0 192 1 255 2 255 3 255 4 192 5 192 6 0 7 255 8 255 9 255 10 0 11 0 12 0
   13 192 14 192 15 192 16 0 17 0 18 0 19 255 
}

array set c_blue {
   0 192 1 192 2 192 3 255 4 255 5 255 6 0 7 0 8 0 9 255 10 255 11 255 12 0
   13 0 14 0 15 192 16 192 17 192 18 0 19 255
}

##
## for now we write ascii readable ppm files:
##
proc save_cells { filename } {

    global cells c_maxx c_maxy
    global c_red c_green c_blue

    set fp [open $filename "w"]
    if {$fp == ""} {
	puts "cannot save"
    } 

    puts $fp "P3\n\# a piet program $filename\n$c_maxx $c_maxy\n255"

    for {set y 0} {$y < $c_maxy} {incr y} {
	for {set x 0} {$x < $c_maxx} {incr x} {
	    set idx $cells($x,$y)
	    puts -nonewline $fp [format "%3d %3d %3d " \
				     $c_red($idx) $c_green($idx) $c_blue($idx)]
	    if {[expr ($x + $y * $c_maxx + 1) % 6] == 0} {
		puts $fp ""
	    } else {
		puts -nonewline $fp " "
	    }
	}
    }
    puts $fp ""
    close $fp
}


##
## load a program:
##
proc load_cells { filename } {

    global cells
    
    ## try ascii p3 first:
    if {[load_p3_cells $filename] >= 0} {
	return
    }

    ## continue with tcl builtin's:
    if { [catch {set img [image create photo -file $filename]} err] } {
	tk_messageBox -message "cannot read from $filename; reason: $err"
	return
    }

    set y 0
    set x 0
    set width 0
    set height 0
    set loop 1

    while { $loop == 1} {

	if { [catch {set rgb [$img get $x $y]} err] } {
	    if {$x == 0} {
		set loop 0
	    } else {
		set width $x
		set x 0
		incr y
	    }
	} else {
	    set idx [col2idx [lindex $rgb 0] [lindex $rgb 1] [lindex $rgb 2]]
	    set cells($x,$y) $idx

	    incr x
	}
    }

    cells_resize $width $y
}

##
## Tk, at least on debian woody, can only handle P6 binary ppm
## and cannot load P3 ascii ppm; so we try to handle this on 
## our own:
##
proc load_p3_cells { filename } {

    global cells c_maxx c_maxy

    # puts "deb: p3 load: $filename"
    
    set fd [open $filename "r"]
    if {$fd == ""} {
	# puts "cannot open $filename for reading"
	return -1
    } 

    ## get tag:
    set rc [gets $fd line]
    while {$rc >= 0 && [string index $line 0] == "#"} {
	set rc [gets $fd line]
    }
    if {$rc < 0} {
	# puts "cannot read $filename: no magic-tag found"
	close $fd
	return -1
    }

    if {$line != "P3"} {
	# puts "cannot read $filename: magic tag P3 not found"
	close $fd
	return -1
    }

    ##
    ## magic tag P3 found - now we can continue 
    ## (and be somewhat more verbose):
    ## 

    ## get size:
    set rc [gets $fd line]
    while {$rc >= 0 && [string index $line 0] == "#"} {
	set rc [gets $fd line]
    }

    if {$rc < 0 || 2 != [scan $line "%d %d" width height]} {
	puts "cannot read $filename: no size found"
	close $fd
	return -1
    }

    ## get #cols:
    set rc [gets $fd line]
    while {$rc >= 0 && [string index $line 0] == "#"} {
	set rc [gets $fd line]
    }

    if {$rc < 0 || 1 != [scan $line "%d" cols]} {
	puts "cannot read $filename: no color param found"
	close $fd
	return -1
    }
    if {$cols != 255} {
	## warn only:
	puts "color param is $cols, but 255 expected - result may be wrong"
    }

    if {$width * $height > 64 * 64} {
	puts "info: maybe picture is too large for npietedit ($width x $height) ..."
    }

    ## read remainig data:
    set data [read $fd]
    close $fd

    for {set y 0} {$y < $height} {incr y} {
	for {set x 0} {$x < $width} {incr x} {
	    
	    set rc 0

	    ##
	    ## this is way ugly and expensive: xxx - fix me
	    ##
	    for {set n 0} {$n < 3} {incr n} {
		## strip whites:
		set data [string trimleft $data]
		set rc [expr $rc + [scan $data "%d" rgb($n)]]
		## strip number:
		set data [string trimleft $data "0123456789"]
	    }

	    if {$rc != 3} {
		puts "cannot read $filename: error reading rgb triples"
		return -1
	    }

	    set idx [col2idx $rgb(0) $rgb(1) $rgb(2)]
	    set cells($x,$y) $idx
	}
    }
    set c_maxx $width
    set c_maxy $height

    cells_resize $c_maxx $c_maxy
    
    return 0
}

##
## shrink or enlarge the number of cells by the given delta:
##
proc cells_enlarge { delta } {

    global cells c_maxx c_maxy c_zp c_width c_height

    if {$delta < 0} {
	if {$c_maxx <= 4 || $c_maxy <= 4} {
	    ## ignore:
	    return
	}
    }

    cells_resize [expr $c_maxx + $delta] [expr $c_maxy + $delta]
}


##
## shrink or enlarge the number of cells to the new size:
##
proc cells_resize {width height} {

    global cells c_maxx c_maxy c_zp c_width c_height

    set c_maxx $width
    set c_maxy $height

    ##
    ## adjust zoom factor:
    ## try too keep it between 8 and 20
    ##
    set size [expr $c_maxx > $c_maxy ? $c_maxx : $c_maxy]
    if { $size > 0} {
	set c_zp [expr $c_width / $size]
	## make no sense to zoom further out:
	if {$c_zp < 8} {
	    set c_zp 8
	}
    } 
    if {$c_zp > 20} {
	set c_zp 20
    }

    # puts "got image width $c_maxx height $c_maxy (zoom $c_zp)"

    ##
    ## paint with new zoom calculated:
    ##
    .paint.canvas create rectangle 0 0 \
	[expr $c_width + 1] [expr $c_height + 1] -fill white -outline ""
    for {set y 0} {$y < $c_maxy} {incr y} {
	for {set x 0} {$x < $c_maxx} {incr x} {
	    if {[catch {set idx $cells($x,$y)} err]} {
		## set non existing cells to white:
		set idx 19
		set cells($x,$y) $idx
	    }
	    paint_rect .paint.canvas $x $y $idx $c_zp
	}
    }

}


##
## args:
##
if {$argc == 1} {
    if {[string index [lindex $argv 0] 0] == "-"} {
	usage
    }
    ##
    ## filename to use:
    ##
    set filename [lindex $argv 0]
    puts "filename: $filename"
    if {[file readable $filename] == 1} {
	load_cells $filename
    }
} elseif {$argc > 1} {
    usage
}
