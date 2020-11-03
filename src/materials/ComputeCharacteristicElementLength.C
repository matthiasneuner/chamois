//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/bft/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

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
