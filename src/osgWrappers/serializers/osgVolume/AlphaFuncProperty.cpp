#include <osgVolume/Property>
#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>

REGISTER_OBJECT_WRAPPER( osgVolume_AlphaFuncProperty,
                         new osgVolume::AlphaFuncProperty,
                         osgVolume::AlphaFuncProperty,
                         "osg::Object osgVolume::Property osgVolume::ScalarProperty osgVolume::AlphaFuncProperty" )
{
}
