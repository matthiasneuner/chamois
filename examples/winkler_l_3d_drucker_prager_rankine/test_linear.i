[Mesh]
  type = FileMesh
  file = mesh_coarse.inp
  construct_node_list_from_side_list = false # prevents from erronously adding side nodes to the alphabetically first nodeset
[]

[GlobalParams]
  volumetric_locking_correction = false
  displacements = 'disp_x disp_y disp_z'
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
[]

[Kernels]
  [div_sig_x]
    type = StressDivergenceTensors
    variable = disp_x
    component = 0
    use_displaced_mesh = false
    save_in = force_x
  []
  [div_sig_y]
    type = StressDivergenceTensors
    variable = disp_y
    component = 1
    use_displaced_mesh = false
    save_in = force_y
  []
  [div_sig_z]
    type = StressDivergenceTensors
    variable = disp_z
    component = 2
    use_displaced_mesh = false
    save_in = force_z
  []
[]

[AuxVariables]
  [von_mises]
    order = CONSTANT
    family = MONOMIAL
  []
  [stress_first_invariant]
    order = CONSTANT
    family = MONOMIAL
  []
  [force_x]
  []
  [force_y]
  []
  [force_z]
  []
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
[elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 25850
    poissons_ratio = 0.18
[]
[strain]
    type = ComputeSmallStrain
[]
[stress]
    type = ComputeLinearElasticStress
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
  petsc_options_value = 'hypre      boomeramg         gmres         301                  0.6                                  4                          5                                 25                             HMIS                             ext+i                           1                         0.3'

  #petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  #petsc_options_value = ' lu       mumps'

  nl_rel_tol = 1e-8
  nl_abs_tol = 1e-8
  l_tol = 1e-4
  l_max_its = 300
  nl_max_its = 20

  #line_search = 'none'

  dtmin = 1e-5
  dtmax= 5e-3
  
  start_time = 0.0
  end_time = 1.0 

  num_steps = 1000
  [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 15
    iteration_window = 3
    linear_iteration_ratio = 100
    growth_factor=1.5
    cutback_factor=0.5
    dt = 5e-3
  []
  [Quadrature]
    order=SECOND
  []
[] 

[Outputs]
  interval = 1
  #execute_on = 'initial timestep_end'
  print_linear_residuals = false
  csv = true
  exodus = true
  [pgraph]
    type = PerfGraphOutput
    execute_on = 'final'  # Default is "final"
    level = 1             # Default is 1
  []
[]
