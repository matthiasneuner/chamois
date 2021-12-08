//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FiniteStrainPressure.h"
#include "Function.h"
#include "MooseError.h"
#include <Fastor/Fastor.h>

registerMooseObject("ChamoisApp", FiniteStrainPressure);

InputParameters
FiniteStrainPressure::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addClassDescription("Applies a pressure on a given boundary in a given direction");
  params.addRequiredParam<unsigned int>("component", "The component for the pressure");
  params.addParam<Real>("factor", 1.0, "The magnitude to use in computing the pressure");
  params.addParam<FunctionName>("function", "The function that describes the pressure");
  params.addParam<PostprocessorName>("postprocessor",
                                     "Postprocessor that will supply the pressure value");
  params.addParam<Real>("alpha", 0.0, "alpha parameter required for HHT time integration scheme");
  params.addRequiredCoupledVar( "displacements", "The 3 displacement components" );
  params.set<bool>("use_displaced_mesh") = false;
  return params;
}

FiniteStrainPressure::FiniteStrainPressure(const InputParameters & parameters)
  : DerivativeMaterialInterface < IntegratedBC > (parameters),
    _component(getParam<unsigned int>("component")),
    _factor(getParam<Real>("factor")),
    _function(isParamValid("function") ? &getFunction("function") : NULL),
    _postprocessor(isParamValid("postprocessor") ? &getPostprocessorValue("postprocessor") : NULL),
    _alpha(getParam<Real>("alpha")),
    _ndisp(this->coupledComponents("displacements")),
    _dvars(_ndisp),
    _grad_disp( coupledGradients( "displacements" ) ),
    _n ( getMaterialProperty< Tensor3R > ( "boundary_normal_vector" ) ),
    _dn_dF ( getMaterialProperty< Tensor333R > ( "dboundary_normal_vector/dgrad_u" ) )
{
  if (_component > 2)
    mooseError("Invalid component given for ", name(), ": ", _component, ".\n");

  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    _dvars[i] = this->getVar("displacements", i)->number();
  }

}

Real
FiniteStrainPressure::computeQpResidual()
{
  Real factor = _factor;

  if (_function)
    factor *= _function->value(_t + _alpha * _dt, _q_point[_qp]);

  if (_postprocessor)
    factor *= *_postprocessor;

  return factor * _n[_qp](_component) * _test[_i][_qp];
}

Real
FiniteStrainPressure::componentJacobian(unsigned int j)
{
  Real factor = _factor;

  if (_function)
    factor *= _function->value(_t + _alpha * _dt, _q_point[_qp]);

  if (_postprocessor)
    factor *= *_postprocessor;


  using Tensor3d = Fastor::Tensor< double, 3 >;
  using Tensor33d = Fastor::Tensor< double, 3, 3 >;
  using Tensor333d = Fastor::Tensor< double, 3, 3, 3 >;
  using Tensor3333d = Fastor::Tensor< double, 3, 3, 3, 3 >;
  enum{I_,J_,K_, i_,j_,k_, l_};
  using I = Fastor::Index < I_>;
  using K = Fastor::Index < K_>;
  using Ii = Fastor::Index < I_, i_>;
  using i = Fastor::Index <  i_>;
  using ij = Fastor::Index < i_, j_>;
  using ijk = Fastor::Index < i_, j_, k_>;
  using ijK = Fastor::Index < i_, j_, K_>;
  using ijl = Fastor::Index < i_, j_, l_>;
  using ijkK = Fastor::Index < i_, j_, k_, K_>;
  using Ik = Fastor::Index < I_, k_>;
  using Ki = Fastor::Index < K_, i_>;
  using IikK = Fastor::Index < I_, i_, k_, K_>;


   const Tensor3d dN_dX { _grad_phi[_j][_qp](0), _grad_phi[_j][_qp](1), _grad_phi[_j][_qp](2)};

   const Tensor33d dn_dq = Fastor::einsum< ijK, K> ( _dn_dF[_qp], dN_dX ) ;

   return factor * dn_dq(_component, j) * _test[_i][_qp]    ;
}

Real
FiniteStrainPressure::computeQpJacobian()
{
  return componentJacobian(_component);
}

Real
FiniteStrainPressure::computeQpOffDiagJacobian(unsigned int jvar)
{
  for (unsigned int i = 0; i < _ndisp; ++i)
    if (jvar == _dvars[i])
      return componentJacobian(i);

  return 0.0;
}
