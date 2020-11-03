[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
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

[Materials]
  [./bft_material]
    type = ComputeBftMaterialHypoElasticStress
    bft_material_name = LINEARELASTIC
    bft_material_parameters = '1000 0.25'
  []
  [./char_element_length]
    type = ComputeCharacteristicElementLength
  []
  [./dstrain]
    type = ComputeIncrementalSmallStrain
    displacements = 'disp_x disp_y'
  []
  [./dstrain_vgt_conv]
    type = ConvertRankTwoTensorToVoigt
    tensor = strain_increment
    tensor_voigt = strain_increment_voigt
    shear_components_twice = true
  [../]
  [./stress_conv]
    type = ConvertRankTwoTensorFromVoigt
    tensor = stress
    tensor_voigt = stress_voigt
    shear_components_half = false
  []
  [./Jacobian_conv]
    type = ConvertRankFourTensorFromVoigt
    tensor = Jacobian_mult
    tensor_voigt = Jacobian_mult_voigt 
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
    boundary = left
    value = 0
  []
  [right]
    type = DirichletBC
    variable = disp_x
    boundary = right
    value = 0
  []
  [right_y]
    type = DirichletBC
    variable = disp_y
    boundary = right
    value = 5
  []
[]

[Executioner]
  type = Steady
  solve_type = 'NEWTON'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]
