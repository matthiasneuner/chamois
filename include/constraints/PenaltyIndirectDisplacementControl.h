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

#include "NodalConstraint.h"

/**
 * A penalty based implementation of the indirect displacement control technique.
 */
class PenaltyIndirectDisplacementControl : public NodalConstraint
{
public:
  static InputParameters validParams();

  PenaltyIndirectDisplacementControl( const InputParameters & parameters );

protected:
  /**
   * Computes the residual for the current secondary node
   */
  virtual Real computeQpResidual( Moose::ConstraintType type ) override;

  /**
   * Computes the jacobian for the constraint
   */
  virtual Real computeQpJacobian( Moose::ConstraintJacobianType type ) override;

  Real computeQpStep();

  // Holds the priamry node set or side set
  std::string _primary_node_set_id;
  // Holds the secondary node set or side set
  std::string _secondary_node_set_id;
  // Penalty if constraint is not satisfied
  Real _penalty;

  // We may use a function to describe the loading scheme
  const Function * const _function;

  // We may normalize the load wrt to the number of primary nodes
  const bool _normalize_load;

  // The c vector used for computation of the linear constraint
  const std::vector< Real > _c_vector;

  // The l parameter
  const Real _l_parameter;

  // The c vector used for computation of the linear constraint, only for the connected nodes
  std::vector< Real > _connected_nodes_c_vector;

  // This is the penalty parameter, and a potential reduction factor if we normalize wrt to the
  // number of primary nodes
  Real _load_factor;

  // For computing the linear constraint per secondary node incrementally, we need to know each
  // nodes share in the total residual!
  //
  // (c^T * u_secondary - L) --> (c^T * ( u_secondary - L * _share_per_secondary_node) )
  //
  // Clearly, _share_per_secondary_node is just 1./number_of_secondary_nodes
  //
  // This way, we can traverse per node!
  Real _share_per_secondary_node;
};
