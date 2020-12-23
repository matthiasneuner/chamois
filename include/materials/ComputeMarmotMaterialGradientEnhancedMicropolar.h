//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "DerivativeMaterialInterface.h"
#include "Marmot/MarmotMaterialGradientEnhancedMicropolar.h"

using Arr3 = std::array< Real, 3 >;
using Arr33 = std::array< Arr3, 3 >;
using Arr333 = std::array< Arr33, 3 >;
using Arr3333 = std::array< Arr333, 3 >;

/**
 * ComputeMarmotMaterialGradientEnhancedMicropolar is a wrapper for gradient-enhanced micropolar
 * constitutive models provided by Marmot.
 */
class ComputeMarmotMaterialGradientEnhancedMicropolar
  : public DerivativeMaterialInterface< Material >
{
public:
  static InputParameters validParams();

  ComputeMarmotMaterialGradientEnhancedMicropolar( const InputParameters & parameters );

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  const std::string _base_name;
  const std::vector< Real > & _material_parameters;

  const std::vector< const VariableGradient * > _grad_disp;
  const std::vector< const VariableGradient * > _grad_disp_old;

  const std::vector< const VariableValue * > _mrot;
  const std::vector< const VariableValue * > _mrot_old;

  const std::vector< const VariableGradient * > _grad_mrot;
  const std::vector< const VariableGradient * > _grad_mrot_old;

  const VariableValue & _k;

  MaterialProperty< Arr3 > & _kirchhoff_moment;

  MaterialProperty< Arr333 > & _dkirchhoff_moment_dF;
  MaterialProperty< Arr33 > & _dkirchhoff_moment_dw;
  MaterialProperty< Arr333 > & _dkirchhoff_moment_dgrad_w;
  MaterialProperty< Arr3 > & _dkirchhoff_moment_dk;

  MaterialProperty< Arr33 > & _pk_i_stress;

  MaterialProperty< Arr3333 > & _dpk_i_stress_dF;
  MaterialProperty< Arr333 > & _dpk_i_stress_dw;
  MaterialProperty< Arr3333 > & _dpk_i_stress_dgrad_w;
  MaterialProperty< Arr33 > & _dpk_i_stress_dk;

  MaterialProperty< Arr33 > & _couple_pk_i_stress;

  MaterialProperty< Arr3333 > & _dcouple_pk_i_stress_dF;
  MaterialProperty< Arr333 > & _dcouple_pk_i_stress_dw;
  MaterialProperty< Arr3333 > & _dcouple_pk_i_stress_dgrad_w;
  MaterialProperty< Arr33 > & _dcouple_pk_i_stress_dk;

  MaterialProperty< Real > & _k_local;

  MaterialProperty< Arr33 > & _dk_local_dF;
  MaterialProperty< Arr3 > & _dk_local_dw;
  MaterialProperty< Arr33 > & _dk_local_dgrad_w;
  MaterialProperty< Real > & _dk_local_dk;

  MaterialProperty< Real > & _nonlocal_radius;

  MaterialProperty< std::vector< Real > > & _statevars;
  const MaterialProperty< std::vector< Real > > & _statevars_old;

  std::unique_ptr< MarmotMaterialGradientEnhancedMicropolar > _the_material;

  const double _time_old[2];
};
