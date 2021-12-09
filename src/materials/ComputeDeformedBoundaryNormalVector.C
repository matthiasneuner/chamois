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

#include "ComputeDeformedBoundaryNormalVector.h"
#include "Marmot/MarmotMicromorphicTensorBasics.h"

registerMooseObject( "ChamoisApp", ComputeDeformedBoundaryNormalVector );

InputParameters
ComputeDeformedBoundaryNormalVector::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription(
      "Compute a gradient-enhanced micropolar material from the Marmot library" );
  params.addRequiredCoupledVar( "displacements", "The 3 displacement components" );
  params.set< bool >( "use_displaced_mesh" ) = false;
  return params;
}

ComputeDeformedBoundaryNormalVector::ComputeDeformedBoundaryNormalVector(
    const InputParameters & parameters )
  : DerivativeMaterialInterface< Material >( parameters ),
    _grad_disp( coupledGradients( "displacements" ) ),
    _n( declareProperty< Tensor3R >( "boundary_normal_vector" ) ),
    _dn_dF( declarePropertyDerivative< Tensor333R >( "boundary_normal_vector", "grad_u" ) )
{
  if ( getParam< bool >( "use_displaced_mesh" ) )
    paramError( "use_displaced_mesh", "This material must be run on the undisplaced mesh" );
}

void
ComputeDeformedBoundaryNormalVector::computeQpProperties()
{
  using namespace Marmot::FastorIndices;

  const auto & I = Marmot::FastorStandardTensors::Spatial3D::I;

  const Tensor33R F = Tensor33R{ { ( *_grad_disp[0] )[_qp]( 0 ),
                                   ( *_grad_disp[0] )[_qp]( 1 ),
                                   ( *_grad_disp[0] )[_qp]( 2 ) },
                                 { ( *_grad_disp[1] )[_qp]( 0 ),
                                   ( *_grad_disp[1] )[_qp]( 1 ),
                                   ( *_grad_disp[1] )[_qp]( 2 ) },
                                 { ( *_grad_disp[2] )[_qp]( 0 ),
                                   ( *_grad_disp[2] )[_qp]( 1 ),
                                   ( *_grad_disp[2] )[_qp]( 2 ) } } +
                      I;

  const Tensor33R FInv = Fastor::inverse( F );
  const Tensor3333R dFInv_dF = -Fastor::einsum< Ik, Ki, to_IikK >( FInv, FInv );

  const double J = Fastor::determinant( F );

  const Tensor33R dJ_dF = J * Fastor::transpose( FInv );

  const Tensor3R N = Tensor3R{ _normals[_qp]( 0 ), _normals[_qp]( 1 ), _normals[_qp]( 2 ) };

  const Tensor3R n = J * Fastor::transpose( FInv ) % N;
  const Tensor333R dn_dF = Fastor::einsum< i, Ik >( Fastor::transpose( FInv ) % N, dJ_dF ) +
                           J * Fastor::einsum< IikK, Fastor::Index< I_ > >( dFInv_dF, N );

  _n[_qp] = n;
  _dn_dF[_qp] = dn_dF;
}
