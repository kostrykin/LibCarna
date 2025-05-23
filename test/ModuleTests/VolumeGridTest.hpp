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

#pragma once

#include <LibCarna/LibCarna.hpp>

namespace LibCarna
{

namespace testing
{



// ----------------------------------------------------------------------------------
// VolumeGridTestUInt16
// ----------------------------------------------------------------------------------

/** \brief
  * Module-tests of the \ref LibCarna::base::VolumeGrid class.
  *
  * \author Leonid Kostrykin
  */
class VolumeGridTestUInt16 : public QObject
{

    Q_OBJECT

private slots:

    /** \brief  Called before the first test function is executed.
      */
    void initTestCase();

    /** \brief  Called after the last test function is executed.
      */
    void cleanupTestCase();

    /** \brief  Called before each test function is executed
      */
    void init();

    /** \brief  Called after each test function is executed
      */
    void cleanup();

 // ---------------------------------------------------------------------------------

    void test_instantiation();

    void test_parenthesisOperator();

 // ---------------------------------------------------------------------------------

private:

    std::unique_ptr< base::VolumeGrid< base::IntensityVolumeUInt16, void > > grid;

}; // VolumeGridTestUInt16



// ----------------------------------------------------------------------------------
// VolumeGridTestUInt8
// ----------------------------------------------------------------------------------

/** \brief
  * Module-tests of the \ref LibCarna::base::VolumeGrid class.
  *
  * \author Leonid Kostrykin
  */
class VolumeGridTestUInt8 : public QObject
{

    Q_OBJECT

private slots:

    /** \brief  Called before the first test function is executed.
      */
    void initTestCase();

    /** \brief  Called after the last test function is executed.
      */
    void cleanupTestCase();

    /** \brief  Called before each test function is executed
      */
    void init();

    /** \brief  Called after each test function is executed
      */
    void cleanup();

 // ---------------------------------------------------------------------------------

    void test_instantiation();

    void test_parenthesisOperator1();

    void test_parenthesisOperator2();

 // ---------------------------------------------------------------------------------

private:

    std::unique_ptr< base::VolumeGrid< base::IntensityVolumeUInt8, void > > grid;

}; // VolumeGridTestUInt8



}  // namespace testing

}  // namespace LibCarna
