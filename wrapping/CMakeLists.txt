itk_wrap_module(VariationalRegistration)

set(WRAPPER_SUBMODULE_ORDER
   # Base regularizer (1-param: TDisplacementField)
   itkVariationalRegistrationRegularizer
   # Derived regularizers (1-param: TDisplacementField)
   # (CurvatureRegularizer is auto-loaded when FFTW is available; see its .wrap)
   itkVariationalRegistrationDiffusionRegularizer
   itkVariationalRegistrationElasticRegularizer
   itkVariationalRegistrationGaussianRegularizer
   # 3-parameter filters (TFixedImage, TMovingImage, TDisplacementField)
   itkContinuousBorderWarpImageFilter
   itkVariationalRegistrationFunction
   itkVariationalRegistrationDemonsFunction
   itkVariationalRegistrationFastNCCFunction
   itkVariationalRegistrationNCCFunction
   itkVariationalRegistrationSSDFunction
   itkVariationalRegistrationFilter
   itkVariationalDiffeomorphicRegistrationFilter
   itkVariationalSymmetricDiffeomorphicRegistrationFilter
   # Multi-resolution (3-param + TRealType=float default)
   itkVariationalRegistrationMultiResolutionFilter
   # StopCriterion (depends on Filter + MultiResolutionFilter types)
   itkVariationalRegistrationStopCriterion
   )

itk_auto_load_submodules()
itk_end_wrap_module()
