//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/bft/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "FunctionDirichletIncrementalBC.h"

registerMooseObject( "ChamoisApp", FunctionDirichletIncrementalBC );

defineLegacyParams( FunctionDirichletIncrementalBC );

InputParameters
FunctionDirichletIncrementalBC::validParams()
{
  InputParameters params = FunctionDirichletBC::validParams();
  params.addClassDescription( "Imposes a FunctionDirichletBC with possibility of an offset, which can be set to the current boundary value at discrete points in time" );
  params.addParam< std::vector< Real > >(
      "reset_times", "Discrete points in time at which the offset is set to the current boundary value" );
  return params;
}

FunctionDirichletIncrementalBC::FunctionDirichletIncrementalBC( const InputParameters & parameters )
  : FunctionDirichletBC( parameters ),
    _the_offset( _u ),
    _reset_times( getParam< std::vector< Real > >( "reset_times" ) )
{
}

Real
FunctionDirichletIncrementalBC::computeQpValue()
{
  return _the_offset[_qp] + FunctionDirichletBC::computeQpValue();
}

void
FunctionDirichletIncrementalBC::initialSetup()
{
    _the_offset = VariableValue ( _u );
}

void
FunctionDirichletIncrementalBC::timestepSetup()
{
    // resetting should be done here but is currently not possible, see #16058
  return FunctionDirichletBC::timestepSetup();
}

void
FunctionDirichletIncrementalBC::residualSetup()
{
  // this should be actually in timestepSetup, however, as reported in #16058, timestepSetup is not called properly after reenabling (_enable=true) this action
  if ( !_reset_times.empty() && _t >= *_reset_times.begin() )
  {
   // delete all passed times
    while ( !_reset_times.empty() && _t >= *_reset_times.begin() )
      _reset_times.erase( _reset_times.begin() );

    _the_offset = VariableValue ( _u );
    _console << "Setting offset for FunctionDirichletIncrementalBC to current boundary value \n";
    
  }

  return FunctionDirichletBC::residualSetup();
}
