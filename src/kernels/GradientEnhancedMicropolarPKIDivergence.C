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

#include "GradientEnhancedMicropolarPKIDivergence.h"

registerMooseObject( "ChamoisApp", GradientEnhancedMicropolarPKIDivergence );

InputParameters
GradientEnhancedMicropolarPKIDivergence::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription(
      "Divergence of a gradient-enhanced rank two tensor PKI (stress, couple stress)" );
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
  params.set< bool >( "use_displaced_mesh" ) = false;
  return params;
}

GradientEnhancedMicropolarPKIDivergence::GradientEnhancedMicropolarPKIDivergence(
    const InputParameters & parameters )
  : DerivativeMaterialInterface< Kernel >( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _tensor_name( getParam< std::string >( "tensor" ) ),
    _pk_i( getMaterialPropertyByName< Tensor33R >( _base_name + _tensor_name ) ),
    _dpk_i_dF(
        getMaterialPropertyDerivative< Tensor3333R >( _base_name + _tensor_name, "grad_u" ) ),
    _dpk_i_dw( getMaterialPropertyDerivative< Tensor333R >( _base_name + _tensor_name, "w" ) ),
    _dpk_i_dgrad_w(
        getMaterialPropertyDerivative< Tensor3333R >( _base_name + _tensor_name, "grad_w" ) ),
    _dpk_i_dk( getMaterialPropertyDerivative< Tensor33R >( _base_name + _tensor_name, "k" ) ),
    _component( getParam< unsigned int >( "component" ) ),
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
GradientEnhancedMicropolarPKIDivergence::computeQpResidual()
{
  Real f_comp_i = 0;

  for ( int K = 0; K < 3; K++ )
    f_comp_i += _grad_test[_i][_qp]( K ) * _pk_i[_qp]( K, _component );

  return f_comp_i;
}

Real
GradientEnhancedMicropolarPKIDivergence::computeQpJacobian()
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
GradientEnhancedMicropolarPKIDivergence::computeQpOffDiagJacobian( unsigned int jvar )
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
GradientEnhancedMicropolarPKIDivergence::computeQpJacobianDisplacement( unsigned int comp_i,
                                                                        unsigned int comp_j )
{
  Real df_comp_i_du_comp_j = 0;

  for ( int K = 0; K < 3; K++ )
  {
    Real dpk_i_stress_du_comp_j = 0;

    for ( int J = 0; J < 3; J++ )
      dpk_i_stress_du_comp_j += _dpk_i_dF[_qp]( K, comp_i, comp_j, J ) * _grad_phi[_j][_qp]( J );

    df_comp_i_du_comp_j += _grad_test[_i][_qp]( K ) * dpk_i_stress_du_comp_j;
  }

  return df_comp_i_du_comp_j;
}

Real
GradientEnhancedMicropolarPKIDivergence::computeQpJacobianMicroRotation( unsigned int comp_i,
                                                                         unsigned int comp_j )
{
  Real df_comp_i_dw_comp_j = 0;

  const Tensor3R dN_dX{ _grad_phi[_j][_qp]( 0 ), _grad_phi[_j][_qp]( 1 ), _grad_phi[_j][_qp]( 2 ) };

  for ( int K = 0; K < 3; K++ )
  {
    Real dpk_i_stress_dw_comp_j = 0;

    dpk_i_stress_dw_comp_j += _dpk_i_dw[_qp]( K, comp_i, comp_j ) * _phi[_j][_qp];

    for ( int J = 0; J < 3; J++ )
      dpk_i_stress_dw_comp_j +=
          _dpk_i_dgrad_w[_qp]( K, comp_i, comp_j, J ) * _grad_phi[_j][_qp]( J );

    df_comp_i_dw_comp_j += _grad_test[_i][_qp]( K ) * dpk_i_stress_dw_comp_j;
  }

  return df_comp_i_dw_comp_j;
}

Real
GradientEnhancedMicropolarPKIDivergence::computeQpJacobianNonlocalDamage( unsigned int comp_i )
{
  Real df_comp_i_dk = 0.0;

  for ( int K = 0; K < 3; K++ )
    df_comp_i_dk += _grad_test[_i][_qp]( K ) * _dpk_i_dk[_qp]( K, comp_i );

  return df_comp_i_dk * _phi[_j][_qp];
}
