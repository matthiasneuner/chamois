[Mesh]
  [prism]
    type = GeneratedMeshGenerator
    xmax=40
    ymax=80
    zmax=1
    nx = 2
    ny = 2
    nz = 1
    dim = 3
    elem_type = HEX8
  []
  [right_top]
    type = ExtraNodesetGenerator
    new_boundary = 'right_top'
    coord = '40 80 0'
    input = prism 
  []
[]

[GlobalParams]
  displacements   = 'disp_x disp_y disp_z'
  order = FIRST
[]

[Variables]
  [disp_x][]
  [disp_y][]
  [disp_z][]
  [microrot_x]  []
  [microrot_y]  []
  [microrot_z]  []
  [nonlocal_damage]  []
[]

[GradientEnhancedMicropolarContinuum]
  [all]
    micro_rotations = 'microrot_x microrot_y microrot_z'
    nonlocal_damage = nonlocal_damage

    save_in_disp_x = 'force_x'
    save_in_disp_y = 'force_y'
    save_in_disp_z = 'force_z'

    marmot_material_name = GOSFORDSANDSTONE

                        #E,     nu,    GcToG,  lb,   lt,       lj2,        polarRatio,               cohesion,   phi,    psi,    A,          hExpDelta,      hExp,   hDilationExp
                        #a1,   a2,     a3,     a4,   softeningModulus,        maxDamage,  nonLocalRadius
    marmot_material_parameters = 
                        '13000  0.35   .1      1   2        1          1.49999         8         30      20      1.00      +11           1.4e3      1
                        0.5   0.0   0.5     0.0   1.1e-1                    0.990     1 '
  []
[]

[AuxVariables]
  [force_y][]
  [force_x][]
  [force_z][]
  [alphaP]
    order=CONSTANT
    family=MONOMIAL
  []
  [omega]
    order=CONSTANT
    family=MONOMIAL
  []
[]

[AuxKernels]
  [alphaP_kernel]
    type = MaterialStdVectorAux
    variable =alphaP 
    property = state_vars
    index = 27
    execute_on = TIMESTEP_END
  []
  [omega_kernel]
    type = MaterialStdVectorAux
    variable = omega
    property = state_vars
    index = 29
    execute_on = TIMESTEP_END
  []
[]

[Postprocessors]
  [rf_tube]
    type = NodalSum
    variable = force_y
    boundary = top
  []
[]

[BCs]
  #
  # SYMMETRY X
  #
  [bottom_x]
    type = DirichletBC
    variable = disp_x
    boundary = bottom
    value = 0
    preset = true
  []
  [bottom_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
    preset = true
  []
  [bottom_z]
    type = DirichletBC
    variable = disp_z
    boundary = bottom
    value = 0
    preset = true
  []
 #
 # LOAD
 #
   [top_z]
     type = DirichletBC
     variable = disp_z
     boundary = top
     value = 0
     preset = true
   []
  [top_y]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = 'top'
    # preset = true
    function = rampConstant2
    preset = true
  []

  [frontback_z]
    type = DirichletBC
    variable = disp_z
    boundary = 'front back'
    value = 0
    preset = true
  []
  [frontback_rx]
    type = DirichletBC
    variable = microrot_x
    boundary = 'front back'
    value = 0
    preset = true
  []
  [frontback_ry]
    type = DirichletBC
    variable = microrot_y
    boundary = 'front back'
    value = 0
    preset = true
  []

 [FiniteStrainPressure]
   [Side1]
     boundary = 'left right'
     function = rampConstant1
   []
 []
[]

[Constraints]
  [x_top]
    type = EqualValueBoundaryConstraint
    variable = disp_x
    secondary = 'top' # boundary
    penalty = 10e+3
  []
[]

[Functions]
  [./rampConstant1]
    type = PiecewiseLinear
    x = '0. 1. 2.'
    y = '0. 1. 1.'
    scale_factor = 20
  [../]
  [./rampConstant2]
    type = PiecewiseLinear
    x = '0. 1. 2.'
    y = '0. 0. 1.'
    scale_factor = -1
  [../]
  [dt_max_fun]
    type = PiecewiseLinear
    x = '0.0 1.0 2.0'
    y = '0.0 0.0 1.0'
  [../]
[]

[NodalKernels]
  [perturbation]
    type = UserForcingFunctionNodalKernel
    variable = disp_x
    function = '-1'
    boundary= right_top
  []
[]


[Preconditioning]
  active='smp2'
  [smp]
    type = SMP
    full = true
    petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
    petsc_options_value = ' lu       strumpack'
  []
  [smp2]
    type = SMP
    full = true

    petsc_options_iname = '     -pc_type
                                -pc_hypre_type
                                -ksp_type
                                -ksp_gmres_restart
                                -pc_hypre_boomeramg_relax_type_all
                                -pc_hypre_boomeramg_strong_threshold
                                -pc_hypre_boomeramg_agg_nl
                                -pc_hypre_boomeramg_agg_num_paths
                                -pc_hypre_boomeramg_max_levels
                                -pc_hypre_boomeramg_coarsen_type
                                -pc_hypre_boomeramg_interp_type
                                -pc_hypre_boomeramg_P_max
                                -pc_hypre_boomeramg_truncfactor' 

    petsc_options_value = '     hypre
                                boomeramg
                                gmres
                                201
                                chebyshev
                                0.75
                                4 
                                2
                                25
                                Falgout
                                ext+i
                                0
                                0.1 '
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'

  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-9
  l_tol = 1e-4
  l_max_its = 300
  nl_max_its = 20
  nl_div_tol = 1e4

  automatic_scaling = true
  compute_scaling_once = true
  verbose = false

  line_search = 'none'

  dtmin = 1e-7
  dtmax= 2e-1
  
  start_time = 0.0
  end_time = 2.0 

  num_steps = 500

  [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 8
    iteration_window = 3
    linear_iteration_ratio = 1000
    growth_factor = 1.2
    cutback_factor = 0.5
    dt=0.1
    time_t =  '0        1.0'
    time_dt = '0.2      5e-3'
    timestep_limiting_function = dt_max_fun
    max_function_change = 5e-2
  []
  [Quadrature]
    type = GAUSS
    order = SECOND
  []
  [Predictor]
    type = SimplePredictor
    scale = 1.0
    skip_after_failed_timestep = true
  []
[] 

[Outputs]
  interval = 1
  print_linear_residuals = false
  csv = true
  exodus = true

  [pgraph]
    type = PerfGraphOutput
    execute_on = 'timestep_end final'  # Default is "final"
    level = 1             # Default is 1
  []
[]
