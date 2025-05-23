/*
 *  Copyright (C) 2010 - 2016 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 * 
 * 
 *  Copyright (C) 2021 - 2025 Leonid Kostrykin
 * 
 */

#ifndef CAMERANAVIGATIONCONTROL_H_6014714286
#define CAMERANAVIGATIONCONTROL_H_6014714286

#include <LibCarna/LibCarna.hpp>
#include <LibCarna/base/CameraControl.hpp>
#include <memory>

/** \file
  * \brief
  * Defines \ref LibCarna::presets::CameraNavigationControl.
  */

namespace LibCarna
{

namespace presets
{



// ----------------------------------------------------------------------------------
// CameraNavigationControl
// ----------------------------------------------------------------------------------
    
/** \brief
  * Implements \ref base::CameraControl, rotates the camera around itself and allows
  * completely free camera movement.
  *
  * \author Leonid Kostrykin
  */
class LIBCARNA CameraNavigationControl : public base::CameraControl
{

    NON_COPYABLE
    
    struct Details;
    const std::unique_ptr< Details > pimpl;

public:

    /** \brief
      * Instantiates.
      */
    CameraNavigationControl();
    
    /** \brief
      * Deletes.
      */
    virtual ~CameraNavigationControl();

    virtual void setCamera( base::Spatial& ) override;
    
    /** \brief
      * Rotates the camera around its local \f$\left(0, 1, 0\right)^\mathrm T\f$
      * vector by \a radians.
      */
    virtual void rotateHorizontally( float radians ) override;
    
    /** \brief
      * Rotates the camera around its local \f$\left(1, 0, 0\right)^\mathrm T\f$
      * vector by \a radians.
      */
    virtual void rotateVertically( float radians ) override;
    
    /** \brief
      * Moves the camera along its local \f$\left(0, 0, 1\right)^\mathrm T\f$ vector
      * by \a units.
      */
    virtual void moveAxially( float units ) override;
    
    virtual void moveLaterally( float unitsX, float unitsY ) override;

}; // CameraNavigationControl



}  // namespace LibCarna :: presets

}  // namespace LibCarna



#endif // CAMERANAVIGATIONCONTROL_H_6014714286
