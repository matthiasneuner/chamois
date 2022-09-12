[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  ny = 2
  nz = 2
  xmin = 0
  xmax = 10
  ymin = 0
  ymax = 20
  zmin = 0
  zmax = 10
  elem_type = HEX20
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
    save_in = force_x
  []
  [div_pki_y]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_y
    tensor = pk_i_stress
    component = 1
    save_in = force_y
  []
  [div_pki_z]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_z
    tensor = pk_i_stress
    component = 2
  []
  [div_pki_couple_stress_x]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_x
    tensor = pk_i_couple_stress
    component = 0
  []
  [div_pki_couple_stress_y]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_y 
    tensor = pk_i_couple_stress
    component = 1
  []
  [div_pki_couple_stress_z]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_z 
    tensor = pk_i_couple_stress
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
  [force_y] []
  [force_x] []
[]

[Postprocessors]
  [bot_react_y]
    type = NodalSum
    variable = force_y
    boundary = bottom
  []
  [bot_react_x]
    type = NodalSum
    variable = force_x
    boundary = bottom
  []
[]

[Materials]
  [marmot_material]
    type = ComputeMarmotMaterialGradientEnhancedMicropolar
    marmot_material_name = GMDRUCKERPRAGER
                                #**E,     nu,    GcToG,  lb,   lt,       polarRatio,     sigmaYield,     hlin,  hExp,  hDeltaExp         phi(deg), psi(deg) 
                                #**a1,   a2,     a3,     a4,   lJ2,      softeningModulus,   weightingParemeter,     maxDamage,  nonLocalRadius
    marmot_material_parameters = '100        0.25   .5    4   8         1.4999999        0.06           0     1     0                 25      0.0 
                                  0.5   0.0   0.5     0.0   10.0        1e-0               1.0                    0.90       8.0'
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
  [front_z]
    type = DirichletBC
    variable = disp_z
    boundary = front
    value = 0
  []
  [back_z]
    type = DirichletBC
    variable = disp_z
    boundary = back
    value = 0
  []
  [top_y]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = top 
    function = '-0.1 * t'
  []
  [top_x]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = top 
    function = '-0.1 * t'
  []
[]

[Preconditioning]
  active='smp'
  [smp]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
    petsc_options_value = ' lu       strumpack'
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'

  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-12
  nl_max_its = 20
  nl_div_tol = 1e2

  automatic_scaling = true

  line_search = 'none'

  dtmin = 1e-4
  dtmax= 1e-1
  
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
    dt = 1e-1
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
  execute_on = 'initial timestep_end final failed'
  print_linear_residuals = false
  csv = true
  exodus=true
  [pgraph]
    type = PerfGraphOutput
    execute_on = 'final'  # Default is "final"
    level = 2             # Default is 1
  []
[]
