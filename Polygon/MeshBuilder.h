/* -*-c++-*- osgXI - Copyright(C) 2010-2012 Wang Guanqun & Wang Rui.
*
* This module is licensed based on the GNU Lesser General Public License
* version 2.1 ("LGPL"), which is located at:
* http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#ifndef OSGMODELING_MESHBUILDER_H
#define OSGMODELING_MESHBUILDER_H

#include <osgModeling/Export>

#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/Geometry>

namespace osgModeling
{

    class PolygonalMesh;
    
    class OSGMODELING_EXPORT MeshBuilder : public osg::NodeVisitor
    {
    public:
        MeshBuilder();
        virtual ~MeshBuilder();
        
        void setPolygonalMesh( PolygonalMesh* mesh ) { _mesh = mesh; }
        PolygonalMesh* getPolygonalMesh() { return _mesh.get(); }
        const PolygonalMesh* getPolygonalMesh() const { return _mesh.get(); }
        
        /** Build a polygonal mesh from a geometry and generate the topology structure.
         *  The polygonal mesh may accept more than 1 geometries.
         */
        static bool buildMesh( PolygonalMesh& mesh, const osg::Geometry& geom );
        
        virtual void apply( osg::Geode& geode );
        
    protected:
        osg::ref_ptr<PolygonalMesh> _mesh;
    };

}  // namespace osgModeling

#endif
