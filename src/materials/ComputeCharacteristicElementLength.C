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

#include "ComputeCharacteristicElementLength.h"
#include "MooseMesh.h"
#include "Assembly.h"

registerMooseObject( "ChamoisApp", ComputeCharacteristicElementLength );

InputParameters
ComputeCharacteristicElementLength::validParams()
{
  InputParameters params = Material::validParams();
  return params;
}

ComputeCharacteristicElementLength::ComputeCharacteristicElementLength(
    const InputParameters & parameters )
  : Material( parameters ),
    _characteristic_element_length( declareProperty< Real >( "characteristic_element_length" ) )
{
}

void
ComputeCharacteristicElementLength::computeQpProperties()
{
  switch ( _mesh.dimension() )
  {
    case 1:
      _characteristic_element_length[_qp] = ( _JxW[_qp] * _coord[_qp] );
      break;
    case 2:
      _characteristic_element_length[_qp] = std::sqrt( _JxW[_qp] * _coord[_qp] );
      break;
    case 3:
      _characteristic_element_length[_qp] = std::cbrt( _JxW[_qp] * _coord[_qp] );
      break;
  }
}
