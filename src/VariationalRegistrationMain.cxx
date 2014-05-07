/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

//
// Add -DUSE_2D_IMPL as COMPILE_FLAG to generate 2D version.
//
#ifdef USE_2D_IMPL
#define DIMENSION 2
#else
#define DIMENSION 3
#endif

// System includes:
#include <iostream>
#include <string>
extern "C"
{
#include "getopt.h"
}

#define ExceptionMacro(x) std::cerr << "ERROR: " x << std::endl;

// Project includes:
#include "itkConfigure.h"

#include "itkExponentialDisplacementFieldImageFilter.h"

#include "itkMultiResolutionVariationalRegistration.h"
#include "itkVariationalRegistrationFilter.h"
#include "itkVariationalDiffeomorphicRegistrationFilter.h"
#include "itkVariationalSymmetricDiffeomorphicRegistrationFilter.h"

#include "itkVariationalRegistrationFunction.h"
#include "itkVariationalRegistrationDemonsFunction.h"
#include "itkVariationalRegistrationSSDFunction.h"
#include "itkVariationalRegistrationNCCFunction.h"

#include "itkVariationalRegistrationRegularizer.h"
#include "itkVariationalRegistrationGaussianRegularizer.h"
#include "itkVariationalRegistrationDiffusionRegularizer.h"
#include "itkVariationalRegistrationElasticRegularizer.h"

#include "itkVariationalRegistrationStopCriterion.h"
#include "itkVariationalRegistrationLogger.h"

#include "itkVariationalRegistrationNCCFunction.h"
//#include "itkContinuousBorderWarpImageFilter.h"

// ITK library includes
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkHistogramMatchingImageFilter.h"

#include <vnl/vnl_math.h>

using namespace itk;

void PrintHelp()
{
  std::cout << std::endl;
  std::cout << "Usage:" << std::endl;
  std::cout << "itkVariationalRegistration -F <fixed image> -M <moving image> "
  "-D <output displacement field> [-S <segmentation mask>] [-W <warped image>] [-G <grid image>]"
  "..." << std::endl << std::endl;
  std::cout << "This is a " << DIMENSION<<"D - implementation." << std::endl;
  std::cout << "Input:" << std::endl;
  std::cout << "-F <fixed image>         Filename of the fixed image." << std::endl;
  std::cout << "-M <moving image>        Filename of the moving image." << std::endl;
  std::cout << "-S <segmentation mask>   Filename of the mask image for the registration." << std::endl;
  std::cout << "-I <initial field>       Filename of the initial deformation field." << std::endl;
  std::cout << std::endl;
  std::cout << "Output:" << std::endl;
  std::cout << "-O <output def. field>   Filename of the output deformation field." << std::endl;
  std::cout << "-W <warped image>        Filename of the output warped image." << std::endl;
  std::cout << "-L <log file>            Filename of the log file of the registration (NYI)." << std::endl;
  std::cout << std::endl;
  std::cout << "Parameters for registration filter:" << std::endl;
  std::cout << "-i <iterations>          Number of iterations." << std::endl;
  std::cout << "-l <levels>              Number of multi-resolution levels." << std::endl;
  std::cout << "-t <tau>                 Registration time step." << std::endl;
  std::cout << "-s 0|1|2                 Select search space." << std::endl;
  std::cout << "                           0: Standard (default)." << std::endl;
  std::cout << "                           1: Diffeomorphic." << std::endl;
  std::cout << "                           2: Symmetric diffeomorphic (NYI)." << std::endl;
  std::cout << "-u 0|1                   Use spacing for regularization." << std::endl;
  std::cout << "                           0: false" << std::endl;
  std::cout << "                           1: true (default)" << std::endl;
  std::cout << std::endl;
  std::cout << "Parameters for regularizer:" << std::endl;
  std::cout << "-r 0|1|2                 Select regularizer." << std::endl;
  std::cout << "                           0: Gaussian smoother." << std::endl;
  std::cout << "                           1: Diffusive regularizer (default)." << std::endl;
  std::cout << "                           2: Elastic regularizer." << std::endl;
  std::cout << "-a <alpha>               Alpha for the regularization (only diffusive)." << std::endl;
  std::cout << "-v <variance>            Variance for the regularization (only gaussian)." << std::endl;
  std::cout << "-m <mu>                  Mu for the regularization (only elastic)." << std::endl;
  std::cout << "-b <lambda>              Lambda for the regularization (only elasic)." << std::endl;
  std::cout << std::endl;
  std::cout << "Parameters for registration function:" << std::endl;
  std::cout << "-f 0|1|2                 Select force term." << std::endl;
  std::cout << "                           0: Demon forces (default)." << std::endl;
  std::cout << "                           1: Sum of Squared Differences." << std::endl;
  std::cout << "                           2: Cross Correlation." << std::endl;
  std::cout << "                           3: Normalized Mutual Information (NYI)." << std::endl;
  std::cout << "-q <radius>              Radius of neighborhood size for Cross Correlation." << std::endl;
  std::cout << "-d 0|1|2                 Select image domain for force calculation." << std::endl;
  std::cout << "                           0: Warped image forces (default)." << std::endl;
  std::cout << "                           1: Fixed image forces." << std::endl;
  std::cout << "                           2: Symmetric forces." << std::endl;
  std::cout << std::endl;
  std::cout << "Parameters for stop criterion:" << std::endl;
  std::cout << "-p 0|1|2                 Select stop criterion policy for multi-resolution." << std::endl;
  std::cout << "                           0: Use default stop criterion." << std::endl;
  std::cout << "                           1: Use simple graduated policy (default)." << std::endl;
  std::cout << "                           2: Use graduated policy." << std::endl;
  std::cout << "-g <grad slope>          Set fitted line slope for stop criterion (default 0.005)." << std::endl;
  std::cout << std::endl;
  std::cout << "Preprocessing and general parameters:" << std::endl;
  std::cout << "-h 0|1                   Perform histogram matching." << std::endl;
  std::cout << "                           0: false (default)" << std::endl;
  std::cout << "                           1: true" << std::endl;
//  std::cout << "-c <low-thr> <up-thr>    Crop intensity values to interval [low-thr, up-thr]." << std::endl;
  std::cout << "-3                       Write 2D displacements as 3D displacements (with zero z-component)." << std::endl;
  std::cout << "-?                       Print this help." << std::endl;
  std::cout << "" << std::endl;
}


int main( int argc, char *argv[] )
{
  if( argc < 2 )
    {
    PrintHelp();
    return 1;
    }

  std::cout << "==========================================" << std::endl;
  std::cout << "====  itkVariationalRegistration (" << DIMENSION<<"D) ====" << std::endl;
  std::cout << "==========================================" << std::endl;
  std::cout << "READING parameters...\n" << std::endl;

  // Initialize parameters with default values
  int c;
  int intVal = 0;

  // Filenames
  char* fixedImageFilename = NULL;
  char* movingImageFilename = NULL;
  char* maskImageFilename = NULL;
  char* outputDeformationFilename = NULL;
  char* outputVelocityFilename = NULL;
  char* warpedImageFilename = NULL;
  char* initialFieldFilename = NULL;
  char* gridImageFilename = NULL;
  char* logFilename = NULL;

  // Registration parameters
  int numberOfIterations = 400;
  int numberOfLevels = 3;
  double timestep = 1.0;
  int searchSpace = 0;            // Standard
  bool useImageSpacing = true;

  // Regularizer parameters
  int regularizerType = 1;       // Diffusive
  float regulAlpha = 0.5;
  float regulVar = 0.5;
  float regulMu = 0.5;
  float regulLambda = 0.5;

  int nccRadius = 2;

  // Force parameters
  int forceType = 0;              // Demon
  int forceDomain = 0;            // Warped moving

  // Stop criterion parameters
  int stopCriterionPolicy = 1; // Simple graduated is default
  float stopCriterionSlope = 0.005;

  // Preproc and general parameters
  bool useHistogramMatching = false;
  bool useDebugMode = false;

  bool bWrite3DDisplacementField = false;

  // Reading parameters
  while( (c = getopt( argc, argv, "Z:F:R:M:T:I:S:O:W:G:L:i:n:l:t:s:u:r:a:v:m:b:f:d:w:p:g:h:q:x?3" )) != -1 )
  {
    switch ( c )
    {
    case 'F':
    case 'R':
      fixedImageFilename = optarg;
      std::cout << "  Fixed image filename:            " << fixedImageFilename << std::endl;
      break;
    case 'M':
    case 'T':
      movingImageFilename = optarg;
      std::cout << "  Moving image filename:           " << movingImageFilename << std::endl;
      break;
    case 'I':
      initialFieldFilename = optarg;
      std::cout << "  Initial deform. field filename:  " << initialFieldFilename << std::endl;
      break;
    case 'S':
      maskImageFilename = optarg;
      std::cout << "  Mask image filename:             " << maskImageFilename << std::endl;
      break;
    case 'O':
      outputDeformationFilename = optarg;
      std::cout << "  Output deform. field filename:   " << outputDeformationFilename << std::endl;
      break;
    case 'Z':
      outputVelocityFilename = optarg;
      std::cout << "  Output velocity field filename:  " << outputVelocityFilename << std::endl;
      break;
    case 'W':
      warpedImageFilename = optarg;
      std::cout << "  Warped image filename:           " << warpedImageFilename << std::endl;
      break;
    case 'G':
      gridImageFilename = optarg;
      std::cout << "  Grid image filename:             " << gridImageFilename << std::endl;
      break;
    case 'L':
      logFilename = optarg;
      std::cout << "  Log filename:                    " << logFilename << std::endl;
      break;
    case 'i':
    case 'n':
      numberOfIterations = atoi( optarg );
      std::cout << "  No. of iterations:               " << numberOfIterations << std::endl;
      break;
    case 'l':
      numberOfLevels = atoi( optarg );
      std::cout << "  No. of multi-resolution levels:  " << numberOfLevels << std::endl;
      break;
    case 't':
      timestep = atof( optarg );
      std::cout << "  Registration time step:          " << timestep << std::endl;
      break;
    case 's':
      searchSpace = atoi( optarg );
      if( searchSpace == 0 )
      {
        std::cout << "  Search space:                    Standard" << std::endl;
      }
      else if( searchSpace == 1 )
      {
        std::cout << "  Search space:                    Diffeomorphic" << std::endl;
      }
      else if( searchSpace == 2 )
      {
        std::cout << "  Search space:                    Symmetric Diffeomorphic" << std::endl;
      }
      else
      {
        std::cout << "  ERROR: Search space unknown!" << std::endl;
        return 0;
      }
      break;
    case 'u':
      intVal = atoi( optarg );
      if( intVal == 0 )
      {
        std::cout << "  Use image spacing:               false" << std::endl;
        useImageSpacing = false;
      }
      else
      {
        std::cout << "  Use image spacing:               true" << std::endl;
        useImageSpacing = true;
      }
      break;
    case 'r':
      regularizerType = atoi( optarg );
      if( regularizerType == 0 )
      {
        std::cout << "  Regularizer:                     Gaussian" << std::endl;
      }
      else if( regularizerType == 1 )
      {
        std::cout << "  Regularizer:                     Diffusive" << std::endl;
      }
      else if( regularizerType == 2 )
      {
        std::cout << "  Regularizer:                     Elastic" << std::endl;
      }
      else
      {
        std::cout << "  ERROR: Regularizer space unknown!" << std::endl;
        return 0;
      }
      break;
    case 'a':
      regulAlpha = atof( optarg );
      std::cout << "  Regularization alpha:            " << regulAlpha << std::endl;
      break;
    case 'v':
      regulVar = atof( optarg );
      std::cout << "  Regularization variance:         " << regulVar << std::endl;
      break;
    case 'm':
      regulMu = atof( optarg );
      std::cout << "  Regularization mu:               " << regulMu << std::endl;
      break;
    case 'b':
      regulLambda = atof( optarg );
      std::cout << "  Regularization lambda:           " << regulLambda << std::endl;
      break;
    case 'f':
      forceType = atoi( optarg );
      if( forceType == 0 )
      {
        std::cout << "  Force type:                      Demons" << std::endl;
      }
      else if ( forceType == 1 )
      {
        std::cout << "  Force type:                      SSD" << std::endl;
      }
      else if( forceType == 2)
      {
        std::cout << "  Force type:                      NCC" << std::endl;
      }
      else
      {
        std::cout << "  ERROR: Force type unknown!" << std::endl;
        return 0;
      }
      break;
    case 'q':
      nccRadius = atoi( optarg );
      std::cout << "  Radius size for NCC:             " << nccRadius << std::endl;
      break;
    case 'd':
      forceDomain = atoi( optarg );
      if( forceDomain == 0 )
        {
        std::cout << "  Force domain:                    Warped moving image" << std::endl;
        }
      else if( forceDomain == 1 )
      {
        std::cout << "  Force domain:                    Fixed image" << std::endl;
      }
      else if( forceDomain == 2 )
      {
        std::cout << "  Calc. forces in:                 Symmetric" << std::endl;
      }
      else
      {
        std::cout << "  ERROR: Force domain unknown!" << std::endl;
        return 0;
      }
      break;
    case 'p':
      stopCriterionPolicy = atoi( optarg );
      if( stopCriterionPolicy == 0 )
      {
        std::cout << "  StopCriterion-Policy:            Default stop criterion on all levels." << std::endl;
      }
      else if( stopCriterionPolicy == 1 )
      {
        std::cout << "  StopCriterion-Policy:            Simple graduated (- increase count on coarse levels," << std::endl;
        std::cout << "                                                     - plus line fitting on finest level)." << std::endl;
      }
      else if( stopCriterionPolicy == 2 )
      {
        std::cout << "  StopCriterion-Policy:            Graduated (- max iterations on coarse levels," << std::endl;
        std::cout << "                                              - increase count on second finest level," << std::endl;
        std::cout << "                                              - plus line fitting on finest level)." << std::endl;
      }
      break;
    case 'g':
      stopCriterionSlope = atof( optarg );
      std::cout << "  StopCrit. Grad. Threshold:       " << stopCriterionSlope << std::endl;
      break;
    case 'h':
      intVal = atoi( optarg );
      if( intVal == 0 )
      {
        std::cout << "  Use histogram matching:          false" << std::endl;
        useHistogramMatching = false;
      }
      else
      {
        std::cout << "  Use histogram matching:          true" << std::endl;
        useHistogramMatching = true;
      }
      break;
    case 'x':
      std::cout << "  Use debug mode:                  true" << std::endl;
      useDebugMode = true;
      break;
      case '3':
#ifdef USE_2D_IMPL
        std::cout << "  Write 3D displacement field:     true" << std::endl;
        bWrite3DDisplacementField = true;
#else
        std::cout << "  Write 3D displacement field:  meaningless for 3D." << std::endl;
#endif
        break;
    case '?':
      PrintHelp();
      exit(0);
    default:
      ExceptionMacro( << "Argument " << (char) c << " not processed" );
      break;
    }
  }

  std::cout << "==========================================" << std::endl;
  std::cout << "INITIALIZING data and filter..." << std::endl;
  //////////////////////////////////////////////
  //
  // check valid arguments.
  //
  //////////////////////////////////////////////
  if( fixedImageFilename == NULL || movingImageFilename == NULL )
    {
    ExceptionMacro( << "No input fixed and/or moving image given!" );
    return -1;
    }
  if( outputDeformationFilename == NULL && warpedImageFilename == NULL )
    {
    ExceptionMacro( << "No output (deformation field or warped image) given!" );
    return -1;
    }

  //////////////////////////////////////////////
  //
  // image variable
  //
  //////////////////////////////////////////////
  typedef Image<Vector<float, DIMENSION>, DIMENSION> DisplacementFieldType;
  typedef DisplacementFieldType::Pointer             DisplacementFieldPointerType;
  typedef ImageFileReader<DisplacementFieldType>     DisplacementFieldReaderType;
  typedef ImageFileWriter<DisplacementFieldType>     DisplacementFieldWriterType;

  typedef Image<short, DIMENSION>                    ImageType;
  typedef ImageType::Pointer                         ImagePointerType;
  typedef ImageFileReader<ImageType>                 ImageReaderType;
  typedef ImageFileWriter<ImageType>                 ImageWriterType;

  typedef VariationalRegistrationDemonsFunction<ImageType,ImageType,DisplacementFieldType>::MaskImageType
                                                     MaskType;
  typedef MaskType::Pointer                          MaskPointerType;
  typedef ImageFileReader<MaskType>                  MaskReaderType;

  ImagePointerType fixedImage;
  ImagePointerType movingImage;
  MaskPointerType maskImage;
  DisplacementFieldPointerType initialDisplacementField;

  ImagePointerType warpedOutputImage;
  DisplacementFieldPointerType outputDisplacementField;
  DisplacementFieldPointerType outputVelocityField;

  //////////////////////////////////////////////
  //
  // Load input images
  //
  //////////////////////////////////////////////
  std::cout << "Loading fixed image ... " << std::endl;
  ImageReaderType::Pointer fixedImageReader;
  fixedImageReader = ImageReaderType::New();

  fixedImageReader->SetFileName( fixedImageFilename );
  fixedImageReader->Update();
  fixedImage = fixedImageReader->GetOutput();

  std::cout << "Loading moving image ... " << std::endl;
  ImageReaderType::Pointer movingImageReader;
  movingImageReader = ImageReaderType::New();

  movingImageReader->SetFileName( movingImageFilename );
  movingImageReader->Update();

  movingImage = movingImageReader->GetOutput();

  if(fixedImage.IsNull() || movingImage.IsNull())
    {
    ExceptionMacro( << "Fixed or moving image data is null" );
    return -1;
    }

  if( maskImageFilename != NULL )
    {
    std::cout << "Loading mask image ... " << std::endl;
    MaskReaderType::Pointer maskReader;
    maskReader = MaskReaderType::New();

    maskReader->SetFileName( maskImageFilename );
    maskReader->Update();

    maskImage = maskReader->GetOutput();

    if(maskImage.IsNull())
      {
      ExceptionMacro( << "Mask image data is null" );
      return -1;
      }
    }
  if( initialFieldFilename != NULL )
    {
    std::cout << "Loading initial deformation field..."  << std::endl;
    DisplacementFieldReaderType::Pointer DisplacementFieldReader;
    DisplacementFieldReader = DisplacementFieldReaderType::New();

    DisplacementFieldReader->SetFileName( initialFieldFilename );
    DisplacementFieldReader->Update();

    initialDisplacementField = DisplacementFieldReader->GetOutput();
    if(initialDisplacementField.IsNull())
      {
      ExceptionMacro( << "Initial deformation field is null" );
      return -1;
      }
    }

  //////////////////////////////////////////////
  //
  // Preprocess input images
  //
  //////////////////////////////////////////////

  //
  // Histogram matching
  //
  if( useHistogramMatching )
    {
    std::cout << "Performing histogram matching of moving image..." << std::endl;
    typedef HistogramMatchingImageFilter<ImageType, ImageType> MatchingFilterType;
    MatchingFilterType::Pointer matcher;

    matcher = MatchingFilterType::New();

    matcher->SetInput( movingImage );
    matcher->SetReferenceImage( fixedImage );
    matcher->SetNumberOfHistogramLevels( 1024 );
    matcher->SetNumberOfMatchPoints( 7 );
    matcher->ThresholdAtMeanIntensityOn();

    try
      {
      matcher->Update();
      }
    catch( itk::ExceptionObject& err )
      {
      ExceptionMacro( << "Could not match input images!" );
      return -1;
      }

    movingImage = matcher->GetOutput();
    }

  //////////////////////////////////////////////
  //
  // Initialize registration filter
  //
  //////////////////////////////////////////////

  //
  // Setup registration function
  //

  typedef VariationalRegistrationFunction<
      ImageType,ImageType,DisplacementFieldType>   FunctionType;
  typedef VariationalRegistrationDemonsFunction<
      ImageType, ImageType, DisplacementFieldType> DemonsFunctionType;
  typedef VariationalRegistrationSSDFunction<
      ImageType, ImageType, DisplacementFieldType> SSDFunctionType;
  typedef VariationalRegistrationNCCFunction<
      ImageType, ImageType, DisplacementFieldType> NCCFunctionType;

  FunctionType::Pointer function;
  switch( forceType )
  {
  case 0:
  {
    DemonsFunctionType::Pointer demonsFunction = DemonsFunctionType::New();
    switch( forceDomain )
    {
    case 0:
      demonsFunction->SetGradientTypeToWarpedMovingImage();
      break;
    case 1:
      demonsFunction->SetGradientTypeToFixedImage();
      break;
    case 2:
      demonsFunction->SetGradientTypeToSymmetric();
      break;
    }

    function = demonsFunction;
  }
  break;
  case 1:
  {
    SSDFunctionType::Pointer ssdFunction = SSDFunctionType::New();
    switch( forceDomain )
    {
    case 0:
      ssdFunction->SetGradientTypeToWarpedMovingImage();
      break;
    case 1:
      ssdFunction->SetGradientTypeToFixedImage();
      break;
    case 2:
      ssdFunction->SetGradientTypeToSymmetric();
      break;
    }

	function = ssdFunction;
  }
  break;
  case 2:
  {
    NCCFunctionType::Pointer nccFunction = NCCFunctionType::New();
    NCCFunctionType::RadiusType r;
    for( unsigned int dim = 0; dim < NCCFunctionType::ImageDimension; dim++ )
      {
      r[dim] = nccRadius;
      }
    nccFunction->SetRadius( r );

    switch( forceDomain )
    {
    case 0:
    	nccFunction->SetGradientTypeToWarpedMovingImage();
      break;
    case 1:
    	nccFunction->SetGradientTypeToFixedImage();
      break;
    case 2:
    	nccFunction->SetGradientTypeToSymmetric();
      break;
    }
    function = nccFunction;
  }
  break;
  }
  //function->SetMovingImageWarper( warper );
  function->SetTimeStep( timestep );


  //
  // Setup regularizer
  //
  typedef VariationalRegistrationRegularizer<DisplacementFieldType>          RegularizerType;
  typedef VariationalRegistrationGaussianRegularizer<DisplacementFieldType>  GaussianRegularizerType;
  typedef VariationalRegistrationDiffusionRegularizer<DisplacementFieldType> DiffusionRegularizerType;
  typedef VariationalRegistrationElasticRegularizer<DisplacementFieldType>   ElasticRegularizerType;

  RegularizerType::Pointer regularizer;
  switch( regularizerType )
  {
  case 0:
    {
    GaussianRegularizerType::Pointer gaussRegularizer = GaussianRegularizerType::New();
    gaussRegularizer->SetStandardDeviations( vcl_sqrt( regulVar ) );
    regularizer = gaussRegularizer;
    }
    break;
  case 1:
    {
    DiffusionRegularizerType::Pointer diffRegularizer = DiffusionRegularizerType::New();
    diffRegularizer->SetAlpha( regulAlpha );
    regularizer = diffRegularizer;
    }
    break;
  case 2:
    {
    ElasticRegularizerType::Pointer elasticRegularizer = ElasticRegularizerType::New();
    elasticRegularizer->SetMu( regulMu );
    elasticRegularizer->SetLambda( regulLambda );
    regularizer = elasticRegularizer;
    }
    break;
  }
  regularizer->InPlaceOff();
  regularizer->SetUseImageSpacing( useImageSpacing );

  //
  // Setup registration filter
  //
  typedef VariationalRegistrationFilter<
      ImageType,ImageType,DisplacementFieldType> RegistrationFilterType;
  typedef VariationalDiffeomorphicRegistrationFilter<
      ImageType,ImageType,DisplacementFieldType> DiffeomorphicRegistrationFilterType;
  typedef VariationalSymmetricDiffeomorphicRegistrationFilter<
      ImageType,ImageType,DisplacementFieldType> SymmetricDiffeomorphicRegistrationFilterType;

  RegistrationFilterType::Pointer regFilter;
  switch( searchSpace )
  {
  case 0:
    regFilter = RegistrationFilterType::New();
    break;
  case 1:
    regFilter = DiffeomorphicRegistrationFilterType::New();
    break;
  case 2:
    regFilter = SymmetricDiffeomorphicRegistrationFilterType::New();
    break;
  }
  regFilter->SetRegularizer( regularizer );
  regFilter->SetDifferenceFunction( function );

  //
  // Setup multi-resolution filter
  //
  unsigned int its[numberOfLevels];
  its[numberOfLevels - 1] = numberOfIterations;
  for( int level = numberOfLevels - 2; level >= 0; --level )
    {
    its[level] = its[level + 1];
    }

  typedef MultiResolutionVariationalRegistration<ImageType,ImageType,DisplacementFieldType> MRRegistrationFilterType;

  MRRegistrationFilterType::Pointer mrRegFilter = MRRegistrationFilterType::New();
  mrRegFilter->SetRegistrationFilter( regFilter );
  mrRegFilter->SetMovingImage( movingImage );
  mrRegFilter->SetFixedImage( fixedImage );
  mrRegFilter->SetMaskImage( maskImage );
  mrRegFilter->SetNumberOfLevels( numberOfLevels );
  mrRegFilter->SetNumberOfIterations( its );
  //mrRegFilter->SetInitialDisplacementField( initialDisplacementField );

  //
  // Setup stop criterion
  //
  typedef VariationalRegistrationStopCriterion<
      RegistrationFilterType,MRRegistrationFilterType> StopCriterionType;
  StopCriterionType::Pointer stopCriterion = StopCriterionType::New();
  stopCriterion->SetRegressionLineSlopeThreshold( stopCriterionSlope );
  stopCriterion->PerformLineFittingMaxDistanceCheckOn();

  switch( stopCriterionPolicy )
  {
    case 1:
      stopCriterion->SetMultiResolutionPolicyToSimpleGraduated();
      break;
    case 2:
      stopCriterion->SetMultiResolutionPolicyToGraduated();
      break;
    default:
      stopCriterion->SetMultiResolutionPolicyToDefault();
      break;
  }

  regFilter->AddObserver( itk::IterationEvent(), stopCriterion );
  mrRegFilter->AddObserver( itk::IterationEvent(), stopCriterion );
  mrRegFilter->AddObserver( itk::InitializeEvent(), stopCriterion );

  //
  // Setup logger
  //
  typedef VariationalRegistrationLogger<
      RegistrationFilterType,MRRegistrationFilterType> LoggerType;
  LoggerType::Pointer logger = LoggerType::New();

  regFilter->AddObserver( itk::IterationEvent(), logger );
  mrRegFilter->AddObserver( itk::IterationEvent(), logger );

  if( useDebugMode )
    {
    regularizer->DebugOn();
    regFilter->DebugOn();
    mrRegFilter->DebugOn();
    stopCriterion->DebugOn();
    logger->DebugOn();
    }

  //
  // Execute registration
  //
  std::cout << "Starting registration..." << std::endl;

  mrRegFilter->Update();

  std::cout << "Registration execution finished." << std::endl;

  // Get output image and deformation field
  if( searchSpace == 0 )
  {
    outputDisplacementField = mrRegFilter->GetOutput();
  }
  else if( searchSpace == 1 || searchSpace == 2 )
    // TODO: Viel zu kompliziert. Eigentlich: GetDisplacementField fuer DisplacementField
    // GetVeloField fuer Velofield. ABER: Geht natuerlich nur fuer Regfilter ...
  {
    outputVelocityField = mrRegFilter->GetOutput();

    // Run a final exponentiation to obtain deformation field:
    // Todo Define param for exponentiation accuracy.

#if ITK_VERSION_MAJOR > 3
    typedef itk::ExponentialDisplacementFieldImageFilter< DisplacementFieldType, DisplacementFieldType> FieldExponentiatorType;
#else
    typedef itk::ExponentialDisplacementFieldImageFilter< DisplacementFieldType, DisplacementFieldType> FieldExponentiatorType;
#endif

    typedef FieldExponentiatorType::Pointer FieldExponentiatorPointer;
    FieldExponentiatorPointer exponentiator = FieldExponentiatorType::New();

    exponentiator->SetInput( outputVelocityField );
    exponentiator->AutomaticNumberOfIterationsOff();
    exponentiator->SetMaximumNumberOfIterations( 2 );
    exponentiator->Update();

    outputDisplacementField = exponentiator->GetOutput();
  }

  //////////////////////////////////////////////
  //
  // Write results
  //
  //////////////////////////////////////////////
  std::cout << "==========================================" << std::endl;
  std::cout << "WRITING output data..." << std::endl;

  if( outputDeformationFilename != NULL && outputDisplacementField.IsNotNull() )
    {
    if( DIMENSION == 2 && bWrite3DDisplacementField )
    {
      std::cout << "Converting deformation field to 3D..." << std::endl;
      typedef Image<Vector<float, 3> , 3> OutDisplacementFieldType;
      typedef OutDisplacementFieldType::Pointer OutDisplacementFieldPointerType;
      typedef ImageFileWriter<OutDisplacementFieldType> OutDisplacementFieldWriterType;

      OutDisplacementFieldPointerType writeField = OutDisplacementFieldType::New();

      DisplacementFieldType::SizeType oldSize = outputDisplacementField->GetLargestPossibleRegion().GetSize();
      OutDisplacementFieldType::SizeType newSize;

      newSize[0] = oldSize[0];
      newSize[1] = oldSize[1];
      newSize[2] = 1;

      writeField->SetRegions( newSize );

      DisplacementFieldType::SpacingType oldSpacing = outputDisplacementField->GetSpacing();
      OutDisplacementFieldType::SpacingType newSpacing;

      newSpacing[0] = oldSpacing[0];
      newSpacing[1] = oldSpacing[1];
      newSpacing[2] = 1;

      writeField->SetSpacing( newSpacing );

      writeField->Allocate();

      ImageRegionIterator<DisplacementFieldType> defIterator(
          outputDisplacementField, outputDisplacementField->GetRequestedRegion() );

      while( ! defIterator.IsAtEnd() )
      {
        DisplacementFieldType::IndexType oldIndex = defIterator.GetIndex();
        OutDisplacementFieldType::IndexType newIndex;

        newIndex[0] = oldIndex[0];
        newIndex[1] = oldIndex[1];
        newIndex[2] = 0;

        DisplacementFieldType::PixelType oldValue = defIterator.Value();
        OutDisplacementFieldType::PixelType newValue;

        newValue[0] = oldValue[0];
        newValue[1] = oldValue[1];
        newValue[2] = 0.0;

        writeField->SetPixel( newIndex, newValue );

        ++defIterator;
      }

      std::cout << "Saving deformation field..." << std::endl;
      OutDisplacementFieldWriterType::Pointer DisplacementFieldWriter;
      DisplacementFieldWriter = OutDisplacementFieldWriterType::New();

      DisplacementFieldWriter->SetInput( writeField );
      DisplacementFieldWriter->SetFileName( outputDeformationFilename );
      DisplacementFieldWriter->Update();
      }
    else
      {
      std::cout << "Saving deformation field..." << std::endl;
      DisplacementFieldWriterType::Pointer DisplacementFieldWriter;
      DisplacementFieldWriter = DisplacementFieldWriterType::New();

      DisplacementFieldWriter->SetInput( outputDisplacementField );
      DisplacementFieldWriter->SetFileName( outputDeformationFilename );
      DisplacementFieldWriter->Update();

      if( outputVelocityFilename != NULL && outputVelocityField.IsNotNull() )
      {
        std::cout << "Saving velocity field..." << std::endl;
        DisplacementFieldWriterType::Pointer velocityFieldWriter;
        velocityFieldWriter = DisplacementFieldWriterType::New();

        velocityFieldWriter->SetInput( outputVelocityField );
        velocityFieldWriter->SetFileName( outputVelocityFilename );
        velocityFieldWriter->Update();
      }
      }
    }

  if( warpedImageFilename != NULL )
    {

	  typedef FunctionType::MovingImageWarperType MovingImageWarperType;
	  MovingImageWarperType::Pointer warper = MovingImageWarperType::New();

	  warper->SetInput( movingImage );
    warper->SetOutputParametersFromImage( fixedImage );
    warper->SetDisplacementField( outputDisplacementField );
    warper->UpdateLargestPossibleRegion();

    ImageWriterType::Pointer imageWriter;
    imageWriter = ImageWriterType::New();

    imageWriter->SetInput( warper->GetOutput() );
    imageWriter->SetFileName( warpedImageFilename );
    imageWriter->Update();
    }

  std::cout << "Variational Registration FINISHED!" << std::endl;
  std::cout << "==========================================\n\n" << std::endl;

  return 0;
}
