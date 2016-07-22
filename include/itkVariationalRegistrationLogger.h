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
#ifndef __itkVariationalRegistrationLogger_h
#define __itkVariationalRegistrationLogger_h

#include "itkCommand.h"
#include "itkEventObject.h"

namespace itk {

/** \class itk::VariationalRegistrationLogger
 *  \brief A simple callback/observer class to print information during the registration process.
 *
 *  VariationalRegistrationLogger is an implementation of the Command design pattern that is
 *  invoked every iteration of the registration process. Use AddObserver() to connect the logger
 *  with VariationalRegistrationFilter and/or VariationalRegistrationMultiResolutionFilter.
 *  VariationalRegistrationLogger prints levels or metric values on IterationEvent or InitializeEvent.
 *
 *  \sa VariationalRegistrationFilter
 *  \sa VariationalRegistrationMultiResolutionFilter
 *
 *  \ingroup VariationalRegistration
 *
 *  \note This class was developed with funding from the German Research
 *  Foundation (DFG: EH 224/3-1 and HA 235/9-1).
 *  \author Alexander Schmidt-Richberg
 *  \author Rene Werner
 *  \author Jan Ehrhardt
 */
template< class TRegistrationFilter, class TMRFilter >
class VariationalRegistrationLogger
  : public Command
{
public:
  /** Standard class typedefs. */
  typedef VariationalRegistrationLogger               Self;
  typedef Command                                     Superclass;
  typedef SmartPointer< Self >                        Pointer;
  typedef SmartPointer< const Self >                  ConstPointer;

  /** Registration and MR filter types */
  typedef TRegistrationFilter                         RegistrationFilterType;
  typedef TMRFilter                                   MRFilterType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Print iterations, levels or metric values on IterationEvent or InitializeEvent */
  virtual void Execute( itk::Object *caller, const itk::EventObject & event ) ITK_OVERRIDE
    {
      Execute( (const itk::Object *)caller, event);
    }

  /** Print iterations, levels or metric values on IterationEvent or InitializeEvent */
  virtual void Execute( const itk::Object *caller, const itk::EventObject & event ) ITK_OVERRIDE;

protected:
  VariationalRegistrationLogger();
  ~VariationalRegistrationLogger();

  /** Print information about the filter. */
  virtual void PrintSelf(std::ostream& os, Indent indent) const ITK_OVERRIDE;

private:
  VariationalRegistrationLogger(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVariationalRegistrationLogger.hxx"
#endif

#endif
