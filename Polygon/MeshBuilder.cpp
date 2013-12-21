/* -*-c++-*- osgXI - Copyright(C) 2010-2012 Wang Guanqun & Wang Rui.
*
* This module is licensed based on the GNU Lesser General Public License
* version 2.1 ("LGPL"), which is located at:
* http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#include <limits.h>
#include <osg/io_utils>
#include <osg/Version>
#include <osg/TriangleIndexFunctor>
#include <osgModeling/PolygonalMesh>
#include <osgModeling/MeshBuilder>
#include <iostream>

using namespace osgModeling;

#define GET_VALUE_AS(arraytype) (static_cast<const arraytype*>(array))->at(index)

static void arrayToTexCoords( const osg::Array* array, unsigned int index, TexCoordList& texcoords )
{
    if ( !array ) return;
    switch ( array->getType() )
    {
    case osg::Array::FloatArrayType:
        { osg::Vec4 v; v.x() = GET_VALUE_AS(osg::FloatArray); texcoords.push_back(TexCoordPair(1, v)); } break;
    case osg::Array::DoubleArrayType:
        { osg::Vec4 v; v.x() = GET_VALUE_AS(osg::DoubleArray); texcoords.push_back(TexCoordPair(1, v)); } break;
    case osg::Array::Vec2ArrayType:
        {
            const osg::Vec2& v = GET_VALUE_AS(osg::Vec2Array);
            texcoords.push_back( TexCoordPair(2, osg::Vec4(v.x(), v.y(), 0.0f, 0.0f)) );
        }
        break;
    case osg::Array::Vec3ArrayType:
        {
            const osg::Vec3& v = GET_VALUE_AS(osg::Vec3Array);
            texcoords.push_back( TexCoordPair(3, osg::Vec4(v, 0.0f)) );
        }
        break;
    case osg::Array::Vec4ArrayType:
        { const osg::Vec4& v = GET_VALUE_AS(osg::Vec4Array); texcoords.push_back(TexCoordPair(4, v)); } break;
    case osg::Array::Vec2dArrayType:
        {
            const osg::Vec2d& v = GET_VALUE_AS(osg::Vec2dArray);
            texcoords.push_back( TexCoordPair(2, osg::Vec4(v.x(), v.y(), 0.0f, 0.0f)) );
        }
        break;
    case osg::Array::Vec3dArrayType:
        {
            const osg::Vec3d& v = GET_VALUE_AS(osg::Vec3dArray);
            texcoords.push_back( TexCoordPair(3, osg::Vec4(v, 0.0f)) );
        }
        break;
    case osg::Array::Vec4dArrayType:
        { const osg::Vec4d& v = GET_VALUE_AS(osg::Vec4dArray); texcoords.push_back(TexCoordPair(4, v)); } break;
    default: break;
    }
}

#define ADD_VECTOR(num) for (int i=0; i<num; ++i) attributes.push_back(v[i]);
#define ADD_SEPVALUE attributes.push_back(-FLT_MAX);

static void arrayToAttributes( const osg::Array* array, unsigned int index, FloatList& attributes )
{
    if ( !array ) return;
    switch ( array->getType() )
    {
    case osg::Array::FloatArrayType:
        { float v = GET_VALUE_AS(osg::FloatArray); attributes.push_back(v); ADD_SEPVALUE; } break;
    case osg::Array::DoubleArrayType:
        { double v = GET_VALUE_AS(osg::DoubleArray); attributes.push_back(v); ADD_SEPVALUE; } break;
    case osg::Array::Vec2ArrayType:
        { const osg::Vec2& v = GET_VALUE_AS(osg::Vec2Array); ADD_VECTOR(2); ADD_SEPVALUE; } break;
    case osg::Array::Vec3ArrayType:
        { const osg::Vec3& v = GET_VALUE_AS(osg::Vec3Array); ADD_VECTOR(3); ADD_SEPVALUE; } break;
    case osg::Array::Vec4ArrayType:
        { const osg::Vec4& v = GET_VALUE_AS(osg::Vec4Array); ADD_VECTOR(4); ADD_SEPVALUE; } break;
    case osg::Array::Vec2dArrayType:
        { const osg::Vec2d& v = GET_VALUE_AS(osg::Vec2dArray); ADD_VECTOR(2); ADD_SEPVALUE; } break;
    case osg::Array::Vec3dArrayType:
        { const osg::Vec3d& v = GET_VALUE_AS(osg::Vec3dArray); ADD_VECTOR(3); ADD_SEPVALUE; } break;
    case osg::Array::Vec4dArrayType:
        { const osg::Vec4d& v = GET_VALUE_AS(osg::Vec4dArray); ADD_VECTOR(4); ADD_SEPVALUE; } break;
    default: break;
    }
}

#undef ADD_VECTOR
#undef ADD_SEPARATE_VALUE
#undef GET_VALUE_AS

struct CollectTrianglesFunctor
{
    typedef std::map<unsigned int, Point*> PointMap;
    
    CollectTrianglesFunctor()
    :   _vertices(0), _normals(0), _texcoordsList(0),
        _mesh(0), _numInvalidFaces(0)
    {}
    
    virtual ~CollectTrianglesFunctor()
    { _pointMap.clear(); }
    
    void setPolygonalMesh( PolygonalMesh* m ) { _mesh = m; }
    void setVertexPointer( const osg::Vec3Array* va ) { _vertices = va; }
    void setNormalPointer( const osg::Vec3Array* na ) { _normals = na; }
#if OSG_VERSION_LESS_THAN(3,1,0)
    void setTexCoordListPointer( const osg::Geometry::ArrayDataList* taList ) { _texcoordsList = taList; }
#else
    void setTexCoordListPointer( const osg::Geometry::ArrayList* taList ) { _texcoordsList = taList; }
#endif
    unsigned int getNumInvalidFaces() const { return _numInvalidFaces; }
    
    Point* getOrCreatePoint( const osg::Vec3& v, unsigned int p )
    {
        PointMap::const_iterator itr = _pointMap.find(p);
        if ( itr!=_pointMap.end() )
        {
            return itr->second;
        }
        else
        {
            Point* pt = NULL;
            if ( _normals && p<_normals->size() )
                pt = _mesh->addPoint( v, _normals->at(p) );
            else
                pt = _mesh->addPoint( v );
            
            if ( _texcoordsList )
            {
                TexCoordList& tclist = pt->getTexCoords();
                for ( unsigned int i=0; i<_texcoordsList->size(); ++i )
                {
#if OSG_VERSION_LESS_THAN(3,1,0)
                    arrayToTexCoords( (*_texcoordsList)[i].array.get(), p, tclist );
#else
                    arrayToTexCoords( (*_texcoordsList)[i].get(), p, tclist );
#endif
                }
            }
            _pointMap[p] = pt;
            return pt;
        }
    }
    
    inline void operator()( unsigned int p1, unsigned int p2, unsigned int p3 )
    {
        if ( p1==p2 || p1==p3 || p2==p3 )
            return;
        
        const osg::Vec3& v1 = _vertices->at(p1);
        const osg::Vec3& v2 = _vertices->at(p2);
        const osg::Vec3& v3 = _vertices->at(p3);
        
        osg::Vec3 normal = (v2-v1) ^ (v3-v1);
        if ( !normal.length2() ) return;
        
        if ( _mesh )
        {
            PointList ptList;
            ptList.push_back( getOrCreatePoint(v1, p1) );
            ptList.push_back( getOrCreatePoint(v2, p2) );
            ptList.push_back( getOrCreatePoint(v3, p3) );
            
            if ( !_mesh->addFace(ptList) )
                _numInvalidFaces++;
        }
    }
    
    const osg::Vec3Array* _vertices;
    const osg::Vec3Array* _normals;
    
#if OSG_VERSION_LESS_THAN(3,1,0)
    const osg::Geometry::ArrayDataList* _texcoordsList;
#else
    const osg::Geometry::ArrayList* _texcoordsList;
#endif
    
    PointMap _pointMap;
    PolygonalMesh* _mesh;
    unsigned int _numInvalidFaces;
};


MeshBuilder::MeshBuilder()
{
    setTraversalMode( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
}

MeshBuilder::~MeshBuilder()
{
}

bool MeshBuilder::buildMesh( PolygonalMesh& mesh, const osg::Geometry& geom )
{
    const osg::Geometry::PrimitiveSetList& primitives = geom.getPrimitiveSetList();
    unsigned int numSurfacePrimitives = 0;
    for ( osg::Geometry::PrimitiveSetList::const_iterator itr=primitives.begin();
          itr!=primitives.end(); ++itr )
    {
        switch ( (*itr)->getMode() )
        {
        case (osg::PrimitiveSet::TRIANGLES):
        case (osg::PrimitiveSet::TRIANGLE_STRIP):
        case (osg::PrimitiveSet::TRIANGLE_FAN):
        case (osg::PrimitiveSet::QUADS):
        case (osg::PrimitiveSet::QUAD_STRIP):
        case (osg::PrimitiveSet::POLYGON):
            ++numSurfacePrimitives;
            break;
        default:
            break;
        }
    }
    if ( !numSurfacePrimitives ) return false;
    
    const osg::Vec3Array* va = dynamic_cast<const osg::Vec3Array*>( geom.getVertexArray() );
    if ( !va ) return false;
    
    osg::TriangleIndexFunctor<CollectTrianglesFunctor> ctf;
    ctf.setPolygonalMesh( &mesh );
    ctf.setVertexPointer( va );
    ctf.setTexCoordListPointer( &(geom.getTexCoordArrayList()) );
    
    if ( geom.getNormalBinding()==osg::Geometry::BIND_PER_VERTEX )
    {
        const osg::Vec3Array* na = dynamic_cast<const osg::Vec3Array*>( geom.getNormalArray() );
        ctf.setNormalPointer( na );
    }
    geom.accept( ctf );
    return true;
}

void MeshBuilder::apply( osg::Geode& geode )
{
    if ( !_mesh ) _mesh = new PolygonalMesh;
    
    for( unsigned int i = 0; i < geode.getNumDrawables(); i++ )
    {
        osg::Geometry* geom = dynamic_cast<osg::Geometry*>( geode.getDrawable(i) );
        if ( geom )
        {
            buildMesh( *_mesh, *geom );
            if ( !_mesh->getStateSet() )
                _mesh->setStateSet( geom->getStateSet() );
        }
    }
    traverse( geode );
}
