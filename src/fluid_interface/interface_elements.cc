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
//Non-inline functions for fluid free surface elements

//OOMPH-LIB headers
#include "interface_elements.h"
#include "../generic/integral.h"


namespace oomph
{

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



 //=========================================================================
 /// \short Set a pointer to the desired contact angle. Optional boolean
 /// (defaults to true)
 /// 
 /// hierher Andrew : Surely we want weak imposition as default?!)
 /// 
 /// chooses strong imposition via hijacking (true) or weak imposition
 /// via addition to momentum equation (false).
//=========================================================================
 void FluidInterfaceBoundingElement::set_contact_angle(double* const &angle_pt, 
                                                   const bool &strong)
 {
  //Set the pointer to the contact angle
  Contact_angle_pt = angle_pt;
  
  // If we are hijacking the kinematic condition (the default)
  // hierher Andrew correct if we change the default setting
  // to do the strong (pointwise form of the contact-angle condition)
  if(strong)
   {
    // Remember what we're doing
    Contact_angle_flag = 1;
    
    //Hijack the bulk element residuals
    dynamic_cast<FluidInterfaceElement*>(bulk_element_pt())
     ->hijack_kinematic_conditions(Bulk_node_number);
   }
  //Otherwise, we'll impose it weakly via the momentum equations.
  //This will require that the appropriate velocity node is unpinned
  // hierher Andrew:what is this last comment supposed to mean? It's either
  // trivial or suggests that we should do something here that isn't done.
  else
   {
    Contact_angle_flag = 2;
   }
 }
 
 

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


//=========================================================================
 /// Add contribution to element's residual vector and Jacobian
//=========================================================================
 void PointFluidInterfaceBoundingElement::
 fill_in_generic_residual_contribution_interface_boundary(
  Vector<double> &residuals, 
  DenseMatrix<double> &jacobian, 
  unsigned flag)
 {
  //Let's get the info from the parent
  FiniteElement* parent_pt = bulk_element_pt();
  
  //Find the dimension of the problem
  unsigned spatial_dim = this->nodal_dimension();
  
  // Outer unit normal to the wall
  Vector<double> wall_normal(spatial_dim);
  
  // Outer unit normal to the free surface
  Vector<double> unit_normal(spatial_dim);
  
  //Storage for the coordinate
  Vector<double> x(spatial_dim);
  
  //Find the dimension of the parent
  unsigned n_dim = parent_pt->dim();
  
  //Dummy local coordinate, of size zero
  Vector<double> s_local(0); 
  
  //Get the x coordinate
  this->interpolated_x(s_local,x);
  
  //Get the unit normal to the wall
  wall_unit_normal(x,wall_normal);
  
  //Find the local coordinates in the parent
  Vector<double> s_parent(n_dim);
  this->get_local_coordinate_in_bulk(s_local,s_parent);
  
  //Just get the outer unit normal
  dynamic_cast<FaceElement*>(parent_pt)->
   outer_unit_normal(s_parent,unit_normal);
  
  //Find the dot product of the two vectors
  double dot = 0.0;
  for(unsigned i=0;i<spatial_dim;i++) 
   {dot += unit_normal[i]*wall_normal[i];}
  
  //Get the value of sigma from the parent
  double sigma_local = dynamic_cast<FluidInterfaceElement*>(parent_pt)->
   sigma(s_parent);
  
  //Are we doing the weak form replacement
  if(Contact_angle_flag==2)
   {
    //Get the wall tangent vector
    Vector<double> wall_tangent(spatial_dim);
    wall_tangent[0] = - wall_normal[1];
    wall_tangent[1] = wall_normal[0];
    
    //Get the capillary number
    double ca_local = ca();
    
    //Just add the appropriate contribution to the momentum equations
    for(unsigned i=0;i<2;i++)
     {
      int local_eqn = nodal_local_eqn(0,this->U_index_interface_boundary[i]);
      if(local_eqn >= 0)
       {
        residuals[local_eqn] += 
         (sigma_local/ca_local)*(sin(contact_angle())*wall_normal[i]
                                 + cos(contact_angle())*wall_tangent[i]);
       }
     }
   }
  // Otherwise [strong imposition (by hijacking) of contact angle or
  // "no constraint at all"], add the appropriate contribution to 
  // the momentum equation
  else
   {
    // Need to find the outer normal of this point, which 
    // is the outer unit normal // hierher Andrew nonsensical comment
    // or am I missing something.
    // hierher Andrew: Also, how does this differ from the weak imposition?
    Vector<double> m(spatial_dim);
    this->outer_unit_normal(s_local,m);
    
    // Get the capillary number
    double ca_local = ca();
    
    //Just add the appropriate contribution to the momentum equations
    for(unsigned i=0;i<2;i++)
     {
      int local_eqn = nodal_local_eqn(0,this->U_index_interface_boundary[i]);
      if(local_eqn >= 0)
       {
        residuals[local_eqn] += (sigma_local/ca_local)*m[i];
       }
     }
   }
  
  //If we are imposing the contact angle strongly (by hijacking) 
  // overwrite the kinematic equation
  if(Contact_angle_flag==1)
   {
    //Read out the kinematic equation number
    int local_eqn = kinematic_local_eqn(0);
    
    //Note that because we have outer unit normals for the free surface
    //and the wall, the cosine of the contact angle is equal to 
    //MINUS the dot product computed above
    if(local_eqn >= 0)
     {
      residuals[local_eqn] = cos(contact_angle()) + dot; 
     }
    //NOTE: The jacobian entries will be computed automatically
    //by finite differences.
   }
  
  // Dummy arguments
  Shape psif(1);
  DShape dpsifds(1,1);
  Vector<double> interpolated_n(1);
  double W=0.0;
 
  // Now add the additional contributions
  add_additional_residual_contributions_interface_boundary(
   residuals,
   jacobian,
   flag,
   psif,
   dpsifds,
   interpolated_n, 
   W);
 }
 

 

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


//=========================================================================
 /// Add contribution to element's residual vector and Jacobian
//=========================================================================
 void LineFluidInterfaceBoundingElement::
 fill_in_generic_residual_contribution_interface_boundary(
  Vector<double> &residuals, 
  DenseMatrix<double> &jacobian, 
  unsigned flag)
 {
  //Let's get the info from the parent
  FiniteElement* parent_pt = bulk_element_pt();
  
  //Find the dimension of the problem
  unsigned spatial_dim = this->nodal_dimension();
  
  //Outer unit normal to the wall
  Vector<double> wall_normal(spatial_dim);
  
  //Outer unit normal to the free surface
  Vector<double> unit_normal(spatial_dim);
  
  //Find the dimension of the parent
  unsigned n_dim = parent_pt->dim();
  
  //Find the local coordinates in the parent
  Vector<double> s_parent(n_dim);
  
  //Storage for the shape functions
  unsigned n_node = this->nnode();
  Shape psi(n_node);
  DShape dpsids(n_node,1);
  Vector<double> s_local(1);
  
  // Loop over intergration points
  unsigned n_intpt = this->integral_pt()->nweight();
  for(unsigned ipt=0;ipt<n_intpt;++ipt)
   {
    //Get the local coordinate of the integration point
    s_local[0] = this->integral_pt()->knot(ipt,0);
    get_local_coordinate_in_bulk(s_local,s_parent);
    
    //Get the local shape functions
    this->dshape_local(s_local,psi,dpsids);
    
    //Zero the position
    Vector<double> x(spatial_dim,0.0);

    //Now construct the position and the tangent
    Vector<double> interpolated_t1(spatial_dim,0.0);
    for(unsigned n=0;n<n_node;n++)
     {
      const double psi_local = psi(n);
      const double dpsi_local = dpsids(n,0);
      for(unsigned i=0;i<spatial_dim;i++)
       {
        double pos = this->nodal_position(n,i);
        interpolated_t1[i] += pos*dpsi_local;
        x[i] += pos*psi_local;
       }
     }
    
    //Now we can calculate the Jacobian term
    double t_length = 0.0;
    for(unsigned i=0;i<spatial_dim;++i) 
     {t_length += interpolated_t1[i]*interpolated_t1[i];}
    double W = std::sqrt(t_length)*this->integral_pt()->weight(ipt);
    
    // Imposition of contact angle in weak form
    if(Contact_angle_flag==2)
     {
      //Get the outer unit normal of the entire interface
      dynamic_cast<FaceElement*>(parent_pt)->
       outer_unit_normal(s_parent,unit_normal);
      
      //Calculate the wall normal
      wall_unit_normal(x,wall_normal);
      
      //Find the dot product of the two
      double dot = 0.0;
      for(unsigned i=0;i<spatial_dim;i++) 
       {dot += unit_normal[i]*wall_normal[i];}
      
      //Find the projection of the outer normal of the surface into the plane
      Vector<double> binorm(spatial_dim);
      for(unsigned i=0;i<spatial_dim;i++)
       {binorm[i] = unit_normal[i] - dot*wall_normal[i];}
      
      //Get the value of sigma from the parent
      const double sigma_local =
       dynamic_cast<FluidInterfaceElement*>(parent_pt)->sigma(s_parent);
      
      //Get the capillary number
      const double ca_local = ca();
      
      //Get the contact angle
      const double theta = contact_angle();
      
      // Add the contributions to the momentum equation
      
      // Loop over the shape functions
      for(unsigned l=0;l<n_node;l++)
       {
        //Loop over the velocity components
        for(unsigned i=0;i<3;i++)
         {
          //Get the equation number for the momentum equation
          int local_eqn =  this->nodal_local_eqn(l,i);

          //If it's not a boundary condition
          if(local_eqn >= 0 )
           {
            //Add the surface-tension contribution to the momentum equation
            residuals[local_eqn] +=  (sigma_local/ca_local)*
             (sin(theta)*wall_normal[i]
              + cos(theta)*binorm[i])*psi(l)*W;
           }
         }
       }
     }
    // Otherwise [strong imposition (by hijacking) of contact angle or
    // "no constraint at all"], add the appropriate contribution to 
    // the momentum equation
    else
     {
      //Storage for the outer vector
      Vector<double> m(3);
      
      // Get the outer unit normal of the line
      this->outer_unit_normal(s_local,m);  
      
      //Get the value of sigma from the parent
      const double sigma_local = 
       dynamic_cast<FluidInterfaceElement*>(parent_pt)->
       sigma(s_parent);
      
      //Get the capillary number
      const double ca_local = ca();
      
      // Loop over the shape functions
      for(unsigned l=0;l<n_node;l++)
       {
        //Loop over the velocity components
        for(unsigned i=0;i<3;i++)
         {
          //Get the equation number for the momentum equation
          int local_eqn =  this->nodal_local_eqn(l,i);
          
          //If it's not a boundary condition
          if(local_eqn >= 0 )
           {
            //Add the surface-tension contribution to the momentum equation
            residuals[local_eqn] +=  m[i]*(sigma_local/ca_local)*psi(l)*W;
           }
         }
       }
     } //End of the line integral terms
    
    
    //If we are imposing the contact angle strongly (by hijacking) 
    // overwrite the kinematic equation
    if(Contact_angle_flag==1)
     {
      //Get the outer unit normal of the whole interface
      dynamic_cast<FaceElement*>(parent_pt)->
       outer_unit_normal(s_parent,unit_normal);
      
      //Calculate the wall normal
      wall_unit_normal(x,wall_normal);
      
      //Find the dot product
      double dot = 0.0;
      for(unsigned i=0;i<spatial_dim;i++) 
       {dot += unit_normal[i]*wall_normal[i];}
      
      //Loop over the test functions
      for(unsigned l=0;l<n_node;l++)
       {
        //Read out the kinematic equation number
        int local_eqn = kinematic_local_eqn(l);
        
        //Note that because we have outer unit normals for the free surface
        //and the wall, the cosine of the contact angle is equal to 
        //MINUS the dot product
        if(local_eqn >= 0)
         {
          residuals[local_eqn] += 
           (cos(contact_angle()) + dot)*psi(l)*W; 
         }
        //NOTE: The jacobian entries will be computed automatically
        //by finite differences.
       }
     } //End of strong form of contact angle condition
    
    // Add any additional residual contributions
    add_additional_residual_contributions_interface_boundary(
     residuals,jacobian,flag,psi,dpsids,unit_normal,W);
   }
 }


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


//============================================================
/// Default value for physical constant (static)
//============================================================
double FluidInterfaceElement::Default_Physical_Constant_Value = 1.0;


//================================================================
/// Calculate the i-th velocity component at local coordinate s
//================================================================
double FluidInterfaceElement::
interpolated_u(const Vector<double> &s, const unsigned &i) 
{
 //Find number of nodes
 unsigned n_node = FiniteElement::nnode();

 //Storage for the local shape function
 Shape psi(n_node);

 //Get values of shape function at local coordinate s
 this->shape(s,psi);
 
 //Initialise value of u
 double interpolated_u = 0.0;

 //Loop over the local nodes and sum
 for(unsigned l=0;l<n_node;l++) {interpolated_u += u(l,i)*psi(l);}

 return(interpolated_u);
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////



//=======================================================================
/// Calculate the residual contribution (kinematic and dynamic BC)
/// for the one-dimensional fluid interface element (plus, indirectly, 
/// any contributions due to node update strategy involving e.g.
/// Lagrange multipliers).
//=======================================================================
void LineFluidInterfaceElement::
fill_in_generic_residual_contribution_interface(Vector<double> &residuals, 
                                                DenseMatrix<double> &jacobian, 
                                                unsigned flag)
{
 //Find out how many nodes there are
 unsigned n_node = this->nnode();

 //Set up memeory for the shape functions
 Shape psif(n_node);
 DShape dpsifds(n_node,1);

 //Set the value of n_intpt
 unsigned n_intpt = this->integral_pt()->nweight();

 //Get the value of the Capillary number
 double Ca = ca();

 //Get the value of the Strouhal numer
 double St = st();

 //Get the value of the external pressure
 double p_ext = pext();

 //Integers to store the local equations and unknowns
 int local_eqn=0, local_unknown=0;

 //Storage for the local cooridinate
 Vector<double> s(1);

 //Loop over the integration points
 for(unsigned ipt=0;ipt<n_intpt;ipt++)
  {
   //Get the local coordinate at the integration point
   s[0] = integral_pt()->knot(ipt,0);

   //Get the integral weight
   double W = this->integral_pt()->weight(ipt);

   //Call the derivatives of the shape function at the knot point
   this->dshape_local_at_knot(ipt,psif,dpsifds);

   //Define and zero the tangent Vectors and local velocities
   Vector<double> interpolated_t1(2,0.0);
   Vector<double> interpolated_u(2,0.0);
   Vector<double> interpolated_x(2,0.0);
   Vector<double> interpolated_dx_dt(2,0.0);

   //Loop over the shape functions
   for(unsigned l=0;l<n_node;l++)
    {
     //Loop over directional components
     for(unsigned i=0;i<2;i++)
      {
       interpolated_x[i] += this->nodal_position(l,i)*psif(l);
       interpolated_dx_dt[i] += this->dnodal_position_dt(l,i)*psif(l);
       
       //Calculate velocity and tangent vector
       interpolated_u[i]  += u(l,i)*psif(l);
       interpolated_t1[i] += this->nodal_position(l,i)*dpsifds(l,0);
      }
    }

   //Calculate the length of the tangent Vector
   double tlength = interpolated_t1[0]*interpolated_t1[0] + 
    interpolated_t1[1]*interpolated_t1[1];

   //Set the Jacobian of the line element
   double J = sqrt(tlength);

   //Normalise the tangent Vector
   interpolated_t1[0] /= J; interpolated_t1[1] /= J;

   //Now calculate the normal Vector
   Vector<double> interpolated_n(2);
   outer_unit_normal(ipt,interpolated_n);
   
   //Also get the (possibly variable) surface tension
   double Sigma = this->sigma(s); 
   
   //Loop over the shape functions
   for(unsigned l=0;l<n_node;l++)
    {
     //Loop over the velocity components
     for(unsigned i=0;i<2;i++)
      {
       //Add the surface-tension contribution to the momentum equation
       local_eqn = this->nodal_local_eqn(l,this->U_index_interface[i]);
      //If it's not a boundary condition
       if(local_eqn >= 0)
        {
     
         //Note that Jacobian not needed here because S and s cancel!
         residuals[local_eqn] -= 
          (Sigma/Ca)*interpolated_t1[i]*dpsifds(l,0)*W;

         //If the element is a free surface, add in the external pressure
         if(Pext_data_pt!=0)
          {
           residuals[local_eqn]-= p_ext*interpolated_n[i]*psif(l)*W*J;
           //Add in the Jacobian term for the external pressure
           if(flag)
            {
             local_unknown = pext_local_eqn();
             if(local_unknown >= 0)
              {
               jacobian(local_eqn,local_unknown) -=
                interpolated_n[i]*psif(l)*W*J;
              }
            }
          }
        } //End of contribution to momentum equation
      }
    
     //Kinematic condition
     local_eqn = kinematic_local_eqn(l);
     if(local_eqn >= 0)
      {
       //Add the kinematic condition u.n = St dx/dt.n
       for(unsigned k=0;k<2;k++)
        {
         residuals[local_eqn] += 
          (interpolated_u[k] - St*interpolated_dx_dt[k])
          *interpolated_n[k]*psif(l)*W*J;
        }
       
       //Add in the jacobian
       if(flag)
        {
         //Loop over shape functions
         for(unsigned l2=0;l2<n_node;l2++)
          {
           //Loop over the components
           for(unsigned i2=0;i2<2;i2++)
            {
             local_unknown = 
              this->nodal_local_eqn(l2,this->U_index_interface[i2]);
             //If it's a non-zero dof add
             if(local_unknown >= 0)
              {
               jacobian(local_eqn,local_unknown) +=
                psif(l2)*interpolated_n[i2]*psif(l)*W*J;
              }
            }
          }
        }     //End of Jacobian contribution
      }
    } //End of loop over shape functions

   // Add additional contribution required from the implementation
   // of the node update (e.g. Lagrange multpliers etc)
   add_additional_residual_contributions_interface(residuals,
                                                   jacobian,
                                                   flag,
                                                   psif,
                                                   dpsifds,
                                                   interpolated_x,
                                                   interpolated_n,
                                                   W,
                                                   J);
   
   

  } //End of loop over integration points
}


//===========================================================================
///Overload the output function
//===========================================================================
void LineFluidInterfaceElement::
output(std::ostream &outfile, const unsigned &n_plot)
{
 //Set output Vector
 Vector<double> s(1);
 
 //Tecplot header info 
 outfile << "ZONE I=" << n_plot << std::endl;
 
 //Loop over plot points
 for(unsigned l=0;l<n_plot;l++)
  {
   s[0] = -1.0 + l*2.0/(n_plot-1);
   
   //Output the x,y,u,v 
   for(unsigned i=0;i<2;i++) outfile << this->interpolated_x(s,i) << " ";
   for(unsigned i=0;i<2;i++) outfile << interpolated_u(s,i) << " ";      

   //Output a dummy pressure
   outfile << 0.0 << std::endl;
  }
 outfile << std::endl;
}


//===========================================================================
///Overload the output function
//===========================================================================
void LineFluidInterfaceElement::
output(FILE* file_pt, const unsigned &n_plot)
{
 //Set output Vector
 Vector<double> s(1);
 
 //Tecplot header info 
 fprintf(file_pt,"ZONE I=%i \n",n_plot);
 
 //Loop over plot points
 for(unsigned l=0;l<n_plot;l++)
  {
   s[0] = -1.0 + l*2.0/(n_plot-1);
   
   //Output the x,y,u,v 
   for(unsigned i=0;i<2;i++) fprintf(file_pt,"%g ",this->interpolated_x(s,i));
   for(unsigned i=0;i<2;i++) fprintf(file_pt,"%g ",this->interpolated_u(s,i));

   //Output a dummy pressure
   fprintf(file_pt,"0.0 \n");
  }
 fprintf(file_pt,"\n");
}



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//=======================================================================
/// Calculate the residuals contribution (kinematic and dynamic BC)
/// for the axisymmetric interface element (plus, indirectly, 
/// any contributions due to node update strategy involving e.g.
/// Lagrange multipliers).
//=======================================================================
void AxisymmetricFluidInterfaceElement::
fill_in_generic_residual_contribution_interface(Vector<double> &residuals, 
                                                DenseMatrix<double> &jacobian, 
                                                unsigned flag)
{
 //Find out how many nodes there are
 unsigned n_node = this->nnode();

 //Set up memeory for the shape functions
 Shape psif(n_node);
 DShape dpsifds(n_node,1);

 //Set the value of n_intpt
 unsigned n_intpt = this->integral_pt()->nweight();

 //Get the value of the Capillary number
 double Ca = ca();

 //Get the value of the Strouhal numer
 double St = st();

 //Get the value of the external pressure
 double p_ext = pext();
 
 //Integers used to hold the local equation numbers and local unknowns
 int local_eqn=0, local_unknown=0;

 //Storage for the local coordinate
 Vector<double> s(1);
 
 //Loop over the integration points
 for(unsigned ipt=0;ipt<n_intpt;ipt++)
  {
   //Get the local coordinate at the integration point
   s[0] = integral_pt()->knot(ipt,0);

   //Get the integral weight
   double W = this->integral_pt()->weight(ipt);

   //Call the derivatives of the shape function
   this->dshape_local_at_knot(ipt,psif,dpsifds);

   //Define and zero the tangent Vectors and local velocities
   Vector<double> interpolated_t1(2,0.0);
   Vector<double> interpolated_u(2,0.0);
   Vector<double> interpolated_x(2,0.0);
   Vector<double> interpolated_dx_dt(2,0.0);

   //Loop over the shape functions
   for(unsigned l=0;l<n_node;l++)
    {
     //Loop over directional components
     for(unsigned i=0;i<2;i++)
      {
       interpolated_x[i] += this->nodal_position(l,i)*psif(l);
       interpolated_dx_dt[i] += this->dnodal_position_dt(l,i)*psif(l);
       
       //Calculate the velocity and tangent vector
       interpolated_u[i]  += u(l,i)*psif(l);
       interpolated_t1[i] += this->nodal_position(l,i)*dpsifds(l,0);
      }
    }

   //The first positional coordinate is the radial coordinate
   double r = interpolated_x[0];

   //Calculate the length of the tangent Vector
   double tlength = interpolated_t1[0]*interpolated_t1[0] + 
    interpolated_t1[1]*interpolated_t1[1];

   //Set the Jacobian of the line element
   double J = sqrt(tlength);

   //Normalise the tangent Vector
   interpolated_t1[0] /= J; interpolated_t1[1] /= J;

   //Now calculate the normal Vector
   Vector<double> interpolated_n(2);
   outer_unit_normal(ipt,interpolated_n);

   //Also get the (possibly variable) surface tension
   double Sigma = this->sigma(s); 

   //Loop over the shape functions
   for(unsigned l=0;l<n_node;l++)
    {
     //RADIAL VELOCITY CONTRIBUTION
     local_eqn = this->nodal_local_eqn(l,this->U_index_interface[0]);
     if(local_eqn >= 0)
      {
       //Note that the Jacobians cancel
       residuals[local_eqn] -= 
        (Sigma/Ca)*r*interpolated_t1[0]*dpsifds(l,0)*W;
       
       //Second residuals term from azimuthal curvature
       residuals[local_eqn] -= (Sigma/Ca)*psif(l)*W*J;

       //If the element is a free surface add in the external pressure
       //term
       if(Pext_data_pt!=0)
        {
         residuals[local_eqn] -= r*p_ext*interpolated_n[0]*psif(l)*W*J;

         //Add in the Jacobian term for the external pressure
         if(flag)
          {
           //If we have not fixed the external pressure
           local_unknown = pext_local_eqn();
           if(local_unknown >= 0)
            {
             jacobian(local_eqn,local_unknown) 
              -= r*interpolated_n[0]*psif(l)*W*J; 
            }
          }
        }
      }
     
     //AXIAL VELOCITY CONTRIBUTION
     local_eqn = this->nodal_local_eqn(l,this->U_index_interface[1]);
     if(local_eqn >= 0)
      {
       residuals[local_eqn] -=
        (Sigma/Ca)*r*interpolated_t1[1]*dpsifds(l,0)*W;

       //If the element is a free surface, add in the external pressure
       //contribution
       if(Pext_data_pt!=0)
        {
         residuals[local_eqn] -= r*p_ext*interpolated_n[1]*psif(l)*W*J;

         //Add in the Jacobian term for the external pressure
         if(flag)
          {
           local_unknown = pext_local_eqn();
           if(local_unknown >= 0)
            {
             jacobian(local_eqn,local_unknown) 
              -= r*interpolated_n[1]*psif(l)*W*J; 
            }
          }
        }
      }
    
     //Kinematic condition
     local_eqn = kinematic_local_eqn(l);
     if(local_eqn >= 0) 
      {
       for(unsigned k=0;k<2;k++)
        {
         residuals[local_eqn] += 
         r*(interpolated_u[k] - St*interpolated_dx_dt[k])
         *interpolated_n[k]*psif(l)*W*J;
        }
       
       //Add in the jacobian
       if(flag)
        {
         //Loop over shape functions
         for(unsigned l2=0;l2<n_node;l2++)
          {
           //Loop over the components
           for(unsigned i2=0;i2<2;i2++)
            {
             local_unknown = this->nodal_local_eqn(l2,U_index_interface[i2]);
             //If it's a non-zero dof add
             if(local_unknown >= 0)
              {
               jacobian(local_eqn,local_unknown) +=
                r*psif(l2)*interpolated_n[i2]*psif(l)*W*J;
              }
            }
          }
        } //End of Jacobian contribution
      }
    } //End of loop over shape functions
   

   // Add additional contribution required from the implementation
   // of the node update (e.g. Lagrange multpliers etc)
   add_additional_residual_contributions_interface(residuals,
                                                   jacobian,
                                                   flag,
                                                   psif,
                                                   dpsifds,
                                                   interpolated_x,
                                                   interpolated_n,
                                                   W,
                                                   J);
   
  } //End of loop over integration points

}

//===========================================================================
///Overload the output function
//===========================================================================
void AxisymmetricFluidInterfaceElement::
output(std::ostream &outfile, const unsigned &n_plot)
{
 //Vector of local coordinates
 Vector<double> s(1);
 
 // Tecplot header info
 outfile << tecplot_zone_string(n_plot);
 
 // Loop over plot points
 unsigned num_plot_points=nplot_points(n_plot);
 for (unsigned iplot=0;iplot<num_plot_points;iplot++)
  {
   // Get local coordinates of plot point
   get_s_plot(iplot,n_plot,s);

   for(unsigned i=0;i<2;i++) outfile << this->interpolated_x(s,i) << " ";
   for(unsigned i=0;i<3;i++) outfile << interpolated_u(s,i) << " ";      

   //Output a dummy pressure
   outfile << 0.0 << std::endl;
  }

 // Write tecplot footer (e.g. FE connectivity lists)
 write_tecplot_zone_footer(outfile,n_plot);

 //Output a final blank line
 outfile << std::endl;

}


//===========================================================================
///Overload the output function
//===========================================================================
void AxisymmetricFluidInterfaceElement::
output(FILE* file_pt, const unsigned &n_plot)
{
 //Set output Vector
 Vector<double> s(1);
 
 //Tecplot header info 
 fprintf(file_pt,"ZONE I=%i \n",n_plot);
 
 //Loop over plot points
 for(unsigned l=0;l<n_plot;l++)
  {
   s[0] = -1.0 + l*2.0/(n_plot-1);
   
   //Output the x,y,u,v 
   for(unsigned i=0;i<2;i++) fprintf(file_pt,"%g ",this->interpolated_x(s,i));
   for(unsigned i=0;i<3;i++) fprintf(file_pt,"%g ",this->interpolated_u(s,i));

   //Output a dummy pressure
   fprintf(file_pt,"0.0 \n");
  }
 fprintf(file_pt,"\n");
}



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//=======================================================================
/// Calculate the residuals (kinematic and dynamic BC) for the 
/// two-dimensional (surface) interface element (plus, indirectly, 
/// any contributions due to node update strategy involving e.g.
/// Lagrange multipliers).
//=======================================================================
void SurfaceFluidInterfaceElement::
fill_in_generic_residual_contribution_interface(Vector<double> &residuals, 
                                                DenseMatrix<double> &jacobian, 
                                                unsigned flag)
{
 //Find out how many nodes there are
 unsigned n_node = this->nnode();

 //Set up memeory for the shape functions
 Shape psif(n_node);

 //We have two local surface coordinates
 DShape dpsifds(n_node,2);

 //Set the value of n_intpt
 unsigned n_intpt = this->integral_pt()->nweight();

 //Get the value of the Capillary number
 double Ca = ca();

 //Get the value of the Strouhal numer
 double St = st();

 //Get the value of the external pressure
 double p_ext = pext();
 
 //Integers used to hold the local equation numbers and local unknowns
 int local_eqn=0, local_unknown=0;

 //Storage for the local coordinate
 Vector<double> s(2);
 
 //Loop over the integration points
 for(unsigned ipt=0;ipt<n_intpt;ipt++)
  {
   //Get the value of the local coordiantes at the integration point
   for(unsigned i=0;i<2;i++) {s[i] = this->integral_pt()->knot(ipt,i);}

   //Get the integral weight
   double W = this->integral_pt()->weight(ipt);

   //Call the derivatives of the shape function
   this->dshape_local_at_knot(ipt,psif,dpsifds);
   
   //Define and zero the tangent Vectors and local velocities
   Vector<double> interpolated_x(3,0.0);
   double interpolated_g[2][3] = {{0.0,0.0,0.0},{0.0,0.0,0.0}};
   double interpolated_u[3] = {0.0,0.0,0.0};
   double interpolated_dx_dt[3] = {0.0,0.0,0.0};
   
   //Loop over the shape functions
   for(unsigned l=0;l<n_node;l++)
    {
     //Loop over directional components (now three)
     for(unsigned i=0;i<3;i++)
      {
       // Coordinate
       interpolated_x[i] += this->nodal_position(l,i)*psif(l);

       //Calculate velocity of mesh
       interpolated_dx_dt[i] += this->dnodal_position_dt(l,i)*psif(l);
       
       //Calculate velocity and tangent vector
       interpolated_u[i]  += u(l,i)*psif(l);

       //Calculate the local tangent vectors
       interpolated_g[0][i] += this->nodal_position(l,i)*dpsifds(l,0);
       interpolated_g[1][i] += this->nodal_position(l,i)*dpsifds(l,1);  
      }
    }


   //Calculate the local metric tensor
   //The dot product of the two tangent vectors
   double gmet[2][2];
   for(unsigned al=0;al<2;al++)
    {
     for(unsigned be=0;be<2;be++)
      {
       //Initialise to zero
       gmet[al][be] = 0.0;
       //Add the dot product contributions
       for(unsigned i=0;i<3;i++)
        {
         gmet[al][be] += interpolated_g[al][i]*interpolated_g[be][i];
        }
      }
    }

   // Define the normal vector (cross product of tangent vectors)
   Vector<double> interpolated_n(3); 
   interpolated_n[0] = 
    interpolated_g[0][1]*interpolated_g[1][2] - 
    interpolated_g[0][2]*interpolated_g[1][1];
   interpolated_n[1] = 
    interpolated_g[0][2]*interpolated_g[1][0] - 
    interpolated_g[0][0]*interpolated_g[1][2]; 
   interpolated_n[2] = interpolated_g[0][0]*interpolated_g[1][1] - 
    interpolated_g[0][1]*interpolated_g[1][0];

   // Calculate the length of the vector
   double slength =  interpolated_n[0]*interpolated_n[0] +
                     interpolated_n[1]*interpolated_n[1] +
                     interpolated_n[2]*interpolated_n[2];
  
   //Set the determinant of the local metric tensor, 
   //which is equal to the length of the normal vector
   double J = sqrt(slength);
   
   //We can now set the sign to get the OUTER UNIT normal
   for(unsigned i=0;i<3;i++)
    {
     interpolated_n[i] *= normal_sign()/J;
    }

   //Now also get the (possible variable) surface tension
   double Sigma = this->sigma(s);

  // Loop over the shape functions
   for(unsigned l=0;l<n_node;l++)
    {
     //Loop over the velocity components
     for(unsigned i=0;i<3;i++)
      {
       //Get the equation number for the momentum equation
       local_eqn = this->nodal_local_eqn(l,this->U_index_interface[i]);

       //If it's not a boundary condition
       if(local_eqn >= 0)
        {
         //Add the surface-tension contribution to the momentum equation
         residuals[local_eqn] -=
          //G0 contribuation
          (interpolated_g[0][i]*(gmet[1][1]*dpsifds(l,0) 
                                 -gmet[1][0]*dpsifds(l,1)) +
           //G1 contribution
           interpolated_g[1][i]*(gmet[0][0]*dpsifds(l,1) 
                                 -gmet[0][1]*dpsifds(l,0)))
          *(Sigma/Ca)*W/J;

         //If the element is a free surface, add in the external pressure
         if(Pext_data_pt!=0)
          {
           //External pressure term
           residuals[local_eqn] -= p_ext*interpolated_n[i]*psif(l)*J*W;

           //Add in the Jacobian term for the external pressure
           //The correct area is included in the length of the normal
           //vector
           if(flag)
            {
             local_unknown = pext_local_eqn();
             if(local_unknown >= 0)
              {
               jacobian(local_eqn,local_unknown) -=
                interpolated_n[i]*psif(l)*J*W;
              }
            }
          } //End of pressure contribution
        }
      } //End of contribution to momentum equation

    
     // Kinematic BC
     local_eqn = kinematic_local_eqn(l);
     if(local_eqn >= 0) 
      {
       //Assemble the kinematic condition
       //The correct area is included in the normal vector
       for(unsigned k=0;k<3;k++)
        {
         residuals[local_eqn] += 
          (interpolated_u[k] - St*interpolated_dx_dt[k])
          *interpolated_n[k]*psif(l)*J*W;
        }
       
       //Add in the jacobian
       if(flag)
        {
         //Loop over shape functions
         for(unsigned l2=0;l2<n_node;l2++)
          {
           //Loop over the components
           for(unsigned i2=0;i2<3;i2++)
            {
             local_unknown = 
              this->nodal_local_eqn(l2,this->U_index_interface[i2]);
             //If it's a non-zero dof add
             if(local_unknown >= 0)
              {
               jacobian(local_eqn,local_unknown) +=
                psif(l2)*interpolated_n[i2]*psif(l)*J*W;
              }
            }
          }
        } //End of Jacobian contribution
      }
    } //End of loop over shape functions
   

   // Add additional contribution required from the implementation
   // of the node update (e.g. Lagrange multpliers etc)
   add_additional_residual_contributions_interface(residuals,
                                                   jacobian,
                                                   flag,
                                                   psif,
                                                   dpsifds,
                                                   interpolated_x,
                                                   interpolated_n,
                                                   W,
                                                   J);

  } //End of loop over integration points
}

//===========================================================================
///Overload the output function
//===========================================================================
void SurfaceFluidInterfaceElement::
output(std::ostream &outfile, const unsigned &nplot)
{
 //Storage for the local coordinate
 Vector<double> s(2);
 
 // Tecplot header info
 outfile << tecplot_zone_string(nplot);
 
 // Loop over plot points
 unsigned num_plot_points=nplot_points(nplot);
 for (unsigned iplot=0;iplot<num_plot_points;iplot++)
  {
   
   // Get local coordinates of plot point
   get_s_plot(iplot,nplot,s);
  
   // Coordinates
   for(unsigned i=0;i<3;i++) 
    {
     outfile << interpolated_x(s,i) << " ";
    }
   
   // Velocities
   for(unsigned i=0;i<3;i++) 
    {
     outfile << interpolated_u(s,i) << " ";
    }
   
   // Dummy Pressure
   outfile << 0.0  << " ";
  
   outfile << std::endl;   
  }
 outfile << std::endl;

 // Write tecplot footer (e.g. FE connectivity lists)
 write_tecplot_zone_footer(outfile,nplot);
}

//===========================================================================
///Overload the output function
//===========================================================================
void SurfaceFluidInterfaceElement::
output(FILE* file_pt, const unsigned &nplot)
{
 //Storage for local coordinates
 Vector<double> s(2);
 
 // Tecplot header info
 fprintf(file_pt,"%s",tecplot_zone_string(nplot).c_str());
 
 // Loop over plot points
 unsigned num_plot_points=nplot_points(nplot);
 for (unsigned iplot=0;iplot<num_plot_points;iplot++)
  {
   // Get local coordinates of plot point
   get_s_plot(iplot,nplot,s);
   
   // Coordinates
   for(unsigned i=0;i<3;i++) 
    {
     fprintf(file_pt,"%g ",interpolated_x(s,i));
    }
   
   // Velocities
   for(unsigned i=0;i<3;i++) 
    {
     fprintf(file_pt,"%g ",interpolated_u(s,i));
    }
   
   // Dummy Pressure
   fprintf(file_pt,"%g \n",0.0);
  }
 fprintf(file_pt,"\n");
 
 // Write tecplot footer (e.g. FE connectivity lists)
 write_tecplot_zone_footer(file_pt,nplot);
}

}