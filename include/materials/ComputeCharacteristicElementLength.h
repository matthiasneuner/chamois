//* Chamois - a MOOSE interface to constitutive models developed at the
//* Unit of Strength of Materials and Structural Analysis -- University of Innsbruck
//* https://www.uibk.ac.at/Marmot/
//*
//* Copyright (C) 2020 Matthias Neuner <matthias.neuner@uibk.ac.at>
//*
//* This Source Code Form is subject to the terms of the Mozilla Public
//* License, v. 2.0. If a copy of the MPL was not distributed with this
//* file, You can obtain one at https://mozilla.org/MPL/2.0/.

#pragma once

#include "Material.h"

/**
 * ComputeCharacteristicElementLength provides a characteristic elementh length for softening
 * materials regularized by means of a mesh adjusted softening modulus
 */
class ComputeCharacteristicElementLength : public Material
{
public:
  static InputParameters validParams();

  ComputeCharacteristicElementLength( const InputParameters & parameters );

protected:
  virtual void computeQpProperties() override;

  MaterialProperty< Real > & _characteristic_element_length;

};
