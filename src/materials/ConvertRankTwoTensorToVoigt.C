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

#include "ConvertRankTwoTensorToVoigt.h"

registerMooseObject( "ChamoisApp", ConvertRankTwoTensorToVoigt );

InputParameters
ConvertRankTwoTensorToVoigt::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription( "Convert any RankTwoTensor to (Abaqus compatible) Voigt notation" );
  params.addRequiredParam< MaterialPropertyName >( "tensor", "The RankTwoTensor to be converted" );
  params.addRequiredParam< MaterialPropertyName >( "tensor_voigt",
                                                   "The RankTwoTensor to be converted" );
  params.addParam< bool >(
      "shear_components_twice", false, "multiply shear terms by factor 2 (i.e., for strains)" );
  return params;
}

ConvertRankTwoTensorToVoigt::ConvertRankTwoTensorToVoigt( const InputParameters & parameters )
  : Material( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _the_rank_two_tensor_name( _base_name + getParam< MaterialPropertyName >( "tensor" ) ),
    _multiply_shear_terms_x2( getParam< bool >( "shear_components_twice" ) ),
    _the_rank_two_tensor( getMaterialProperty< RankTwoTensor >( _the_rank_two_tensor_name ) ),
    _the_rank_two_tensor_in_voigt( declareProperty< std::array< Real, 6 > >(
        _base_name + getParam< MaterialPropertyName >( "tensor_voigt" ) ) )
{
}

void
ConvertRankTwoTensorToVoigt::computeQpProperties()
{
  _the_rank_two_tensor_in_voigt[_qp][0] = _the_rank_two_tensor[_qp]( 0, 0 );
  _the_rank_two_tensor_in_voigt[_qp][1] = _the_rank_two_tensor[_qp]( 1, 1 );
  _the_rank_two_tensor_in_voigt[_qp][2] = _the_rank_two_tensor[_qp]( 2, 2 );
  _the_rank_two_tensor_in_voigt[_qp][3] = _the_rank_two_tensor[_qp]( 0, 1 );
  _the_rank_two_tensor_in_voigt[_qp][4] = _the_rank_two_tensor[_qp]( 0, 2 );
  _the_rank_two_tensor_in_voigt[_qp][5] = _the_rank_two_tensor[_qp]( 1, 2 );

  if ( _multiply_shear_terms_x2 )
  {
    _the_rank_two_tensor_in_voigt[_qp][3] *= 2;
    _the_rank_two_tensor_in_voigt[_qp][4] *= 2;
    _the_rank_two_tensor_in_voigt[_qp][5] *= 2;
  }
}
