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
