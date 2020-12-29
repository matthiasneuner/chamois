[Mesh]
  type = FileMesh
  file = mesh.inp
[]

[GlobalParams]
  order = SECOND
  displacements   = 'disp_x disp_y disp_z'
  micro_rotations = 'microrot_x microrot_y microrot_z'
  nonlocal_damage = 'nonlocal_damage'
  use_displaced_mesh = false
[]

[Variables]
  [disp_x] []
  [disp_y] []
  [disp_z] []
  [microrot_x] []
  [microrot_y] []
  [microrot_z] []
  [nonlocal_damage] []
[]

[Kernels]
  [div_pki_x]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_x
    tensor = pk_i_stress
    component = 0
  []
  [div_pki_y]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_y
    tensor = pk_i_stress
    component = 1
  []
  [div_pki_z]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_z
    tensor = pk_i_stress
    component = 2
    save_in = force_z
  []
  [div_pki_couple_stress_x]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_x
    tensor = couple_pk_i_stress
    component = 0
  []
  [div_pki_couple_stress_y]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_y 
    tensor = couple_pk_i_stress
    component = 1
  []
  [div_pki_couple_stress_z]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_z 
    tensor = couple_pk_i_stress
    component = 2
  []
  [mom_pki_couple_stress_x]
    type = GradientEnhancedMicropolarKirchhoffMoment
    variable = microrot_x
    tensor = kirchhoff_moment
    component = 0
  []
  [mom_pki_couple_stress_y]
    type = GradientEnhancedMicropolarKirchhoffMoment
    variable = microrot_y 
    tensor = kirchhoff_moment
    component = 1
  []
  [mom_pki_couple_stress_z]
    type = GradientEnhancedMicropolarKirchhoffMoment
    variable = microrot_z 
    tensor = kirchhoff_moment
    component = 2
  []
  [helmholtz]
    type = GradientEnhancedMicropolarDamage
    variable = nonlocal_damage
  []
[]

[AuxVariables]
  [force_z] []
[]

[Postprocessors]
  [bot_react_z]
    type = NodalSum
    variable = force_z
    boundary = bottom
  []
[]

[Materials]
  [marmot_material]
    type = ComputeMarmotMaterialGradientEnhancedMicropolar
    marmot_material_name = GMDRUCKERPRAGER
                                  #E,   nu,    GcToG,   lb,     lt,     polarRatio,     sigmaYield,     h,  phi(deg),   psi(deg)    a1,   a2,   a3,     a4,   lJ2,  softeningModulus,   weightingParemeter,     maxDamage,  nonLocalRadius
    marmot_material_parameters = '100   0.25   .5       2       4       1.4999999       0.060e1         10   00          00.0        0.5   0.0   0.5     0.0   4e5   1e-0                1.0                     0.00        4.0'
  []
[]

[BCs]
  [bottom_x]
    type = DirichletBC
    variable = microrot_x
    boundary = bottom
    value = 0
  []
  [bottom_y]
    type = DirichletBC
    variable = microrot_y
    boundary = bottom
    value = 0
  []
  [bottom_z]
    type = DirichletBC
    variable = disp_z
    boundary = bottom
    value = 0
  []

  [sym_x]
    type = DirichletBC
    variable = disp_x
    boundary = xsym
    value = 0
  []
  [sym_x_roty]
    type = DirichletBC
    variable = microrot_y
    boundary = xsym
    value = 0
  []
  [sym_x_rotz]
    type = DirichletBC
    variable = microrot_z
    boundary = xsym
    value = 0
  []

  [sym_y]
    type = DirichletBC
    variable = disp_y
    boundary = ysym
    value = 0
  []
  [sym_y_rotx]
    type = DirichletBC
    variable = microrot_x
    boundary = ysym
    value = 0
  []
  [sym_y_rotz]
    type = DirichletBC
    variable = microrot_z
    boundary = ysym
    value = 0
  []

  [top_x]
    type = DirichletBC
    variable = disp_x
    boundary = top
    value = 0
  []
  [top_y]
    type = DirichletBC
    variable = disp_y
    boundary = top
    value = 0
  []
  [top_z]
    type = FunctionDirichletBC
    variable = disp_z
    boundary = top 
    function = '-250.0 * t'
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

  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-8
  l_tol = 1e-3
  l_max_its = 250
  nl_max_its = 20
  nl_div_tol = 1e2

  automatic_scaling=true
  compute_scaling_once =true
  verbose=false

  line_search = none

  dtmin = 1e-5
  dtmax= 5e-3
  
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
    dt = 1e-3
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
  interval = 1
  print_linear_residuals = false
  csv = true
  exodus = true
  [pgraph]
    type = PerfGraphOutput
    execute_on = 'final'  # Default is "final"
    level = 2             # Default is 1
  []
[]
