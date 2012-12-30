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
// Common base class for all Tet Meshes
#ifndef OOMPH_TETMESH_HEADER
#define OOMPH_TETMESH_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//oomph-lib includes
#include "Vector.h"
#include "nodes.h"
#include "matrices.h"
#include "mesh.h"

namespace oomph
{

//=======================================================================
/// Base class defining a closed (orientable) surface for Tetgen-based
/// mesh generation.
//=======================================================================
class TetgenMeshClosedSurface
{
  
public:

 /// Empty constructor
 TetgenMeshClosedSurface()
  {
   //Set default value for the region (no region)
   Region = 0;
  }

 /// Empty destructor
 virtual ~TetgenMeshClosedSurface(){}

 /// Coordinate of the internal point
 const double &internal_point(const unsigned &i) const
  {return Internal_point[i];}

 void set_hole(const Vector<double> &hole_point)
 {
  Region = -1;
  Internal_point = hole_point;
 }

 /// Set the region
 void set_region(const int &region,
                 const Vector<double> &region_point)
 {
  Internal_point = region_point;
  Region = region;
 }

 /// Return the region flag
 const int &region() {return Region;}

 /// Is this a hole
 bool is_hole() {if (Region==-1) {return true;} else {return false;}}

 /// Is it a specified region (i.e. region is positive)
 bool is_region() {if(Region > 0) {return true;} else {return false;}}

  protected:

 ///\short Integer to indicate the region contained within the surface
 /// Default values -10 (no internal point set), -1 (it's a hole)
 int Region;

 /// Storage for an internal point
 Vector<double> Internal_point;

};


//========================================================================
/// Class that represents a (closed) tetgen boundary bounded by polygonal
/// planar facets
//========================================================================
class TetgenMeshFacetedSurface : public TetgenMeshClosedSurface
{
  public:

 ///\short Constructor: Takes a vector of vertex coordinates,
 /// a vector of facet connectivities and an optional vector of
 /// boundary ids associated with each facet. The final boolean argument
 /// is used to control whether boundaries can be split. If it is false
 /// then additional nodes will NOT be added on any boundaries.
 TetgenMeshFacetedSurface(const Vector<Vector<double> > &vertex_coordinate,
                          const Vector<Vector<unsigned> > &facet,
                          const bool &split_boundaries=true) :
 TetgenMeshClosedSurface(), Vertex_coordinate(vertex_coordinate), Facet(facet),
  Split_boundaries(split_boundaries)
 {
   //Check for dimensions
#ifdef PARANOID
   //Find the number of vertices
   const unsigned n_vertex=Vertex_coordinate.size();
   for (unsigned i=0;i<n_vertex;i++)
    {
     if (Vertex_coordinate[i].size()!=3)
      {
       std::ostringstream error_stream;
       error_stream 
        << "TetgenMeshPoly should only be used in 3D!\n"
        << "Your Vector of coordinates, contains data for " 
        << Vertex_coordinate[i].size() 
        << "-dimensional coordinates." << std::endl;
       throw OomphLibError(error_stream.str(),
                           "TetgenMeshPoly::TetgenMeshPoly()",
                           OOMPH_EXCEPTION_LOCATION);
      }
    }

   //Check for same size
#endif
  }
 
 ///\short Constructor: Takes a vector of vertex coordinates,
 /// a vector of facet connectivities and an optional vector of
 /// boundary ids associated with each facet.
 /// The final boolean argument
 /// is used to control whether boundaries can be split. If it is false
 /// then additional nodes will NOT be added on any boundaries. 
 TetgenMeshFacetedSurface(const Vector<Vector<double> > &vertex_coordinate,
                          const Vector<Vector<unsigned> > &facet,
                          const Vector<unsigned> &facet_boundary_id,
                          const bool &split_boundaries=true) :
  Vertex_coordinate(vertex_coordinate), Facet(facet),
   Facet_boundary_id(facet_boundary_id), Split_boundaries(split_boundaries)
 {
  //Find the number of vertices
  const unsigned n_vertex=Vertex_coordinate.size();

  //Check for dimensions
#ifdef PARANOID
   for (unsigned i=0;i<n_vertex;i++)
    {
     if (Vertex_coordinate[i].size()!=3)
      {
       std::ostringstream error_stream;
       error_stream 
        << "TetgenMeshPoly should only be used in 3D!\n"
        << "Your Vector of coordinates, contains data for " 
        << Vertex_coordinate[i].size() 
        << "-dimensional coordinates." << std::endl;
       throw OomphLibError(error_stream.str(),
                           "TetgenMeshPoly::TetgenMeshPoly()",
                           OOMPH_EXCEPTION_LOCATION);
      }
    }

   //Check for same size
#endif

   //Assign boundary ids to the verticess. It doesn't matter which one is used
   //just the first
   //If boundary ids have been attached to the facets
   if(Facet_boundary_id.size()!=0)
    {
     //Resize the vertex boundary id vector
     Vertex_boundary_id.resize(n_vertex);
     //Now loop over the facets and assign ids to the nodes
     //This will involve some overwriting, but it doesn't matter
     const unsigned n_facet = Facet.size();
     for(unsigned f=0;f<n_facet;++f)
      {
       //Find the number of nodes
       const unsigned n_node = Facet[f].size();
       for(unsigned i=0;i<n_node;++i)
        {
         Vertex_boundary_id[Facet[f][i]] = Facet_boundary_id[f];
        }
      }
    }
 }

 
 
 /// Empty destructor
 virtual ~TetgenMeshFacetedSurface(){}

 /// Number of vertices
 unsigned nvertex() const {return Vertex_coordinate.size();}
 
 /// Number of facets
 unsigned nfacet() const {return Facet.size();}
  
 /// Boundary id
 unsigned facet_boundary_id(const unsigned &i) const 
  {return Facet_boundary_id[i];}

 ///Return the vertex boundary id
 const unsigned &vertex_boundary_id(const unsigned &n) const
  {return Vertex_boundary_id[n];}
 
 /// i-th coordinats of n-th vertex
 const double &vertex_coordinate(const unsigned &n, const unsigned &i)
  const 
  {return Vertex_coordinate[n][i];}

 /// Coordinate vector of i-th vertex (const version)
 Vector<double> vertex_coordinate(const unsigned& i) const
  {
   return Vertex_coordinate[i];
  }   

 /// Coordinate vector of i-th vertex
 Vector<double> &vertex_coordinate(const unsigned& i)
  {
   return Vertex_coordinate[i];
  }
 
 /// Definition of ith facet via connectivity list (const version)
 Vector<unsigned> facet(const unsigned &i) const
 {
  return Facet[i];
 }

 /// Definition of ith facet via connectivity list
 Vector<unsigned> &facet(const unsigned &i)
 {
  return Facet[i];
 }

 /// Test whether boundaries can be split
 bool can_boundaries_be_split() {return Split_boundaries;}
 
private:
  
 /// Vector of Vector of vertex coordinates
 Vector<Vector<double> > Vertex_coordinate;

 /// Vector of vectors of facet connectivity
 Vector<Vector<unsigned> > Facet;
   
 /// Vector of vertex boundary ids
 Vector<unsigned> Vertex_boundary_id;

 /// Vector of Facet boundary ids
 Vector<unsigned> Facet_boundary_id;

 /// Boolean to indicate whether extra vertices can be added on the boundary
 bool Split_boundaries;

};




///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


//================================================================
/// Base class for triangle meshes (meshes made of 3D tet elements).
//================================================================
class TetMeshBase : public virtual Mesh
{

public:

 /// Constructor (empty)
 TetMeshBase() {}

 /// Broken copy constructor
 TetMeshBase(const TetMeshBase& node) 
  { 
   BrokenCopy::broken_copy("TetMeshBase");
  } 
 
 /// Broken assignment operator
 void operator=(const TetMeshBase&) 
  {
   BrokenCopy::broken_assign("TetMeshBase");
  }
 
 /// Destructor (empty)
 virtual ~TetMeshBase(){}

 /// \short Global static data that specifies the permitted 
 /// error in the setup of the boundary coordinates
 static double Tolerance_for_boundary_finding;

 /// Setup lookup schemes which establish whic elements are located
 /// next to mesh's boundaries (wrapper to suppress doc).
 void setup_boundary_element_info()
  {
   std::ofstream outfile;
   setup_boundary_element_info(outfile);
  }

 /// \short Setup lookup schemes which establish which elements are located
 /// next to mesh's boundaries. Doc in outfile (if it's open).
 void setup_boundary_element_info(std::ostream &outfile);

};

}

#endif
