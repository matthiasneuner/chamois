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

#include "ComputeMarmotMaterialGradientEnhancedMicropolar.h"

// Moose defines a registerMaterial macro, which is really just an alias to registerObject.
// This macro is not used at all in the complete mooseframework, but it clashes with the
// registerMaterial function in namespace Marmot
#undef registerMaterial
#include "Marmot/Marmot.h"

using Tensor3d = Fastor::Tensor< double, 3 >;
using Tensor33d = Fastor::Tensor< double, 3, 3 >;
using Tensor333d = Fastor::Tensor< double, 3, 3, 3 >;
using Tensor3333d = Fastor::Tensor< double, 3, 3, 3, 3 >;

registerMooseObject( "ChamoisApp", ComputeMarmotMaterialGradientEnhancedMicropolar );

InputParameters
ComputeMarmotMaterialGradientEnhancedMicropolar::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription(
      "Compute a gradient-enhanced micropolar material from the Marmot library" );
  params.addParam< std::string >( "base_name",
                                  "Optional parameter that allows the user to define "
                                  "multiple mechanics material systems on the same "
                                  "block, i.e. for multiple phases" );
  params.addRequiredCoupledVar( "displacements", "The 3 displacement components" );
  params.addRequiredCoupledVar( "micro_rotations", "The 3 micro rotation variables" );
  params.addRequiredCoupledVar( "nonlocal_damage", "The nonlocal damage variable" );
  params.addRequiredParam< std::string >( "marmot_material_name",
                                          "Material name for the MarmotMaterial" );
  params.addRequiredParam< std::vector< Real > >( "marmot_material_parameters",
                                                  "Material Parameters for the MarmotMaterial" );
  return params;
}

ComputeMarmotMaterialGradientEnhancedMicropolar::ComputeMarmotMaterialGradientEnhancedMicropolar(
    const InputParameters & parameters )
  : DerivativeMaterialInterface< Material >( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _material_parameters( getParam< std::vector< Real > >( "marmot_material_parameters" ) ),

    _grad_disp( coupledGradients( "displacements" ) ),
    _grad_disp_old( coupledGradientsOld( "displacements" ) ),

    _mrot( coupledValues( "micro_rotations" ) ),
    _mrot_old( coupledValuesOld( "micro_rotations" ) ),

    _grad_mrot( coupledGradients( "micro_rotations" ) ),
    _grad_mrot_old( coupledGradientsOld( "micro_rotations" ) ),

    _k( coupledValue( "nonlocal_damage" ) ),

    _kirchhoff_moment( declareProperty< Arr3 >( _base_name + "kirchhoff_moment" ) ),

    _dkirchhoff_moment_dF( declareProperty< Arr333 >( _base_name + "dkirchhoff_moment_dF" ) ),
    _dkirchhoff_moment_dw( declareProperty< Arr33 >( _base_name + "dkirchhoff_moment_dw" ) ),
    _dkirchhoff_moment_dgrad_w(
        declareProperty< Arr333 >( _base_name + "dkirchhoff_moment_dgrad_w" ) ),
    _dkirchhoff_moment_dk( declareProperty< Arr3 >( _base_name + "dkirchhoff_moment_dk" ) ),

    _pk_i_stress( declareProperty< Arr33 >( _base_name + "pk_i_stress" ) ),

    _dpk_i_stress_dF( declareProperty< Arr3333 >( _base_name + "dpk_i_stress_dF" ) ),
    _dpk_i_stress_dw( declareProperty< Arr333 >( _base_name + "dpk_i_stress_dw" ) ),
    _dpk_i_stress_dgrad_w( declareProperty< Arr3333 >( _base_name + "dpk_i_stress_dgrad_w" ) ),
    _dpk_i_stress_dk( declareProperty< Arr33 >( _base_name + "dpk_i_stress_dk" ) ),

    _couple_pk_i_stress( declareProperty< Arr33 >( _base_name + "couple_pk_i_stress" ) ),

    _dcouple_pk_i_stress_dF( declareProperty< Arr3333 >( _base_name + "dcouple_pk_i_stress_dF" ) ),
    _dcouple_pk_i_stress_dw( declareProperty< Arr333 >( _base_name + "dcouple_pk_i_stress_dw" ) ),
    _dcouple_pk_i_stress_dgrad_w(
        declareProperty< Arr3333 >( _base_name + "dcouple_pk_i_stress_dgrad_w" ) ),
    _dcouple_pk_i_stress_dk( declareProperty< Arr33 >( _base_name + "dcouple_pk_i_stress_dk" ) ),

    _k_local( declareProperty< Real >( _base_name + "k_local" ) ),

    _dk_local_dF( declareProperty< Arr33 >( _base_name + "dk_local_dF" ) ),
    _dk_local_dw( declareProperty< Arr3 >( _base_name + "dk_local_dw" ) ),
    _dk_local_dgrad_w( declareProperty< Arr33 >( _base_name + "dk_local_dgrad_w" ) ),
    _dk_local_dk( declareProperty< Real >( _base_name + "dk_local_dk" ) ),

    _nonlocal_radius( declareProperty< Real >( "nonlocal_radius" ) ),

    _statevars( declareProperty< std::vector< Real > >( _base_name + "state_vars" ) ),
    _statevars_old( getMaterialPropertyOld< std::vector< Real > >( _base_name + "state_vars" ) ),
    _time_old{ _t, _t }
{
  const auto materialCode = MarmotLibrary::MarmotMaterialFactory::getMaterialCodeFromName(
      getParam< std::string >( "marmot_material_name" ) );

  _the_material = std::unique_ptr< MarmotMaterialGradientEnhancedMicropolar >(
      dynamic_cast< MarmotMaterialGradientEnhancedMicropolar * >(
          MarmotLibrary::MarmotMaterialFactory::createMaterial(
              materialCode, _material_parameters.data(), _material_parameters.size(), 0 ) ) );

  if ( !_the_material )
    mooseError(
        "Failed to instance a MarmotMaterialGradientEnhancedMicropolar material with name " +
        getParam< std::string >( "marmot_material_name" ) );
}

void
ComputeMarmotMaterialGradientEnhancedMicropolar::initQpStatefulProperties()
{
  _statevars[_qp].resize( _the_material->getNumberOfRequiredStateVars() );
  for ( auto & sdv : _statevars[_qp] )
    sdv = 0.0;

  _the_material->assignStateVars( _statevars[_qp].data(), _statevars[_qp].size() );
  _the_material->initializeYourself();
}

static Fastor::Tensor< double, 3, 3, 3 >
levi_civita_pd()
{
  Fastor::Tensor< double, 3, 3, 3 > LeCi_pd( 0.0 );
  LeCi_pd( 0, 1, 2 ) = 1.;
  LeCi_pd( 1, 2, 0 ) = 1.;
  LeCi_pd( 2, 0, 1 ) = 1.;
  LeCi_pd( 1, 0, 2 ) = -1.;
  LeCi_pd( 2, 1, 0 ) = -1.;
  LeCi_pd( 0, 2, 1 ) = -1.;

  return LeCi_pd;
}

void
ComputeMarmotMaterialGradientEnhancedMicropolar::computeQpProperties()
{
  _statevars[_qp] = _statevars_old[_qp];

  _the_material->assignStateVars( _statevars[_qp].data(), _statevars[_qp].size() );

  double pNewDt = 1e36;

  // clang-format off
  const static Tensor33d I = { { 1, 0, 0 }, 
                               { 0, 1, 0 }, 
                               { 0, 0, 1 } };

  const static Tensor333d LeCi = levi_civita_pd () ;

  const MarmotMaterialGradientEnhancedMicropolar::DeformationIncrement< 3 > _deformation_increment{
    .F_n = Tensor33d{
      { (*_grad_disp_old[0])[_qp](0), (*_grad_disp_old[0])[_qp](1), (*_grad_disp_old[0])[_qp](2) },
      { (*_grad_disp_old[1])[_qp](0), (*_grad_disp_old[1])[_qp](1), (*_grad_disp_old[1])[_qp](2) },
      { (*_grad_disp_old[2])[_qp](0), (*_grad_disp_old[2])[_qp](1), (*_grad_disp_old[2])[_qp](2) } }
      +I,

    .F_np = Tensor33d {
      { (*_grad_disp[0])[_qp](0), (*_grad_disp[0])[_qp](1), (*_grad_disp[0])[_qp](2) },
      { (*_grad_disp[1])[_qp](0), (*_grad_disp[1])[_qp](1), (*_grad_disp[1])[_qp](2) },
      { (*_grad_disp[2])[_qp](0), (*_grad_disp[2])[_qp](1), (*_grad_disp[2])[_qp](2) } }
      + I,

    .W_n = Tensor3d {
      (*_mrot_old[0])[_qp],
      (*_mrot_old[1])[_qp],
      (*_mrot_old[2])[_qp] },

    .W_np = Tensor3d {
      (*_mrot[0])[_qp],
      (*_mrot[1])[_qp],
      (*_mrot[2])[_qp] },
        
    .dWdX_n = Tensor33d {
      { (*_grad_mrot_old[0])[_qp](0), (*_grad_mrot_old[0])[_qp](1), (*_grad_mrot_old[0])[_qp](2) },
      { (*_grad_mrot_old[1])[_qp](0), (*_grad_mrot_old[1])[_qp](1), (*_grad_mrot_old[1])[_qp](2) },
      { (*_grad_mrot_old[2])[_qp](0), (*_grad_mrot_old[2])[_qp](1), (*_grad_mrot_old[2])[_qp](2) } },

    .dWdX_np = Tensor33d {
      { (*_grad_mrot[0])[_qp](0), (*_grad_mrot[0])[_qp](1), (*_grad_mrot[0])[_qp](2) },
      { (*_grad_mrot[1])[_qp](0), (*_grad_mrot[1])[_qp](1), (*_grad_mrot[1])[_qp](2) },
      { (*_grad_mrot[2])[_qp](0), (*_grad_mrot[2])[_qp](1), (*_grad_mrot[2])[_qp](2) } },

    .N = _k[_qp]
  };
  // clang-format on

  MarmotMaterialGradientEnhancedMicropolar::ConstitutiveResponse< 3 > _response;
  MarmotMaterialGradientEnhancedMicropolar::AlgorithmicModuli< 3 > _algorithmic_moduli;
  MarmotMaterialGradientEnhancedMicropolar::TimeIncrement _time_increment{ _time_old, _dt };

  const bool need_jacobian = _fe_problem.currentlyComputingJacobian();

  _the_material->computeStress(
      _response, _algorithmic_moduli, _deformation_increment, _time_increment, pNewDt );

  if ( pNewDt < 1.0 )
    throw MooseException( "MarmotMaterial " + getParam< std::string >( "marmot_material_name" ) +
                          " requests a smaller timestep." );

  // convert kirchhoff stresses to PKI stress ( classical & couple )
  // and compute the moment of the kirchhoff stress tensor

  // clang-format off
  enum{I_,J_,K_, i_,j_,k_, l_};
  using Ii = Fastor::Index < I_, i_>;
  using ij = Fastor::Index < i_, j_>;
  using ijk = Fastor::Index < i_, j_, k_>;
  using ijl = Fastor::Index < i_, j_, l_>;
  using ijkK = Fastor::Index < i_, j_, k_, K_>;
  using Ik = Fastor::Index < I_, k_>;
  using Ki = Fastor::Index < K_, i_>;
  using IikK = Fastor::Index < I_, i_, k_, K_>;

  using to_IikK = Fastor::OIndex < I_, i_, k_, K_>;
  using to_IjkK = Fastor::OIndex < I_, j_, k_, K_>;

  const auto& F_np = _deformation_increment.F_np;

  const Tensor33d    FInv    =   Fastor::inverse ( F_np );
  const Tensor3333d dFInv_dF = - Fastor::einsum< Ik, Ki, to_IikK > ( FInv, FInv);

  const Tensor33d pk_i_stress                = Fastor::einsum < Ii, ij >  ( FInv,            _response.S ) ;
  const Tensor33d pk_i_couple_stress         = Fastor::einsum < Ii, ij >  ( FInv,            _response.M ) ;
  const Tensor3d  kirchhoff_moment           = Fastor::einsum < ijl, ij > ( LeCi,            _response.S ) ;

  _k_local[_qp]         = _response.L;
  _nonlocal_radius[_qp] = _response.nonLocalRadius;

  std::copy (  pk_i_stress.data(),                 pk_i_stress.data()                + 9,      &_pk_i_stress[_qp][0][0] );
  std::copy (  pk_i_couple_stress.data(),          pk_i_couple_stress.data()         + 9,      &_couple_pk_i_stress[_qp][0][0] );
  std::copy (  kirchhoff_moment.data(),            kirchhoff_moment.data()           + 3,      &_kirchhoff_moment[_qp][0] );

  if ( need_jacobian ) {
    const Tensor333d dkirchhoff_moment_dF          = Fastor::einsum < ijl, ijkK >          ( LeCi, _algorithmic_moduli.dS_dF );
    const Tensor33d  dkirchhoff_moment_dw          = Fastor::einsum < ijl, ijk >           ( LeCi, _algorithmic_moduli.dS_dW ) ;
    const Tensor333d dkirchhoff_moment_dgrad_w     = Fastor::einsum < ijl, ijkK >          ( LeCi, _algorithmic_moduli.dS_ddWdX ) ;
    const Tensor3d   dkirchhoff_moment_dk          = Fastor::einsum < ijl, ij >            ( LeCi, _algorithmic_moduli.dS_dN ) ;
    std::copy ( dkirchhoff_moment_dF.data(),        dkirchhoff_moment_dF.data()        + 3 * 9, &_dkirchhoff_moment_dF[_qp][0][0][0] );
    std::copy ( dkirchhoff_moment_dw.data(),        dkirchhoff_moment_dw.data()        + 3 * 3, &_dkirchhoff_moment_dw[_qp][0][0] );
    std::copy ( dkirchhoff_moment_dgrad_w.data(),   dkirchhoff_moment_dgrad_w.data()   + 3 * 9, &_dkirchhoff_moment_dgrad_w[_qp][0][0][0] );
    std::copy ( dkirchhoff_moment_dk.data(),        dkirchhoff_moment_dk.data()        + 3 * 1, &_dkirchhoff_moment_dk[_qp][0] );

    const Tensor3333d dpk_i_stress_dF             = Fastor::einsum < Ii, ijkK >          ( FInv, _algorithmic_moduli.dS_dF ) +
                                                    Fastor::einsum < IikK, ij, to_IjkK > ( dFInv_dF,        _response.S );
    const Tensor333d  dpk_i_stress_dw             = Fastor::einsum < Ii, ijk >           ( FInv, _algorithmic_moduli.dS_dW ) ;
    const Tensor3333d dpk_i_stress_dgrad_w        = Fastor::einsum < Ii, ijkK >          ( FInv, _algorithmic_moduli.dS_ddWdX ) ;
    const Tensor33d   dpk_i_stress_dk             = Fastor::einsum < Ii, ij >            ( FInv, _algorithmic_moduli.dS_dN ) ;
    std::copy ( dpk_i_stress_dF.data(),             dpk_i_stress_dF.data()             + 9 * 9, &_dpk_i_stress_dF[_qp][0][0][0][0] );
    std::copy ( dpk_i_stress_dw.data(),             dpk_i_stress_dw.data()             + 9 * 3, &_dpk_i_stress_dw[_qp][0][0][0] );
    std::copy ( dpk_i_stress_dgrad_w.data(),        dpk_i_stress_dgrad_w.data()        + 9 * 9, &_dpk_i_stress_dgrad_w[_qp][0][0][0][0] );
    std::copy ( dpk_i_stress_dk.data(),             dpk_i_stress_dk.data()             + 9 * 1, &_dpk_i_stress_dk[_qp][0][0] );

    const Tensor3333d dpk_i_couple_stress_dF      = Fastor::einsum < Ii, ijkK >          ( FInv, _algorithmic_moduli.dM_dF ) +
                                                    Fastor::einsum < IikK, ij, to_IjkK > ( dFInv_dF,        _response.M );
    const Tensor333d  dpk_i_couple_stress_dw      = Fastor::einsum < Ii, ijk >           ( FInv, _algorithmic_moduli.dM_dW ) ;
    const Tensor3333d dpk_i_couple_stress_dgrad_w = Fastor::einsum < Ii, ijkK >          ( FInv, _algorithmic_moduli.dM_ddWdX ) ;
    const Tensor33d   dpk_i_couple_stress_dk      = Fastor::einsum < Ii, ij >            ( FInv, _algorithmic_moduli.dM_dN ) ;
    std::copy ( dpk_i_couple_stress_dF.data(),      dpk_i_couple_stress_dF.data()      + 9 * 9, &_dcouple_pk_i_stress_dF[_qp][0][0][0][0] );
    std::copy ( dpk_i_couple_stress_dw.data(),      dpk_i_couple_stress_dw.data()      + 9 * 3, &_dcouple_pk_i_stress_dw[_qp][0][0][0] );
    std::copy ( dpk_i_couple_stress_dgrad_w.data(), dpk_i_couple_stress_dgrad_w.data() + 9 * 9, &_dcouple_pk_i_stress_dgrad_w[_qp][0][0][0][0] );
    std::copy ( dpk_i_couple_stress_dk.data(),      dpk_i_couple_stress_dk.data()      + 9 * 1, &_dcouple_pk_i_stress_dk[_qp][0][0] );

    _dk_local_dk[_qp] = _algorithmic_moduli.dL_dN;
    std::copy ( _algorithmic_moduli.dL_dF.data(),     _algorithmic_moduli.dL_dF.data()    + 1 * 9, &_dk_local_dF[_qp][0][0] );
    std::copy ( _algorithmic_moduli.dL_dW.data(),     _algorithmic_moduli.dL_dW.data()    + 1 * 3, &_dk_local_dw[_qp][0] );
    std::copy ( _algorithmic_moduli.dL_ddWdX.data(),  _algorithmic_moduli.dL_ddWdX.data() + 1 * 9, &_dk_local_dgrad_w[_qp][0][0] );
  }
  // clang-format on
}
