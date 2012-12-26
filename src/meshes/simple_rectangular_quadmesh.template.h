//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.90. August 3, 2009.
//LIC// 
//LIC// Copyright (C) 2006-2009 Matthias Heil and Andrew Hazel
//LIC// 
//LIC// This library is free software; you can redistribute it and/or
//LIC// modify it under the terms of the GNU Lesser General Public
//LIC// License as published by the Free Software Foundation; either
//LIC// version 2.1 of the License, or (at your option) any later version.
//LIC// 
//LIC// This library is distributed in the hope that it will be useful,
//LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
//LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//LIC// Lesser General Public License for more details.
//LIC// 
//LIC// You should have received a copy of the GNU Lesser General Public
//LIC// License along with this library; if not, write to the Free Software
//LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//LIC// 02110-1301  USA.
//LIC// 
//LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
//LIC// 
//LIC//====================================================================
#ifndef OOMPH_SIMPLE_RECTANGULAR_QUADMESH_HEADER
#define OOMPH_SIMPLE_RECTANGULAR_QUADMESH_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

// OOMPH-LIB headers
#include "../generic/mesh.h"
#include "../generic/matrices.h"
#include "../generic/quadtree.h"
#include "../generic/quad_mesh.h"

namespace oomph
{

//=======================================================================
/// Simple rectangular 2D Quad mesh class.
/// Nx  : number of elements in the x direction
///
/// Ny  : number of elements in the y direction
///
/// Lx  : length in the x direction
///
/// Ly  : length in the y direction
///
/// Ordering of elements: 'Lower left' to 'lower right' then 'upwards'
///
/// Timestepper defaults to Steady.
//=======================================================================
template <class ELEMENT>
class SimpleRectangularQuadMesh : public virtual QuadMeshBase
{

  public:

 /// \short Constructor: Pass number of elements in the horizontal 
 /// and vertical directions, and the corresponding dimensions.
 /// Timestepper defaults to Steady.
 SimpleRectangularQuadMesh(const unsigned &Nx, const unsigned &Ny, 
                           const double &Lx, const double &Ly,
                           TimeStepper* time_stepper_pt=
                           &Mesh::Default_TimeStepper); 

 /// Access function for number of elements in x directions
 const unsigned& nx() const {return NX;}

 /// Access function for number of elements in y directions
 const unsigned& ny() const {return NY;}
 
private:

 /// Number of elements in x direction
 unsigned NX;

 /// Number of elements in y direction
 unsigned NY;

};

}

#endif
