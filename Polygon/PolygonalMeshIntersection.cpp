/* -*-c++-*- osgXI - Copyright(C) 2010-2012 Wang Guanqun & Wang Rui.
*
* This module is licensed based on the GNU Lesser General Public License
* version 2.1 ("LGPL"), which is located at:
* http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#include <osg/io_utils>
#include <osgModeling/PolygonalMesh>
#include <iostream>
#include <algorithm>

using namespace osgModeling;

// TODO
// Not implemented here. Try something like BSP here?

unsigned int PolygonalMesh::intersect( const osg::Vec3& s, const osg::Vec3& e, PointList& results, float bias )
{
    return 0;
}

unsigned int PolygonalMesh::intersect( const osg::Vec3& s, const osg::Vec3& e, HalfEdgeList& results, float bias )
{
    return 0;
}

unsigned int PolygonalMesh::intersect( const osg::Vec3& s, const osg::Vec3& e, FaceList& results )
{
    return 0;
}

unsigned int PolygonalMesh::intersect( const osg::Polytope& polytope, PointList& results )
{
    return 0;
}

unsigned int PolygonalMesh::intersect( const osg::Polytope& polytope, HalfEdgeList& results )
{
    return 0;
}

unsigned int PolygonalMesh::intersect( const osg::Polytope& polytope, FaceList& results )
{
    return 0;
}
