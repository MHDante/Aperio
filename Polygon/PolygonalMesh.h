/* -*-c++-*- osgXI - Copyright(C) 2010-2012 Wang Guanqun & Wang Rui.
*
* This module is licensed based on the GNU Lesser General Public License
* version 2.1 ("LGPL"), which is located at:
* http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#ifndef OSGMODELING_POLYGONALMESH_H
#define OSGMODELING_POLYGONALMESH_H

#include <osgModeling/BaseElement>

#include <osg/Geometry>
#include <vector>
#include <set>

namespace osgModeling
{

    struct dereference_less
    {
        template<class T, class U>
        bool operator()( const T& lhs, const U& rhs ) const
        { return *lhs < *rhs; }
    };
    
    class OSGMODELING_EXPORT PolygonalMesh : public osg::Drawable
    {
    // Static settings
    public:
        static void setDisplayFaceColor( const osg::Vec4& color );
        static const osg::Vec4& getDisplayFaceColor();
        
        static void setDisplayPointColor( const osg::Vec4& color );
        static const osg::Vec4& getDisplayPointColor();
        
        static void setDisplayEdgeColor( const osg::Vec4& color );
        static const osg::Vec4& getDisplayEdgeColor();
        
        static void setDisplayHalfEdgeColor( const osg::Vec4& color );
        static const osg::Vec4& getDisplayHalfEdgeColor();
        
        static void setDisplayPointSize( float size );
        static float getDisplayPointSize();
        
        static void setDisplayEdgeWidth( float width );
        static float getDisplayEdgeWidth();
        
    // Basic methods
    public:
        typedef std::pair<unsigned long, unsigned long> HalfEdgeID;
        typedef std::map<HalfEdgeID, osg::ref_ptr<HalfEdge> > HalfEdgeMap;
        typedef std::set<osg::ref_ptr<Point>, dereference_less> PointSet;
        
        /** Add a new point.
         *  Juse create an empty point object.
         *  @return The point.
         */
        Point* addPoint( const osg::Vec3& v, const osg::Vec3& n=osg::Z_AXIS );
        
        /** Remove a point and all halfedges attached with it.
         *  removeEdge() function is used internal here.
         *  @return FALSE if the point is NULL.
         */
        bool removePoint( Point* pt );
        
        /** Add TWO new halfedges linking 2 input points. Not recommended for public users.
         *  The points and newly allocated halfedges are all INVALID before addFace() function.
         *  @return The halfedge from 'p1' to 'p2' or NULL. The twin is also allocated.
         */
        HalfEdge* addEdge( Point* p1, Point* p2 );
        
        /** Relink a halfedge to a new endpoint. The edge's map will also be modified.
         *  @return FALSE if relinking is impossible.
         */
        bool relinkEdge( HalfEdge* half, Point* end );
        
        /** Remove a halfedge and its TWIN.
         *  The circulations (next and prev) of halfedges will be modified to keep the topology usable.
         *  removeFace() function is used internal to remove faces linked with deleted halfedges.
         *  @return FALSE if the halfedge is NULL.
         */
        bool removeEdge( HalfEdge* half );
        
        /** Just create an empty face object. Not recommended for public users.
         *  @return The face.
         */
        Face* addFace();
        
        /** Add a new face using a list of halfedges ordered counter-clockwise.
         *  The function will fail if halfedges are incontinuous or internal.
         *  Every input halfedge will be linked with the newly allocated face.
         *  @return The face or NULL.
         */
        Face* addFace( const HalfEdgeList& edges );
        
        /** Add a face using a list of points ordered counter-clockwise.
         *  The function will collect every halfedge between each 2 points and construct the face then.
         *  @return The face or NULL.
         */
        Face* addFace( const PointList& points );
        
        /** Remove a face and unlink any edges bordering it.
         *  The function won't delete related halfedges or change their circulations.
         *  @return FALSE if the face is NULL.
         */
        bool removeFace( Face* face );
        
        /**  Erase a point from the points list, assuming it already disconnected from the mesh.
         *   Not recommended for public users.
         */
        bool erase( Point* pt );
        
        /**  Erase a pair of halfedges from the halfedges list, assuming them already disconnected from the mesh.
         *   Not recommended for public users.
         */
        bool erase( HalfEdge* half );
        
        /**  Erase a face from the faces list, assuming it already disconnected from the mesh.
         *   Not recommended for public users.
         */
        bool erase( Face* face );
        
    // User modification methods
    public:
        /** Remove a digon face, that is, a degenerate face with two edges and two points.
         *  Digons are not considered proper and will not be rendered while running.
         *  @return FALSE
         */
        bool removeDigon( Face* face );
        
        /** Check if possible to collapse a pair of halfedges.
         *  Some unsafe collapses doesn't preserve topology. This function can be used to avoid them.
         */
        bool isEdgeCollapsible( HalfEdge* half );
        
        /** Collapse a pair of halfedges into a point (endpoint of the halfedge 'half').
         *  Works for triangles ONLY, and will failed if either of the two faces is not a triangle.
         *  Totally 1 point, 3 edges and 2 faces will be deleted, which always preserves topology.
         *  @return The result point.
         */
        Point* collapseEdge( HalfEdge* half );
        
        /** Collapse a face into a point (its first corner).
         *  Totally 2 points, 6 edges and 4 faces will be deleted, which always preserves topology.
         *  @return The result point.
         */
        Point* collapseFace( Face* face );
        
        /** Merge two points into one point.
         *  @return FALSE if unable to merge points.
         */
        bool attachPoints( Point* pt1, Point* pt2 );
        
        /** Separate a point into N points, where N = faces attached to this point.
         *  Halfedges for newly allocated points will also be created.
         *  @return The points list.
         */
        PointList detachPoint( Point* pt );
        
        /** Merge two pairs of halfedges arrived at the same point 'pt' into one pair.
         *  The point should be shared with ONLY two pairs of halfedges, that is, its valence should be 2.
         *  @return A result halfedge or NULL.
         */
        HalfEdge* mergeEdges( Point* pt );
        
        /** Merge two faces attached with a pair of halfedges (one of which is 'half') into one face.
         *  Simply merge them, no matter whether the result is concave or not.
         *  @return The result face or NULL.
         */
        Face* mergeFaces( HalfEdge* half );
        
        /** Flip a pair of halfedges to spin faces on both sides.
         *  The endpoint of each halfedge will move to their next parts.
         *  @return FALSE if unable to flip the edge.
         */
        bool flipEdge( HalfEdge* half );
        
        /** Split a pair of halfedges into two pairs sharing the same point 'pt'.
         *  This function will increase the size of faces linked with origin halfedges.
         *  @return The splitted edges list (only record one halfedge of each).
         */
        HalfEdgeList splitEdge( HalfEdge* half, Point* pt );
        
        /** Split a face into two faces sharing a new edge 'pt1' to 'pt2'.
         *  This function will add one more faces besides the original one.
         *  @return The newly allocated face or NULL.
         */
        Face* splitFace( Face* face, Point* pt1, Point* pt2 );
        
        /** Split an n-sized face into n triangles sharing the same point 'pt'.
         *  This function will add n-1 more faces besides the original one.
         *  @return The splitted faces list.
         */
        FaceList splitFace( Face* face, Point* pt );
        
        /** Split an n-sized face into n quad sidewalls and a n-sized ceiling (but not extruded).
         *  This function does pre-extruding operation of a face actually, with a scale parameter
         *  to define size of the new ceiling face (0.0 - 1.0).
         *  The original face will be reset as the ceiling and n more faces will be added.
         *  @return The splitted faces list.
         */
        FaceList splitFace( Face* face, float scale );
        
        /** Triangulate an n-sized face into n-2 triangles.
         *  The function will add n-3 more faces besides the original one.
         *  Simply split a face into triangles, no matter whether it's concave or not.
         *  @return The triangles list.
         */
        FaceList triangulateFace( Face* face );
        
    // Useful manipulation methods
    public:
        /** Intersect the mesh with a linesegment and return number of results and related element list. */
        unsigned int intersect( const osg::Vec3& s, const osg::Vec3& e, PointList& results, float bias=10.0f );
        unsigned int intersect( const osg::Vec3& s, const osg::Vec3& e, HalfEdgeList& results, float bias=10.0f );
        unsigned int intersect( const osg::Vec3& s, const osg::Vec3& e, FaceList& results );
        
        /** Intersect the mesh with a polytope and return number of results and related element list. */
        unsigned int intersect( const osg::Polytope& polytope, PointList& results );
        unsigned int intersect( const osg::Polytope& polytope, HalfEdgeList& results );
        unsigned int intersect( const osg::Polytope& polytope, FaceList& results );
        
        /** Get the result of Euler's formula: V - E + F.
         *  It equals to 2C - 2G - B, where C = connections, G = genus and B = boundaries.
         */
        int getEuler() const
        { return _pointSet.size() + _faceList.size() - 0.5*_halfEdgeMap.size(); }
        
        /** Get or create a half edge from start to end. */
        HalfEdge* getOrCreateHalfEdge( Point* start, Point* end );
        
        void writePoint( std::ostream& out, Point* pt ) const;
        void writeHalfEdge( std::ostream& out, HalfEdge* half ) const;
        void writeFace( std::ostream& out, Face* face ) const;
        
    // General public methods
    public:
        PolygonalMesh();
        PolygonalMesh( const PolygonalMesh& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
        META_Object( osgModeling, PolygonalMesh );
        
        PointSet& getPoints() { return _pointSet; }
        const PointSet& getPoints() const { return _pointSet; }
        
        HalfEdgeMap& getHalfEdges() { return _halfEdgeMap; }
        const HalfEdgeMap& getHalfEdges() const { return _halfEdgeMap; }
        
        FaceList& getFaces() { return _faceList; }
        const FaceList& getFaces() const { return _faceList; }
        
        /** Compute the bounding box. */
        virtual osg::BoundingBox computeBound() const;
        
        /** The actual implementation of OpenGL drawing calls. TODO: not implemented! */
        virtual void drawImplementation( osg::RenderInfo& renderInfo ) const;
        
        virtual bool supports( const AttributeFunctor& ) const { return false; }
        virtual void accept( AttributeFunctor& ) {}
        
        virtual bool supports( const ConstAttributeFunctor& ) const { return false; }
        virtual void accept( ConstAttributeFunctor& ) const {}
        
        virtual bool supports( const osg::PrimitiveFunctor& ) const { return true; }
        virtual void accept( osg::PrimitiveFunctor& ) const;
        
        virtual bool supports( const osg::PrimitiveIndexFunctor& ) const { return true; }
        virtual void accept( osg::PrimitiveIndexFunctor& ) const;
        
        /** Clear all topological data. */
        void clear();
        
        /** Save the polygonal mesh to a geometry. */
        bool saveGeometry( osg::Geometry& geom );
        
        /** Set if we should render half-edges as lines, which will reduce the performance */
        void setShowHalfEdges( bool b ) { _showHalfEdges = b; }
        bool getShowHalfEdges() const { return _showHalfEdges; }
        
        /** Set if we should apply texture coordinates if any */
        void setUseTexCoords( bool b ) { _useTexCoords = b; }
        bool getUseTexCoords() const { return _useTexCoords; }
        
        /** Set if showing mesh faces */
        void setShowFaces( bool b ) { _showFaces = b; }
        bool getShowFaces() const { return _showFaces; }
        
        /** Set if showing mesh points */
        void setShowPoints( bool b ) { _showPoints = b; }
        bool getShowPoints() const { return _showPoints; }
        
        /** Set if showing mesh edges */
        void setShowEdges( bool b ) { _showEdges = b; }
        bool getShowEdges() const { return _showEdges; }
        
    protected:
        virtual ~PolygonalMesh();
        
        template <typename T>
        void traversePolygonElements( T& drawer ) const;
        
        void drawVertices( const PointList& ptList ) const;
        void drawHalfEdges( const HalfEdgeList& edgeList, const osg::Vec3& center ) const;
        void getDrawingParameters( HalfEdge* half, const osg::Vec3& refCenter, float offsetRatio,
                                   osg::Vec3& start, osg::Vec3& end ) const;
        
        PointSet _pointSet;
        HalfEdgeMap _halfEdgeMap;
        FaceList _faceList;
        
        unsigned long _id;
        bool _showHalfEdges;
        bool _useTexCoords;
        
        bool _showFaces;
        bool _showPoints;
        bool _showEdges;
    };
    
    // INLINE METHODS
    
    template <typename T> void PolygonalMesh::traversePolygonElements( T& drawer ) const
    {
        typedef std::vector<PointList> StandbyQueue;
        StandbyQueue standbyQuads, standbyPolygons;
        
        drawer.getFunctor()->begin( GL_TRIANGLES );
        for ( FaceList::const_iterator itr=_faceList.begin(); itr!=_faceList.end(); ++itr )
        {
            Face* face = (*itr).get();
            if ( !face->valid() ) continue;
            
            PointList ptList = face->getPoints();
            unsigned int size = ptList.size();
            switch ( size )
            {
            case 3:
                drawer.drawVertices( ptList );
                break;
            case 4:
                standbyQuads.push_back( ptList );
                break;
            default:
                if ( size>4 )
                    standbyPolygons.push_back( ptList );
                break;
            }
        }
        drawer.getFunctor()->end();
        
        if ( standbyQuads.size()>0 )
        {
            drawer.getFunctor()->begin( GL_QUADS );
            for ( StandbyQueue::const_iterator itr=standbyQuads.begin(); itr!=standbyQuads.end(); ++itr )
            {
                drawer.drawVertices( *itr );
            }
            drawer.getFunctor()->end();
        }
        
        if ( standbyPolygons.size()>0 )
        {
            for ( StandbyQueue::const_iterator itr=standbyPolygons.begin(); itr!=standbyPolygons.end(); ++itr )
            {
                drawer.getFunctor()->begin( GL_POLYGON );
                drawer.drawVertices( *itr );
                drawer.getFunctor()->end();
            }
        }
    }

}  // namespace osgModeling

#endif
