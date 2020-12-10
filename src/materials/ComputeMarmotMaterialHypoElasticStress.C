//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/Marmot/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ComputeMarmotMaterialHypoElasticStress.h"

// Moose defines a registerMaterial macro, which is really just an alias to registerObject.
// This macro is not used at all in the complete mooseframework, but it clashes with the
// registerMaterial function in namespace Marmot
#undef registerMaterial
#include "Marmot/Marmot.h"

registerMooseObject( "ChamoisApp", ComputeMarmotMaterialHypoElasticStress );

InputParameters
ComputeMarmotMaterialHypoElasticStress::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription(
      "Compute stress using a hypoelastic material model from MarmotUserLibrary" );
  params.addParam< std::string >( "base_name",
                                  "Optional parameter that allows the user to define "
                                  "multiple mechanics material systems on the same "
                                  "block, i.e. for multiple phases" );
  params.addRequiredParam< std::string >( "marmot_material_name",
                                          "Material name for the MarmotMaterialHypoElastic" );
  params.addRequiredParam< std::vector< Real > >(
      "marmot_material_parameters", "Material Parameters for the MarmotMaterialHypoElastic" );
  return params;
}

ComputeMarmotMaterialHypoElasticStress::ComputeMarmotMaterialHypoElasticStress(
    const InputParameters & parameters )
  : DerivativeMaterialInterface< Material >( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _material_parameters( getParam< std::vector< Real > >( "marmot_material_parameters" ) ),
    _statevars( declareProperty< std::vector< Real > >( _base_name + "state_vars" ) ),
    _statevars_old( getMaterialPropertyOld< std::vector< Real > >( _base_name + "state_vars" ) ),
    _stress_voigt( declareProperty< std::array< Real, 6 > >( _base_name + "stress_voigt" ) ),
    _stress_voigt_old(
        getMaterialPropertyOld< std::array< Real, 6 > >( _base_name + "stress_voigt" ) ),
    _dstress_voigt_dstrain_voigt(
        declareProperty< std::array< Real, 6 * 6 > >( _base_name + "Jacobian_mult_voigt" ) ),
    _dstrain_voigt( getMaterialProperty< std::array< Real, 6 > >( "strain_increment_voigt" ) ),
    _characteristic_element_length( getMaterialProperty< Real >( "characteristic_element_length" ) ),
    _time_old{ _t, _t }
{
  const auto materialCode = MarmotLibrary::MarmotMaterialFactory::getMaterialCodeFromName(
      getParam< std::string >( "marmot_material_name" ) );

  _the_material = std::unique_ptr< MarmotMaterialHypoElastic >(
      dynamic_cast< MarmotMaterialHypoElastic * >( MarmotLibrary::MarmotMaterialFactory::createMaterial(
          materialCode, _material_parameters.data(), _material_parameters.size(), 0 ) ) );
}
void
ComputeMarmotMaterialHypoElasticStress::initQpStatefulProperties()
{
  _statevars[_qp].resize( _the_material->getNumberOfRequiredStateVars() );
  for ( auto & sdv : _statevars[_qp] )
    sdv = 0.0;
  for ( auto & s : _stress_voigt[_qp] )
    s = 0.0;
}

void
ComputeMarmotMaterialHypoElasticStress::computeQpProperties()
{
  _statevars[_qp] = _statevars_old[_qp];
  _stress_voigt[_qp] = _stress_voigt_old[_qp];

  _the_material->assignStateVars( _statevars[_qp].data(), _statevars[_qp].size() );

  _the_material->setCharacteristicElementLength( _characteristic_element_length[_qp] );

  double pNewDt = 1e36;
  _the_material->computeStress( _stress_voigt[_qp].data(),
                               _dstress_voigt_dstrain_voigt[_qp].data(),
                               _dstrain_voigt[_qp].data(),
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
