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

#ifndef VECTORFIELD_H_6014714286
#define VECTORFIELD_H_6014714286

/** \file
  * \brief
  * Defines \ref LibCarna::base::math::VectorField.
  */

#include <LibCarna/base/math.hpp>
#include <LibCarna/LibCarna.hpp>

namespace LibCarna
{

namespace base
{

namespace math
{



// ----------------------------------------------------------------------------------
// VectorField
// ----------------------------------------------------------------------------------

/** \brief
  * Abstract definition of an \f$\mathbb Z_0^3 \to T\f$ vector field where \f$T\f$ is
  * \a ValueType.
  *
  * \author Leonid Kostrykin
  */
template< typename ValueType >
class VectorField
{

public:

    /** \brief
      * Holds the co-domain type of the vector field.
      */
    typedef ValueType Value;

    /** \brief
      * Does nothing.
      */
    virtual ~VectorField()
    {
    }

    /** \brief
      * Returns value of specified voxel.
      */
    virtual ValueType operator()
        ( unsigned int x
        , unsigned int y
        , unsigned int z ) const = 0;

    /** \overload
      */
    virtual ValueType operator()( const Vector3ui& at ) const;

}; // VectorField


template< typename ValueType >
ValueType VectorField< ValueType >::operator()( const math::Vector3ui& at ) const
{
    return ( *this )( at.x(), at.y(), at.z() );
}



}  // namespace LibCarna :: base :: math

}  // namespace LibCarna :: base

}  // namespace LibCarna

#endif // VECTORFIELD_H_6014714286
