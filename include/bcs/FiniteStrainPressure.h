//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "DerivativeMaterialInterface.h"
#include "IntegratedBC.h"

#include "FastorHelper.h"

class Function;

/**
 * FiniteStrainPressure applies a pressure on a given boundary in the direction defined by component
 */
class FiniteStrainPressure : public DerivativeMaterialInterface < IntegratedBC > 
{
public:
  static InputParameters validParams();

  FiniteStrainPressure(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  virtual Real computeQpOffDiagJacobian(unsigned int);

  Real componentJacobian(unsigned int component);

  const int _component;

  const Real _factor;

  const Function * const _function;

  const PostprocessorValue * const _postprocessor;

  /// _alpha Parameter for HHT time integration scheme
  const Real _alpha;

  /// number of coupled displacement variables
  const unsigned int _ndisp;
  /// coupled displacement variables
  std::vector<unsigned int> _dvars;

  const std::vector< const VariableGradient * > _grad_disp;

  /* const MaterialProperty < Ten33d > & theTensor; */

  const MaterialProperty< Tensor3R > & _n;

  const MaterialProperty< Tensor333R  > & _dn_dF;
};
