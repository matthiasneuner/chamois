[Mesh]
  type = FileMesh
  file = mesh_coarse.inp
  construct_node_list_from_side_list = false # prevents from erronously adding side nodes to the alphabetically first nodeset
[]

[GlobalParams]
  volumetric_locking_correction = false
  displacements = 'disp_x disp_y disp_z'
  nonlocal_damage = nonlocal_damage
  order = SECOND
  family = LAGRANGE
[]

[Variables]
  [disp_x] []
  [disp_y] []
  [disp_z] []
  [nonlocal_damage] []
[]

[Kernels]
  [div_sig_x]
    type = GradientEnhancedStressDivergenceTensors
    variable = disp_x
    component = 0
    use_displaced_mesh = false
    save_in = force_x
  []
  [div_sig_y]
    type = GradientEnhancedStressDivergenceTensors
    variable = disp_y
    component = 1
    use_displaced_mesh = false
    save_in = force_y
  []
  [div_sig_z]
    type = GradientEnhancedStressDivergenceTensors
    variable = disp_z
    component = 2
    use_displaced_mesh = false
    save_in = force_z
  []
  [implicit_gradient_damage]
    type = ImplicitGradientEnhancedDamage
    variable = nonlocal_damage 
    use_displaced_mesh = false
  []
[]

[AuxVariables]
#  [alphaP]
#    order=CONSTANT
#    family=MONOMIAL
#  []
#  [omega]
#    order=CONSTANT
#    family=MONOMIAL
#  []
  [von_mises]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_first_invariant]
    order = CONSTANT
    family = MONOMIAL
  []
  [force_x] []
  [force_y] []
  [force_z] []
[]


[Postprocessors]
  [bot_react_y]
    type = NodalSum
    variable = force_y
    boundary = bottom
  []
[]

[AuxKernels]
  [von_mises_kernel]
    type = RankTwoScalarAux
    variable = von_mises
    rank_two_tensor = stress
    execute_on = timestep_end
    scalar_type = VonMisesStress
  []
  [first_invariant_kernel]
    type = RankTwoScalarAux
    variable = stress_first_invariant
    rank_two_tensor = stress
    execute_on = timestep_end
    scalar_type = FirstInvariant
  []
[]


[Materials]
  [marmot_material_Elastic]
    type = ComputeMarmotMaterialGradientEnhancedHypoElasticStress
    block='steel_HEX20'
    marmot_material_name = GRADIENTENHANCEDDRUCKERPRAGER
    marmot_material_parameters = '25850  0.18            20e3          2e3                     0      25        15          15    ${LDAM}     1.00         ${EPSF}      0.0'
  []
  [marmot_material]
    type = ComputeMarmotMaterialGradientEnhancedHypoElasticStress
    block='concrete_HEX20'
    marmot_material_name = GRADIENTENHANCEDDRUCKERPRAGER
    marmot_material_parameters = '25850  0.18            20e3          2.65                    0      25        15          15    ${LDAM}   1.00         ${EPSF}       .99'
  []
  [dstrain]
    type = ComputeIncrementalSmallStrain
  []
  [dstrain_vgt_conv]
    type = ConvertRankTwoTensorToVoigt
    tensor = strain_increment
    tensor_voigt = strain_increment_voigt
    shear_components_twice = true
  []
  [stress_conv]
    type = ConvertRankTwoTensorFromVoigt
    tensor = stress
    tensor_voigt = stress_voigt
    shear_components_half = false
  []
  [Jacobian_conv]
    type = ConvertRankFourTensorFromVoigt
    tensor = Jacobian_mult
    tensor_voigt = Jacobian_mult_voigt
    shear_components_half_ij = false
    shear_components_half_kl = false
    tensor_voigt_uses_row_major_layout = false
  []
  [dstress_dk_conv]
    type = ConvertRankTwoTensorFromVoigt
    tensor = dstress_dnonlocal_damage
    tensor_voigt = dstress_voigt_dnonlocal_damage
    shear_components_half = false
  []
  [dlocal_damage_dstrain_conv]
    type = ConvertRankTwoTensorFromVoigt
    tensor = dlocal_damage_dstrain
    tensor_voigt = dlocal_damage_dstrain_voigt
    shear_components_half = false
  []
[]

[BCs]
  [bottom_x]
    type = DirichletBC
    variable = disp_x
    boundary = bottom
    value = 0
  []
  [bottom_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  []
  [bottom_z]
    type = DirichletBC
    variable = disp_z
    boundary = bottom 
    value = 0
  []
  [sym_z]
    type = DirichletBC
    variable = disp_z
    boundary = zsym
    value = 0 
  []
  [load]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = load
    function = '1 * t'
  []
[]

[Preconditioning]
  [smp]
    type = SMP
    full = true
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'

    petsc_options_iname = '-pc_type   -pc_hypre_type    -ksp_type     -ksp_gmres_restart  -pc_hypre_boomeramg_strong_threshold -pc_hypre_boomeramg_agg_nl -pc_hypre_boomeramg_agg_num_paths -pc_hypre_boomeramg_max_levels -pc_hypre_boomeramg_coarsen_type -pc_hypre_boomeramg_interp_type -pc_hypre_boomeramg_P_max -pc_hypre_boomeramg_truncfactor'
    petsc_options_value = 'hypre      boomeramg         gmres         301                  0.6                                  4                          5                                 25                             HMIS                             FF1                           1                         0.3'

#   petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
#   petsc_options_value = ' lu       mumps'

  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-12
  l_tol = 1e-3
  l_max_its = 100
  nl_max_its = 20
  nl_div_tol = 1e1

  automatic_scaling=true
  compute_scaling_once = true
  verbose=false

  line_search = 'none'

  dtmin = 1e-4
  dtmax= 1e-2
  
  start_time = 0.0
  end_time = 1.0 

  num_steps = 1000
  [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 15
    iteration_window = 3
    linear_iteration_ratio = 1000
    growth_factor=1.5
    cutback_factor=0.5
    dt = 1e-2
  []
  [Quadrature]
    order=SECOND
  []
  [Predictor]
    type = SimplePredictor
    scale = 1.0
  []
[] 

[Outputs]
  file_base='${JOB}_LDAM_${LDAM}_EPSF_${EPSF}'
  interval = 1
  print_linear_residuals = false
  csv = true
  exodus = true
  [pgraph]
    type = PerfGraphOutput
    execute_on = 'final'  # Default is "final"
    level = 1             # Default is 1
  []
[]
