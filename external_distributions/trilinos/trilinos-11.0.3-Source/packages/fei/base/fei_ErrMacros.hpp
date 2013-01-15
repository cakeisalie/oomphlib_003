/*
// @HEADER
// ************************************************************************
//             FEI: Finite Element Interface to Linear Solvers
//                  Copyright (2005) Sandia Corporation.
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation, the
// U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Alan Williams (william@sandia.gov) 
//
// ************************************************************************
// @HEADER
*/

#ifndef _fei_ErrMacros_hpp_
#define _fei_ErrMacros_hpp_

#include "fei_iostream.hpp"

//
//This file simply holds the macros used to check error returns
//and print appropriate output using cerr.
//

#ifdef CHK_ERR
#undef CHK_ERR
#endif

#ifndef fei_file
#define fei_file "unknown_fei_file"
#endif

#define CHK_ERR(a) { int fei_ErrorCode = a; \
                    if (fei_ErrorCode != 0) { \
                      fei::console_out() << " FEI ERROR, " << fei_file << ", line " \
                           << __LINE__ << " " << fei_ErrorCode << FEI_ENDL; \
                      return(fei_ErrorCode); \
                   } }

#ifdef ERReturn
#undef ERReturn
#endif

#define ERReturn(a) { fei::console_out() << " FEI ERROR, " << fei_file << ", line " \
                           << __LINE__ << FEI_ENDL; \
			   return(-1); }

#ifdef voidERReturn
#undef voidERReturn
#endif

#define voidERReturn { fei::console_out() << " FEI ERROR, " << fei_file \
			   << ", line " << __LINE__ << FEI_ENDL; \
			   return; }

#endif
