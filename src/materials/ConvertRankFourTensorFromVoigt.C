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

#include "ConvertRankFourTensorFromVoigt.h"

registerMooseObject( "ChamoisApp", ConvertRankFourTensorFromVoigt );

InputParameters
ConvertRankFourTensorFromVoigt::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription(
      "Convert any RankFourTensor from (Abaqus compatible) Voigt notation" );
  params.addParam< std::string >( "base_name",
                                  "Optional parameter that allows the user to define "
                                  "multiple mechanics material systems on the same "
                                  "block, i.e. for multiple phases" );
  params.addRequiredParam< MaterialPropertyName >( "tensor", "The RankFourTensor to be converted" );
  params.addRequiredParam< MaterialPropertyName >( "tensor_voigt", "The Voigt RankFourTensor (in matrix form) to be converted" );
  params.addParam< bool >(
      "shear_components_half_ij", false, "divide shear terms by factor 2 (i.e., for strains)" );
  params.addParam< bool >(
      "shear_components_half_kl", false, "divide shear terms by factor 2 (i.e., for strains)" );
  params.addParam< bool >(
      "tensor_voigt_uses_row_major_layout", true, "input tensor is in row major" );
  return params;
}

ConvertRankFourTensorFromVoigt::ConvertRankFourTensorFromVoigt( const InputParameters & parameters )
  : Material( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _the_rank_four_tensor_name( _base_name + getParam< MaterialPropertyName >( "tensor" ) ),
    _divide_shear_terms_by_2_ij( getParam< bool >( "shear_components_half_ij" ) ),
    _divide_shear_terms_by_2_kl( getParam< bool >( "shear_components_half_kl" ) ),
    _the_r4t_voigt_uses_row_major_layout(
        getParam< bool >( "tensor_voigt_uses_row_major_layout" ) ),
    _the_rank_four_tensor( declareProperty< RankFourTensor >( _the_rank_four_tensor_name ) ),
    _the_rank_four_tensor_in_voigt(
        getMaterialProperty< std::array< Real, 6 * 6 > >( _base_name + getParam< MaterialPropertyName >( "tensor_voigt" ) ) )
{
}

void
ConvertRankFourTensorFromVoigt::computeQpProperties()
{

  const static std::array< std::array< unsigned int, 3 >, 3 > comp2vgt{
      { { 0, 3, 4 }, 
        { 3, 1, 5 }, 
        { 4, 5, 2 } } };

  const auto & v = _the_rank_four_tensor_in_voigt[_qp];

  for ( unsigned i = 0; i < 3; ++i )
    for ( unsigned j = 0; j < 3; ++j )
      for ( unsigned k = 0; k < 3; ++k )
        for ( unsigned l = 0; l < 3; ++l )
        {
          _the_rank_four_tensor[_qp]( i, j, k, l ) = _the_r4t_voigt_uses_row_major_layout
                                                      ? v[comp2vgt[i][j] * 6 + comp2vgt[k][l]]
                                                      : v[comp2vgt[i][j] + comp2vgt[k][l] * 6];

          if ( i != j && _divide_shear_terms_by_2_ij )
            _the_rank_four_tensor[_qp]( i, j, k, l ) *= 0.5;
          if ( k != l && _divide_shear_terms_by_2_kl )
            _the_rank_four_tensor[_qp]( i, j, k, l ) *= 2;
        }
}
