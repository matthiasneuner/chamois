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

#include "DerivativeMaterialInterface.h"
#include "Marmot/MarmotMaterialGradientEnhancedHypoElastic.h"

/**
 * ComputeMarmotMaterialGradientEnhancedHypoElastic is a wrapper for hypoelastic constitutive models
 * provided by the MarmotUserLibrary.
 */
class ComputeMarmotMaterialGradientEnhancedHypoElastic
  : public DerivativeMaterialInterface< Material >
{
public:
  static InputParameters validParams();

  ComputeMarmotMaterialGradientEnhancedHypoElastic( const InputParameters & parameters );

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
