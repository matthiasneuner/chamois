//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/Marmot/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "GradientEnhancedMicropolarKirchhoffMoment.h"

registerMooseObject( "ChamoisApp", GradientEnhancedMicropolarKirchhoffMoment );

InputParameters
GradientEnhancedMicropolarKirchhoffMoment::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription( "Moment of a gradient-enhanced rank two tensor Kirchhoff" );
  params.addParam< std::string >( "base_name", "Material property base name" );
  params.addRequiredParam< std::string >( "tensor", "Name of the tensor this kernel is acting on" );
  params.addRequiredParam< unsigned int >( "component",
                                           "An integer corresponding to the direction "
                                           "the variable this kernel acts in. (0 for x, "
                                           "1 for y, 2 for z)" );
  params.addRequiredCoupledVar( "displacements",
                                "The string of displacements suitable for the problem statement" );
  params.addRequiredCoupledVar(
      "micro_rotations", "The string of micro rotations suitable for the problem statement" );
  params.addRequiredCoupledVar( "nonlocal_damage", "The nonlocal damage field" );
  return params;
}

GradientEnhancedMicropolarKirchhoffMoment::GradientEnhancedMicropolarKirchhoffMoment(
    const InputParameters & parameters )
  : ALEKernel( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _moment_name( getParam< std::string >( "tensor" ) ),
    _kirchhoff_moment( getMaterialPropertyByName< Arr3 >( _base_name + _moment_name ) ),
    _dkirchhoff_moment_dF(
        getMaterialPropertyByName< Arr333 >( _base_name + "d" + _moment_name + "_dF" ) ),
    _dkirchhoff_moment_dw(
        getMaterialPropertyByName< Arr33 >( _base_name + "d" + _moment_name + "_dw" ) ),
    _dkirchhoff_moment_dgrad_w(
        getMaterialPropertyByName< Arr333 >( _base_name + "d" + _moment_name + "_dgrad_w" ) ),
    _dkirchhoff_moment_dk(
        getMaterialPropertyByName< Arr3 >( _base_name + "d" + _moment_name + "_dk" ) ),
    _component( getParam< unsigned int >( "component" ) ),
    _ndisp( coupledComponents( "displacements" ) ),
    _disp_var( _ndisp ),
    _nmrot( coupledComponents( "micro_rotations" ) ),
    _mrot_var( _nmrot ),
    _nonlocal_damage_var( coupled( "nonlocal_damage" ) )
{
  if ( _ndisp != 3 || _nmrot != 3 )
    mooseError( "Gradient-enhanced micropolar kernels are implemented only for 3D!" );

  for ( unsigned int i = 0; i < _ndisp; ++i )
    _disp_var[i] = coupled( "displacements", i );
  for ( unsigned int i = 0; i < _nmrot; ++i )
    _mrot_var[i] = coupled( "micro_rotations", i );
}

Real
GradientEnhancedMicropolarKirchhoffMoment::computeQpResidual()
{
  return -1 * _test[_i][_qp] * _kirchhoff_moment[_qp][_component];
}

Real
GradientEnhancedMicropolarKirchhoffMoment::computeQpJacobian()
{
  const auto ivar = _var.number();

  for ( unsigned int i = 0; i < _ndisp; ++i )
    if ( ivar == _disp_var[i] )
      return computeQpJacobianDisplacement( _component, _component );

  for ( unsigned int i = 0; i < _nmrot; ++i )
    if ( ivar == _mrot_var[i] )
      return computeQpJacobianMicroRotation( _component, _component );

  mooseError( "Jacobian for unknown variable requested" );
  return 0.0;
}

Real
GradientEnhancedMicropolarKirchhoffMoment::computeQpOffDiagJacobian( unsigned int jvar )
{
  for ( unsigned int j = 0; j < _ndisp; ++j )
    if ( jvar == _disp_var[j] )
      return computeQpJacobianDisplacement( _component, j );

  for ( unsigned int j = 0; j < _nmrot; ++j )
    if ( jvar == _mrot_var[j] )
      return computeQpJacobianMicroRotation( _component, j );

  if ( jvar == _nonlocal_damage_var )
    return computeQpJacobianNonlocalDamage( _component );

  mooseError( "Jacobian for unknown variable requested" );
  return 0.0;
}

Real
GradientEnhancedMicropolarKirchhoffMoment::computeQpJacobianDisplacement( unsigned int comp_i,
                                                                          unsigned int comp_j )
{
  Real dmom_comp_i_du_comp_j = 0.0;

  for ( int M = 0; M < 3; M++ )
    dmom_comp_i_du_comp_j +=
        _dkirchhoff_moment_dF[_qp][comp_i][comp_j][M] * _grad_phi_undisplaced[_j][_qp]( M );

  return -1 * _test[_i][_qp] * dmom_comp_i_du_comp_j;
}

Real
GradientEnhancedMicropolarKirchhoffMoment::computeQpJacobianMicroRotation( unsigned int comp_i,
                                                                           unsigned int comp_j )
{

  Real dmom_comp_i_dw_comp_j = _dkirchhoff_moment_dw[_qp][comp_i][comp_j] * _phi[_j][_qp];

  for ( int M = 0; M < 3; M++ )
    dmom_comp_i_dw_comp_j +=
        _dkirchhoff_moment_dgrad_w[_qp][comp_i][comp_j][M] * _grad_phi_undisplaced[_j][_qp]( M );

  return -1 * _test[_i][_qp] * dmom_comp_i_dw_comp_j;
}

Real
GradientEnhancedMicropolarKirchhoffMoment::computeQpJacobianNonlocalDamage( unsigned int comp_i )
{
  const Real dmom_comp_i_dk = _dkirchhoff_moment_dk[_qp][comp_i];

  return -1 * _test[_i][_qp] * dmom_comp_i_dk * _phi[_j][_qp];
}