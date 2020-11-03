//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "FunctionDirichletBC.h"

// Forward Declarations
class FunctionDirichletIncrementalBC;
class Function;

template <>
InputParameters validParams<FunctionDirichletIncrementalBC>();

/**
 * Defines a boundary condition that forces the value to be a user specified
 * function at the boundary with possibility of an offset.
 */
class FunctionDirichletIncrementalBC : public FunctionDirichletBC
{
public:
  static InputParameters validParams();

  FunctionDirichletIncrementalBC(const InputParameters & parameters);

protected:
  virtual Real computeQpValue() override;
  virtual void residualSetup() override;
  virtual void timestepSetup() override;
  virtual void initialSetup() override;

  std::vector <Real> _reset_times;

  VariableValue  _the_offset;
};
