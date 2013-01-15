/* @HEADER@ */
// ************************************************************************
// 
//                              Sundance
//                 Copyright (2005) Sandia Corporation
// 
// Copyright (year first published) Sandia Corporation.  Under the terms 
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government 
// retains certain rights in this software.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//                                                                                 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA                                                                                
// Questions? Contact Kevin Long (krlong@sandia.gov), 
// Sandia National Laboratories, Livermore, California, USA
// 
// ************************************************************************
/* @HEADER@ */

#ifndef SUNDANCE_UNKNOWNFUNCTION_H
#define SUNDANCE_UNKNOWNFUNCTION_H

#include "SundanceDefs.hpp"
#include "SundanceUnknownFunctionStub.hpp"
#include "SundanceUnknownFunctionData.hpp"
#include "SundanceFuncWithBasis.hpp"
#include "SundanceSpectralBasis.hpp"

namespace Sundance
{
using namespace Teuchos;

  

/** 
 * UnknownFunction represents an unknown function in a finite
 * element problem. Unknown functions can be scalar or vector valued, as
 * determined at runtime through the type of basis with which
 * they are constructed.
 */
class UnknownFunction : public UnknownFunctionStub,
                        public FuncWithBasis
{
public:
  /** */
  UnknownFunction(const BasisFamily& basis, const std::string& name="");

  /** */
  UnknownFunction(const BasisFamily& basis, 
    const SpectralBasis& spBasis,
    const std::string& name="");


  /** virtual destructor */
  virtual ~UnknownFunction() {;}

  /* boilerplate */
  GET_RCP(ExprBase);
};

}



#endif