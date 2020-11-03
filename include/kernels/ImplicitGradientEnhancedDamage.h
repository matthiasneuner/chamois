//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Kernel.h"
#include "RankTwoTensor.h"
#include "DerivativeMaterialInterface.h"

/**
 * This class computes the off-diagonal Jacobian component of stress divergence residual system
 * Contribution from damage order parameter c
 * Useful if user wants to add the off diagonal Jacobian term
 */

class ImplicitGradientEnhancedDamage : public Kernel
{
public:
  static InputParameters validParams();

  ImplicitGradientEnhancedDamage( const InputParameters & parameters );

protected:
  Real computeQpResidual() override ;

  Real computeQpJacobian() override ;

  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;


  const unsigned int _nonlocal_damage_var;
  unsigned int _ndisp;
  std::vector<unsigned int> _disp_var;

  /// Base name of the material system that this kernel applies to
  const std::string                     _base_name;

  const MaterialProperty<Real>          & _nonlocal_radius;
  const MaterialProperty<Real>          & _k_local;
  const MaterialProperty<RankTwoTensor> & _dk_local_dstrain;
};
