//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/bft/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ConvertRankTwoTensorFromVoigt.h"

registerMooseObject( "ChamoisApp", ConvertRankTwoTensorFromVoigt );

InputParameters
ConvertRankTwoTensorFromVoigt::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription( "Convert any RankTwoTensor from (Abaqus compatible) Voigt notation" );
  params.addParam< std::string >( "base_name",
                                  "Optional parameter that allows the user to define "
                                  "multiple mechanics material systems on the same "
                                  "block, i.e. for multiple phases" );
  params.addRequiredParam< MaterialPropertyName >( "tensor", "The outputRankTwoTensor to be converted" );
  params.addRequiredParam< MaterialPropertyName >( "tensor_voigt", "The Voigt RankTwoTensor to be converted" );
  params.addParam< bool >(
      "shear_components_half", false, "divide shear terms by factor 2 (i.e., for strains)" );
  return params;
}

ConvertRankTwoTensorFromVoigt::ConvertRankTwoTensorFromVoigt( const InputParameters & parameters )
  : Material( parameters ),
    _base_name( isParamValid( "base_name" ) ? getParam< std::string >( "base_name" ) + "_" : "" ),
    _the_rank_two_tensor_name( _base_name + getParam< MaterialPropertyName >( "tensor" ) ),
    _divide_shear_terms_by_2( getParam< bool >( "shear_components_half" ) ),
    _the_rank_two_tensor( declareProperty< RankTwoTensor >( _the_rank_two_tensor_name ) ),
    _the_rank_two_tensor_in_voigt(
        getMaterialProperty< std::array< Real, 6 > >( _base_name + getParam< MaterialPropertyName >( "tensor_voigt" ) ) 
        )
{
}

void
ConvertRankTwoTensorFromVoigt::computeQpProperties()
{
  const auto & v = _the_rank_two_tensor_in_voigt[_qp];

  _the_rank_two_tensor[_qp] = RankTwoTensor( v[0],
                                          v[1],
                                          v[2],
                                          _divide_shear_terms_by_2 ? v[5] / 2 : v[5],
                                          _divide_shear_terms_by_2 ? v[4] / 2 : v[4],
                                          _divide_shear_terms_by_2 ? v[3] / 2 : v[3] );
}
