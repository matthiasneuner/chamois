//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/Marmot/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ComputeMarmotMaterialGradientEnhancedHypoElastic.h"

// Moose defines a registerMaterial macro, which is really just an alias to registerObject.
// This macro is not used at all in the complete mooseframework, but it clashes with the
// registerMaterial function in namespace Marmot
#undef registerMaterial
#include "Marmot/Marmot.h"

registerMooseObject( "ChamoisApp", ComputeMarmotMaterialGradientEnhancedHypoElastic );

InputParameters
ComputeMarmotMaterialGradientEnhancedHypoElastic::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription(
      "Compute stress using a hypoelastic material model from MarmotUserLibrary" );
  params.addParam< std::string >( "base_name",
                                  "Optional parameter that allows the user to define "
                                  "multiple mechanics material systems on the same "
                                  "block, i.e. for multiple phases" );
  params.addCoupledVar( "nonlocal_damage", "The nonlocal damage field" );
  params.addRequiredParam< std::string >( "marmot_material_name",
                                          "Material name for the MarmotMaterialHypoElastic" );
  params.addRequiredParam< std::vector< Real > >(
      "marmot_material_parameters", "Material Parameters for the MarmotMaterialHypoElastic" );
  return params;
}

ComputeMarmotMaterialGradientEnhancedHypoElastic::ComputeMarmotMaterialGradientEnhancedHypoElastic(
    const InputParameters & parameters )
  : DerivativeMaterialInterface< Material >( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _material_parameters( getParam< std::vector< Real > >( "marmot_material_parameters" ) ),
    _k( coupledValue( "nonlocal_damage" ) ),
    _k_old( coupledValueOld( "nonlocal_damage" ) ),
    _statevars( declareProperty< std::vector< Real > >( _base_name + "state_vars" ) ),
    _statevars_old( getMaterialPropertyOld< std::vector< Real > >( _base_name + "state_vars" ) ),
    _stress_voigt( declareProperty< std::array< Real, 6 > >( _base_name + "stress_voigt" ) ),
    _stress_voigt_old(
        getMaterialPropertyOld< std::array< Real, 6 > >( _base_name + "stress_voigt" ) ),
    _dstrain_voigt( getMaterialProperty< std::array< Real, 6 > >( "strain_increment_voigt" ) ),
    _dstress_voigt_dstrain_voigt(
        declareProperty< std::array< Real, 6 * 6 > >( _base_name + "dstress_voigt_dstrain_voigt" ) ),
    _k_local( declareProperty< Real >( "local_damage" ) ),
    _nonlocal_radius( declareProperty< Real >( "nonlocal_radius" ) ),
    _dstress_voigt_dk(
        declareProperty< std::array< Real, 6 > >( "dstress_voigt_dnonlocal_damage" ) ),
    _dk_local_dstrain_voigt(
        declareProperty< std::array< Real, 6 > >( "dlocal_damage_dstrain_voigt" ) ),
    _time_old{ _t, _t }
{
  const auto materialCode = MarmotLibrary::MarmotMaterialFactory::getMaterialCodeFromName(
      getParam< std::string >( "marmot_material_name" ) );

  _the_material = std::unique_ptr< MarmotMaterialGradientEnhancedHypoElastic >(
      dynamic_cast< MarmotMaterialGradientEnhancedHypoElastic * >(
          MarmotLibrary::MarmotMaterialFactory::createMaterial(
              materialCode, _material_parameters.data(), _material_parameters.size(), 0  ) ) );
  if ( !_the_material )
    mooseError( "Failed to instance a MarmotMaterialGradientEnhancedHypoElastic material with name " +
                getParam< std::string >( "marmot_material_name" ) );
}
void
ComputeMarmotMaterialGradientEnhancedHypoElastic::initQpStatefulProperties()
{
  _statevars[_qp].resize( _the_material->getNumberOfRequiredStateVars() );
  for ( auto & sdv : _statevars[_qp] )
    sdv = 0.0;
  for ( auto & s : _stress_voigt[_qp] )
    s = 0.0;
}

void
ComputeMarmotMaterialGradientEnhancedHypoElastic::computeQpProperties()
{
  _statevars[_qp] = _statevars_old[_qp];
  _stress_voigt[_qp] = _stress_voigt_old[_qp];

  _the_material->assignStateVars( _statevars[_qp].data(), _statevars[_qp].size() );

  const auto _dk = _k[_qp] - _k_old[_qp];

  double pNewDt = 1e36;
  _the_material->computeStress( _stress_voigt[_qp].data(),
                                _k_local[_qp],
                                _nonlocal_radius[_qp],
                                _dstress_voigt_dstrain_voigt[_qp].data(),
                                _dk_local_dstrain_voigt[_qp].data(),
                                _dstress_voigt_dk[_qp].data(),
                                _dstrain_voigt[_qp].data(),
                                _k_old[_qp],
                                _dk,
                                _time_old,
                                _dt,
                                pNewDt );

  if ( pNewDt < 1.0 )
  {
      _console 
          << _dstrain_voigt[_qp][0] << " " 
          << _dstrain_voigt[_qp][1] << " " 
          << _dstrain_voigt[_qp][2] << " " 
          << _dstrain_voigt[_qp][3] << " " 
          << _dstrain_voigt[_qp][4] << " " 
          << _dstrain_voigt[_qp][5] << "\n" ;
    throw MooseException( "MarmotMaterial " + getParam< std::string >( "marmot_material_name" ) +
                          " requests a smaller timestep." );


  }
}
