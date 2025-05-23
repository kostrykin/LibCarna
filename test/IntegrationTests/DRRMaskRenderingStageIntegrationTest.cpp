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

#include "DRRMaskRenderingStageIntegrationTest.hpp"
#include <LibCarna/base/Node.hpp>
#include <LibCarna/base/Color.hpp>
#include <LibCarna/base/FrameRenderer.hpp>
#include <LibCarna/presets/MaskRenderingStage.hpp>



static const unsigned int DRRMaskRenderingStageIntegrationTest_WIDTH  = 640;
static const unsigned int DRRMaskRenderingStageIntegrationTest_HEIGHT = 480;



// ----------------------------------------------------------------------------------
// DRRMaskRenderingStageIntegrationTest
// ----------------------------------------------------------------------------------

void DRRMaskRenderingStageIntegrationTest::initTestCase()
{
    qglContextHolder.reset( new QGLContextHolder() );
    testFramebuffer.reset( new TestFramebuffer( qglContextHolder->glContext(), DRRMaskRenderingStageIntegrationTest_WIDTH, DRRMaskRenderingStageIntegrationTest_HEIGHT ) );
}


void DRRMaskRenderingStageIntegrationTest::cleanupTestCase()
{
    testFramebuffer.reset();
    qglContextHolder.reset();
}


void DRRMaskRenderingStageIntegrationTest::init()
{
    presets::DRRStage* const drr = new presets::DRRStage( GEOMETRY_TYPE_VOLUMETRIC );
    drr->setSampleRate( 200 );
    drr->setWaterAttenuation( 5e-3f );
    drr->setBaseIntensity( 1.f );
    drr->setLowerThreshold( base::HUV( -400 ) );
    drr->setUpperThreshold( base::HUV( +400 ) );
    drr->setUpperMultiplier( 1.5f );
    drr->setRenderingInverse( false );

    renderer.reset( new base::FrameRenderer( qglContextHolder->glContext(), DRRMaskRenderingStageIntegrationTest_WIDTH, DRRMaskRenderingStageIntegrationTest_HEIGHT, true ) );
    renderer->setBackgroundColor( base::Color::BLACK_NO_ALPHA );
    renderer->appendStage( drr );

    scene.reset( new TestScene() );
}


void DRRMaskRenderingStageIntegrationTest::cleanup()
{
    scene.reset();
    renderer.reset();
}


void DRRMaskRenderingStageIntegrationTest::test_shared_geometry_node()
{
    base::Geometry& geometry = scene->volumeGeometry();

    //! [mask_rendering_setup]
    mr = new presets::MaskRenderingStage( GEOMETRY_TYPE_VOLUMETRIC );
    renderer->appendStage( mr );
    //! [mask_rendering_setup]

    /* Perform global thresholding to create the mask.
     */
    //! [create_mask]
    mask.reset( new base::IntensityVolumeUInt8( scene->volume().size ) );
    for( std::size_t pos = 0; pos < mask->buffer().size(); ++pos )
    {
        mask->buffer()[ pos ] = scene->volume().buffer()[ pos ] > 48000 ? 255 : 0;
    }

    base::BufferedVectorFieldTexture< base::IntensityVolumeUInt8 >& maskTexture
        = base::BufferedVectorFieldTexture< base::IntensityVolumeUInt8 >::create( *mask );
    //! [create_mask]

    //! [attach_mask]
    geometry.putFeature( mr->maskRole, maskTexture );
    maskTexture.release();
    //! [attach_mask]

    renderer->render( scene->cam(), *scene->root );
    VERIFY_FRAMEBUFFER( *testFramebuffer );
}


void DRRMaskRenderingStageIntegrationTest::test_dedicated_geometry_node()
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


void DRRMaskRenderingStageIntegrationTest::test_helper_geometry_node()
{
    const TestScene& scene = *( this->scene );
    const base::math::Vector3f spacings = scene.getSpacings();

    const static unsigned int GEOMETRY_TYPE_MASK = 4;
    mr = new presets::MaskRenderingStage( GEOMETRY_TYPE_MASK );
    renderer->appendStage( mr );

    /* Perform global thresholding and configure geometry node for the mask data.
     */
    typedef helpers::VolumeGridHelper< base::IntensityVolumeUInt8 > UInt8GridHelper;
    UInt8GridHelper gridHelper( scene.volume().size );
    gridHelper.setIntensitiesRole( mr->maskRole );
    gridHelper.loadIntensities( [&scene]( const base::math::Vector3ui& voxel )
        {
            const base::HUV huv = base::HUV( scene.volume()( voxel ) );
            return huv > 1976 ? 1.f : 0.f;
        }
    );
    base::Node* const geometry = gridHelper.createNode( GEOMETRY_TYPE_MASK, UInt8GridHelper::Spacing( spacings ) );
    scene.root->attachChild( geometry );
    gridHelper.releaseGeometryFeatures();

    renderer->render( scene.cam(), *scene.root );
    VERIFY_FRAMEBUFFER( *testFramebuffer );
}


void DRRMaskRenderingStageIntegrationTest::test_render_borders()
{
    const TestScene& scene = *( this->scene );
    const base::math::Vector3f spacings = scene.getSpacings();

    const static unsigned int GEOMETRY_TYPE_MASK = 4;
    mr = new presets::MaskRenderingStage( GEOMETRY_TYPE_MASK );
    mr->setFilling( false );
    renderer->appendStage( mr );

    /* Perform global thresholding and configure geometry node for the mask data.
     */
    typedef helpers::VolumeGridHelper< base::IntensityVolumeUInt8 > UInt8GridHelper;
    UInt8GridHelper gridHelper( scene.volume().size );
    gridHelper.setIntensitiesRole( mr->maskRole );
    gridHelper.loadIntensities( [&scene]( const base::math::Vector3ui& voxel )
        {
            const base::HUV huv = base::HUV( scene.volume()( voxel ) );
            return huv >= -400 ? 1.f : 0.f;
        }
    );
    base::Node* const geometry = gridHelper.createNode( GEOMETRY_TYPE_MASK, UInt8GridHelper::Spacing( spacings ) );
    scene.root->attachChild( geometry );
    gridHelper.releaseGeometryFeatures();

    renderer->render( scene.cam(), *scene.root );
    VERIFY_FRAMEBUFFER( *testFramebuffer );
}


void DRRMaskRenderingStageIntegrationTest::test_wrong_mask_role()
{
    const TestScene& scene = *( this->scene );
    const base::math::Vector3f spacings = scene.getSpacings();

    const static unsigned int GEOMETRY_TYPE_MASK = 4;
    mr = new presets::MaskRenderingStage( GEOMETRY_TYPE_MASK, 100 ); // <-- role is 100
    mr->setFilling( false );
    renderer->appendStage( mr );

    /* Perform global thresholding and configure geometry node for the mask data.
     */
    typedef helpers::VolumeGridHelper< base::IntensityVolumeUInt8 > UInt8GridHelper;
    UInt8GridHelper gridHelper( scene.volume().size );
    gridHelper.setIntensitiesRole( 0 ); // <-- role is 0
    gridHelper.loadIntensities( [&scene]( const base::math::Vector3ui& voxel )
        {
            const base::HUV huv = base::HUV( scene.volume()( voxel ) );
            return huv >= -400 ? 1.f : 0.f;
        }
    );
    base::Node* const geometry = gridHelper.createNode( GEOMETRY_TYPE_MASK, UInt8GridHelper::Spacing( spacings ) );
    scene.root->attachChild( geometry );
    gridHelper.releaseGeometryFeatures();

    QVERIFY_EXCEPTION_THROWN(
        renderer->render( scene.cam(), *scene.root ),
        base::AssertionFailure
    );
}