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

#ifndef RENDERSTAGE_H_6014714286
#define RENDERSTAGE_H_6014714286

#include <LibCarna/LibCarna.hpp>
#include <LibCarna/base/noncopyable.hpp>
#include <LibCarna/base/math.hpp>

/** \file
  * \brief
  * Defines \ref LibCarna::base::RenderStage.
  */

namespace LibCarna
{

namespace base
{



// ----------------------------------------------------------------------------------
// RenderStage
// ----------------------------------------------------------------------------------

/** \brief
  * Base abstract class of each rendering stage. Refer to the documentation of the
  * \ref RenderingProcess "rendering process".
  *
  * \author Leonid Kostrykin
  */
class LIBCARNA RenderStage
{

    NON_COPYABLE

    struct Details;
    const std::unique_ptr< Details > pimpl;

public:

    /** \brief
      * Instantiates in enabled-state.
      */
    RenderStage();

    /** \brief
      * Deletes.
      */
    virtual ~RenderStage();
    
    /** \brief
      * Sets whether the view transform is pass-invariant for the duration of a
      * single frame.
      *
      * \note
      * This method is for internal usage only.
      */
    void setViewTransformFixed( bool viewTransformFixed );
    
    /** \brief
      * Tells whether the view transform is pass-invariant for the duration of a
      * single frame.
      */
    bool isViewTransformFixed() const;
    
    /** \brief
      * Orders this stage to reshape its buffers according to the specified \a width and \a height.
      *
      * \attention
      * Always call the base implementaion!
      *
      * \param fr references the frame renderer this stage belongs to.
      * \param width is the \ref LibCarna::base::Viewport "root viewport" width.
      * \param height is the \ref LibCarna::base::Viewport "root viewport" height.
      */
    virtual void reshape( FrameRenderer& fr, unsigned int width, unsigned int height );
    
    /** \brief
      * Tells whether this stage is ready for rendering.
      *
      * In particular, this method returns `true` if this rendering stage only requires \ref reshape to be called, if
      * width or height actually have changed. In contrary, i.e. if this method returns `false`, \ref reshape will be
      * called before every frame.
      */
    bool isInitialized() const;
    
    /** \brief
      * Called once before each frame.
      */
    virtual void prepareFrame( Node& root );

    /** \brief
      * Called once per pass.
      *
      * If this rendering stage maintains one or more \ref LibCarna::base::RenderQueue objects, than this is the right
      * place to \ref LibCarna::base::RenderQueue::build "build" them. Note that the queues need to be rebuilt only *once
      * per frame* and not *per pass*, unless \ref isViewTransformFixed is `false`. If it is `true` and this is not the
      * first invocation of this method since the last time \ref prepareFrame was called,
      * \ref LibCarna::base::RenderQueue::rewind "rewinding" the queue will be sufficient.
      */
    virtual void renderPass( const math::Matrix4f& viewTransform, RenderTask& rt, const Viewport& vp ) = 0;
    
    /** \brief
      * Tells whether this stage is enabled. Disabled stages are not rendered by \ref RenderTask "render tasks".
      */
    bool isEnabled() const;
    
    /** \brief
      * Sets whether this stage is enabled. Disabled stages are not rendered by \ref RenderTask "render tasks".
      */
    void setEnabled( bool );
    
    /** \brief
      * References the renderer this stage belongs to.
      * 
      * \pre `isInitialized() == true`
      */
    base::FrameRenderer& renderer();
    
    /** \overload
      */
    const base::FrameRenderer& renderer() const;
    
    /** \brief
      * Adds \a listener to the set of listeners this instance notifies in \f$\mathcal O\left(\log n\right)\f$.
      */
    void addRenderStageListener( RenderStageListener& listener );
    
    /** \brief
      * Removes \a listener from the set of listeners this instance notifies in \f$\mathcal O\left(\log n\right)\f$.
      */
    void removeRenderStageListener( RenderStageListener& listener );

}; // RenderStage



}  // namespace LibCarna :: base

}  // namespace LibCarna

#endif // RENDERSTAGE_H_6014714286
