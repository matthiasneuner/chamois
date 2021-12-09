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

#include "FunctionDirichletIncrementalBC.h"

registerMooseObject( "ChamoisApp", FunctionDirichletIncrementalBC );

defineLegacyParams( FunctionDirichletIncrementalBC );

InputParameters
FunctionDirichletIncrementalBC::validParams()
{
  InputParameters params = FunctionDirichletBC::validParams();
  params.addClassDescription(
      "Imposes a FunctionDirichletBC with possibility of an offset, which can be set to the "
      "current boundary value at discrete points in time" );
  params.addParam< std::vector< Real > >(
      "reset_times",
      "Discrete points in time at which the offset is set to the current boundary value" );
  return params;
}

FunctionDirichletIncrementalBC::FunctionDirichletIncrementalBC( const InputParameters & parameters )
  : FunctionDirichletBC( parameters ),
    _reset_times( getParam< std::vector< Real > >( "reset_times" ) ),
    _the_offset( _u )
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
  _the_offset = VariableValue( _u );
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
  // this should be actually in timestepSetup, however, as reported in #16058, timestepSetup is not
  // called properly after reenabling (_enable=true) this action
  if ( !_reset_times.empty() && _t >= *_reset_times.begin() )
  {
    // delete all passed times
    while ( !_reset_times.empty() && _t >= *_reset_times.begin() )
      _reset_times.erase( _reset_times.begin() );

    _the_offset = VariableValue( _u );
    _console << "Setting offset for FunctionDirichletIncrementalBC to current boundary value \n";
  }

  return FunctionDirichletBC::residualSetup();
}
