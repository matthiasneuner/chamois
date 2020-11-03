//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/bft/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ImplicitGradientEnhancedDamage.h"

registerMooseObject( "ChamoisApp", ImplicitGradientEnhancedDamage );

InputParameters
ImplicitGradientEnhancedDamage::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription( "Classical 'Helmholtz'-like equation used for implicit gradient-enhanced damage or plasticity models" );
  params.addRequiredCoupledVar( "nonlocal_damage", "The nonlocal damage field" );
  params.addRequiredCoupledVar( "displacements", "The string of displacements suitable for the problem statement" );
  params.set< bool >( "use_displaced_mesh" ) = false;
  return params;
}

ImplicitGradientEnhancedDamage::ImplicitGradientEnhancedDamage( const InputParameters & parameters )
  : Kernel( parameters ),
    _nonlocal_damage_var( coupled( "nonlocal_damage" ) ),
    _ndisp( coupledComponents( "displacements" ) ),
    _disp_var( _ndisp ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _nonlocal_radius( getMaterialPropertyByName< Real >( "nonlocal_radius" ) ),
    _k_local( getMaterialPropertyByName< Real >( "local_damage" ) ),
    _dk_local_dstrain( getMaterialPropertyByName< RankTwoTensor >( "dlocal_damage_dstrain" ) )
{
  for ( unsigned int i = 0; i < _ndisp; ++i )
    _disp_var[i] = coupled( "displacements", i );
}

Real
ImplicitGradientEnhancedDamage::computeQpResidual()
{
  return std::pow( _nonlocal_radius[_qp], 2 ) *  _grad_test[_i][_qp] * _grad_u[_qp] + _test[_i][_qp] * ( _u[_qp] - _k_local[_qp] );
}

Real
ImplicitGradientEnhancedDamage::computeQpJacobian()
{
  return std::pow( _nonlocal_radius[_qp], 2 ) * _grad_test[_i][_qp] * _grad_phi[_j][_qp] + _test[_i][_qp] * _phi[_j][_qp];
}

Real
ImplicitGradientEnhancedDamage::computeQpOffDiagJacobian(unsigned int jvar)
{
  for (unsigned int i = 0; i < _ndisp; ++i)
      if (jvar == _disp_var[i])
             return - _test[_i][_qp] * ( _dk_local_dstrain[_qp] * _grad_phi[_j][_qp] ) ( i ) ; // TODO: check notation used in MOOSE, because according to theory left index of dkde should contract with grad_phi instead of right one
}
