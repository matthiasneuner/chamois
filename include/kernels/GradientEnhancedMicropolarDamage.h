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

#include "DerivativeMaterialInterface.h"
#include "Kernel.h"
#include "FastorHelper.h"

// Forward Declarations

/**
 * Computes the classical 2.o Helmholtz like equation for nonlocal damage
 */
    class GradientEnhancedMicropolarDamage : public DerivativeMaterialInterface < Kernel >
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
  const MaterialProperty< Tensor33R > & _dk_local_dF;
  const MaterialProperty< Tensor3R > & _dk_local_dw;
  const MaterialProperty< Tensor33R > & _dk_local_dgrad_w;
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
