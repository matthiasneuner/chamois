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

#include "FunctionDirichletBC.h"

/**
 * Defines a boundary condition that forces the value to be a user specified
 * function at the boundary with possibility of an offset.
 */
class FunctionDirichletIncrementalBC : public FunctionDirichletBC
{
public:
  static InputParameters validParams();

  FunctionDirichletIncrementalBC( const InputParameters & parameters );

protected:
  virtual Real computeQpValue() override;
  virtual void residualSetup() override;
  virtual void timestepSetup() override;
  virtual void initialSetup() override;

  std::vector< Real > _reset_times;

  VariableValue _the_offset;
};
