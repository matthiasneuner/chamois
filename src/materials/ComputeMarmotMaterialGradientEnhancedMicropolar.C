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
#include "Marmot/MarmotMicromorphicTensorBasics.h"

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
  params.set< bool >( "use_displaced_mesh" ) = false;
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

    _kirchhoff_moment( declareProperty< Tensor3R >( _base_name + "kirchhoff_moment" ) ),

    _dkirchhoff_moment_dF(
        declarePropertyDerivative< Tensor333R >( _base_name + "kirchhoff_moment", "grad_u" ) ),
    _dkirchhoff_moment_dw(
        declarePropertyDerivative< Tensor33R >( _base_name + "kirchhoff_moment", "w" ) ),
    _dkirchhoff_moment_dgrad_w(
        declarePropertyDerivative< Tensor333R >( _base_name + "kirchhoff_moment", "grad_w" ) ),
    _dkirchhoff_moment_dk(
        declarePropertyDerivative< Tensor3R >( _base_name + "kirchhoff_moment", "k" ) ),

    _pk_i_stress( declareProperty< Tensor33R >( _base_name + "pk_i_stress" ) ),

    _dpk_i_stress_dF(
        declarePropertyDerivative< Tensor3333R >( _base_name + "pk_i_stress", "grad_u" ) ),
    _dpk_i_stress_dw( declarePropertyDerivative< Tensor333R >( _base_name + "pk_i_stress", "w" ) ),
    _dpk_i_stress_dgrad_w(
        declarePropertyDerivative< Tensor3333R >( _base_name + "pk_i_stress", "grad_w" ) ),
    _dpk_i_stress_dk( declarePropertyDerivative< Tensor33R >( _base_name + "pk_i_stress", "k" ) ),

    _pk_i_couple_stress( declareProperty< Tensor33R >( _base_name + "pk_i_couple_stress" ) ),

    _dpk_i_couple_stress_dF(
        declarePropertyDerivative< Tensor3333R >( _base_name + "pk_i_couple_stress", "grad_u" ) ),
    _dpk_i_couple_stress_dw(
        declarePropertyDerivative< Tensor333R >( _base_name + "pk_i_couple_stress", "w" ) ),
    _dpk_i_couple_stress_dgrad_w(
        declarePropertyDerivative< Tensor3333R >( _base_name + "pk_i_couple_stress", "grad_w" ) ),
    _dpk_i_couple_stress_dk(
        declarePropertyDerivative< Tensor33R >( _base_name + "pk_i_couple_stress", "k" ) ),

    _k_local( declareProperty< Real >( _base_name + "k_local" ) ),

    _dk_local_dF( declarePropertyDerivative< Tensor33R >( _base_name + "k_local", "grad_u" ) ),
    _dk_local_dw( declarePropertyDerivative< Tensor3R >( _base_name + "k_local", "w" ) ),
    _dk_local_dgrad_w( declarePropertyDerivative< Tensor33R >( _base_name + "k_local", "grad_w" ) ),
    _dk_local_dk( declarePropertyDerivative< Real >( _base_name + "k_local", "k" ) ),

    _nonlocal_radius( declareProperty< Real >( "nonlocal_radius" ) ),

    _statevars( declareProperty< std::vector< Real > >( _base_name + "state_vars" ) ),
    _statevars_old( getMaterialPropertyOld< std::vector< Real > >( _base_name + "state_vars" ) ),
    _time_old{ _t, _t }
{
  if ( getParam< bool >( "use_displaced_mesh" ) )
    paramError( "use_displaced_mesh", "This material must be run on the undisplaced mesh" );

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

/* static Fastor::Tensor< double, 3, 3, 3 > */
/* levi_civita_pd() */
/* { */
/*   Fastor::Tensor< double, 3, 3, 3 > LeCi_pd( 0.0 ); */
/*   LeCi_pd( 0, 1, 2 ) = 1.; */
/*   LeCi_pd( 1, 2, 0 ) = 1.; */
/*   LeCi_pd( 2, 0, 1 ) = 1.; */
/*   LeCi_pd( 1, 0, 2 ) = -1.; */
/*   LeCi_pd( 2, 1, 0 ) = -1.; */
/*   LeCi_pd( 0, 2, 1 ) = -1.; */

/*   return LeCi_pd; */
/* } */

void
ComputeMarmotMaterialGradientEnhancedMicropolar::computeQpProperties()
{
  _statevars[_qp] = _statevars_old[_qp];

  _the_material->assignStateVars( _statevars[_qp].data(), _statevars[_qp].size() );

  double pNewDt = 1e36;

  // clang-format off
  const auto& I = Marmot::FastorStandardTensors::Spatial3D::I;

  const MarmotMaterialGradientEnhancedMicropolar::DeformationIncrement< 3 > _deformation_increment{
    .F_n = Tensor33R{
      { (*_grad_disp_old[0])[_qp](0), (*_grad_disp_old[0])[_qp](1), (*_grad_disp_old[0])[_qp](2) },
      { (*_grad_disp_old[1])[_qp](0), (*_grad_disp_old[1])[_qp](1), (*_grad_disp_old[1])[_qp](2) },
      { (*_grad_disp_old[2])[_qp](0), (*_grad_disp_old[2])[_qp](1), (*_grad_disp_old[2])[_qp](2) } }
      +I,

    .F_np = Tensor33R {
      { (*_grad_disp[0])[_qp](0), (*_grad_disp[0])[_qp](1), (*_grad_disp[0])[_qp](2) },
      { (*_grad_disp[1])[_qp](0), (*_grad_disp[1])[_qp](1), (*_grad_disp[1])[_qp](2) },
      { (*_grad_disp[2])[_qp](0), (*_grad_disp[2])[_qp](1), (*_grad_disp[2])[_qp](2) } }
      + I,

    .W_n = Tensor3R {
      (*_mrot_old[0])[_qp],
      (*_mrot_old[1])[_qp],
      (*_mrot_old[2])[_qp] },

    .W_np = Tensor3R {
      (*_mrot[0])[_qp],
      (*_mrot[1])[_qp],
      (*_mrot[2])[_qp] },
        
    .dWdX_n = Tensor33R {
      { (*_grad_mrot_old[0])[_qp](0), (*_grad_mrot_old[0])[_qp](1), (*_grad_mrot_old[0])[_qp](2) },
      { (*_grad_mrot_old[1])[_qp](0), (*_grad_mrot_old[1])[_qp](1), (*_grad_mrot_old[1])[_qp](2) },
      { (*_grad_mrot_old[2])[_qp](0), (*_grad_mrot_old[2])[_qp](1), (*_grad_mrot_old[2])[_qp](2) } },

    .dWdX_np = Tensor33R {
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
  using namespace Marmot::FastorIndices;

  const auto& LeCi = Marmot::FastorStandardTensors::Spatial3D::LeviCivita;

  const auto& F_np = _deformation_increment.F_np;

  const Tensor33R    FInv    =   Fastor::inverse ( F_np );
  const Tensor3333R dFInv_dF = - Fastor::einsum< Ik, Ki, to_IikK > ( FInv, FInv);

  _pk_i_stress[_qp]                = Fastor::einsum < Ii, ij >  ( FInv,            _response.S ) ;
  _pk_i_couple_stress[_qp]         = Fastor::einsum < Ii, ij >  ( FInv,            _response.M ) ;
  _kirchhoff_moment[_qp]           = Fastor::einsum < ijl, ij > ( LeCi,            _response.S ) ;

  _k_local[_qp]         = _response.L;
  _nonlocal_radius[_qp] = _response.nonLocalRadius;

  if ( need_jacobian ) {
    _dkirchhoff_moment_dF[_qp]          = Fastor::einsum < ijl, ijkK >          ( LeCi, _algorithmic_moduli.dS_dF );
    _dkirchhoff_moment_dw[_qp]          = Fastor::einsum < ijl, ijk >           ( LeCi, _algorithmic_moduli.dS_dW ) ;
    _dkirchhoff_moment_dgrad_w[_qp]     = Fastor::einsum < ijl, ijkK >          ( LeCi, _algorithmic_moduli.dS_ddWdX ) ;
    _dkirchhoff_moment_dk[_qp]          = Fastor::einsum < ijl, ij >            ( LeCi, _algorithmic_moduli.dS_dN ) ;

    _dpk_i_stress_dF[_qp]             = Fastor::evaluate(Fastor::einsum < Ii, ijkK >          ( FInv, _algorithmic_moduli.dS_dF ) +
                                        Fastor::einsum < IikK, ij, to_IjkK > ( dFInv_dF,        _response.S ));
    _dpk_i_stress_dw[_qp]             = Fastor::einsum < Ii, ijk >           ( FInv, _algorithmic_moduli.dS_dW ) ;
    _dpk_i_stress_dgrad_w[_qp]        = Fastor::einsum < Ii, ijkK >          ( FInv, _algorithmic_moduli.dS_ddWdX ) ;
    _dpk_i_stress_dk[_qp]             = Fastor::einsum < Ii, ij >            ( FInv, _algorithmic_moduli.dS_dN ) ;

    _dpk_i_couple_stress_dF[_qp]      = Fastor::evaluate(Fastor::einsum < Ii, ijkK >          ( FInv, _algorithmic_moduli.dM_dF ) +
                                        Fastor::einsum < IikK, ij, to_IjkK > ( dFInv_dF,        _response.M ));
    _dpk_i_couple_stress_dw[_qp]      = Fastor::einsum < Ii, ijk >           ( FInv, _algorithmic_moduli.dM_dW ) ;
    _dpk_i_couple_stress_dgrad_w[_qp] = Fastor::einsum < Ii, ijkK >          ( FInv, _algorithmic_moduli.dM_ddWdX ) ;
    _dpk_i_couple_stress_dk[_qp]      = Fastor::einsum < Ii, ij >            ( FInv, _algorithmic_moduli.dM_dN ) ;

    _dk_local_dF[_qp]                 = _algorithmic_moduli.dL_dF;
    _dk_local_dw[_qp]                 = _algorithmic_moduli.dL_dW;
    _dk_local_dgrad_w[_qp]            = _algorithmic_moduli.dL_ddWdX;
    _dk_local_dk[_qp]                 = _algorithmic_moduli.dL_dN;
  }
  // clang-format on
}
