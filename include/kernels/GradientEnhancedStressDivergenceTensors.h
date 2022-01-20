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

  GradientEnhancedStressDivergenceTensors( const InputParameters & parameters );

protected:
  virtual Real computeQpOffDiagJacobian( unsigned int jvar );

  /// The MOOSE variable numbers of the nonlocal damage variables
  unsigned int _nonlocal_damage_var;

  const MaterialProperty< RankTwoTensor > & _dstress_dk;
};
