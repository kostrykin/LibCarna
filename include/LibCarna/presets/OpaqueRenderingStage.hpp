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

#ifndef OPAQUERENDERINGSTAGE_H_6014714286
#define OPAQUERENDERINGSTAGE_H_6014714286

#include <LibCarna/base/MeshRenderingStage.hpp>

/** \file
  * \brief
  * Defines \ref LibCarna::presets::OpaqueRenderingStage.
  */

namespace LibCarna
{

namespace presets
{



// ----------------------------------------------------------------------------------
// OpaqueRenderingStage
// ----------------------------------------------------------------------------------

/** \brief
  * Implements rendering stage that renders opaque meshes.
  *
  * The `%OpaqueRenderingStage` constructor takes a geometry type parameter:
  *
  * \snippet ModuleTests/OpaqueRenderingStageTest.cpp opaque_stage_instantiation
  *
  * The concept of geometry types is explained \ref GeometryTypes "here".
  *
  * \note
  * In the \ref RenderingProcess "rendering process" this stage will usually be
  * inserted \em before such stages that render transparent geometry, like
  * \ref TransparentRenderingStage and \ref VolumeRenderingStage.
  *
  * The following example code configures the \ref SceneGraph "scene" s.t. it
  * produces the rendering presented further below:
  *
  * \snippet ModuleTests/OpaqueRenderingStageTest.cpp opaque_stage_scene_setup
  *
  * The concept of materials, meshes and other geometry feature is explained
  * \ref GeometryFeatures "here".
  *
  * \image html OpaqueRenderingStageTest/fromFront.png "exemplary rendering of two box meshes from code above"
  *
  * \author Leonid Kostrykin
  */
typedef base::MeshRenderingStage< base::Renderable::VideoResourcesOrder< base::MeshRenderingMixin::DEFAULT_ROLE_MATERIAL > >
    OpaqueRenderingStage;



}  // namespace LibCarna :: presets

}  // namespace LibCarna

#endif // OPAQUERENDERINGSTAGE_H_6014714286
