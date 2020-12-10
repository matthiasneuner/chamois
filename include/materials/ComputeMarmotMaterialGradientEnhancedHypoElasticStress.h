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
#include "Marmot/MarmotMaterialGradientEnhancedHypoElastic.h"

/**
 * ComputeMarmotMaterialGradientEnhancedHypoElasticStress is a wrapper for hypoelastic constitutive models
 * provided by the MarmotUserLibrary.
 */
class ComputeMarmotMaterialGradientEnhancedHypoElasticStress : public DerivativeMaterialInterface< Material >
{
public:
  static InputParameters validParams();

  ComputeMarmotMaterialGradientEnhancedHypoElasticStress( const InputParameters & parameters );

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  const std::string _base_name;
  const std::vector< Real > & _material_parameters;

  const VariableValue & _k;
  const VariableValue & _k_old;

  MaterialProperty< std::vector< Real > > & _statevars;
  const MaterialProperty< std::vector< Real > > & _statevars_old;

  MaterialProperty< std::array< Real, 6 > > & _stress_voigt;
  const MaterialProperty< std::array< Real, 6 > > & _stress_voigt_old;
  const MaterialProperty< std::array< Real, 6 > > & _dstrain_voigt;
  MaterialProperty< std::array< Real, 6 * 6 > > & _dstress_voigt_dstrain_voigt;

  MaterialProperty< Real > & _k_local;
  MaterialProperty< Real > & _nonlocal_radius;
  MaterialProperty< std::array< Real, 6 > > & _dstress_voigt_dk;
  MaterialProperty< std::array< Real, 6 > > & _dk_local_dstrain_voigt;

  std::unique_ptr< MarmotMaterialGradientEnhancedHypoElastic > _the_material;

  const double _time_old[2];
};
