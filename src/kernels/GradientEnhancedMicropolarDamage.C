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

#include "GradientEnhancedMicropolarDamage.h"

registerMooseObject( "ChamoisApp", GradientEnhancedMicropolarDamage );

InputParameters
GradientEnhancedMicropolarDamage::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription( "2. order Helmholtz like PDE for describing nonlocal damage" );
  params.addParam< std::string >( "base_name", "Material property base name" );
  params.addRequiredCoupledVar( "displacements",
                                "The string of displacements suitable for the problem statement" );
  params.addRequiredCoupledVar(
      "micro_rotations", "The string of micro rotations suitable for the problem statement" );
  params.addRequiredCoupledVar( "nonlocal_damage", "The nonlocal damage field" );
  params.set< bool >( "use_displaced_mesh" ) = false;
  return params;
}

GradientEnhancedMicropolarDamage::GradientEnhancedMicropolarDamage(
    const InputParameters & parameters )
  : DerivativeMaterialInterface< Kernel >( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _k_local( getMaterialPropertyByName< Real >( _base_name + "k_local" ) ),
    _nonlocal_radius( getMaterialPropertyByName< Real >( _base_name + "nonlocal_radius" ) ),
    _dk_local_dF( getMaterialPropertyDerivative< Tensor33R >( _base_name + "k_local", "grad_u" ) ),
    _dk_local_dw( getMaterialPropertyDerivative< Tensor3R >( _base_name + "k_local", "w" ) ),
    _dk_local_dgrad_w(
        getMaterialPropertyDerivative< Tensor33R >( _base_name + "k_local", "grad_w" ) ),
    _dk_local_dk( getMaterialPropertyDerivative< Real >( _base_name + "k_local", "k" ) ),
    _ndisp( coupledComponents( "displacements" ) ),
    _disp_var( _ndisp ),
    _nmrot( coupledComponents( "micro_rotations" ) ),
    _mrot_var( _nmrot ),
    _nonlocal_damage_var( coupled( "nonlocal_damage" ) )
{
  if ( getParam< bool >( "use_displaced_mesh" ) )
    paramError( "use_displaced_mesh", "This kernel must be run on the undisplaced mesh" );

  if ( _ndisp != 3 || _nmrot != 3 )
    mooseError( "Gradient-enhanced micropolar kernels are implemented only for 3D!" );

  for ( unsigned int i = 0; i < _ndisp; ++i )
    _disp_var[i] = coupled( "displacements", i );
  for ( unsigned int i = 0; i < _nmrot; ++i )
    _mrot_var[i] = coupled( "micro_rotations", i );
}

Real
GradientEnhancedMicropolarDamage::computeQpResidual()
{
  return std::pow( _nonlocal_radius[_qp], 2 ) * _grad_test[_i][_qp] * _grad_u[_qp] +
         _test[_i][_qp] * ( _u[_qp] - _k_local[_qp] );
}

Real
GradientEnhancedMicropolarDamage::computeQpJacobian()
{
  return computeQpJacobianNonlocalDamage();
}

Real
GradientEnhancedMicropolarDamage::computeQpOffDiagJacobian( unsigned int jvar )
{
  for ( unsigned int j = 0; j < _ndisp; ++j )
    if ( jvar == _disp_var[j] )
      return computeQpJacobianDisplacement( j );

  for ( unsigned int j = 0; j < _nmrot; ++j )
    if ( jvar == _mrot_var[j] )
      return computeQpJacobianMicroRotation( j );

  mooseError( "Jacobian for unknown variable requested" );
  return 0.0;
}

Real
GradientEnhancedMicropolarDamage::computeQpJacobianDisplacement( unsigned int comp_j )
{
  Real df_du_j = 0;

  for ( int K = 0; K < 3; K++ )
    df_du_j += -1 * _dk_local_dF[_qp]( comp_j, K ) * _grad_phi[_j][_qp]( K );

  return _test[_i][_qp] * df_du_j;
}

Real
GradientEnhancedMicropolarDamage::computeQpJacobianMicroRotation( unsigned int comp_j )
{
  Real df_dw_j = -1 * _dk_local_dw[_qp]( comp_j ) * _phi[_j][_qp];

  for ( int K = 0; K < 3; K++ )
    df_dw_j += -1 * _dk_local_dgrad_w[_qp]( comp_j, K ) * _grad_phi[_j][_qp]( K );

  return _test[_i][_qp] * df_dw_j;
}

Real
GradientEnhancedMicropolarDamage::computeQpJacobianNonlocalDamage()
{
  return std::pow( _nonlocal_radius[_qp], 2 ) * _grad_test[_i][_qp] * _grad_phi[_j][_qp] +
         _test[_i][_qp] * _phi[_j][_qp];
}
