project(upenn.cbica.captk.brain.phiestimator)

mitk_create_plugin(
  #DEPENDS MitkCore
  EXPORT_DIRECTIVE EXAMPLE_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS PRIVATE MitkQtWidgetsExt MitkCaPTkPHIEstimator
  PACKAGE_DEPENDS PUBLIC ITK|ITKIOImageBase+ITKIONIFTI
  )
