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
 * Computes the contribution of the (nonsymmetric) kirchhoff stress tensor 
 * to the balane of angular momentum in the context of the micropolar continuum
 */
class GradientEnhancedMicropolarKirchhoffMoment : public ALEKernel
{
public:
  static InputParameters validParams();

  GradientEnhancedMicropolarKirchhoffMoment( const InputParameters & parameters );

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian( unsigned int jvar ) override;

  Real computeQpJacobianDisplacement( unsigned int comp_i, unsigned int comp_j );
  Real computeQpJacobianMicroRotation( unsigned int comp_i, unsigned int comp_j );
  Real computeQpJacobianNonlocalDamage( unsigned int comp_i );

  /// Base name of the material system that this kernel applies to
  const std::string _base_name;
  /// Tensor of which the moment is computed
  const std::string _moment_name;
  /// The tensor
  const MaterialProperty< Arr3 > & _kirchhoff_moment;

  //// Derivatives of the w.r.t. deformation gradient, micro rotations, material gradient of the micro rotations and the nonlocal damage driving field
  const MaterialProperty< Arr333 > & _dkirchhoff_moment_dF;
  const MaterialProperty< Arr33 > & _dkirchhoff_moment_dw;
  const MaterialProperty< Arr333 > & _dkirchhoff_moment_dgrad_w;
  const MaterialProperty< Arr3 > & _dkirchhoff_moment_dk;

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
