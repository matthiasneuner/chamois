//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/bft/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

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
