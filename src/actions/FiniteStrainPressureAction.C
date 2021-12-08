//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FiniteStrainPressureAction.h"
#include "Factory.h"
#include "FEProblem.h"
#include "Conversion.h"

registerMooseAction( "ChamoisApp", FiniteStrainPressureAction, "add_bc" );

registerMooseAction( "ChamoisApp", FiniteStrainPressureAction, "add_material" );

InputParameters
FiniteStrainPressureAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("Set up FiniteStrainPressure boundary conditions");

  params.addRequiredParam<std::vector<BoundaryName>>(
      "boundary", "The list of boundary IDs from the mesh where the pressure will be applied");

  params.addParam<std::vector<VariableName>>(
      "displacements",
      "The displacements appropriate for the simulation geometry and coordinate system");

  params.addParam<std::vector<AuxVariableName>>("save_in_disp_x",
                                                "The save_in variables for x displacement");
  params.addParam<std::vector<AuxVariableName>>("save_in_disp_y",
                                                "The save_in variables for y displacement");
  params.addParam<std::vector<AuxVariableName>>("save_in_disp_z",
                                                "The save_in variables for z displacement");

  params.addParam<Real>("factor", 1.0, "The factor to use in computing the pressure");
  params.addParam<Real>("hht_alpha",
                        0,
                        "alpha parameter for mass dependent numerical damping induced "
                        "by HHT time integration scheme");
  params.addDeprecatedParam<Real>(
      "alpha", "alpha parameter for HHT time integration", "Please use hht_alpha");
  params.addParam<FunctionName>("function", "The function that describes the pressure");
  return params;
}

FiniteStrainPressureAction::FiniteStrainPressureAction(const InputParameters & params)
  : Action(params)
{
  _save_in_vars.push_back(getParam<std::vector<AuxVariableName>>("save_in_disp_x"));
  _save_in_vars.push_back(getParam<std::vector<AuxVariableName>>("save_in_disp_y"));
  _save_in_vars.push_back(getParam<std::vector<AuxVariableName>>("save_in_disp_z"));

  _has_save_in_vars.push_back(params.isParamValid("save_in_disp_x"));
  _has_save_in_vars.push_back(params.isParamValid("save_in_disp_y"));
  _has_save_in_vars.push_back(params.isParamValid("save_in_disp_z"));
}

void
FiniteStrainPressureAction::act()
{

  std::string deformed_boundary_normal_vector_material_name = "ComputeDeformedBoundaryNormalVector";
  std::string kernel_name = "FiniteStrainPressure";

  std::vector<VariableName> displacements = getParam<std::vector<VariableName>>("displacements");

  // Create pressure BCs
  if ( _current_task == "add_bc" ){
      for (unsigned int i = 0; i < displacements.size(); ++i)
      {
        // Create unique kernel name for each of the components
        std::string unique_kernel_name = kernel_name + "_" + _name + "_" + Moose::stringify(i);

        InputParameters params = _factory.getValidParams(kernel_name);
        params.applyParameters(parameters(), {"factor"});
        params.set<bool>("use_displaced_mesh") = false;
        params.set<Real>("alpha") =
            isParamValid("alpha") ? getParam<Real>("alpha") : getParam<Real>("hht_alpha");

        params.set<unsigned int>("component") = i;
        params.set<NonlinearVariableName>("variable") = displacements[i];

        if (_has_save_in_vars[i])
          params.set<std::vector<AuxVariableName>>("save_in") = _save_in_vars[i];

        else
          params.set<Real>("factor") = getParam<Real>("factor");
        _problem->addBoundaryCondition(kernel_name, unique_kernel_name, params);
     }
  }
  else if ( _current_task == "add_material" )
  {
    auto deformed_boundary_normal_vector_material_parameters = _factory.getValidParams( deformed_boundary_normal_vector_material_name );
    deformed_boundary_normal_vector_material_parameters.applyParameters( parameters() );

    _problem->addMaterial( deformed_boundary_normal_vector_material_name , name() + "_material", deformed_boundary_normal_vector_material_parameters );
  }
}
