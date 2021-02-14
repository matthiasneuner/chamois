[Mesh]
  type = FileMesh
  file = mesh_concrete_tube.e
[]

[GlobalParams]
  displacements   = 'disp_x disp_y disp_z'
  use_displaced_mesh = false
  order = SECOND
[]

[Variables]
  [disp_x][]
  [disp_y][]
  [disp_z][]
  [microrot_steel_x] block = 'tube' []
  [microrot_steel_y] block = 'tube' []
  [microrot_steel_z] block = 'tube' []
  [microrot_concrete_x] block = 'concrete' []
  [microrot_concrete_y] block = 'concrete' []
  [microrot_concrete_z] block = 'concrete' []
  [nonlocal_damage_steel] block = 'tube' []
  [nonlocal_damage_concrete] block = 'concrete' []
[]

[Kernels]
  [div_pki_x]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_x
    tensor = pk_i_stress
    component = 0
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []
  [div_pki_y]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_y
    tensor = pk_i_stress
    component = 1
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []
  [div_pki_z]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_z
    tensor = pk_i_stress
    component = 2
    save_in = force_z
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []
  [div_pki_couple_stress_x]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_steel_x
    tensor = couple_pk_i_stress
    component = 0
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []
  [div_pki_couple_stress_y]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_steel_y 
    tensor = couple_pk_i_stress
    component = 1
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []
  [div_pki_couple_stress_z]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_steel_z 
    tensor = couple_pk_i_stress
    component = 2
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []
  [mom_pki_couple_stress_x]
    type = GradientEnhancedMicropolarKirchhoffMoment
    variable = microrot_steel_x
    tensor = kirchhoff_moment
    component = 0
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []
  [mom_pki_couple_stress_y]
    type = GradientEnhancedMicropolarKirchhoffMoment
    variable = microrot_steel_y 
    tensor = kirchhoff_moment
    component = 1
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []
  [mom_pki_couple_stress_z]
    type = GradientEnhancedMicropolarKirchhoffMoment
    variable = microrot_steel_z 
    tensor = kirchhoff_moment
    component = 2
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []
  [helmholtz]
    type = GradientEnhancedMicropolarDamage
    variable = nonlocal_damage_steel
    block = 'tube'
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
  []


  [div_pki_x_concrete]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_x
    tensor = pk_i_stress
    component = 0
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
  []
  [div_pki_y_concrete]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_y
    tensor = pk_i_stress
    component = 1
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
  []
  [div_pki_z_concrete]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = disp_z
    tensor = pk_i_stress
    component = 2
    save_in = force_z
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
  []
  [div_pki_couple_stress_x_concrete]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_concrete_x
    tensor = couple_pk_i_stress
    component = 0
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
  []
  [div_pki_couple_stress_y_concrete]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_concrete_y 
    tensor = couple_pk_i_stress
    component = 1
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
  []
  [div_pki_couple_stress_z_concrete]
    type = GradientEnhancedMicropolarPKIDivergence
    variable = microrot_concrete_z 
    tensor = couple_pk_i_stress
    component = 2
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
  []
  [mom_pki_couple_stress_x_concrete]
    type = GradientEnhancedMicropolarKirchhoffMoment
    variable = microrot_concrete_x
    tensor = kirchhoff_moment
    component = 0
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
  []
  [mom_pki_couple_stress_y_concrete]
    type = GradientEnhancedMicropolarKirchhoffMoment
    variable = microrot_concrete_y 
    tensor = kirchhoff_moment
    component = 1
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
  []
  [mom_pki_couple_stress_z_concrete]
    type = GradientEnhancedMicropolarKirchhoffMoment
    variable = microrot_concrete_z 
    tensor = kirchhoff_moment
    component = 2
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
  []
  [helmholtz_concrete]
    type = GradientEnhancedMicropolarDamage
    variable = nonlocal_damage_concrete
    block = 'concrete'
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
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
  [bot_react_z]
    type = NodalSum
    variable = force_z
    boundary = tube_top
  []
[]

[Materials]
  [marmot_material_tube]
    type = ComputeMarmotMaterialGradientEnhancedMicropolar
    micro_rotations = 'microrot_steel_x microrot_steel_y microrot_steel_z'
    nonlocal_damage = 'nonlocal_damage_steel'
    block = 'tube'
    marmot_material_name = GMDRUCKERPRAGER
                                  # E,          nu,     GcToG,      lb,     lt,     polarRatio,     sigmaYield,     
                                  # hLin,       hExp,   hDeltaExp,  phi(deg),       psi(deg)    
                                  # a1,         a2,     a3,         a4,     lJ2,           
                                  # epsF,       m,      maxDmg,     nonLocalRadius
    marmot_material_parameters = '  210000e-3   0.33    .1          .1      .2       1.4999999      250e-3
                                    0.2         10       380e-3      0.0     0 
                                    0.5         0.0     0.5         0.0     1e10
                                    1e-0        1.0     0.00        4.0'
  []
  [marmot_material_concrete]
    type = ComputeMarmotMaterialGradientEnhancedMicropolar
    micro_rotations = 'microrot_concrete_x microrot_concrete_y microrot_concrete_z'
    nonlocal_damage = 'nonlocal_damage_concrete'
    block = 'concrete'
    marmot_material_name = GMCDPFINITESTRAIN
        #E,         nu,         GcToG,      lbending,   lTorsion,   lJ2          polarRatio,     
        #fcu,       fcy,        ftu,        fbu,        Df,         epsF,        omegaMax,     lDamage,     m
        #Ah,        Bh,         Ch,         Dh,         As,     
        #a1,        a2,         a3,         a4,         
    marmot_material_parameters = '
        35000.0e-3  0.15        0.1         2.          3.99        4           1.499999 
        41.37e-3     15e-3     2.5e-3     46e-3       0.85        0.0050       .995           2           1.01 
        0.08        0.003       2.0         0.000001    0.75 
        0.5         0.0         0.5         0'
  []
[]

[BCs]
  #
  # SYMMETRY X
  #
  [sym_x]
    type = DirichletBC
    variable = disp_x
    boundary = xsym
    value = 0
  []
  [mr_y_sym_x]
    type = DirichletBC
    variable = microrot_steel_y
    boundary = xsym
    value = 0
  []
  [mr_z_sym_x]
    type = DirichletBC
    variable = microrot_steel_z
    boundary = xsym
    value = 0
  []
  [mr_concrete_y_sym_x]
    type = DirichletBC
    variable = microrot_concrete_y
    boundary = xsym
    value = 0
  []
  [mr_concrete_z_sym_x]
    type = DirichletBC
    variable = microrot_concrete_z
    boundary = xsym
    value = 0
  []
  #
  # SYMMETRY X
  #
  [sym_y]
    type = DirichletBC
    variable = disp_y
    boundary = ysym
    value = 0
  []
  [mr_x_sym_y]
    type = DirichletBC
    variable = microrot_steel_x
    boundary = ysym
    value = 0
  []
  [mr_z_sym_y]
    type = DirichletBC
    variable = microrot_steel_z
    boundary = ysym
    value = 0
  []
  [mr_concrete_x_sym_y]
    type = DirichletBC
    variable = microrot_concrete_x
    boundary = ysym
    value = 0
  []
  [mr_concrete_z_sym_y]
    type = DirichletBC
    variable = microrot_concrete_z
    boundary = ysym
    value = 0
  []
  #
  # SYMMETRY Z
  #
  [sym_z]
    type = DirichletBC
    variable = disp_z
    boundary = zsym
    value = 0
  []
  [mr_x_sym_z]
    type = DirichletBC
    variable = microrot_steel_x
    boundary = zsym
    value = 0
  []
  [mr_y_sym_z]
    type = DirichletBC
    variable = microrot_steel_y
    boundary = zsym
    value = 0
  []
  [mr_concrete_x_sym_z]
    type = DirichletBC
    variable = microrot_concrete_x
    boundary = zsym
    value = 0
  []
  [mr_concrete_y_sym_z]
    type = DirichletBC
    variable = microrot_concrete_y
    boundary = zsym
    value = 0
  []
  #
  # LOAD
  #
  [top_x]
    type = DirichletBC
    variable = disp_x
    boundary = tube_top_center
    value = 0
  []
  [top_y]
    type = DirichletBC
    variable = disp_y
    boundary = tube_top_center
    value = 0
  []
  [top_z]
    type = FunctionDirichletBC
    variable = disp_z
    boundary = 'tube_top concrete_top'
    preset = false
    function = '-22.225 * t'
  []
[]

[Preconditioning]
  active='smp'
  [smp]
    type = SMP
    full = true
  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = ' lu       strumpack'
#    petsc_options_iname = '     -pc_type
#                                -pc_hypre_type
#                                -ksp_type
#                                -ksp_gmres_restart
#                                -pc_hypre_boomeramg_relax_type_all
#                                -pc_hypre_boomeramg_strong_threshold
#                                -pc_hypre_boomeramg_agg_nl
#                                -pc_hypre_boomeramg_agg_num_paths
#                                -pc_hypre_boomeramg_max_levels
#                                -pc_hypre_boomeramg_coarsen_type
#                                -pc_hypre_boomeramg_interp_type
#                                -pc_hypre_boomeramg_P_max
#                                -pc_hypre_boomeramg_truncfactor' 
#
#    petsc_options_value = '     hypre
#                                boomeramg
#                                gmres
#                                301
#                                Chebyshev
#                                0.65
#                                5 
#                                2
#                                25
#                                HMIS
#                                ext+i
#                                4
#                                0.4 '
  []
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'

  nl_rel_tol = 1e-12
  nl_abs_tol = 1e-8
  l_tol = 1e-4
  l_max_its = 250
  nl_max_its = 20
  nl_div_tol = 1e2

  automatic_scaling=true
  compute_scaling_once =true
  verbose=false

  line_search = none

  dtmin = 1e-6
  dtmax= 2e-2
  
  start_time = 0.0
  end_time = 1.0 

  num_steps = 200

  [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 8
    iteration_window = 3
    linear_iteration_ratio = 1000
    growth_factor=1.2
    cutback_factor=0.8
    dt = 1.0e-4
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
