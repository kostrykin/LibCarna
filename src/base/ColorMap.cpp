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

#include <LibCarna/base/ColorMap.hpp>
#include <LibCarna/base/math/Span.hpp>
#include <LibCarna/base/Sampler.hpp>
#include <LibCarna/base/glew.hpp>
#include <LibCarna/base/ShaderManager.hpp>
#include <LibCarna/base/Texture.hpp>

namespace LibCarna
{

namespace base
{



// ----------------------------------------------------------------------------------
// ColorMap :: Details
// ----------------------------------------------------------------------------------

struct ColorMap::Details
{
    Details( unsigned int resolution );
    
    std::vector< base::Color > colorMap;
    std::unique_ptr< base::Texture< 1 > > texture;
    std::unique_ptr< base::Sampler      > sampler;
    bool isDirty;
    void update();

    std::size_t locationByIntensity( float intensity );
    void sampleDownTo( unsigned int resolution );

    float minIntensity;
    float maxIntensity;
};


ColorMap::Details::Details( unsigned int resolution )
    : colorMap( resolution )
    , isDirty( true )
    , minIntensity( ColorMap::DEFAULT_MINIMUM_INTENSITY )
    , maxIntensity( ColorMap::DEFAULT_MAXIMUM_INTENSITY )
{
}


std::size_t ColorMap::Details::locationByIntensity( float intensity )
{
    if( intensity < 0 ) intensity = 0;
    if( intensity > 1 ) intensity = 1;
    const std::size_t maxLocation = colorMap.size() - 1;
    float location = intensity * maxLocation + 0.5;
    if( location > maxLocation ) location = maxLocation;
    return static_cast< std::size_t >( location );
}


void ColorMap::Details::update()
{
    if( isDirty )
    {
        /* Create the texture if it was not created yet.
        */
        if( texture.get() == nullptr )
        {
            texture.reset( new base::Texture< 1 >( GL_RGBA8, GL_RGBA ) );
        }

        /* Verify that the texture size is supported by the hardware (and perform down-sampling if required).
         */
        const unsigned int maxTextureSize = base::Texture< 1 >::maxTextureSize();
        if( colorMap.size() > maxTextureSize )
        {
            std::stringstream ss;
            ss << "Color map size (" << colorMap.size() << ") exceeds maximum texture size (" << maxTextureSize << ").";
            base::Log::instance().record( base::Log::warning, ss.str() );
            sampleDownTo( maxTextureSize );
        }
        
        /* Update the texture.
         */
        base::Texture< 1 >::Resolution textureSize;
        textureSize.x() = colorMap.size();
        texture->update( textureSize, GL_UNSIGNED_BYTE, &colorMap[ 0 ] );
        isDirty = false;
        base::Log::instance().record( base::Log::debug, "Color map updated." );
    }
}


void ColorMap::Details::sampleDownTo( unsigned int resolution )
{
    LIBCARNA_ASSERT( resolution <= colorMap.size() );
    if( resolution < colorMap.size() )
    {
        std::vector< base::Color > newColorMap( resolution );
        for( std::size_t location = 0; location < resolution; ++location )
        {
            const float intensity = static_cast< float >( location ) / ( resolution - 1 );
            newColorMap[ location ] = colorMap[ locationByIntensity( intensity ) ];
        }
        colorMap = newColorMap;
        isDirty = true;
    }
}



// ----------------------------------------------------------------------------------
// ColorMap
// ----------------------------------------------------------------------------------

const float ColorMap::DEFAULT_MINIMUM_INTENSITY = 0.0f;
const float ColorMap::DEFAULT_MAXIMUM_INTENSITY = 1.0f;


ColorMap::ColorMap( unsigned int resolution )
    : pimpl( new Details( resolution ) )
{
}


ColorMap::~ColorMap()
{
}


void ColorMap::clear()
{
    std::fill( pimpl->colorMap.begin(), pimpl->colorMap.end(), base::Color::BLACK_NO_ALPHA );
    pimpl->isDirty = true;
}


ColorMap& ColorMap::writeLinearSegment( const base::math::Span< float >& intensityRange, const base::math::Span< base::Color > colorRange )
{
    LIBCARNA_ASSERT( intensityRange.first <= intensityRange.last );
    const std::size_t locFirst = pimpl->locationByIntensity( intensityRange.first );
    const std::size_t locLast  = pimpl->locationByIntensity( intensityRange.last  );
    const int rangeSize = locLast - locFirst + 1;
    const base::math::Vector4f color0 = colorRange.first;
    const base::math::Vector4f color1 = colorRange.last;

    for( std::size_t offset = 0; offset < rangeSize; ++offset )
    {
        const float lambda = ( rangeSize == 1 ? 0.5f : offset / static_cast< float >( rangeSize - 1 ) );
        const base::Color color = base::math::mix< base::math::Vector4f >( color0, color1, lambda );
        pimpl->colorMap[ locFirst + offset ] = color;
    }
    pimpl->isDirty = true;

    return *this;
}


ColorMap& ColorMap::writeLinearSegment( float intensityFirst, float intensityLast, const base::Color& colorFirst, const base::Color& colorLast )
{
    return writeLinearSegment( base::math::Span< float >( intensityFirst, intensityLast ), base::math::Span< base::Color >( colorFirst, colorLast ) );
}


ColorMap& ColorMap::writeLinearSpline( const std::vector< base::Color >& colors )
{
    LIBCARNA_ASSERT( colors.size() >= 2 );
    unsigned int segmentIndex = 0;
    float intensityLast = 0;
    for( auto colorItr = colors.begin(); colorItr + 1 != colors.end(); ++colorItr )
    {
        const base::Color color0 = *colorItr;
        const base::Color color1 = *( colorItr + 1 );

        float intensityFirst = intensityLast;
        intensityLast = static_cast< float >( segmentIndex + 1 ) / ( colors.size() - 1 );

        writeLinearSegment( intensityFirst, intensityLast, color0, color1 );
        ++segmentIndex;
    }
    return *this;
}


const std::vector< base::Color >& ColorMap::getColorList() const
{
    return pimpl->colorMap;
}


void ColorMap::bind( int unit ) const
{
    /* Create the sampler, if it was not created yet.
     */
    if( pimpl->sampler.get() == nullptr )
    {
        pimpl->sampler.reset( new base::Sampler
            ( base::Sampler::WRAP_MODE_CLAMP, base::Sampler::WRAP_MODE_CLAMP, base::Sampler::WRAP_MODE_CLAMP
            , base::Sampler::FILTER_NEAREST, base::Sampler::FILTER_NEAREST ) );
    }
    
    /* Upload data if dirty.
     */
    pimpl->update();
        
    /* Bind the color map.
    */
    pimpl->texture->bind( unit );
    pimpl->sampler->bind( unit );
}


ColorMap& ColorMap::operator=( const ColorMap& other )
{
    pimpl->colorMap = other.pimpl->colorMap;
    pimpl->isDirty = true;
    pimpl->minIntensity = other.minimumIntensity();
    pimpl->maxIntensity = other.maximumIntensity();
    return *this;
}


void ColorMap::setMinimumIntensity( float minimumIntensity )
{
    /* Check pre-conditions.
     */
    LIBCARNA_ASSERT( 0 <= minimumIntensity && minimumIntensity <= 1 );

    /* Update intensity limits.
     */
    pimpl->minIntensity = minimumIntensity;
    if( pimpl->maxIntensity < pimpl->minIntensity )
    {
        pimpl->maxIntensity = pimpl->minIntensity;
    }

    /* Check post-conditions.
     */
    LIBCARNA_ASSERT( minimumIntensity == this->minimumIntensity() );
    LIBCARNA_ASSERT( minimumIntensity <= this->maximumIntensity() );
}


float ColorMap::minimumIntensity() const
{
    return pimpl->minIntensity;
}


void ColorMap::setMaximumIntensity( float maximumIntensity )
{
    /* Check pre-conditions.
     */
    LIBCARNA_ASSERT( 0 <= maximumIntensity && maximumIntensity <= 1 );

    /* Update intensity limits.
     */
    pimpl->maxIntensity = maximumIntensity;
    if( pimpl->minIntensity > pimpl->maxIntensity )
    {
        pimpl->minIntensity = pimpl->maxIntensity;
    }

    /* Check post-conditions.
     */
    LIBCARNA_ASSERT( this->maximumIntensity() == maximumIntensity );
    LIBCARNA_ASSERT( this->minimumIntensity() <= maximumIntensity );
}


float ColorMap::maximumIntensity() const
{
    return pimpl->maxIntensity;
}



}  // namespace LibCarna :: base

}  // namespace LibCarna
