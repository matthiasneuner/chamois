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

using Tensor3d = Fastor::Tensor< double, 3 >;
using Tensor33d = Fastor::Tensor< double, 3, 3 >;
using Tensor333d = Fastor::Tensor< double, 3, 3, 3 >;
using Tensor3333d = Fastor::Tensor< double, 3, 3, 3, 3 >;

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
    _n ( declareProperty< TensorData3R > ( "boundary_normal_vector" ) ),
    _dn_dF ( declarePropertyDerivative< TensorData333R > ( "boundary_normal_vector" , "grad_u" ) )
{
  if ( getParam< bool >( "use_displaced_mesh" ) )
    paramError( "use_displaced_mesh", "This material must be run on the undisplaced mesh" );

}

void
ComputeDeformedBoundaryNormalVector::computeQpProperties()
{
    using Tensor3d = Fastor::Tensor< double, 3 >;
    using Tensor33d = Fastor::Tensor< double, 3, 3 >;
    using Tensor333d = Fastor::Tensor< double, 3, 3, 3 >;
    using Tensor3333d = Fastor::Tensor< double, 3, 3, 3, 3 >;

  const static Tensor33d eye = { { 1, 0, 0 }, 
                               { 0, 1, 0 }, 
                               { 0, 0, 1 } };

    const Tensor33d F = Tensor33d {
      { (*_grad_disp[0])[_qp](0), (*_grad_disp[0])[_qp](1), (*_grad_disp[0])[_qp](2) },
      { (*_grad_disp[1])[_qp](0), (*_grad_disp[1])[_qp](1), (*_grad_disp[1])[_qp](2) },
      { (*_grad_disp[2])[_qp](0), (*_grad_disp[2])[_qp](1), (*_grad_disp[2])[_qp](2) } }
      + eye;

  enum{I_,J_,K_, i_,j_,k_, l_};
  using I = Fastor::Index < I_>;
  using K = Fastor::Index < K_>;
  using Ii = Fastor::Index < I_, i_>;
  using i = Fastor::Index <  i_>;
  using ij = Fastor::Index < i_, j_>;
  using ijk = Fastor::Index < i_, j_, k_>;
  using ijK = Fastor::Index < i_, j_, K_>;
  using ijl = Fastor::Index < i_, j_, l_>;
  using ijkK = Fastor::Index < i_, j_, k_, K_>;
  using Ik = Fastor::Index < I_, k_>;
  using Ki = Fastor::Index < K_, i_>;
  using IikK = Fastor::Index < I_, i_, k_, K_>;

  using to_IikK = Fastor::OIndex < I_, i_, k_, K_>;

  const Tensor33d    FInv    =   Fastor::inverse ( F );
  const Tensor3333d dFInv_dF = - Fastor::einsum< Ik, Ki, to_IikK > ( FInv, FInv);

  const double J = Fastor::determinant(F);

  const Tensor33d dJ_dF = J * Fastor::transpose ( FInv );

  const Tensor3d N = Tensor3d{_normals[_qp](0), _normals[_qp](1), _normals[_qp](2)};

  const Tensor3d  n = J * Fastor::transpose( FInv ) % N;
  const Tensor333d dn_dF = Fastor::einsum<i, Ik>( Fastor::transpose( FInv ) % N , dJ_dF ) + J * Fastor::einsum< IikK, I> ( dFInv_dF, N );

  _n[_qp] = n;
  _dn_dF[_qp] = dn_dF;

}
