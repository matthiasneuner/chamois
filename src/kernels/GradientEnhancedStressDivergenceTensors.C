//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/bft/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GradientEnhancedStressDivergenceTensors.h"

registerMooseObject("ChamoisApp", GradientEnhancedStressDivergenceTensors);

InputParameters
GradientEnhancedStressDivergenceTensors::validParams()
{
  InputParameters params = StressDivergenceTensors::validParams();
  params.addClassDescription( "Enhanced StressDivergenceTensors kernel considering off diagonal jacobian terms due to gradient-enhanced stress effects." );
  params.addRequiredCoupledVar("nonlocal_damage", "The nonlocal damage field");
  return params;
}

GradientEnhancedStressDivergenceTensors::GradientEnhancedStressDivergenceTensors(const InputParameters & parameters)
  : StressDivergenceTensors(parameters),
   _nonlocal_damage_var (  coupled("nonlocal_damage") ),
    _dstress_dk ( getMaterialPropertyByName < RankTwoTensor > (_base_name + "dstress_dnonlocal_damage") )
{
}

Real
GradientEnhancedStressDivergenceTensors::computeQpOffDiagJacobian(unsigned int jvar)
{
    if ( jvar == _nonlocal_damage_var ) 
        return _grad_test[_i][_qp] * _dstress_dk[_qp].row( _component) * _phi[_j][_qp];

  return StressDivergenceTensors::computeQpOffDiagJacobian(jvar);
}
