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

#include "NodalScalarKernel.h"

class Function;

/**
 * An exact indirect diplacement control contraint.
 *
 */
class IndirectDisplacementControlScalarKernel : public NodalScalarKernel
{
public:
  static InputParameters validParams();

  IndirectDisplacementControlScalarKernel( const InputParameters & parameters );

  virtual void computeResidual() override;
  virtual void computeJacobian() override;

protected:
  Real getStep();

  const unsigned int _n_constrained_variables;
  const std::vector< unsigned int > _constrained_variables_numbers;
  const std::vector< const VariableValue * > _constrained_variables_values;

  const Function * const _function;

  const std::vector< Real > _c_vector;

  const Real _l_parameter;

  const unsigned int _n_constrained_nodes;
};
