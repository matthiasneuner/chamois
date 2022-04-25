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
#include "FiniteStrainPressure.h"
#include "Function.h"
#include "MooseError.h"
#include "Marmot/MarmotMicromorphicTensorBasics.h"

registerMooseObject( "ChamoisApp", FiniteStrainPressure );

InputParameters
FiniteStrainPressure::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addClassDescription( "Applies a pressure on a given boundary in a given direction" );
  params.addRequiredParam< unsigned int >( "component", "The component for the pressure" );
  params.addParam< Real >( "factor", 1.0, "The magnitude to use in computing the pressure" );
  params.addParam< FunctionName >( "function", "The function that describes the pressure" );
  params.addParam< PostprocessorName >( "postprocessor",
                                        "Postprocessor that will supply the pressure value" );
  params.addParam< Real >(
      "alpha", 0.0, "alpha parameter required for HHT time integration scheme" );
  params.addRequiredCoupledVar( "displacements", "The 3 displacement components" );
  params.addCoupledVar( "lambda", "load controlling parameter, e.g., for arc length method" );
  params.set< bool >( "use_displaced_mesh" ) = false;
  return params;
}

FiniteStrainPressure::FiniteStrainPressure( const InputParameters & parameters )
  : DerivativeMaterialInterface< IntegratedBC >( parameters ),
    _component( getParam< unsigned int >( "component" ) ),
    _factor( getParam< Real >( "factor" ) ),
    _function( isParamValid( "function" ) ? &getFunction( "function" ) : NULL ),
    _postprocessor( isParamValid( "postprocessor" ) ? &getPostprocessorValue( "postprocessor" )
                                                    : NULL ),
    _alpha( getParam< Real >( "alpha" ) ),
    _ndisp( this->coupledComponents( "displacements" ) ),
    _dvars( _ndisp ),
    _grad_disp( coupledGradients( "displacements" ) ),
    _lambda_var( isCoupledScalar("lambda") ? coupledScalar("lambda") : 0 ),
    _lambda_value( isCoupledScalar("lambda") ? &coupledScalarValue("lambda") : nullptr),
    _n( getMaterialProperty< Tensor3R >( "boundary_normal_vector" ) ),
    _dn_dF( getMaterialPropertyDerivative< Tensor333R >( "boundary_normal_vector", "grad_u" ) )
{
  if ( _component > 2 )
    mooseError( "Invalid component given for ", name(), ": ", _component, ".\n" );

  for ( unsigned int i = 0; i < _ndisp; ++i )
  {
    _dvars[i] = this->getVar( "displacements", i )->number();
  }

}

Real
FiniteStrainPressure::computeQpPressure()
{
  Real factor = 1.0;

  if ( _function )
    factor *= _function->value( _t + _alpha * _dt, _q_point[_qp] );

  if ( _postprocessor )
    factor *= *_postprocessor;

  return factor * _n[_qp]( _component ) * _test[_i][_qp];
}

Real FiniteStrainPressure::getAmplification()
{
   return _lambda_value ? (*_lambda_value)[0] : 1.0 ;
}

Real
FiniteStrainPressure::computeQpResidual()
{
    return getAmplification() * computeQpPressure();
}

Real
FiniteStrainPressure::componentJacobianDisplacement( unsigned int j )
{
  using namespace Marmot::FastorIndices;

  Real factor = 1.0;

  if ( _function )
    factor *= _function->value( _t + _alpha * _dt, _q_point[_qp] );

  if ( _postprocessor )
    factor *= *_postprocessor;

  const Tensor3R dN_dX{ _grad_phi[_j][_qp]( 0 ), _grad_phi[_j][_qp]( 1 ), _grad_phi[_j][_qp]( 2 ) };

  const Tensor33R dn_dq = Fastor::einsum< ijk, k >( _dn_dF[_qp], dN_dX );

  return factor * dn_dq( _component, j ) * _test[_i][_qp];
}

Real
FiniteStrainPressure::computeQpJacobian()
{
  return getAmplification() * componentJacobianDisplacement( _component );
}

Real
FiniteStrainPressure::computeQpOffDiagJacobian( unsigned int jvar )
{
  for ( unsigned int i = 0; i < _ndisp; ++i )
    if ( jvar == _dvars[i] )
      return getAmplification() * componentJacobianDisplacement( i );
  
  return 0.0;
}

Real
FiniteStrainPressure::computeQpOffDiagJacobianScalar( unsigned int jvar )
{
  if ( jvar == _lambda_var )
  {
      return computeQpPressure();
  }

  return 0.0;
}
