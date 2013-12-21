/* -*-c++-*- osgXI - Copyright(C) 2010-2012 Wang Guanqun & Wang Rui.
*
* This module is licensed based on the GNU Lesser General Public License
* version 2.1 ("LGPL"), which is located at:
* http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#ifndef OSGMODELING_BASEELEMENT_H
#define OSGMODELING_BASEELEMENT_H

#include <osgModeling/Export>

#include <osg/Geometry>
#include <vector>
#include <set>

namespace osgModeling
{

    class Point;
    class HalfEdge;
    class Face;
    
    typedef std::pair<short, osg::Vec4> TexCoordPair;
    typedef std::vector<TexCoordPair> TexCoordList;
    typedef std::vector<float> FloatList;
    typedef std::vector< osg::ref_ptr<Point> > PointList;
    typedef std::vector< osg::ref_ptr<HalfEdge> > HalfEdgeList;
    typedef std::vector< osg::ref_ptr<Face> > FaceList;
    
    class OSGMODELING_EXPORT Point : public osg::Referenced
    {
    public:
        Point( unsigned long id ) : _id(id), _edge(0) {}
        Point( unsigned long id, const osg::Vec3& v ) : _id(id), _vertex(v), _edge(0) {}
        bool valid() const { return _edge.valid(); }
        
        unsigned long id() const { return _id; }
        
        void setEdge( HalfEdge* edge ) { _edge = edge; }
        HalfEdge* getEdge() { return _edge.get(); }
        const HalfEdge* getEdge() const { return _edge.get(); }
        
        void setValue( const osg::Vec3& v ) { _vertex = v; }
        osg::Vec3& getValue() { return _vertex; }
        const osg::Vec3& getValue() const { return _vertex; }
        
        void setNormal( const osg::Vec3& v ) { _normal = v; }
        osg::Vec3& getNormal() { return _normal; }
        const osg::Vec3& getNormal() const { return _normal; }
        
        TexCoordList& getTexCoords() { return _texcoords; }
        const TexCoordList& getTexCoords() const { return _texcoords; }
        
        inline bool setBoundaryEdge();         /**< Find a boundary halfedge and bind the point to it */
        inline unsigned int computeValence() const;   /**< Number of halfedges departing from the point */
        
        inline bool isBoundary() const;
        inline bool isAdjacent( Point* pt ) const;
        inline bool isAdjacent( Face* face ) const;
        
        inline PointList getAdjacentPoints();       /**< All points adjacent to the point */
        inline HalfEdgeList getAdjacentHalfEdges(); /**< All halfedges emanating from the point */
        inline FaceList getAdjacentFaces();         /**< All faces attached with the point */
        
        void clear() { _edge = 0; }
        bool operator<( const Point& rhs ) const { return _id<rhs._id; }
        
    protected:
        unsigned long _id;
        osg::Vec3 _vertex;
        osg::Vec3 _normal;
        TexCoordList _texcoords;      /**< Texcoords (number of elements, and values) */
        osg::ref_ptr<HalfEdge> _edge; /**< A half-edge emanating from this point */
    };
    
    class OSGMODELING_EXPORT HalfEdge : public osg::Referenced
    {
    public:
        HalfEdge( Point* pt ) : _end(pt), _face(0), _twin(0), _next(0), _prev(0) {}
        bool unlinked() const { return !(_next.valid() && _prev.valid()); }
        bool valid() const { return _end.valid() && _twin.valid() && !unlinked(); }
        
        Point* getStart() { return _twin->_end.get(); }
        const Point* getStart() const { return _twin->_end.get(); }
        
        Point* getEnd() { return _end.get(); }
        const Point* getEnd() const { return _end.get(); }
        
        void setLeft( Face* f ) { _face = f; }
        Face* getLeft() { return _face.get(); }                     /**< The face at current halfedge's side */
        const Face* getLeft() const { return _face.get(); }         /**< The face at current halfedge's side */
        
        void setRight( Face* f ) { _twin->_face = f; }
        Face* getRight() { return _twin->_face.get(); }             /**< The face at twin halfedge's side */
        const Face* getRight() const { return _twin->_face.get(); } /**< The face at twin halfedge's side */
        
        void setTwin( HalfEdge* half ) { _twin = half; if (half) half->_twin = this; }
        HalfEdge* getTwin() { return _twin.get(); }
        const HalfEdge* getTwin() const { return _twin.get(); }
        
        inline void setNext( HalfEdge* half );
        HalfEdge* getNext() { return _next.get(); }
        const HalfEdge* getNext() const { return _next.get(); }
        
        HalfEdge* getPrev() { return _prev.get(); }
        const HalfEdge* getPrev() const { return _prev.get(); }
        
        HalfEdge* getRotateNext() { return _twin->_next.get(); }
        const HalfEdge* getRotateNext() const { return _twin->_next.get(); }
        
        HalfEdge* getRotatePrev() { return _twin->_prev.get(); }
        const HalfEdge* getRotatePrev() const { return _twin->_prev.get(); }
        
        osg::Vec3& operator[]( unsigned int i ) { return i==0?getStart()->getValue():getEnd()->getValue(); }
        const osg::Vec3& operator[]( unsigned int i ) const
        { return i==0?getStart()->getValue():getEnd()->getValue(); }
        
        osg::Vec3 getTangent() const { return getEnd()->getValue() - getStart()->getValue(); }
        double computeLength() const { return getTangent().length(); }
        inline double computeAngle() const;    /**< Compute corner angle between this and next halfedges */
        
        bool isBoundary() const { return !_face.valid(); }
        bool isDegenerate() const { return _end==_twin->_end; }
        
        inline HalfEdgeList getCirculations(); /**< The whole circulations of the halfedge */
        
        void clear() { _end = 0; _face = 0; _twin = 0; _next = 0; _prev = 0; }
        void setEnd( Point* pt ) { _end = pt; }  /**< Used internal. NOT recommended for public users. */
        
    protected:
        osg::ref_ptr<Point> _end;     /**< the point at the end of this half-edge */
        osg::ref_ptr<Face> _face;     /**< the face which this half-edge borders */
        osg::ref_ptr<HalfEdge> _twin; /**< the oppositely oriented adjacent half-edge */
        osg::ref_ptr<HalfEdge> _next; /**< the next half-edge around a face */
        osg::ref_ptr<HalfEdge> _prev; /**< the previous half-edge around a face */
    };
    
    class OSGMODELING_EXPORT Face : public osg::Referenced
    {
    public:
        Face() : _border(0) {}
        bool valid() const { return _border.valid(); }
        
        void setBorder( HalfEdge* edge ) { _border = edge; }
        HalfEdge* getBorder() { return _border.get(); }
        const HalfEdge* getBorder() const { return _border.get(); }
        
        inline unsigned int computeSize() const;
        inline osg::Vec3 computeNormal( bool autoNormalize=true ) const;
        inline osg::Vec3 computeCentroid() const;
        
        inline bool isBoundary() const;
        inline bool isDegenerate() const;
        inline bool contains( Point* pt ) const;
        inline bool contains( HalfEdge* half ) const;
        inline bool isAdjacent( Face* face ) const;
        
        inline PointList getPoints();        /**< All points attached with the face */
        inline HalfEdgeList getBorders();    /**< All halfedges bordering the face */
        inline FaceList getAdjacentFaces();  /**< All faces adjacent to the face */
        
        void clear() { _border = 0; }
        
    protected:
        osg::ref_ptr<HalfEdge> _border; /**< a half-edge bordering this face */
    };
    
    // INLINE METHODS
    
    #define TRAVERSE_THIS_POINT_BEGIN( edge ) \
        HalfEdge* edge = _edge.get(); do { if ( !edge ) break;
    #define TRAVERSE_THIS_POINT_END( edge ) \
        edge = edge->getRotateNext(); } while ( _edge!=edge );
    
    #define TRAVERSE_THIS_FACE_BEGIN( edge ) \
        HalfEdge* edge = _border.get(); do { if ( !edge ) break;
    #define TRAVERSE_THIS_FACE_END( edge ) \
        edge = edge->getNext(); } while ( _border!=edge );
    
    bool Point::setBoundaryEdge()
    {
        TRAVERSE_THIS_POINT_BEGIN( edge )
            if ( edge->isBoundary() )
            {
                setEdge( edge );
                return true;
            }
        TRAVERSE_THIS_POINT_END( edge )
        return false;
    }
    
    unsigned int Point::computeValence() const
    {
        unsigned int count = 0;
        TRAVERSE_THIS_POINT_BEGIN( edge );
            count++;
        TRAVERSE_THIS_POINT_END( edge );
        return count;
    }
    
    bool Point::isBoundary() const
    {
        TRAVERSE_THIS_POINT_BEGIN( edge );
            if ( edge->isBoundary() ) return true;
        TRAVERSE_THIS_POINT_END( edge );
        return false;
    }
    
    bool Point::isAdjacent( Point* pt ) const
    {
        TRAVERSE_THIS_POINT_BEGIN( edge );
            if ( edge->getEnd()==pt ) return true;
        TRAVERSE_THIS_POINT_END( edge );
        return false;
    }
    
    bool Point::isAdjacent( Face* face ) const
    {
        TRAVERSE_THIS_POINT_BEGIN( edge );
            if ( edge->getLeft()==face ) return true;
        TRAVERSE_THIS_POINT_END( edge );
        return false;
    }
    
    PointList Point::getAdjacentPoints()
    {
        PointList points;
        TRAVERSE_THIS_POINT_BEGIN( edge );
            points.push_back( edge->getEnd() );
        TRAVERSE_THIS_POINT_END( edge );
        return points;
    }
    
    HalfEdgeList Point::getAdjacentHalfEdges()
    {
        HalfEdgeList edges;
        TRAVERSE_THIS_POINT_BEGIN( edge );
            edges.push_back( edge );
        TRAVERSE_THIS_POINT_END( edge );
        return edges;
    }
    
    FaceList Point::getAdjacentFaces()
    {
        FaceList faces;
        TRAVERSE_THIS_POINT_BEGIN( edge );
            if ( !edge->isBoundary() )
                faces.push_back( edge->getLeft() );
        TRAVERSE_THIS_POINT_END( edge );
        return faces;
    }
    
    void HalfEdge::setNext( HalfEdge* half )
    {
        _next = half;
        if ( half ) half->_prev = this;
        
        if ( !half )
            osg::notify(osg::WARN) << "setNext(): Null halfedge as next?" << std::endl;
        /*else if ( half->isBoundary()!=this->isBoundary() )
        {
            osg::notify(osg::WARN) << "setNext(): Linking boundary and unboundary halfedge?" << std::endl;
        }*/
    }
    
    double HalfEdge::computeAngle() const
    {
        osg::Vec3 e1 = -getTangent(), e2 = getNext()->getTangent();
        e1.normalize();
        e2.normalize();
        return acos( e1*e2 );
    }
    
    HalfEdgeList HalfEdge::getCirculations()
    {
        HalfEdgeList edges;
        edges.push_back( this );
        HalfEdge* edge = this->getNext();
        while ( edge && edge!=this )
        {
            edges.push_back( edge );
            edge = edge->getNext();
        }
        return edges;
    }
    
    unsigned int Face::computeSize() const
    {
        unsigned int size = 0;
        TRAVERSE_THIS_FACE_BEGIN( edge );
            size++;
        TRAVERSE_THIS_FACE_END( edge );
        return size;
    }
    
    osg::Vec3 Face::computeNormal( bool autoNormalize ) const
    {
        osg::Vec3 normal;
        TRAVERSE_THIS_FACE_BEGIN( edge );
            if ( edge->getNext() )
            {
                normal += edge->getTangent() ^ edge->getNext()->getTangent();
                normal *= edge->computeAngle();
            }
        TRAVERSE_THIS_FACE_END( edge );
        if ( autoNormalize ) normal.normalize();
        return normal;
    }
    
    osg::Vec3 Face::computeCentroid() const
    {
        unsigned int size = 0;
        osg::Vec3 centroid;
        TRAVERSE_THIS_FACE_BEGIN( edge );
            size++;
            centroid += (*edge)[1];
        TRAVERSE_THIS_FACE_END( edge );
        if ( size>0 ) centroid /= size;
        return centroid;
    }
    
    bool Face::isBoundary() const
    {
        TRAVERSE_THIS_FACE_BEGIN( edge );
            if ( edge->getTwin()->isBoundary() ) return true;
        TRAVERSE_THIS_FACE_END( edge );
        return false;
    }
    
    bool Face::isDegenerate() const
    {
        if ( _border->getPrev()==_border->getNext() ) return true;
        
        TRAVERSE_THIS_FACE_BEGIN( edge );
            if ( edge->isDegenerate() ) return true;
        TRAVERSE_THIS_FACE_END( edge );
        return false;
    }
    
    bool Face::contains( Point* pt ) const
    {
        TRAVERSE_THIS_FACE_BEGIN( edge );
            if ( edge->getEnd()==pt ) return true;
        TRAVERSE_THIS_FACE_END( edge );
        return false;
    }
    
    bool Face::contains( HalfEdge* half ) const
    {
        TRAVERSE_THIS_FACE_BEGIN( edge );
            if ( edge==half ) return true;
        TRAVERSE_THIS_FACE_END( edge );
        return false;
    }
    
    bool Face::isAdjacent( Face* face ) const
    {
        if ( face==this ) return true;
        TRAVERSE_THIS_FACE_BEGIN( edge );
            if ( edge->getRight()==face ) return true;
        TRAVERSE_THIS_FACE_END( edge );
        return false;
    }
    
    PointList Face::getPoints()
    {
        PointList points;
        TRAVERSE_THIS_FACE_BEGIN( edge );
            points.push_back( edge->getEnd() );
        TRAVERSE_THIS_FACE_END( edge );
        return points;
    }
    
    HalfEdgeList Face::getBorders()
    {
        HalfEdgeList edges;
        TRAVERSE_THIS_FACE_BEGIN( edge );
            edges.push_back( edge );
        TRAVERSE_THIS_FACE_END( edge );
        return edges;
    }
    
    FaceList Face::getAdjacentFaces()
    {
        FaceList faces;
        TRAVERSE_THIS_FACE_BEGIN( edge );
            if ( !edge->getTwin()->isBoundary() )
                faces.push_back( edge->getRight() );
        TRAVERSE_THIS_FACE_END( edge );
        return faces;
    }

}  // namespace osgModeling

#endif
