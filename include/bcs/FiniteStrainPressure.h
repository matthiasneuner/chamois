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
#include "IntegratedBC.h"

#include "FastorHelper.h"

class Function;

/**
 * FiniteStrainPressure applies a pressure on a given boundary in the direction defined by component
 */
class FiniteStrainPressure : public DerivativeMaterialInterface< IntegratedBC >
{
public:
  static InputParameters validParams();

  FiniteStrainPressure( const InputParameters & parameters );

protected:
  virtual Real computeQpResidual();

  virtual Real computeQpJacobian();

  virtual Real computeQpOffDiagJacobian( unsigned int );

  Real componentJacobian( unsigned int component );

  const int _component;

  const Real _factor;

  const Function * const _function;

  const PostprocessorValue * const _postprocessor;

  /// _alpha Parameter for HHT time integration scheme
  const Real _alpha;

  /// number of coupled displacement variables
  const unsigned int _ndisp;
  /// coupled displacement variables
  std::vector< unsigned int > _dvars;

  const std::vector< const VariableGradient * > _grad_disp;

  const MaterialProperty< Tensor3R > & _n;

  const MaterialProperty< Tensor333R > & _dn_dF;
};
