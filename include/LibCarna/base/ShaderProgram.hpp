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

#ifndef SHADERPROGRAM_H_6014714286
#define SHADERPROGRAM_H_6014714286

/** \file
  * \brief
  * Defines \ref LibCarna::base::ShaderProgram
  */

#include <LibCarna/base/math.hpp>
#include <LibCarna/base/noncopyable.hpp>
#include <stack>

namespace LibCarna
{

namespace base
{



// ----------------------------------------------------------------------------------
// ShaderProgram
// ----------------------------------------------------------------------------------

/** \brief
  * Maintains an OpenGL shader program. Realizes the RAII-idiom.
  *
  * \see
  * The concept of shaders is explained \ref RenderingPipeline "here".
  *
  * \author Leonid Kostrykin
  */
class LIBCARNA ShaderProgram
{

    NON_COPYABLE
    
    ShaderProgram();

public:

    // ------------------------------------------------------------------------------
    // ShaderProgram :: Factory
    // ------------------------------------------------------------------------------

    /** \brief
      * Creates \ref ShaderProgram "OpenGL shader program".
      *
      * \author Leonid Kostrykin
      */
    class Factory
    {
    
        NON_COPYABLE
        
        struct Details;
        const std::unique_ptr< Details > pimpl;
    
    public:
    
        /** \brief
          * Prepares creation of new shader program.
          */
        Factory();
        
        /** \brief
          * Cleans up.
          */
        ~Factory();
        
        /** \brief
          * Uses \a shader as \ref RenderingPipeline "vertex shader".
          * \pre `shader.type == Shader::TYPE_VERTEX_SHADER`
          */
        void setVertexShader  ( const Shader& shader );
        
        /** \brief
          * Uses \a shader as \ref RenderingPipeline "geometry shader".
          * \pre `shader.type == Shader::TYPE_GEOMETRY_SHADER`
          */
        void setGeometryShader( const Shader& shader );
        
        /** \brief
          * Uses \a shader as \ref RenderingPipeline "fragment shader".
          * \pre `shader.type == Shader::TYPE_FRAGMENT_SHADER`
          */
        void setFragmentShader( const Shader& shader );
        
        /** \brief
          * Creates new OpenGL shader program.
          *
          * \throw AssertionFailure
          *     thrown when creation of OpenGL program or the linking of the shader
          *     objects fails.
          */
        ShaderProgram* create() const;
        
    }; // ShaderProgram :: Factory

    // ------------------------------------------------------------------------------

    /** \brief
      * Deletes the maintained OpenGL shader program.
      */
    virtual ~ShaderProgram();

    /** \brief
      * Holds the shader program ID.
      */
    const unsigned int id;

private:

    /** \brief
      * Queries this shader program for errors.
      */
    void checkErrors() const;

}; // ShaderProgram



}  // namespace LibCarna :: base

}  // namespace LibCarna

#endif // SHADERPROGRAM_H_6014714286
