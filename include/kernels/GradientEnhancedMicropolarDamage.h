//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ALEKernel.h"

using Arr3 = std::array< Real, 3 >;
using Arr33 = std::array< Arr3, 3 >;
using Arr333 = std::array< Arr33, 3 >;
using Arr3333 = std::array< Arr333, 3 >;

// Forward Declarations

/**
 * Computes the classical 2.o Helmholtz like equation for nonlocal damage
 */
class GradientEnhancedMicropolarDamage : public ALEKernel
{
public:
  static InputParameters validParams();

  GradientEnhancedMicropolarDamage( const InputParameters & parameters );

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian( unsigned int jvar ) override;

  Real computeQpJacobianDisplacement( unsigned int comp_j );
  Real computeQpJacobianMicroRotation( unsigned int comp_j );
  Real computeQpJacobianNonlocalDamage();

  /// Base name of the material system that this kernel applies to
  const std::string _base_name;

  const MaterialProperty< Real > & _k_local;
  const MaterialProperty< Real > & _nonlocal_radius;
  /// Derivatives of the w.r.t. deformation gradient, micro rotations, material gradient of the micro rotations and the nonlocal damage driving field
  const MaterialProperty< Arr33 > & _dk_local_dF;
  const MaterialProperty< Arr3 > & _dk_local_dw;
  const MaterialProperty< Arr33 > & _dk_local_dgrad_w;
  const MaterialProperty< Real > & _dk_local_dk;

  /// Coupled displacement variables
  unsigned int _ndisp;
  /// Displacement variables IDs
  std::vector< unsigned int > _disp_var;

  /// Coupled micro rotation variables
  unsigned int _nmrot;
  /// Micro rotation variables IDs
  std::vector< unsigned int > _mrot_var;

  /// The MOOSE variable number of the nonlocal damage variable
  unsigned int _nonlocal_damage_var;
};
