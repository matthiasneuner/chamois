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

#include "GradientEnhancedMicropolarContinuumAction.h"
#include <string>
#include <vector>
#include "FEProblem.h"
#include "Factory.h"

registerMooseAction( "ChamoisApp", GradientEnhancedMicropolarContinuumAction, "add_kernel" );

registerMooseAction( "ChamoisApp", GradientEnhancedMicropolarContinuumAction, "add_material" );

const std::vector< std::string > GradientEnhancedMicropolarContinuumAction::excludedParameters = {
    "marmot_material_name",
    "marmot_material_parameters",
    "save_in_disp_x",
    "save_in_disp_y",
    "save_in_disp_z",
};

InputParameters
GradientEnhancedMicropolarContinuumAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addRequiredCoupledVar( "displacements",
                                "The string of displacements suitable for the problem statement" );
  params.addRequiredCoupledVar(
      "micro_rotations", "The string of micro rotations suitable for the problem statement" );
  params.addRequiredCoupledVar( "nonlocal_damage", "The nonlocal damage field" );
  params.addParam< std::string >( "base_name", "Material property base name" );
  params.addParam< std::vector< AuxVariableName > >( "save_in_disp_x",
                                                     "Store displacement residuals" );
  params.addParam< std::vector< AuxVariableName > >( "save_in_disp_y",
                                                     "Store displacement residuals" );
  params.addParam< std::vector< AuxVariableName > >( "save_in_disp_z",
                                                     "Store displacement residuals" );
  params.addParam< std::vector< SubdomainName > >( "block",
                                                   "The list of ids of the blocks (subdomain) "
                                                   "that the kernels will be "
                                                   "applied to" );
  params.addRequiredParam< std::string >( "marmot_material_name",
                                          "Material name for the MarmotMaterial" );
  params.addRequiredParam< std::vector< Real > >( "marmot_material_parameters",
                                                  "Material Parameters for the MarmotMaterial" );
  return params;
}

GradientEnhancedMicropolarContinuumAction::GradientEnhancedMicropolarContinuumAction(
    const InputParameters & parameters )
  : Action( parameters ),
    _ndisp( getParam< std::vector< VariableName > >( "displacements" ).size() ),
    _nmrot( getParam< std::vector< VariableName > >( "micro_rotations" ).size() )
{
  if ( _ndisp != 3 || _nmrot != 3 )
    mooseError( "Gradient-enhanced micropolar kernels are implemented only for 3D!" );

  if ( parameters.isParamSetByUser( "use_displaced_mesh" ) )
  {
    bool use_displaced_mesh_param = getParam< bool >( "use_displaced_mesh" );
    if ( use_displaced_mesh_param )
      mooseError( "use_displaced_mesh must be set to false" );
  }
}

void
GradientEnhancedMicropolarContinuumAction::act()
{
  if ( _current_task == "add_kernel" )
    addKernels();
  else if ( _current_task == "add_material" )
    addMaterial();
}

void
GradientEnhancedMicropolarContinuumAction::addKernels()
{
  std::string pki_stress_kernel( "GradientEnhancedMicropolarPKIDivergence" );

  for ( unsigned int i = 0; i < _ndisp; ++i )
  {
    const std::string kernel_name = name() + "_div_pki_stress_" + Moose::stringify( i );

    InputParameters pki_stress_kernel_params = _factory.getValidParams( pki_stress_kernel );
    pki_stress_kernel_params.applyParameters( parameters(), excludedParameters );

    pki_stress_kernel_params.set< unsigned int >( "component" ) = i;
    pki_stress_kernel_params.set< NonlinearVariableName >( "variable" ) =
        getParam< std::vector< VariableName > >( "displacements" )[i];
    pki_stress_kernel_params.set< std::string >( "tensor" ) = "pk_i_stress";

    if ( i == 0 && isParamValid( "save_in_disp_x" ) )
      pki_stress_kernel_params.set< std::vector< AuxVariableName > >( "save_in" ) =
          getParam< std::vector< AuxVariableName > >( "save_in_disp_x" );

    if ( i == 1 && isParamValid( "save_in_disp_y" ) )
      pki_stress_kernel_params.set< std::vector< AuxVariableName > >( "save_in" ) =
          getParam< std::vector< AuxVariableName > >( "save_in_disp_y" );

    if ( i == 2 && isParamValid( "save_in_disp_z" ) )
      pki_stress_kernel_params.set< std::vector< AuxVariableName > >( "save_in" ) =
          getParam< std::vector< AuxVariableName > >( "save_in_disp_z" );

    _problem->addKernel( pki_stress_kernel, kernel_name, pki_stress_kernel_params );
  }

  for ( unsigned int i = 0; i < _nmrot; ++i )
  {
    const std::string kernel_name = name() + "_div_pki_couple_stress_" + Moose::stringify( i );

    InputParameters pki_stress_kernel_params = _factory.getValidParams( pki_stress_kernel );
    pki_stress_kernel_params.applyParameters( parameters(), excludedParameters );

    pki_stress_kernel_params.set< unsigned int >( "component" ) = i;
    pki_stress_kernel_params.set< NonlinearVariableName >( "variable" ) =
        getParam< std::vector< VariableName > >( "micro_rotations" )[i];
    pki_stress_kernel_params.set< std::string >( "tensor" ) = "pk_i_couple_stress";

    _problem->addKernel( pki_stress_kernel, kernel_name, pki_stress_kernel_params );
  }

  std::string kirchhoff_moment_kernel( "GradientEnhancedMicropolarKirchhoffMoment" );

  for ( unsigned int i = 0; i < _nmrot; ++i )
  {
    const std::string kernel_name = name() + "_kirchhoff_moment_" + Moose::stringify( i );

    InputParameters kirchhoff_moment_kernel_params =
        _factory.getValidParams( kirchhoff_moment_kernel );
    kirchhoff_moment_kernel_params.applyParameters( parameters(), excludedParameters );

    kirchhoff_moment_kernel_params.set< unsigned int >( "component" ) = i;
    kirchhoff_moment_kernel_params.set< NonlinearVariableName >( "variable" ) =
        getParam< std::vector< VariableName > >( "micro_rotations" )[i];
    kirchhoff_moment_kernel_params.set< std::string >( "tensor" ) = "kirchhoff_moment";
    _problem->addKernel( kirchhoff_moment_kernel, kernel_name, kirchhoff_moment_kernel_params );
  }

  std::string nonlocal_damage_kernel( "GradientEnhancedMicropolarDamage" );
  InputParameters nonlocal_damage_kernel_params = _factory.getValidParams( nonlocal_damage_kernel );

  nonlocal_damage_kernel_params.applyParameters( parameters(), excludedParameters );

  const std::string kernel_name = name() + "_nonlocal_damage";

  nonlocal_damage_kernel_params.set< NonlinearVariableName >( "variable" ) =
      getParam< std::vector< VariableName > >( "nonlocal_damage" )[0];

  _problem->addKernel( nonlocal_damage_kernel, kernel_name, nonlocal_damage_kernel_params );
}

void
GradientEnhancedMicropolarContinuumAction::addMaterial()
{
  std::string materialType = "ComputeMarmotMaterialGradientEnhancedMicropolar";
  auto materialParameters = _factory.getValidParams( materialType );
  materialParameters.applyParameters( parameters() );

  materialParameters.set< std::string >( "marmot_material_name" ) =
      getParam< std::string >( "marmot_material_name" );
  materialParameters.set< std::vector< Real > >( "marmot_material_parameters" ) =
      getParam< std::vector< Real > >( "marmot_material_parameters" );

  _problem->addMaterial( materialType, name() + "_material", materialParameters );
}
