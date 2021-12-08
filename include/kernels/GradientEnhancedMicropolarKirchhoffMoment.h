/* ---------------------------------------------------------------------
 *       _                           _     
 *   ___| |__   __ _ _ __ ___   ___ (_)___ 
 *  / __| '_ \ / _` | '_ ` _ \ / _ \| / __|
 * | (__| | | | (_| | | | | | | (_) | \__ \
 *  \___|_| |_|\__,_|_| |_| |_|\___/|_|___/
 * 
 * Chamois - a MOOSE interface to constitutive models developed at the
 * Unit of Strength of Materials and Structural Analysis
 * University of Innsbruck,
 * 2020 - today
 *
 * Matthias Neuner matthias.neuner@uibk.ac.at
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The full text of the license can be found in the file LICENSE.md at
 * the top level directory of chamois.
 * ---------------------------------------------------------------------
 */

#pragma once

#include "Kernel.h"
#include "FastorHelper.h"

// Forward Declarations

/**
 * Computes the contribution of the (nonsymmetric) kirchhoff stress tensor 
 * to the balane of angular momentum in the context of the micropolar continuum
 */
class GradientEnhancedMicropolarKirchhoffMoment : public Kernel
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
  const MaterialProperty< Tensor3R > & _kirchhoff_moment;

  //// Derivatives of the w.r.t. deformation gradient, micro rotations, material gradient of the micro rotations and the nonlocal damage driving field
  const MaterialProperty< Tensor333R > & _dkirchhoff_moment_dF;
  const MaterialProperty< Tensor33R > & _dkirchhoff_moment_dw;
  const MaterialProperty< Tensor333R > & _dkirchhoff_moment_dgrad_w;
  const MaterialProperty< Tensor3R > & _dkirchhoff_moment_dk;

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
