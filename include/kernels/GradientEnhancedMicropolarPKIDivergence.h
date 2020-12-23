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
 * Computes the divergence of 2.order tensors,
 * e.g, for the linear momentum equation (PKI stress) or
 * the angular momentum equation (nominal couple stress tensor)
 */
class GradientEnhancedMicropolarPKIDivergence : public ALEKernel
{
public:
  static InputParameters validParams();

  GradientEnhancedMicropolarPKIDivergence( const InputParameters & parameters );

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian( unsigned int jvar ) override;

  Real computeQpJacobianDisplacement( unsigned int comp_i, unsigned int comp_j );
  Real computeQpJacobianMicroRotation( unsigned int comp_i, unsigned int comp_j );
  Real computeQpJacobianNonlocalDamage( unsigned int comp_i );

  /// Base name of the material system that this kernel applies to
  const std::string _base_name;
  /// Tensor of which the divergence is computed
  const std::string _tensor_name;
  /// The tensor
  const MaterialProperty< Arr33 > & _pk_i;

  //// Derivatives of the w.r.t. deformation gradient, micro rotations, material gradient of the micro rotations and the nonlocal damage driving field
  const MaterialProperty< Arr3333 > & _dpk_i_dF;
  const MaterialProperty< Arr333 > & _dpk_i_dw;
  const MaterialProperty< Arr3333 > & _dpk_i_dgrad_w;
  const MaterialProperty< Arr33 > & _dpk_i_dk;

  /// An integer corresponding to the direction this kernel acts in
  const unsigned int _component;

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
