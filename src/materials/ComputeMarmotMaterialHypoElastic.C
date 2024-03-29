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

#include "ComputeMarmotMaterialHypoElastic.h"

// Moose defines a registerMaterial macro, which is really just an alias to registerObject.
// This macro is not used at all in the complete mooseframework, but it clashes with the
// registerMaterial function in namespace Marmot
#undef registerMaterial
#include "Marmot/Marmot.h"

registerMooseObject( "ChamoisApp", ComputeMarmotMaterialHypoElastic );

InputParameters
ComputeMarmotMaterialHypoElastic::validParams()
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

ComputeMarmotMaterialHypoElastic::ComputeMarmotMaterialHypoElastic(
    const InputParameters & parameters )
  : DerivativeMaterialInterface< Material >( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _material_parameters( getParam< std::vector< Real > >( "marmot_material_parameters" ) ),
    _statevars( declareProperty< std::vector< Real > >( _base_name + "state_vars" ) ),
    _statevars_old( getMaterialPropertyOld< std::vector< Real > >( _base_name + "state_vars" ) ),
    _stress_voigt( declareProperty< std::array< Real, 6 > >( _base_name + "stress_voigt" ) ),
    _stress_voigt_old(
        getMaterialPropertyOld< std::array< Real, 6 > >( _base_name + "stress_voigt" ) ),
    _dstress_voigt_dstrain_voigt( declareProperty< std::array< Real, 6 * 6 > >(
        _base_name + "dstress_voigt_dstrain_voigt" ) ),
    _dstrain_voigt( getMaterialProperty< std::array< Real, 6 > >( "strain_increment_voigt" ) ),
    _characteristic_element_length(
        getMaterialProperty< Real >( "characteristic_element_length" ) ),
    _time_old{ _t, _t }
{
  const auto materialCode = MarmotLibrary::MarmotMaterialFactory::getMaterialCodeFromName(
      getParam< std::string >( "marmot_material_name" ) );

  _the_material =
      std::unique_ptr< MarmotMaterialHypoElastic >( dynamic_cast< MarmotMaterialHypoElastic * >(
          MarmotLibrary::MarmotMaterialFactory::createMaterial(
              materialCode, _material_parameters.data(), _material_parameters.size(), 0 ) ) );
}
void
ComputeMarmotMaterialHypoElastic::initQpStatefulProperties()
{
  _statevars[_qp].resize( _the_material->getNumberOfRequiredStateVars() );
  for ( auto & sdv : _statevars[_qp] )
    sdv = 0.0;
  for ( auto & s : _stress_voigt[_qp] )
    s = 0.0;
}

void
ComputeMarmotMaterialHypoElastic::computeQpProperties()
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
    _console << _dstrain_voigt[_qp][0] << " " << _dstrain_voigt[_qp][1] << " "
             << _dstrain_voigt[_qp][2] << " " << _dstrain_voigt[_qp][3] << " "
             << _dstrain_voigt[_qp][4] << " " << _dstrain_voigt[_qp][5] << "\n";
    throw MooseException( "MarmotMaterial " + getParam< std::string >( "marmot_material_name" ) +
                          " requests a smaller timestep." );
  }
}
