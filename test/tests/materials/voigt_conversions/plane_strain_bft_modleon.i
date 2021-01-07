[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 5
  ny = 5
  nz = 0
  xmin = 0
  xmax = 50
  ymin = 0
  ymax = 50
  zmin = 0
  zmax = 0
  elem_type = QUAD4
[]

[GlobalParams]
  displacements = 'disp_x disp_y'
[]

[Variables]
  [disp_x]
  []
  [disp_y]
  []
[]

[Kernels]
  [div_sig_x]
    type = StressDivergenceTensors
    variable = disp_x
    component = 0
  []
  [div_sig_y]
    type = StressDivergenceTensors
    variable = disp_y
    component = 1
  []
[]

[AuxVariables]
  [the_characteristic_element_length]
    order=CONSTANT
    family=MONOMIAL
  []
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
  [the_characteristic_element_length_kernel]
    type = MaterialRealAux
    variable = the_characteristic_element_length
    property = characteristic_element_length
    execute_on = TIMESTEP_END
  []
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
[]

[Materials]
  [marmot_material]
    type = ComputeMarmotMaterialHypoElastic
    marmot_material_name = MODLEON
    marmot_material_parameters = ' 30000.0	 0.15 	13 	 47.4 	  55   	 4.74 	 0.85 	 0.12 	 0.003	 2.0	 0.000001	 15.0	 0.10	 1'
  []
  [char_element_length]
    type = ComputeCharacteristicElementLength
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
    tensor_voigt = dstress_voigt_dstrain_voigt
    shear_components_half_ij = false
    shear_components_half_kl = false
    tensor_voigt_uses_row_major_layout = false
  []
[]

[BCs]
  [left_x]
    type = DirichletBC
    variable = disp_x
    boundary = left
    value = 0
  []
  [left_y]
    type = DirichletBC
    variable = disp_y
    boundary = bottom
    value = 0
  []
  [right]
    type = FunctionDirichletBC
    variable = disp_x
    boundary = right
    function = '-0.5 * t'
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

  petsc_options_iname = '-pc_type   -pc_hypre_type    -ksp_type     -ksp_gmres_restart  -pc_hypre_boomeramg_strong_threshold'
  petsc_options_value = 'hypre      boomeramg         gmres         301                 0.25'
  #petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  #petsc_options_value = ' lu       mumps'

  nl_rel_tol = 1e-10
  nl_abs_tol = 1e-10
  l_tol = 1e-10
  l_max_its = 30
  nl_max_its = 20

  line_search = 'none'

  dtmin = 1e-3
  dtmax= 5e-2
  
  start_time = 0.0
  end_time = 1.0 

  num_steps = 100
  [TimeStepper]
    type = IterationAdaptiveDT
    optimal_iterations = 8
    iteration_window = 1
    dt = 1e-3
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
