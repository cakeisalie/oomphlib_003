/* @HEADER@ */
//
 /* @HEADER@ */


#include "PlayaParameterListPreconditionerFactory.hpp"
#include "PlayaGenericRightPreconditioner.hpp"

#ifndef HAVE_TEUCHOS_EXPLICIT_INSTANTIATION
#include "PlayaVectorImpl.hpp"
#include "PlayaLinearOperatorImpl.hpp"
#endif

using namespace Playa;
using namespace Teuchos;

Preconditioner<double>  ParameterListPreconditionerFactory::
createPreconditioner(const LinearOperator<double>& A) const 
{
  const std::string& pType = params_.get<string>("Type");

  Preconditioner<double> rtn;
    
  if (pType=="ML")
  {
    std::string precType = params_.get<string>("Problem Type");
    ParameterList mlParams;
    ML_Epetra::SetDefaults(precType, mlParams);
    ParameterList::ConstIterator iter;
    ParameterList mlSettings = params_.sublist("ML Settings");
    for (iter=mlSettings.begin(); iter!=mlSettings.end(); ++iter)
    {
      const std::string& name = mlSettings.name(iter);
      const ParameterEntry& entry = mlSettings.entry(iter);
      mlParams.setEntry(name, entry);
    }
    RCP<LinearOperatorBase<double> > mlp 
      = rcp(new MLOperator(A, mlParams));
    LinearOperator<double> P = mlp;
    rtn = new GenericRightPreconditioner<double>(P);
  }
  else if (pType=="Ifpack")
  {
    ParameterList iluSettings = params_.sublist("Ifpack Settings");
    RCP<PreconditionerFactoryBase<double> > pf 
      = rcp(new ILUKPreconditionerFactory<double>(iluSettings));
    rtn = pf->createPreconditioner(A);
  }
  else
  {
    TEUCHOS_TEST_FOR_EXCEPTION(true, std::runtime_error,
      "preconditioner type=[" << pType << "] not recognized");
  }

  return rtn;
}