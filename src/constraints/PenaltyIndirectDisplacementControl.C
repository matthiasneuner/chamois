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

// MOOSE includes
#include "MooseError.h"
#include "PenaltyIndirectDisplacementControl.h"
#include "MooseMesh.h"
#include "Function.h"
#include <iostream>

registerMooseObject( "MooseApp", PenaltyIndirectDisplacementControl );

InputParameters
PenaltyIndirectDisplacementControl::validParams()
{
  InputParameters params = NodalConstraint::validParams();
  params.addClassDescription(
      "Constrains secondary node to move as a linear combination of primary nodes." );
  params.addParam< BoundaryName >(
      "primary_node_set", "NaN", "The boundary ID associated with the primary side" );
  params.addParam< BoundaryName >(
      "secondary_node_set", "NaN", "The boundary ID associated with the secondary side" );
  params.addRequiredParam< Real >( "penalty", "The penalty used for the boundary term" );
  params.addParam< FunctionName >( "function", "the function" );
  params.addParam< bool >(
      "normalize_load",
      true,
      "Normalize the applied load with respect to the number of pimary nodes" );
  params.addRequiredParam< std::vector< Real > >( "c_vector",
                                                  "the projection vector for the constraint" );
  params.addRequiredParam< Real >( "l", "length paremeter" );
  return params;
}

PenaltyIndirectDisplacementControl::PenaltyIndirectDisplacementControl(
    const InputParameters & parameters )
  : NodalConstraint( parameters ),
    _primary_node_set_id( getParam< BoundaryName >( "primary_node_set" ) ),
    _secondary_node_set_id( getParam< BoundaryName >( "secondary_node_set" ) ),
    _penalty( getParam< Real >( "penalty" ) ),
    _function( isParamValid( "function" ) ? &getFunction( "function" ) : NULL ),
    _normalize_load( getParam< bool >( "normalize_load" ) ),
    _c_vector( getParam< std::vector< Real > >( "c_vector" ) ),
    _l_parameter( getParam< Real >( "l" ) )
{

  // we get an instance of the libmesh mesh
  const auto & lm_mesh = _mesh.getMesh();

  if ( _secondary_node_set_id == "NaN" )
    mooseError( "Please specify secondary_node_set." );

  if ( _primary_node_set_id == "NaN" )
    mooseError( "Please specify primary_node_set." );

  std::vector< dof_id_type > secondary_node_list =
      _mesh.getNodeList( _mesh.getBoundaryID( _secondary_node_set_id ) );

  // We fill the connected nodes vector. This is actually the vector of secondary nodes
  // But we do that only if the node is on our current partition
  /* for (auto n :  nodelist) */
  for ( unsigned int i = 0; i < secondary_node_list.size(); i++ )
  {
    const auto n = secondary_node_list[i];

    const Node * const node = lm_mesh.query_node_ptr( n );
    if ( node && node->processor_id() == _subproblem.processor_id() )
    {
      _connected_nodes.push_back( n ); // defining secondary nodes in the base class

      const auto c = _c_vector[i];
      _connected_nodes_c_vector.push_back( c ); // Also store the respective c vector
    }
  }

  _share_per_secondary_node = 1. / secondary_node_list.size();

  const auto & node_to_elem_map = _mesh.nodeToElemMap();

  // In contrast to the secondary nodes, all partitions get access to all primary nodes.
  // Accordingly, we need to ghost the attached elements for all partitions to make the variable
  // values accessible

  // Add elements connected to primary node to Ghosted Elements
  // We traverse all primary dofs ...
  std::vector< dof_id_type > primary_nodelist =
      _mesh.getNodeList( _mesh.getBoundaryID( _primary_node_set_id ) );

  // We fill the connected nodes vector. This is actually the vector of secondary nodes
  // But we do that only if the node is on our current partition
  for ( auto dof_id : primary_nodelist )
  {

    auto node_to_elem_pair = node_to_elem_map.find( dof_id );

    // Our mesh may be distributed
    // The following actually does not work, so we should revisit this section for
    // distributed meshes!
    if ( node_to_elem_pair == node_to_elem_map.end() )
      continue;

    // defining primary nodes in base class
    // As mentioned, all paritions get access to all primary nodes, so no matter what, add them!
    _primary_node_vector.push_back( dof_id );

    const std::vector< dof_id_type > & elems = node_to_elem_pair->second;

    // In order to really get access to variable values on all partitions,
    // we ghost the attached elements
    for ( const auto & elem_id : elems )
      _subproblem.addGhostedElem( elem_id );
  }

  // Determine the actual penalty stiffness
  _load_factor = _penalty;

  // We may account for the number of nodes where we apply the load
  // by normalizing the penalty stiffness
  if ( _normalize_load )
  {
    Real normalization = 1. / primary_nodelist.size();
    _load_factor *= normalization;
  }
}

Real
PenaltyIndirectDisplacementControl::computeQpStep()
{
  Real step = _t;

  if ( _function )
    step = _function->value( _t, _point_zero );

  return step;
}

Real
PenaltyIndirectDisplacementControl::computeQpResidual( Moose::ConstraintType type )
{
  switch ( type )
  {
    case Moose::Primary:
      return _load_factor * ( _connected_nodes_c_vector[_i] * _u_secondary[_i] -
                              computeQpStep() * _l_parameter * _share_per_secondary_node );
    case Moose::Secondary:
      return 0;
  }
  return 0.;
}

Real
PenaltyIndirectDisplacementControl::computeQpJacobian( Moose::ConstraintJacobianType type )
{
  switch ( type )
  {
    case Moose::PrimaryPrimary:
      return 0;
    case Moose::PrimarySecondary:
      return _load_factor * _connected_nodes_c_vector[_i];
    case Moose::SecondarySecondary:
      return 0;
    case Moose::SecondaryPrimary:
      return 0;
    default:
      mooseError( "Unsupported type" );
      break;
  }
  return 0.;
}
