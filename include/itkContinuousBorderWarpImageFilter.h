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
#ifndef itkContinuousBorderWarpImageFilter_h
#define itkContinuousBorderWarpImageFilter_h

#include "itkWarpImageFilter.h"

namespace itk
{

/** \class ContinuousBorderWarpImageFilter
 *  \brief Warps an image using an input deformation field with continuous boundary conditions.
 *
 *  ContinuousBorderWarpImageFilter warps an existing image with respect to
 *  a given displacement field and a given interpolation scheme.The default
 *  interpolation typed used is the LinearInterpolateImageFunction.
 *  The user can specify a particular interpolation function via
 *  SetInterpolator().
 *
 *  Positions mapped to outside of the input image buffer are assigned
 *  the value of the next boundary pixel in the image.
 *
 *  The input image is set via SetInput(). The input displacement field
 *  is set via SetDisplacementField().
 *
 *  This filter is implemented as a multithreaded filter.
 *
 *  \sa WarpImageFilter
 *
 *  \warning This filter assumes that the input type, output type
 *  and displacement field type all have the same number of dimensions.
 *
 *  \ingroup VariationalRegistration
 *  \ingroup GeometricTransforms
 *  \ingroup MultiThreaded
 *
 *  \note This class was developed with funding from the German Research
 *  Foundation (DFG: EH 224/3-1 and HA 235/9-1).
 *  \author Alexander Schmidt-Richberg
 *  \author Rene Werner
 *  \author Jan Ehrhardt
 */
template <
  class TInputImage,
  class TOutputImage,
  class TDisplacementField
  >
class ContinuousBorderWarpImageFilter :
    public WarpImageFilter<TInputImage, TOutputImage, TDisplacementField>
{
public:
  /** Standard class typedefs. */
  typedef ContinuousBorderWarpImageFilter                               Self;
  typedef WarpImageFilter<TInputImage,TOutputImage, TDisplacementField> Superclass;
  typedef SmartPointer<Self>                                            Pointer;
  typedef SmartPointer<const Self>                                      ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( ContinuousBorderWarpImageFilter, WarpImageFilter );

  /** Typedef to describe the output image region type. */
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

  /** Inherit some types from the superclass. */
  typedef typename Superclass::InputImageType         InputImageType;
  typedef typename Superclass::InputImagePointer      InputImagePointer;
  typedef typename Superclass::OutputImageType        OutputImageType;
  typedef typename Superclass::OutputImagePointer     OutputImagePointer;
  typedef typename Superclass::InputImageConstPointer InputImageConstPointer;
  typedef typename OutputImageType::IndexType         IndexType;
  typedef typename OutputImageType::SizeType          SizeType;
  typedef typename OutputImageType::PixelType         PixelType;
  typedef typename OutputImageType::SpacingType       SpacingType;

  /** Determine the image dimension. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TOutputImage::ImageDimension );
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension );
  itkStaticConstMacro(DeformationFieldDimension, unsigned int,
      TDisplacementField::ImageDimension );

  /** Displacement field typedef support. */
  typedef TDisplacementField                        DisplacementFieldType;
  typedef typename DisplacementFieldType::Pointer   DisplacementFieldPointer;
  typedef typename DisplacementFieldType::PixelType DisplacementType;

  /** Interpolator typedef support. */
  typedef typename Superclass::InterpolatorType       InterpolatorType;

  /** Point type */
  typedef typename Superclass::PointType              PointType;

protected:
  ContinuousBorderWarpImageFilter() {};
  ~ContinuousBorderWarpImageFilter() ITK_OVERRIDE {};

  /** WarpImageFilter is implemented as a multi-threaded filter.
   * As such, it needs to provide and implementation for
   * ThreadedGenerateData(). */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
      ThreadIdType threadId ) ITK_OVERRIDE;

private:
  ContinuousBorderWarpImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkContinuousBorderWarpImageFilter.hxx"
#endif

#endif
