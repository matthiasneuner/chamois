[Mesh]
  type = FileMesh
  file = brokenshire_mesh.e
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
    boundary = clamp_b_load
  []
  [cmod_a_x]
    type = NodalMaxValue
    variable = disp_x
    boundary = cmod_a
  []
  [cmod_a_y]
    type = NodalMaxValue
    variable = disp_y
    boundary = cmod_a
  []
  [cmod_a_z]
    type = NodalMaxValue
    variable = disp_z
    boundary = cmod_a
  []
  [cmod_b_x]
    type = NodalMaxValue
    variable = disp_x
    boundary = cmod_b
  []
  [cmod_b_y]
    type = NodalMaxValue
    variable = disp_y
    boundary = cmod_b
  []
  [cmod_b_z]
    type = NodalMaxValue
    variable = disp_z
    boundary = cmod_b
  []
[]

[Materials]
  [marmot_material]
    type = ComputeMarmotMaterialGradientEnhancedMicropolar
    marmot_material_name = GMCDPFINITESTRAIN
        #E,         nu,         GcToG,      lbending,   lTorsion,   lJ2          polarRatio,     
        #fcu,       fcy,        ftu,        fbu,        Df,         epsF,        omegaMax,     lDamage,     m
        #Ah,        Bh,         Ch,         Dh,         As,     
        #a1,        a2,         a3,         a4,         
    marmot_material_parameters = '
        34900.0e-3  0.15        0.1         2.          3.99        4           1.499999 
        40.3e-3     40.3e-3     2.34e-3     46e-3       0.85        0.0050       .995           2           1.01 
        0.08        0.003       2.0         0.000001    0.75 
        0.5         0.0         0.5         0'
    block = 'concrete'
  []
  [marmot_material_clamps]
    type = ComputeMarmotMaterialGradientEnhancedMicropolar
    marmot_material_name = GMNEOHOOKE
                                  #E,       nu,    GcToG,   lb,     lt,     polarRatio
    marmot_material_parameters = '210000e-3    0.21   .5       10       20       1.4999999'
    block = 'clamp_a clamp_b'
  []
[]

[BCs]
  [clamp_a_supp_x]
    type = DirichletBC
    variable = disp_x
    boundary = clamp_a_supp
    value = 0
  []
  [clamp_a_supp_y]
    type = DirichletBC
    variable = disp_y
    boundary = clamp_a_supp
    value = 0
  []
  [clamp_a_supp_z]
    type = DirichletBC
    variable = disp_z
    boundary = clamp_a_supp
    value = 0
  []
  [clamp_b_supp_y]
    type = DirichletBC
    variable = disp_y
    boundary = clamp_b_supp
    value = 0
  []
  [clamp_b_supp_z]
    type = DirichletBC
    variable = disp_z
    boundary = clamp_b_supp
    value = 0
  []
  [clamp_a_load_x]
    type = DirichletBC
    variable = disp_x
    boundary = clamp_a_load
    value = 0
  []
  [clamp_a_load_y]
    type = DirichletBC
    variable = disp_y
    boundary = clamp_a_load
    value = 0
  []
  [clamp_a_load_z]
    type = DirichletBC
    variable = disp_z
    boundary = clamp_a_load
    value = 0
  []
  [clamp_b_load_y]
    type = FunctionDirichletBC
    variable = disp_y
    boundary = clamp_b_load
    preset = false
    function = '-10.0 * t'
  []
[]

[Preconditioning]
  active='smp'
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
                                Chebyshev
                                0.65
                                5 
                                2
                                25
                                HMIS
                                ext+i
                                4
                                0.4 '
  []

  [fsp]
      type = FSP
      solve_type = 'NEWTON'
      # It is the starting point of splitting
      topsplit = 'urn' # 'urn' should match the following block name
      [./urn]
          splitting = 'u r n' # 'u' and 'n' are the names of subsolvers
          splitting_type  = additive
          petsc_options_iname = '-ksp_type -ksp_gmres_restart '  
          petsc_options_value = 'gmres 301 '
      [../]
      [./u]
          vars = 'disp_x disp_y disp_z'
          # PETSc options for this subsolver
          # A prefix will be applied, so just put the options for this subsolver only
          petsc_options_iname = '   -pc_type -pc_hypre_type -ksp_type -pc_hypre_boomeramg_relax_type_all -pc_hypre_boomeramg_strong_threshold -pc_hypre_boomeramg_agg_nl -pc_hypre_boomeramg_agg_num_paths -pc_hypre_boomeramg_max_levels -pc_hypre_boomeramg_coarsen_type -pc_hypre_boomeramg_interp_type -pc_hypre_boomeramg_P_max -pc_hypre_boomeramg_truncfactor -pc_hypre_boomeramg_tol -ksp_max_it'  
          petsc_options_value = '   hypre boomeramg richardson Chebyshev 0.85 0 1 25 HMIS ext+i 5 0.4 0.0 1'
      [../]
      [./r]
          vars = 'microrot_x microrot_y microrot_z'
          # PETSc options for this subsolver
          petsc_options_iname = '   -pc_type -pc_hypre_type -ksp_type -pc_hypre_boomeramg_relax_type_all -pc_hypre_boomeramg_strong_threshold -pc_hypre_boomeramg_agg_nl -pc_hypre_boomeramg_agg_num_paths -pc_hypre_boomeramg_max_levels -pc_hypre_boomeramg_coarsen_type -pc_hypre_boomeramg_interp_type -pc_hypre_boomeramg_P_max -pc_hypre_boomeramg_truncfactor -pc_hypre_boomeramg_tol -ksp_max_it'  
          petsc_options_value = '   hypre boomeramg richardson Chebyshev 0.85 4 5 25 HMIS ext+i 5 0.4 0.0 1'
      [../]
      [./n]
          vars = 'nonlocal_damage'
          # PETSc options for this subsolver
          petsc_options_iname = '   -pc_type -pc_hypre_type -ksp_type -pc_hypre_boomeramg_relax_type_all -pc_hypre_boomeramg_strong_threshold -pc_hypre_boomeramg_agg_nl -pc_hypre_boomeramg_agg_num_paths -pc_hypre_boomeramg_max_levels -pc_hypre_boomeramg_coarsen_type -pc_hypre_boomeramg_interp_type -pc_hypre_boomeramg_P_max -pc_hypre_boomeramg_truncfactor -pc_hypre_boomeramg_tol -ksp_max_it'  
          petsc_options_value = '   hypre boomeramg richardson Chebyshev 0.65 4 5 25 HMIS ext+i 3 0.4 0.0 1'
      [../]
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'NEWTON'

  nl_rel_tol = 1e-5
  nl_abs_tol = 1e-8
  l_tol = 1e-4
  l_max_its = 200
  nl_max_its = 20
  nl_div_tol = 1e2

  automatic_scaling=true
  compute_scaling_once =true
  verbose=false

  line_search = none

  dtmin = 1e-6
  dtmax= 5e-3
  
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
    dt = 2.5e-3
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
