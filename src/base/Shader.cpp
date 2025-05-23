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

#include <LibCarna/base/glew.hpp>
#include <LibCarna/base/Shader.hpp>
#include <LibCarna/base/ShaderCompilationError.hpp>
#include <cstdlib>
#include <memory>
#include <sstream>

namespace LibCarna
{

namespace base
{



// ----------------------------------------------------------------------------------
// Shader
// ----------------------------------------------------------------------------------

const unsigned int Shader::TYPE_VERTEX_SHADER   = GL_VERTEX_SHADER;
const unsigned int Shader::TYPE_GEOMETRY_SHADER = GL_GEOMETRY_SHADER;
const unsigned int Shader::TYPE_FRAGMENT_SHADER = GL_FRAGMENT_SHADER;


Shader::Shader( unsigned int type, const std::string& src )
    : id( glCreateShader( type ) )
    , type( type )
{
    try
    {
        LIBCARNA_ASSERT_EX( id != 0, "Failed to acquire shader object!" );

        const char* pcSrc = src.c_str();
        const GLint nSrcLength = src.length();

        glShaderSource( id, 1, &pcSrc, &nSrcLength );
        glCompileShader( id );

        // check errors

        GLint nInfoLogLength;
        glGetShaderiv( id, GL_INFO_LOG_LENGTH, &nInfoLogLength );
        if( nInfoLogLength > 1 )
        {
            const std::unique_ptr< GLchar > buf( new GLchar[ nInfoLogLength ] );
            glGetShaderInfoLog( id, nInfoLogLength, 0, buf.get() );
            const std::string err( buf.get() );
            if( err != "No errors." )
            {
                throw ShaderCompilationError( err );
            }
        }
    }
    catch( ... )
    {
        release();
        throw;
    }
}


Shader::~Shader()
{
    release();
}


void Shader::release()
{
    if( id )
    {
        glDeleteShader( id );
    }
}



}  // namespace LibCarna :: base

}  // namespace LibCarna
