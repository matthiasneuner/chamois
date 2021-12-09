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

#include "GradientEnhancedStressDivergenceTensors.h"

registerMooseObject( "ChamoisApp", GradientEnhancedStressDivergenceTensors );

InputParameters
GradientEnhancedStressDivergenceTensors::validParams()
{
  InputParameters params = StressDivergenceTensors::validParams();
  params.addClassDescription( "Enhanced StressDivergenceTensors kernel considering off diagonal "
                              "jacobian terms due to gradient-enhanced stress effects." );
  params.addRequiredCoupledVar( "nonlocal_damage", "The nonlocal damage field" );
  return params;
}

GradientEnhancedStressDivergenceTensors::GradientEnhancedStressDivergenceTensors(
    const InputParameters & parameters )
  : StressDivergenceTensors( parameters ),
    _nonlocal_damage_var( coupled( "nonlocal_damage" ) ),
    _dstress_dk(
        getMaterialPropertyByName< RankTwoTensor >( _base_name + "dstress_dnonlocal_damage" ) )
{
}

Real
GradientEnhancedStressDivergenceTensors::computeQpOffDiagJacobian( unsigned int jvar )
{
  if ( jvar == _nonlocal_damage_var )
    return _grad_test[_i][_qp] * _dstress_dk[_qp].row( _component ) * _phi[_j][_qp];

  return StressDivergenceTensors::computeQpOffDiagJacobian( jvar );
}
