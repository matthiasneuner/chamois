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
