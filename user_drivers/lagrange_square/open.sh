#!/bin/bash

RAYTARGET=$HOSTNAME # cake, rice

if [ "$RAYTARGET" == "cake" ]; then
OOMPH_DIR="oomphlib/oomphlib_current"
WORKING_DIR=$OOMPH_DIR"/user_drivers/lagrange_square"

SRC_NS_DIR=$OOMPH_DIR"/src/navier_stokes"
SRC_GEN_DIR=$OOMPH_DIR"/src/generic"

gvim -p \
  ~/$WORKING_DIR/square0.cc \
  ~/$SRC_NS_DIR/lagrange_enforced_flow_preconditioner.h \
  ~/$SRC_NS_DIR/constrained_navier_stokes_preconditioners.h \
  ~/$SRC_NS_DIR/constrained_navier_stokes_preconditioners.cc \
  ~/$SRC_GEN_DIR/block_preconditioner.h \
  ~/$SRC_GEN_DIR/block_preconditioner.cc \
  ~/$SRC_GEN_DIR/general_purpose_block_preconditioners.h
elif [ "$RAYTARGET" == rice ]; then
  echo "Not done for $RAYTARGET"
  read -p "Press [Enter] key to quit."
  exit
else
  echo "No such target for $RAYTARGET"
  read -p "Press [Enter] key to quit."
  exit
fi


