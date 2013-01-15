#!MC 1000
$!VarSet |MFBD| = '/misc/biowulf/dmeyer/wed11/oomph/demo_drivers/beam/unsteady_ring'
$!DRAWGRAPHICS FALSE
$!CREATEMIRRORZONES 
  SOURCEZONES =  [1-26]
  MIRRORVAR = 'Y' 
$!CREATEMIRRORZONES 
  SOURCEZONES =  [1-26]
  MIRRORVAR = 'X' 
$!CREATEMIRRORZONES 
  SOURCEZONES =  [27-52]
  MIRRORVAR = 'X' 
$!FIELD [40-52]  GROUP = 2
$!FIELD [66-78]  GROUP = 2
$!FIELD [92-104]  GROUP = 2
$!FIELD [1-13,27-39,53-65,79-91]  VECTOR{COLOR = BLUE}
$!FIELD [1-13,27-39,53-65,79-91]  VECTOR{ARROWHEADSTYLE = FILLED}
$!FIELD [1-13,27-39,53-65,79-91]  VECTOR{VECTORTYPE = HEADATPOINT}
$!FIELD [1-13,27-39,53-65,79-91]  MESH{COLOR = BLACK}
$!FIELD [1-13,27-39,53-65,79-91]  MESH{LINETHICKNESS = 0.8}
$!FIELD [14-26,40-52,66-78,92-104]  MESH{COLOR = BLACK}
$!FIELD [14-26,40-52,66-78,92-104]  MESH{LINETHICKNESS = 0.8}
$!FIELD [14-26,40-52,66-78,92-104]  VECTOR{COLOR = RED}
$!FIELD [14-26,40-52,66-78,92-104]  VECTOR{ARROWHEADSTYLE = FILLED}
$!RemoveVar |MFBD|