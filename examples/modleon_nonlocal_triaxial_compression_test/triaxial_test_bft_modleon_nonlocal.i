[Mesh]
  type = FileMesh
  file = structured_triaxial_compression_test_mesh.inp
[]

[GlobalParams]
  volumetric_locking_correction = false
  displacements = 'disp_x disp_y disp_z'
  nonlocal_damage = nonlocal_damage
  order = SECOND
  family = LAGRANGE
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
  [disp_z]
  []
  [nonlocal_damage]
  []
[]

[Kernels]
  [div_sig_x]
    type = GradientEnhancedStressDivergenceTensors
    #type = StressDivergenceTensors
    variable = disp_x
    component = 0
    use_displaced_mesh = false
  []
  [div_sig_y]
    type = GradientEnhancedStressDivergenceTensors
    #type = StressDivergenceTensors
    variable = disp_y
    component = 1
    use_displaced_mesh = false
  []
  [div_sig_z]
    type = GradientEnhancedStressDivergenceTensors
    #type = StressDivergenceTensors
    variable = disp_z
    component = 2
    use_displaced_mesh = false
  []
  [implicit_gradient_damage]
    type = ImplicitGradientEnhancedDamage
    variable = nonlocal_damage 
  []
[]

[AuxVariables]
  [alphaP]
    order=CONSTANT
    family=MONOMIAL
  []
  [omega]
    order=CONSTANT
    family=MONOMIAL
  []
  [von_mises]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_first_invariant]
    order = CONSTANT
    family = MONOMIAL
  []
[]

[AuxKernels]
  [alphaP_kernel]
    type = MaterialStdVectorAux
    variable =alphaP 
    property = state_vars
    index = 0
    execute_on = TIMESTEP_END
  []
  [omegal_kernel]
    type = MaterialStdVectorAux
    variable = omega
    property = state_vars
    index = 4
    execute_on = TIMESTEP_END
  []
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
  [bft_material_Elastic]
    type = ComputeBftMaterialHypoElasticNonLocalStress
    block='top_HEX20'
    bft_material_name = MODLEONNONLOCAL
    bft_material_parameters = '30000.0  0.15  13e3   47.4e3    55e3     4.74e3   0.85   0.12   0.003  2.0  0.000001  15.0  10  1.00  0.0017  1'
  []
  [bft_material_Elastic2]
    type = ComputeBftMaterialHypoElasticNonLocalStress
    block='tbottom_HEX20'
    bft_material_name = MODLEONNONLOCAL
    bft_material_parameters = '30000.0  0.15  13e3   47.4e3    55e3     4.74e3   0.85   0.12   0.003  2.0  0.000001  15.0  10  1.00  0.0017  1'
  []
  [bft_material]
    type = ComputeBftMaterialHypoElasticNonLocalStress
    block='notWeakElements_HEX20'
    bft_material_name = MODLEONNONLOCAL
    bft_material_parameters = '30000.0  0.15  13   47.4    55     4.74   0.85   0.12   0.003  2.0  0.000001  15.0  10  1.00  0.0017  1'
  []
  [bft_material_weak]
    type = ComputeBftMaterialHypoElasticNonLocalStress
    block='weakElements_HEX20'
    bft_material_name = MODLEONNONLOCAL
    bft_material_parameters = '31000.0  0.15  13   44.4    53     4.54   0.85   0.12   0.003  2.0  0.000001  15.0  10  1.00  0.0017  1'
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


[Functions]
  [./rampConstant1]
    type = PiecewiseLinear
    x = '0. 1. 2.'
    y = '0. 1. 1.'
    scale_factor = 1.5
  [../]
  [./rampConstant2]
    type = PiecewiseLinear
    x = '0. 1. 2.'
    y = '0. 0. 1.'
    scale_factor = -5
  [../]
[]

[BCs]
  [origin]
    type = DirichletBC
    variable = disp_x
    boundary = origin 
    value = 0
  []
  [bottom_z]
    type = DirichletBC
    variable = disp_z
    boundary = bottom 
    value = 0
  []
  [sym_y]
    type = DirichletBC
    variable = disp_y
    boundary = ysym
    value = 0 
  []
  [disp_top]
    type = FunctionDirichletIncrementalBC
    variable = disp_z
    boundary = top 
    function = rampConstant2
    reset_times = '1.0'
  []
  [bottom_nl]
    type = DirichletBC
    variable = nonlocal_damage
    boundary = bottom 
    value = 0
  []
  [top_nl]
    type = DirichletBC
    variable = nonlocal_damage
    boundary = top
    value = 0
  []
  [Pressure]
    [Side1]
      boundary = 'sleeveTop'
      function = rampConstant1
      displacements = 'disp_x disp_y disp_z'
    []
  []
[]
[Controls]
  [./preconsolidation]
    type = TimePeriod
    disable_objects = 'BCs/disp_top'
    start_time = '0'
    end_time = '1.0'
  []
  [axialdisplacement]
    type = TimePeriod
    enable_objects = 'BCs/disp_top'
    start_time = '1.0'
    end_time = '2.0'
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
  petsc_options_value = 'hypre      boomeramg         gmres         31                  0.8                                  4                          5                                 25                             HMIS                             ext+i                           2                         0.3'

  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-10
  l_tol = 1e-5
  l_max_its = 30
  nl_max_its = 10

  line_search = 'none'

  dtmin = 1e-5
  dtmax= 1e-1
  
  start_time = 0.0
  end_time = 2.0 

  num_steps = 200
  [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 8
    iteration_window = 3
    dt = 0.1
    time_t =  '0        1.0'
    time_dt = '0.1      1e-3'
  []
  [Quadrature]
    order=SECOND
  []
[] 

[Outputs]
  interval = 1
  execute_on = 'initial timestep_end final failed'
  print_linear_residuals = false
  csv = true
  exodus = true
  [pgraph]
    type = PerfGraphOutput
    execute_on = 'final'  # Default is "final"
    level = 2             # Default is 1
  []
[]
