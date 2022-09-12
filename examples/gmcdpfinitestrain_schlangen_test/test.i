[Mesh]
  type = FileMesh
  file = schlangen_mesh.e
[]

[GlobalParams]
  displacements   = 'disp_x disp_y disp_z'
  micro_rotations = 'microrot_x microrot_y microrot_z'
  nonlocal_damage = 'nonlocal_damage'
  use_displaced_mesh = false
  order = SECOND
[]

[Variables]
  [disp_x][]
  [disp_y][]
  [disp_z][]
  [microrot_x]  []
  [microrot_y]  []
  [microrot_z]  []
  [nonlocal_damage][]
[]

[GradientEnhancedMicropolarContinuum]
  [conc]
    block = concrete
    displacements   = 'disp_x disp_y disp_z'
    marmot_material_name = GMCDPFINITESTRAIN
        #E,         nu,         GcToG,      lbending,   lTorsion,   lJ2          polarRatio,     
        #fcu,       fcy,        ftu,        fbu,        Df,         epsF,        omegaMax,     lDamage,     m
        #Ah,        Bh,         Ch,         Dh,         As,     
        #a1,        a2,         a3,         a4,         
    marmot_material_parameters = '
                                    30000.0    0.15   0.1     2           4       2          1.499999
                                    47.4       13     4.74   55         0.85           0.0050       .99             2       1.00
                                    0.08       0.003  2.0    0.000001   15.0
                                    0.5        0.00   0.5     0           '
  []
  [steel]
    block = steel 
    displacements   = 'disp_x disp_y disp_z'
    marmot_material_name = GMNEOHOOKE
                                  #E,       nu,    GcToG,   lb,     lt,     polarRatio
    marmot_material_parameters = '210000    0.21   .1       10e-3       20e-3       1.4999999'

    save_in_disp_x = 'force_x'
    save_in_disp_y = 'force_y'
    save_in_disp_z = 'force_z'
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
    block='concrete'
  []
  [omega_kernel]
    type = MaterialStdVectorAux
    variable = omega
    property = state_vars
    index = 29
    execute_on = TIMESTEP_END
    block='concrete'
  []
[]

[Postprocessors]
  [bot_react_y]
    type = NodalSum
    variable = force_y
    boundary = 'all_bc'
  []
  [cmsd_left_y]
    type = NodalMaxValue
    variable = disp_y
    boundary = cmod_left
  []
  [cmsd_right_y]
    type = NodalMaxValue
    variable = disp_y
    boundary = cmod_right
  []
  [cmsd]
    type = DifferencePostprocessor
    value1 = cmsd_right_y
    value2 = cmsd_left_y
  []
[]

[BCs]
  [fix_supp_x]
    type = DirichletBC
    variable = disp_x
    boundary = fixed_bc
    value = 0
    preset = false
  []
  [fix_supp_y]
    type = DirichletBC
    variable = disp_y
    boundary = fixed_bc
    value = 0
    preset = false
  []
  [fix_supp_z]
    type = DirichletBC
    variable = disp_z
    boundary = fixed_bc
    value = 0
    preset = false
  []
  [supp_y]
    type = DirichletBC
    variable = disp_y
    boundary = supp_y
    value = 0
    preset = false
  []
  [supp_z]
    type = DirichletBC
    variable = disp_z
    boundary = supp_y
    value = 0
    preset = false
  []
  [back_conc]
    type = DirichletBC
    variable = disp_z
    boundary = face_back_concrete
    value = 0
    preset = false
  []
  [back_steel]
    type = DirichletBC
    variable = disp_z
    boundary = face_back_steel
    value = 0
    preset = false
  []
  [back_conc_rotx]
    type = DirichletBC
    variable = microrot_x
    boundary = face_back_concrete
    value = 0
    preset = false
  []
  [back_steel_rotx]
    type = DirichletBC
    variable = microrot_x
    boundary = face_back_steel
    value = 0
    preset = false
  []
  [back_conc_roty]
    type = DirichletBC
    variable = microrot_y
    boundary = face_back_concrete
    value = 0
    preset = false
  []
  [back_steel_roty]
    type = DirichletBC
    variable = microrot_y
    boundary = face_back_steel
    value = 0
    preset = false
  []
[]

[Functions]
  [./rampConstant1]
    type = PiecewiseLinear
    x = '0. 10. '
    y = '0. 10. '
    scale_factor =1
  [../]
[]

[Constraints]
   [./load_h]
     type = PenaltyIndirectDisplacementControl
     variable = disp_y
     variable_secondary = disp_y
     primary_node_set = high_p
     secondary_node_set = cmod
     c_vector = '-1 1'
     penalty = 1e6
     l = 0.5
     function = rampConstant1
     normalize_load = true
   [../]
   [./load_l]
     type = PenaltyIndirectDisplacementControl
     variable = disp_y
     variable_secondary = disp_y
     primary_node_set = low_p
     secondary_node_set = cmod
     c_vector = '-1 1'
     penalty = 1e5
     l = 0.5
     function = rampConstant1
     normalize_load = true
   [../]
[]

[Preconditioning]
  active='smp'
   [smp2]
     type = SMP
     full = true
     petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
     petsc_options_value = ' lu       strumpack'
   []
  [smp]
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
                                301
                                symmetric-SOR/Jacobi
                                0.65
                                5 
                                2
                                25
                                HMIS
                                ext+i
                                4
                                0.4 '
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'

  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-8
  l_tol = 1e-4
  l_max_its = 200
  nl_max_its = 10
  nl_div_tol = 1e2

  automatic_scaling= true
  verbose=false

  line_search = none

  dtmin = 1e-6
  dtmax= 1e-2
  
  start_time = 0.0
  end_time = 1.0 

  num_steps = 1000

  [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 8
    iteration_window = 3
    linear_iteration_ratio = 200
    growth_factor=1.2
    cutback_factor=0.8
    dt = 1.0e-2
  []
  [Quadrature]
    order=SECOND
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
  exodus=true
  [pgraph]
    type = PerfGraphOutput
    execute_on = 'timestep_end final'  # Default is "final"
    level = 2             # Default is 1
  []
[]
