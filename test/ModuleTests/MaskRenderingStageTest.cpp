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

#include "MaskRenderingStageTest.hpp"
#include <LibCarna/base/Node.hpp>
#include <LibCarna/base/Color.hpp>
#include <LibCarna/base/FrameRenderer.hpp>
#include <LibCarna/presets/MaskRenderingStage.hpp>



static const unsigned int MaskRenderingStageTest_WIDTH  = 640;
static const unsigned int MaskRenderingStageTest_HEIGHT = 480;



// ----------------------------------------------------------------------------------
// MaskRenderingStageTest
// ----------------------------------------------------------------------------------

void MaskRenderingStageTest::initTestCase()
{
    qglContextHolder.reset( new QGLContextHolder() );
    testFramebuffer.reset( new TestFramebuffer( qglContextHolder->glContext(), MaskRenderingStageTest_WIDTH, MaskRenderingStageTest_HEIGHT ) );
}


void MaskRenderingStageTest::cleanupTestCase()
{
    testFramebuffer.reset();
    qglContextHolder.reset();
}


void MaskRenderingStageTest::init()
{
    renderer.reset( new base::FrameRenderer( qglContextHolder->glContext(), MaskRenderingStageTest_WIDTH, MaskRenderingStageTest_HEIGHT, true ) );
    renderer->setBackgroundColor( base::Color::BLACK_NO_ALPHA );
    scene.reset( new TestScene() );
}


void MaskRenderingStageTest::cleanup()
{
    scene.reset();
    renderer.reset();
}


void MaskRenderingStageTest::test_shared_geometry_node()
{
    base::Geometry& geometry = scene->volumeGeometry();

    mr = new presets::MaskRenderingStage( GEOMETRY_TYPE_VOLUMETRIC );
    renderer->appendStage( mr );

    /* Perform global thresholding to create the mask.
     */
    mask.reset( new base::IntensityVolumeUInt8( scene->volume().size ) );
    for( std::size_t pos = 0; pos < mask->buffer().size(); ++pos )
    {
        mask->buffer()[ pos ] = scene->volume().buffer()[ pos ] > 48000 ? 255 : 0;
    }

    base::BufferedVectorFieldTexture< base::IntensityVolumeUInt8 >& maskTexture
        = base::BufferedVectorFieldTexture< base::IntensityVolumeUInt8 >::create( *mask );
    geometry.putFeature( mr->maskRole, maskTexture );
    maskTexture.release();

    renderer->render( scene->cam(), *scene->root );
    VERIFY_FRAMEBUFFER( *testFramebuffer );
}


void MaskRenderingStageTest::test_dedicated_geometry_node()
{
    const static unsigned int GEOMETRY_TYPE_MASK = 4;
    mr = new presets::MaskRenderingStage( GEOMETRY_TYPE_MASK );
    renderer->appendStage( mr );

    /* Perform global thresholding to create the mask.
     */
    mask.reset( new base::IntensityVolumeUInt8( scene->volume().size ) );
    for( std::size_t pos = 0; pos < mask->buffer().size(); ++pos )
    {
        mask->buffer()[ pos ] = scene->volume().buffer()[ pos ] > 48000 ? 255 : 0;
    }

    /* Configure geometry node for mask data.
     */
    base::BufferedVectorFieldTexture< base::IntensityVolumeUInt8 >& maskTexture
        = base::BufferedVectorFieldTexture< base::IntensityVolumeUInt8 >::create( *mask );
    base::Geometry* const geometry = new base::Geometry( GEOMETRY_TYPE_MASK );
    geometry->putFeature( mr->maskRole, maskTexture );
    geometry->localTransform = base::math::scaling4f( scene->scale() );
    scene->root->attachChild( geometry );
    maskTexture.release();

    renderer->render( scene->cam(), *scene->root );
    VERIFY_FRAMEBUFFER( *testFramebuffer );
}
