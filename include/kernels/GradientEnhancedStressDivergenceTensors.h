//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "StressDivergenceTensors.h"

// Forward Declarations

/**
 * Computes grad_i(stress_{i component})
 * This is exactly the same as StressDivergenceTensors,
 * only the Jacobian entries are correct for the GradientEnhanced case
 */
class GradientEnhancedStressDivergenceTensors : public StressDivergenceTensors
{
public:
  static InputParameters validParams();

  GradientEnhancedStressDivergenceTensors(const InputParameters & parameters);

protected:
  virtual Real computeQpOffDiagJacobian(unsigned int jvar);

  /// The MOOSE variable numbers of the nonlocal damage variables
  unsigned int _nonlocal_damage_var;

  const MaterialProperty< RankTwoTensor> & _dstress_dk;
};
