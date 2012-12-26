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
//Functions for the SpineNode/SpineElement/SpineMesh classes
//oomph-lib headers

#include "spines.h"
#include <cstdlib>

namespace oomph
{


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//Functions for the SpineNode class
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


//===================================================================
/// Update function, call the update function in the Node's SpineMesh.
//===================================================================
void SpineNode::node_update(const bool& update_all_time_levels_for_new_node)
{
 Spine_mesh_pt->spine_node_update(this);

 //Perform any auxiliary updates (i.e. reseting boundary conditions)
 if(Aux_node_update_fct_pt != 0)
  {
   Aux_node_update_fct_pt(this);
  }
}





///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//Functions for the SpineMesh class
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


//=============================================================
/// Destructor to clean up the memory allocated to the spines 
//=============================================================
SpineMesh:: ~SpineMesh()
{
 //Set the range of Spine_pt
 unsigned long Spine_pt_range = Spine_pt.size();
 //Loop over the entries in reverse and free memory
 for(unsigned long i=Spine_pt_range;i>0;i--)
  {
   delete Spine_pt[i-1]; Spine_pt[i-1] = 0;
  }
}

//============================================================
/// Update function to update all nodes of mesh.
/// [Doesn't make sense to use this mesh with SolidElements anyway,
/// so we buffer the case if update_all_solid_nodes (which defaults
/// to false) is set to true.]
//============================================================
void SpineMesh::node_update(const bool& update_all_solid_nodes)
{
#ifdef PARANOID
   if (update_all_solid_nodes)
    {
     std::string error_message =
      "Doesn't make sense to use an SpineMesh with\n";
     error_message +=
      "SolidElements so specifying update_all_solid_nodes=true\n";
     error_message += "doesn't make sense either\n";

     throw OomphLibError(error_message,
                         "SpineMesh::node_update()",
                         OOMPH_EXCEPTION_LOCATION);
    }
#endif

 //Loop over all the nodes
 unsigned long Node_pt_range = Node_pt.size();
 for(unsigned long l=0;l<Node_pt_range;l++)
  {
#ifdef PARANOID
   if(!dynamic_cast<SpineNode*>(Node_pt[l]))
    {
     std::ostringstream error_stream;
     error_stream  
      << "Error: Node " << l << "is a " << typeid(Node_pt[l]).name() 
      << ", not a SpineNode" << std::endl;
     throw OomphLibError(error_stream.str(),
                         "SpineMesh::node_update()",
                         OOMPH_EXCEPTION_LOCATION);
    } 
#endif
   
   //Need to cast to spine node to get to update function
   dynamic_cast<SpineNode*>(Node_pt[l])->node_update();
  }
}

//====================================================================
/// Assign (global) equation numbers to spines, nodes and elements
//====================================================================
unsigned long SpineMesh::assign_global_eqn_numbers
(Vector<double *> &Dof_pt)
{
 //First call the general mesh assign_eqn_numbers
 unsigned long equation_number = Mesh::assign_global_eqn_numbers(Dof_pt);

 // Loop over spines and set global equation numbers for the spine heights
 // (they are the only Data items whose global eqn numbers are assigned
 // here)
 unsigned long Spine_pt_range = Spine_pt.size();
 for(unsigned long i=0;i<Spine_pt_range;i++)
  {
   Spine_pt[i]->spine_height_pt()->assign_eqn_numbers(equation_number,Dof_pt);
  }

 //Return total number of equations
 return(equation_number);
}

//=======================================================================
/// Overload the dump function so that the spine data is also dumped
//=======================================================================
void SpineMesh::dump(std::ofstream &dump_file) const
{
 //Call the standard mesh dump function
 Mesh::dump(dump_file);

 //Now loop over the spine data and dump the spine height data
 //The ASSUMPTION is that the geometric data is stored elsewhere and will
 //be dumped elsewhere

 //Find the number of spines
 unsigned long n_spine = nspine();
 // Doc number of spines
 dump_file << n_spine << " # number of spines " << std::endl;

 //Loop over the spines
 for(unsigned long s=0;s<n_spine;s++)
  {
   spine_pt(s)->spine_height_pt()->dump(dump_file);
  }
}

//========================================================================
/// Overload the read function so that the spine data is also read
//========================================================================
void SpineMesh::read(std::ifstream &restart_file)
{
 //Call the standard mesh read function
 Mesh::read(restart_file);

 //Now loop over the spine data and dump the spine height data
 //The ASSUMPTION is that the geometric data is stored elsewhere and will
 //be dumped elsewhere

 //Get the number of spines
 unsigned long n_spine = nspine();

 std::string input_string;
 // Read line up to termination sign
 getline(restart_file,input_string,'#');
 //Ignore the restr of the line
 restart_file.ignore(80,'\n');

 //check the number of spines
 unsigned long check_n_spine = atoi(input_string.c_str());

 if(check_n_spine != n_spine)
  {
   std::ostringstream error_stream;
    error_stream 
     << "Number of spines in the restart file, " << check_n_spine 
     << std::endl << "does not equal the number of spines in the mesh " 
     << n_spine << std::endl;

    throw OomphLibError(error_stream.str(),
                        "SpineMesh::read()",
                        OOMPH_EXCEPTION_LOCATION);
  }

 //Loop over the spines and read the data
 for(unsigned long s=0;s<n_spine;s++)
  {
   spine_pt(s)->spine_height_pt()->read(restart_file);
  }
}

}
