[Mesh]
  [prism]
    type = GeneratedMeshGenerator
    xmax=40
    ymax=80
    zmax=1
    nx = 4
    ny = 8
    nz = 2
    dim = 3
    elem_type = HEX8
  []
  [left_top]
    type = ExtraNodesetGenerator
    new_boundary = 'left_top'
    coord = '00 80 0'
    input = prism 
  []
  [right_top]
    type = ExtraNodesetGenerator
    new_boundary = 'right_top'
    coord = '40 80 0'
    input = left_top
  []
  [right_bottom]
    type = ExtraNodesetGenerator
    new_boundary = 'right_bottom'
    coord = '40 00 0'
    input = right_top
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



[Constraints]
   [./ced]
     type = PenaltyIndirectDisplacementControl
     variable = disp_y
     primary_node_set = left_top
     secondary_node_set = right_top
     c_vector = '-1'
     penalty = 1e6
     l = .1
     [../]
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
                        '30e3 0.35   .1      1   2        1          1.49999         8e10         30      20      1.00      +11           1.4e3      1
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
#[FiniteStrainPressure]
#  [fps]
#     boundary = 'top'
#     lambda = "lambda"
# []
#[]
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
  nl_abs_tol = 1e-9
  l_tol = 1e-4
  l_max_its = 300
  nl_max_its = 20
  nl_div_tol = 1e4

#  automatic_scaling = true
#  compute_scaling_once = true
#  verbose = false

  line_search = 'none'

  dtmin = 1e0
  dtmax= 1e-1
  
  start_time = 0.0
  end_time = 1.0 

  num_steps = 500

  [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 8
    iteration_window = 3
    linear_iteration_ratio = 1000
    growth_factor = 1.2
    cutback_factor = 0.5
    dt=1.0
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
  print_linear_residuals = true
  csv = true
  exodus = true

  [pgraph]
    type = PerfGraphOutput
    execute_on = 'timestep_end final'  # Default is "final"
    level = 1             # Default is 1
  []
[]
