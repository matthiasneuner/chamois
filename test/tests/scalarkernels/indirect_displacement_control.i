[Mesh]
  [prism]
    type = GeneratedMeshGenerator
    xmax=40
    ymax=80
    zmax=1
    nx = 4
    ny = 8
    nz = 1
    dim = 3
    elem_type = HEX20
  []
  [right_top]
    type = ExtraNodesetGenerator
    new_boundary = 'right_top'
    coord = '40 80 0'
    input = prism 
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
  order = SECOND
[]

[Variables]
  [disp_x][]
  [disp_y][]
  [disp_z][]
  [microrot_x]  []
  [microrot_y]  []
  [microrot_z]  []
  [nonlocal_damage]  []
  [lambda]
  order = FIRST
    family = SCALAR
  []
[]


[ScalarKernels]
    [./ced]
    type = IndirectDisplacementControlScalarKernel
    variable = lambda
    # constrained_variables = 'disp_x disp_y disp_z'
    # c_vector = '0 1 0 0 -1 0'
    constrained_variables = 'disp_y '
    c_vector = ' 1 -1 '
    boundary = 'right_bottom right_top'
    l=5
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
                        '130  0.35   .1      1   2        1          1.49999         8         30      20      1.00      +11           1.4e3      1
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
[FiniteStrainPressure]
  [fps]
     boundary = 'top'
     lambda = "lambda"
 []
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
                                 symmetric-SOR/Jacobi 
                                 0.75
                                 4 
                                 2
                                 25
                                 Falgout
                                 ext+i
                                 0
                                 0.1 '
   []

[FSP]
  type = FSP
#  petsc_options_iname = '-snes_type -ksp_type -ksp_rtol -ksp_atol -ksp_max_it -snes_atol -snes_rtol -snes_max_it -snes_max_funcs'
#  petsc_options_value = 'newtonls      gmres     1e-3     1e-15       200       1e-10        1e-15       200           100000'
  topsplit = 'uv'
[uv]
  petsc_options_iname = '-pc_fieldsplit_schur_fact_type -pc_fieldsplit_schur_precondition'
  petsc_options_value = 'full selfp'
  splitting = 'u v'
  splitting_type = schur
[]
[u]
   vars = 'disp_x disp_y disp_z microrot_x microrot_y microrot_z nonlocal_damage'
    petsc_options_iname = '     -pc_type
                                -pc_hypre_type
                                -ksp_type
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
                                preonly 
                                symmetric-SOR/Jacobi 
                                0.75
                                4 
                                2
                                25
                                Falgout
                                ext+i
                                0
                                0.1 '
#    petsc_options_iname = '-pc_type -ksp_type '
#    petsc_options_value = ' hypre preonly  '
[]
[v]
   vars = 'lambda'
   #petsc_options_iname = '-pc_type -ksp_type -sub_pc_type -sub_pc_factor_levels'
   #petsc_options_value = '  jacobi  preonly        ilu            7'
   petsc_options_iname = '-pc_type -ksp_type -sub_pc_type -sub_pc_factor_levels'
   petsc_options_value = '  jacobi preonly        lu            7'
[]
[]

[vcp]
    solve_type = NEWTON
    type = VCP
    full = true
    lm_variable = 'lambda'
    primary_variable = disp_y
    preconditioner = 'AMG'
    is_lm_coupling_diagonal = false
    adaptive_condensation =false
    petsc_options_iname = ' -pc_factor_shift_type -pc_factor_shift_amount'
    petsc_options_value = ' NONZERO 1e-15'
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
