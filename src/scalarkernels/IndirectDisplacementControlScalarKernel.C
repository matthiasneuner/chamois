//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "IndirectDisplacementControlScalarKernel.h"

// MOOSE includes
#include "Assembly.h"
#include "MooseVariableScalar.h"
#include "Function.h"

registerMooseObject( "MooseApp", IndirectDisplacementControlScalarKernel );

InputParameters
IndirectDisplacementControlScalarKernel::validParams()
{
  InputParameters params = NodalScalarKernel::validParams();
  params.addClassDescription( "Constrain two nodes to have identical values." );
  params.addRequiredCoupledVar( "constrained_variables", "Variable(s) to put the constraint on" );
  params.addParam< FunctionName >( "function", "the function" );
  params.addRequiredParam< std::vector< Real > >( "c_vector",
                                                  "the projection vector for the constraint" );
  params.addRequiredParam< Real >( "l", "length paremeter" );
  return params;
}

IndirectDisplacementControlScalarKernel::IndirectDisplacementControlScalarKernel(
    const InputParameters & parameters )
  : NodalScalarKernel( parameters ),
    _n_constrained_variables( coupledComponents( "constrained_variables" ) ),
    _constrained_variables_numbers( coupledIndices( "constrained_variables" ) ),
    _constrained_variables_values( coupledValues( "constrained_variables" ) ),
    _function( isParamValid( "function" ) ? &getFunction( "function" ) : NULL ),
    _c_vector( getParam< std::vector< Real > >( "c_vector" ) ),
    _l_parameter( getParam< Real >( "l" ) ),
    _n_constrained_nodes( _node_ids.size() )
{
  if ( _n_constrained_nodes * _n_constrained_variables != _c_vector.size() )
    mooseError( "c vector does not match total number of constrained dofs" );
    
  const auto & node_to_elem_map = _mesh.nodeToElemMap();

  // Add elements connected to constrained nodes to Ghosted Elements
  for (const auto & dof : _node_ids)
  {
    auto node_to_elem_pair = node_to_elem_map.find(dof);
    mooseAssert(node_to_elem_pair != node_to_elem_map.end(), "Missing entry in node to elem map");
    const std::vector<dof_id_type> & elems = node_to_elem_pair->second;

    for (const auto & elem_id : elems)
      _subproblem.addGhostedElem(elem_id);
  }

}

Real
IndirectDisplacementControlScalarKernel::getStep()
{
  Real step = _t;

  if ( _function )
    step = _function->value( _t + _dt, _point_zero );

  return step;
}

void
IndirectDisplacementControlScalarKernel::computeResidual()
{
  prepareVectorTag( _assembly, _var.number() );

  auto step = getStep();

  _local_re( 0 ) = -_l_parameter * step;

  for ( unsigned int iNode = 0; iNode < _n_constrained_nodes; iNode++ )
    for ( unsigned int jDim = 0; jDim < _n_constrained_variables; jDim++ )
      _local_re( 0 ) += _c_vector[iNode * _n_constrained_variables + jDim] *
                        ( *_constrained_variables_values[jDim] )[iNode];

  assignTaggedLocalResidual();
}

void
IndirectDisplacementControlScalarKernel::computeJacobian()
{
  auto step = getStep();

  prepareMatrixTag( _assembly, _var.number(), _var.number() );

  // put zeroes on the diagonal (we have to do it, otherwise PETSc will complain!)
  for ( unsigned int i = 0; i < _local_ke.m(); i++ )
    for ( unsigned int j = 0; j < _local_ke.n(); j++ )
      _local_ke( i, j ) = 0.;

  assignTaggedLocalMatrix();

  for ( unsigned int jDim = 0; jDim < _n_constrained_variables; jDim++ )
  {
    prepareMatrixTag( _assembly, _var.number(), _constrained_variables_numbers[jDim] );
    for ( unsigned int iNode = 0; iNode < _n_constrained_nodes; iNode++ )
      _local_ke( 0, iNode ) = _c_vector[iNode * _n_constrained_variables + jDim];
    assignTaggedLocalMatrix();
  }
}
