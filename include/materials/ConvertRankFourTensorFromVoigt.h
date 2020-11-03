//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Material.h"

/**
 * ConvertRankFourTensorFromVoigtVoigt defines a strain increment and rotation increment (=1), for
 * small strains.
 */
class ConvertRankFourTensorFromVoigt : public Material
{
public:
  static InputParameters validParams();

  ConvertRankFourTensorFromVoigt( const InputParameters & parameters );

  virtual void computeQpProperties() override;

protected:
  const std::string _base_name;
  const MaterialPropertyName _the_rank_four_tensor_name;
  const bool _divide_shear_terms_by_2_ij;
  const bool _divide_shear_terms_by_2_kl;
  const bool _the_r4t_voigt_uses_row_major_layout;
  MaterialProperty< RankFourTensor > & _the_rank_four_tensor;
  const MaterialProperty< std::array< Real, 6 * 6 > > & _the_rank_four_tensor_in_voigt;
};
