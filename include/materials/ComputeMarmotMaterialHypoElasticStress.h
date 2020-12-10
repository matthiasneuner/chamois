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
#include "Marmot/MarmotMaterialHypoElastic.h"

/**
 * ComputeMarmotMaterialHypoElasticStress is a wrapper for hypoelastic constitutive models provided by
 * the MarmotUserLibrary.
 */
class ComputeMarmotMaterialHypoElasticStress : public DerivativeMaterialInterface< Material >
{
public:
  static InputParameters validParams();

  ComputeMarmotMaterialHypoElasticStress( const InputParameters & parameters );

protected:
  virtual void initQpStatefulProperties() override;
  virtual void computeQpProperties() override;

  const std::string _base_name;
  const std::vector< Real > & _material_parameters;

  MaterialProperty< std::vector< Real > > & _statevars;
  const MaterialProperty< std::vector< Real > > & _statevars_old;

  MaterialProperty< std::array< Real, 6 > > & _stress_voigt;
  const MaterialProperty< std::array< Real, 6 > > & _stress_voigt_old;
  MaterialProperty< std::array< Real, 6 * 6 > > & _dstress_voigt_dstrain_voigt;
  const MaterialProperty< std::array< Real, 6 > > & _dstrain_voigt;

  const MaterialProperty< Real > & _characteristic_element_length;

  std::unique_ptr< MarmotMaterialHypoElastic > _the_material;

  const double _time_old[2];
};
