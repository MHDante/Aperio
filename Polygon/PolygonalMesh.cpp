/* -*-c++-*- osgXI - Copyright(C) 2010-2012 Wang Guanqun & Wang Rui.
*
* This module is licensed based on the GNU Lesser General Public License
* version 2.1 ("LGPL"), which is located at:
* http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
*/

#include <osg/GL>
#include <osg/io_utils>
#include <osgUtil/SmoothingVisitor>
#include <osgUtil/TriStripVisitor>
#include <osgModeling/PolygonalMesh>
#include <iostream>
#include <algorithm>

using namespace osgModeling;

#define TRAVERSE_POINT_BEGIN( edge, pt ) \
    HalfEdge* edge = pt->getEdge(); do { if ( !edge ) break;
#define TRAVERSE_POINT_END( edge, pt ) \
    edge = edge->getRotateNext(); } while ( pt->getEdge()!=edge );

#define TRAVERSE_FACE_BEGIN( edge, f ) \
    HalfEdge* edge = f->getBorder(); do { if ( !edge ) break;
#define TRAVERSE_FACE_END( edge, f ) \
    edge = edge->getNext(); } while ( f->getBorder()!=edge );

PolygonalMesh::PolygonalMesh()
:   _id(1), _showHalfEdges(false), _useTexCoords(true),
    _showFaces(true), _showPoints(false), _showEdges(false)
{
    setUseDisplayList( false );
    setSupportsDisplayList( false );
}

PolygonalMesh::PolygonalMesh( const PolygonalMesh &copy, const osg::CopyOp &copyop )
:   osg::Drawable(copy, copyop),
    _pointSet(copy._pointSet), _halfEdgeMap(copy._halfEdgeMap), _faceList(copy._faceList),
    _id(copy._id), _showHalfEdges(copy._showHalfEdges), _useTexCoords(copy._useTexCoords),
    _showFaces(copy._showFaces), _showPoints(copy._showPoints), _showEdges(copy._showEdges)
{
}

PolygonalMesh::~PolygonalMesh()
{
    clear();
}

// STATIC METHODS

static osg::Vec4 g_displayFaceColor(1.0f, 1.0f, 1.0f, 1.0f);
static osg::Vec4 g_displayPointColor(1.0f, 0.0f, 0.0f, 1.0f);
static osg::Vec4 g_displayEdgeColor(1.0f, 0.0f, 0.0f, 1.0f);
static osg::Vec4 g_displayHalfEdgeColor(0.0f, 0.0f, 1.0f, 1.0f);
static float g_displayPointSize = 5.0f;
static float g_displayEdgeWidth = 2.0f;

void PolygonalMesh::setDisplayFaceColor( const osg::Vec4& color ) { g_displayFaceColor = color; }
const osg::Vec4& PolygonalMesh::getDisplayFaceColor() { return g_displayFaceColor; }

void PolygonalMesh::setDisplayPointColor( const osg::Vec4& color ) { g_displayPointColor = color; }
const osg::Vec4& PolygonalMesh::getDisplayPointColor() { return g_displayPointColor; }

void PolygonalMesh::setDisplayEdgeColor( const osg::Vec4& color ) { g_displayEdgeColor = color; }
const osg::Vec4& PolygonalMesh::getDisplayEdgeColor() { return g_displayEdgeColor; }

void PolygonalMesh::setDisplayHalfEdgeColor( const osg::Vec4& color ) { g_displayHalfEdgeColor = color; }
const osg::Vec4& PolygonalMesh::getDisplayHalfEdgeColor() { return g_displayHalfEdgeColor; }

void PolygonalMesh::setDisplayPointSize( float size ) { g_displayPointSize = size; }
float PolygonalMesh::getDisplayPointSize() { return g_displayPointSize; }

void PolygonalMesh::setDisplayEdgeWidth( float width ) { g_displayEdgeWidth = width; }
float PolygonalMesh::getDisplayEdgeWidth() { return g_displayEdgeWidth; }

// BASIC PUBLIC METHODS

Point* PolygonalMesh::addPoint( const osg::Vec3& v, const osg::Vec3& n )
{
    Point* pt = new Point( _id++, v );
    pt->setNormal( n );
    _pointSet.insert( pt );
    return pt;
}

bool PolygonalMesh::removePoint( Point* pt )
{
    if ( !pt ) return false;
    HalfEdge* edge = pt->getEdge(), *curr = NULL;
    do
    {
        curr = edge;
        edge = edge->getRotateNext();
        if ( curr==edge || curr==edge->getTwin() )
            edge = edge->getRotateNext();
        removeEdge( curr );
    } while ( curr!=edge );
    
    return erase(pt);
}

HalfEdge* PolygonalMesh::addEdge( Point* p1, Point* p2 )
{
    HalfEdge* half12 = new HalfEdge( p2 );
    HalfEdge* half21 = new HalfEdge( p1 );
    half12->setTwin( half21 );
    
    _halfEdgeMap[HalfEdgeID(p1->id(), p2->id())] = half12;
    _halfEdgeMap[HalfEdgeID(p2->id(), p1->id())] = half21;
    return half12;
}

bool PolygonalMesh::relinkEdge( HalfEdge* half, Point* end )
{
    if ( !half || !end ) return false;
    
    unsigned long id1 = half->getStart()->id(), id2 = half->getEnd()->id();
    HalfEdgeMap::iterator itr = _halfEdgeMap.find( HalfEdgeID(id1, id2) );
    if ( itr!=_halfEdgeMap.end() ) _halfEdgeMap.erase( itr );
    
    HalfEdgeID edgeID( id1, end->id() );
    if ( _halfEdgeMap.find(edgeID)!=_halfEdgeMap.end() )
    {
        OSG_WARN << "relinkEdge(): Will make a non-manifold halfedge." << std::endl;
        return false;
    }
    
    half->setEnd( end );
    _halfEdgeMap[ edgeID ] = half;
    return true;
}

bool PolygonalMesh::removeEdge( HalfEdge* half )
{
    if ( !half ) return false;
    HalfEdge* twin = half->getTwin();
    
    removeFace( half->getLeft() );
    removeFace( twin->getLeft() );
    
    Point* pt = twin->getEnd();
    HalfEdge* eIn = half->getPrev();
    HalfEdge* eOut = twin->getNext();
    if ( pt->getEdge()==half )
        pt->setEdge( eOut==half ? NULL : eOut );
    eIn->setNext( eOut );
    
    pt = half->getEnd();
    eIn = twin->getPrev();
    eOut = half->getNext();
    if ( pt->getEdge()==twin )
        pt->setEdge( eOut==half ? NULL : eOut );
    eIn->setNext( eOut );
    
    return erase(half);
}

Face* PolygonalMesh::addFace()
{
    Face* face = new Face;
    _faceList.push_back( face );
    return face;
}

Face* PolygonalMesh::addFace( const HalfEdgeList& edges )
{
    typedef std::pair<HalfEdge*, HalfEdge*> SettingNextPair;
    typedef std::vector<SettingNextPair> SettingNextList;
    SettingNextList settingNextList;
    PointList pointsToReset;
    
    unsigned int size = edges.size();
    if ( size<3 )
    {
        OSG_WARN << "addFace(): Find degenerated face." << std::endl;
        return NULL;
    }
    
    // Test topological structure
    for ( unsigned int i=0; i<size; ++i )
    {
        HalfEdge* e1 = edges[i % size].get();
        HalfEdge* e2 = edges[(i+1) % size].get();
        if ( !e1->isBoundary() )
        {
            OSG_WARN << "addFace(): Find non-manifold edge." << std::endl;
            return NULL;
        }
        
        if ( e1->getEnd()!=e2->getStart() )
        {
            OSG_WARN << "addFace(): Find disjoined edges." << std::endl;
            return NULL;
        }
    }
    
    // Relink halfedge patches
    for ( unsigned int i=0; i<size; ++i )
    {
        HalfEdge* e1 = edges[i % size].get();
        HalfEdge* e2 = edges[(i+1) % size].get();
        if ( e1->unlinked() || e2->unlinked() ) continue;
        if ( e1->getNext()==e2 ) continue;
        
        HalfEdge* out1 = e2->getTwin(), *out2 = e1->getTwin();
        HalfEdge* boundary1 = out1, *boundary2 = NULL;
        do
        {
            if ( !boundary1 ) break;
            boundary1 = boundary1->getNext()->getTwin();
        } while ( !boundary1->isBoundary() && boundary1!=e1 && boundary1!=out1 );
        
        if ( !boundary1 ) continue;
        boundary2 = boundary1->getNext();
        if ( boundary2==e2 || boundary1==out1 )
        {
            OSG_WARN << "addFace(): Invalid relinking." << std::endl;
            return NULL;
        }
        /*else if ( !boundary1->isBoundary() || !boundary2->isBoundary() )
        {
            OSG_WARN << "addFace(): Invalid boundaries." << std::endl;
            return NULL;
        }*/
        
        settingNextList.push_back( SettingNextPair(boundary1, e1->getNext()) );
        settingNextList.push_back( SettingNextPair(e2->getPrev(), boundary2) );
        settingNextList.push_back( SettingNextPair(e1, e2) );
    }
    
    // Create the face
    Face* face = new Face;
    face->setBorder( edges.back() );
    
    // Link newly created halfedges
    for ( unsigned int i=0; i<size; ++i )
    {
        HalfEdge* e1 = edges[i % size].get();
        HalfEdge* e2 = edges[(i+1) % size].get();
        HalfEdge* out1 = e2->getTwin(), *out2 = e1->getTwin();
        Point* pt = e1->getEnd();
        e1->setLeft( face );
        
        if ( e1->unlinked() || e2->unlinked() )
        {
            if ( e1->unlinked() && e2->unlinked() )
            {
                if ( !pt->getEdge() )
                {
                    settingNextList.push_back( SettingNextPair(out1, out2) );
                    pt->setEdge( out2 );
                }
                else
                {
                    HalfEdge* boundary2 = pt->getEdge(), *boundary1 = boundary2->getPrev();
                    settingNextList.push_back( SettingNextPair(boundary1, out2) );
                    settingNextList.push_back( SettingNextPair(out1, boundary2) );
                }
            }
            else if ( e1->unlinked() )
            {
                HalfEdge* boundary1 = e2->getPrev();
                settingNextList.push_back( SettingNextPair(boundary1, out2) );
                pt->setEdge( out2 );
            }
            else if ( e2->unlinked() )
            {
                HalfEdge* boundary2 = e1->getNext();
                settingNextList.push_back( SettingNextPair(out1, boundary2) );
                pt->setEdge( boundary2 );
            }
            settingNextList.push_back( SettingNextPair(e1, e2) );
        }
        else
        {
            if ( pt->getEdge()==e2 )
                pointsToReset.push_back( pt );
        }
    }
    
    // Apply setting next functions
    for ( SettingNextList::iterator itr=settingNextList.begin();
          itr!=settingNextList.end(); ++itr )
    {
        itr->first->setNext( itr->second );
    }
    
    for ( PointList::iterator itr=pointsToReset.begin();
          itr!=pointsToReset.end(); ++itr )
    {
        (*itr)->setBoundaryEdge();
    }
    
    _faceList.push_back( face );
    return face;
}

Face* PolygonalMesh::addFace( const PointList& points )
{
    HalfEdgeList edges;
    unsigned int size = points.size();
    for ( unsigned int i=0; i<size; ++i )
    {
        Point* p1 = points[i % size];
        Point* p2 = points[(i+1) % size];
        HalfEdge* half = getOrCreateHalfEdge( p1, p2 );
        if ( half ) edges.push_back( half );
    }
    return addFace( edges );
}

bool PolygonalMesh::removeFace( Face* face )
{
    if ( !face ) return false;
    TRAVERSE_FACE_BEGIN( edge, face );
        edge->setLeft( NULL );
    TRAVERSE_FACE_END( edge, face );
    
    return erase(face);
}

bool PolygonalMesh::erase( Point* pt )
{
    unsigned int numRemoved = _pointSet.erase( pt );
    pt->clear();
    return numRemoved>0;
}

bool PolygonalMesh::erase( HalfEdge* half )
{
    bool ok = true;
    HalfEdge* twin = half->getTwin();
    unsigned long id1 = twin->getEnd()->id(), id2 = half->getEnd()->id();
    
    HalfEdgeMap::iterator itr = _halfEdgeMap.find( HalfEdgeID(id1, id2) );
    if ( itr!=_halfEdgeMap.end() && half==itr->second ) _halfEdgeMap.erase( itr );
    else ok = false;
    
    itr = _halfEdgeMap.find( HalfEdgeID(id2, id1) );
    if ( itr!=_halfEdgeMap.end() && twin==itr->second ) _halfEdgeMap.erase( itr );
    else ok = false;
    
    half->clear();
    twin->clear();
    return ok;
}

bool PolygonalMesh::erase( Face* face )
{
    bool ok = true;
    FaceList::iterator itr = std::find(_faceList.begin(), _faceList.end(), face);
    if ( itr!=_faceList.end() ) _faceList.erase( itr );
    else ok = false;
    
    face->clear();
    return ok;
}

// USER MODIFICATION METHODS

bool PolygonalMesh::removeDigon( Face* face )
{
    if ( !face ) return false;
    
    HalfEdge* e1 = face->getBorder(), *e2 = e1->getNext();
    if ( !e2 || e1->getPrev()!=e2 ) return false;
    
    HalfEdge* twin1 = e1->getTwin(), *twin2 = e2->getTwin();
    Face* left = e1->getLeft(), *right = twin1->getLeft();
    Point* start = twin2->getEnd(), *end = e2->getEnd();
    
    // Update halfedges
    e2->setLeft( right );
    e2->setNext( twin1->getNext() );
    twin1->getPrev()->setNext( e2 );
    
    // Update points
    if ( end->getEdge()==e1 )
    {
        end->setEdge( twin2 );
        end->setBoundaryEdge();
    }
    if ( start->getEdge()==twin1 )
    {
        start->setEdge( e2 );
        start->setBoundaryEdge();
    }
    
    // Update faces
    if ( right )
    {
        if ( right->getBorder()==twin1 )
            right->setBorder( e2 );
    }
    
    // Erase unused data
    erase( e1 );
    erase( left );
    return true;
}

bool PolygonalMesh::isEdgeCollapsible( HalfEdge* half )
{
    if ( !half ) return false;
    
    HalfEdge* twin = half->getTwin();
    if ( !half->valid() || !twin->valid() || !half->getLeft() || !half->getRight() )
        return false;
    
    Point* start = twin->getEnd(), *end = half->getEnd();
    PointList list1 = start->getAdjacentPoints();
    PointList list2 = end->getAdjacentPoints();
    
    unsigned int count = 0;
    for ( PointList::iterator itr=list1.begin(); itr!=list1.end(); ++itr )
    {
        PointList::iterator itr2 = std::find(list2.begin(), list2.end(), *itr);
        if ( itr2!=list2.end() ) count++;
    }
    return count==2;
}

Point* PolygonalMesh::collapseEdge( HalfEdge* half )
{
    if ( !half ) return NULL;
    
    Face* left = half->getLeft(), *right = half->getRight();
    HalfEdge* twin = half->getTwin();
    if ( !half->valid() || !twin->valid() || !left || !right )
        return NULL;
    
    HalfEdge* prev0 = half->getPrev(), *next0 = half->getNext();
    HalfEdge* prev1 = twin->getPrev(), *next1 = twin->getNext();
    Point* start = twin->getEnd(), *end = half->getEnd();
    Point* p1 = next0->getEnd(), *p2 = next1->getEnd();
    Face* f1 = prev0->getRight(), *f2 = next1->getRight();
    
    // Check if collapseing 2 triangles
    if ( next0->getNext()!=prev0 || next1->getNext()!=prev1 )
    {
        OSG_WARN << "collapseEdge(): Not support non-triangular faces." << std::endl;
        return NULL;
    }
    
    // Update halfedges
    HalfEdgeList edges = start->getAdjacentHalfEdges();
    for ( HalfEdgeList::iterator itr=edges.begin(); itr!=edges.end(); ++itr )
    {
        HalfEdge* edge = (*itr)->getTwin();
        if ( edge==twin || edge==prev0 || edge==next1->getTwin() ) continue;
        relinkEdge( edge, end );
    }
    
    next0->setLeft( f1 );
    next0->setNext( prev0->getRotateNext() );
    prev0->getRotatePrev()->setNext( next0 );
    
    prev1->setLeft( f2 );
    prev1->setNext( next1->getRotateNext() );
    next1->getRotatePrev()->setNext( prev1 );
    
    // Update points
    if ( end->getEdge()==twin )
    {
        end->setEdge( next0 );
        end->setBoundaryEdge();
    }
    
    if ( p1->getEdge()==prev0 )
    {
        p1->setEdge( next0->getTwin() );
        p1->setBoundaryEdge();
    }
    if ( p2->getEdge()==next1->getTwin() )
    {
        p2->setEdge( prev1 );
        p2->setBoundaryEdge();
    }
    
    // Update faces
    if ( f1 )
    {
        if ( f1->getBorder()==prev0->getTwin() )
            f1->setBorder( next0 );
    }
    if ( f2 )
    {
        if ( f2->getBorder()==next1->getTwin() )
            f2->setBorder( prev1 );
    }
    
    // Erase unused data
    erase( half ); erase( prev0 ); erase( next1 );
    erase( left ); erase( right );
    erase( start );
    return end;
}

HalfEdge* PolygonalMesh::mergeEdges( Point* pt )
{
    if ( !pt ) return NULL;
    
    HalfEdgeList edges = pt->getAdjacentHalfEdges();
    if ( edges.size()!=2 ) return NULL;
    
    HalfEdge* out1 = edges.front(), *out2 = edges.back();
    HalfEdge* in1 = out1->getTwin(), *in2 = out2->getTwin();
    if ( !out1->valid() || !out2->valid() ) return NULL;
    
    // Update faces
    if ( out1->getLeft() ) out1->getLeft()->setBorder( out1 );
    if ( in1->getLeft() ) in1->getLeft()->setBorder( in1 );
    
    // Update halfedges
    in2->getPrev()->setNext( out1 );
    in1->setNext( out2->getNext() );
    
    // Update points
    Point* end = out2->getEnd();
    relinkEdge( in1, end );
    if ( end->getEdge()==in2 )
    {
        end->setEdge( out1 );
        end->setBoundaryEdge();
    }
    
    // Erase unused data
    erase( out2 );
    erase( pt );
    return out1;
}

Face* PolygonalMesh::mergeFaces( HalfEdge* half )
{
    if ( !half ) return NULL;
    
    Face* left = half->getLeft(), *right = half->getRight();
    HalfEdge* twin = half->getTwin();
    if ( !half->valid() || !twin->valid() || !left || !right )
        return NULL;
    
    HalfEdge* prev0 = half->getPrev(), *next0 = half->getNext();
    HalfEdge* prev1 = twin->getPrev(), *next1 = twin->getNext();
    
    // Update faces
    if ( left->getBorder()==half )
        left->setBorder( next0 );
    HalfEdge* edge = next1;
    do
    {
        edge->setLeft( left );
        edge = edge->getNext();
    } while ( edge && edge!=twin );
    
    // Update halfedges
    prev0->setNext( next1 );
    prev1->setNext( next0 );
    
    // Update points
    Point* start = twin->getEnd(), *end = half->getEnd();
    if ( end->getEdge()==twin )
    {
        end->setEdge( next0 );
        end->setBoundaryEdge();
    }
    if ( start->getEdge()==half )
    {
        start->setEdge( next1 );
        start->setBoundaryEdge();
    }
    
    // Erase unused data
    erase( half );
    erase( right );
    return left;
}

bool PolygonalMesh::flipEdge( HalfEdge* half )
{
    if ( !half ) return false;
    if ( !half->getLeft() || !half->getRight() ) return false;
    
    HalfEdge* twin = half->getTwin();
    if ( !half->valid() || !twin->valid() ) return false;
    
    HalfEdge* prev0 = half->getPrev(), *next0 = half->getNext(), *successor0 = next0->getNext();
    HalfEdge* prev1 = twin->getPrev(), *next1 = twin->getNext(), *successor1 = next1->getNext();
    
    // Update faces
    half->getLeft()->setBorder( prev0 );
    twin->getLeft()->setBorder( prev1 );
    next0->setLeft( twin->getLeft() );
    next1->setLeft( half->getLeft() );
    
    // Update halfedges
    half->setNext( successor0 );
    next1->setNext( half );
    prev0->setNext( next1 );
    
    // Update points
    Point* start = twin->getEnd(), *end = half->getEnd();
    if ( end->getEdge()==twin )
    {
        end->setEdge( next0 );
        end->setBoundaryEdge();
    }
    if ( start->getEdge()==half )
    {
        start->setEdge( next1 );
        start->setBoundaryEdge();
    }
    relinkEdge( half, next0->getEnd() );
    relinkEdge( twin, next1->getEnd() );
    
    twin->setNext( successor1 );
    next0->setNext( twin );
    prev1->setNext( next0 );
    return true;
}

FaceList PolygonalMesh::splitFace( Face* face, Point* pt )
{
    FaceList flist;
    if ( face && pt )
    {
        HalfEdge* e1 = face->getBorder();
        if ( !e1 ) return flist;
        
        HalfEdge* e2 = e1->getNext();
        HalfEdge* e0 = getOrCreateHalfEdge( e1->getEnd(), pt );
        if ( e0->valid() ) return flist;
        
        // Reset current face to one of split faces
        e1->setNext( e0 );
        e0->setLeft( face );
        flist.push_back( face );
        
        // Create other split faces
        e0 = e0->getTwin();
        while ( e1!=e2 )
        {
            HalfEdge* eNext = e2->getNext();
            HalfEdge* eNew = getOrCreateHalfEdge( e2->getEnd(), pt );
            if ( eNew->valid() ) return flist;
            
            Face* fNew = addFace(); fNew->setBorder( e2 );
            flist.push_back( fNew );
            
            eNew->setNext( e0 ); eNew->setLeft( fNew );
            e0->setNext( e2 ); e0->setLeft( fNew );
            e2->setNext( eNew ); e2->setLeft( fNew );
            
            e0 = eNew->getTwin();
            e2 = eNext;
        }
        
        e0->setNext( e1 );
        e1->getNext()->setNext( e0 );
        e0->setLeft( face );
        pt->setEdge( e0 );
    }
    return flist;
}

FaceList PolygonalMesh::triangulateFace( Face* face )
{
    FaceList flist;
    if ( face )
    {
        HalfEdge* e1 = face->getBorder();
        if ( !e1 ) return flist;
        flist.push_back( face );
        
        // Create split triangles
        HalfEdge* e2 = e1->getNext();
        Point* start = e1->getStart();
        while ( e2->getNext()->getEnd()!=start )
        {
            HalfEdge* eNext = e2->getNext();
            HalfEdge* eNew = getOrCreateHalfEdge( e2->getEnd(), start );
            if ( eNew->valid() ) return flist;
            
            Face* fNew = addFace(); fNew->setBorder( e1 );
            flist.push_back( fNew );
            
            e1->setNext( e2 ); e1->setLeft( fNew );
            e2->setNext( eNew ); e2->setLeft( fNew );
            eNew->setNext( e1 ); eNew->setLeft( fNew );
            
            e1 = eNew->getTwin();
            e2 = eNext;
        }
        
        // Reset current face to one of split triangles
        face->setBorder( e1 );
        e1->setLeft( face );
        e1->setNext( e2 );
        e2->getNext()->setNext( e1 );
    }
    return flist;
}

// USER INFORMATION METHODS

HalfEdge* PolygonalMesh::getOrCreateHalfEdge( Point* start, Point* end )
{
    if ( start && end )
    {
        HalfEdgeMap::iterator itr = _halfEdgeMap.find( HalfEdgeID(start->id(), end->id()) );
        if ( itr!=_halfEdgeMap.end() )
            return itr->second.get();
        return addEdge( start, end );
    }
    return NULL;
}

void PolygonalMesh::writePoint( std::ostream& out, Point* pt ) const
{
    out << "Point: " << "ID " << pt->id() << "; (" << pt->getValue() << "); ";
    if ( !pt->valid() )
    {
        out << "HALFEDGE?" << std::endl;
    }
    else
    {
        out << "Valence: " << pt->computeValence() << "; ";
        out << "Boundary: " << pt->isBoundary() << std::endl;
    }
}

void PolygonalMesh::writeHalfEdge( std::ostream& out, HalfEdge* half ) const
{
    out << "HalfEdge: ";
    if ( !half->getEnd() ) out << "ENDPOINT? ";
    else if ( !half->getTwin() ) out << "TWIN? ";
    else if ( !half->getStart() ) out << "TWINENDPOINT? ";
    else
    {
        HalfEdge* twin = half->getTwin();
        out << "(" << half->getStart()->id() << " -> " << half->getEnd()->id() << "); ";
        out << "Twin: " << twin->getStart()->id() << "->" << twin->getEnd()->id() << "; ";
    }
    
    HalfEdge* prev = half->getPrev(), *next = half->getNext();
    if ( !prev ) out << "PREVIOUS? ";
    else out << "Prev: " << prev->getStart()->id() << "->" << prev->getEnd()->id() << "; ";
    if ( !next ) out << "NEXT? ";
    else out << "Next: " << next->getStart()->id() << "->" << next->getEnd()->id() << "; ";
    out << "Boundary: " << half->isBoundary() << std::endl;
}

void PolygonalMesh::writeFace( std::ostream& out, Face* face ) const
{
    out << "Face: ";
    if ( !face->valid() )
    {
        out << "HALFEDGE?" << std::endl;
    }
    else
    {
        PointList ptList = face->getPoints();
        for ( PointList::iterator itr=ptList.begin(); itr!=ptList.end(); ++itr )
            out << (*itr)->id() << "->";
        out << ptList.front()->id() << "; ";
        out << "Size: " << face->computeSize() << "; Normal: " << face->computeNormal() << "; ";
        out << "Boundary: " << face->isBoundary() << std::endl;
    }
}

// GENERAL PUBLIC METHODS

osg::BoundingBox PolygonalMesh::computeBound() const
{
    osg::BoundingBox bb;
    for ( PointSet::const_iterator itr=_pointSet.begin(); itr!=_pointSet.end(); ++itr )
    {
        Point* pt = (*itr).get();
        if ( pt->valid() ) bb.expandBy( pt->getValue() );
    }
    return bb;
}

#define RESET_GL_PRIMITIVE(mode) glEnd(); glBegin(mode);

void PolygonalMesh::drawImplementation( osg::RenderInfo& ) const
{
    typedef std::vector<PointList> StandbyQueue;
    StandbyQueue standbyQuads, standbyPolygons;
    
    typedef std::pair<HalfEdgeList, osg::Vec3> DrawHalfEdgePair;
    std::vector<DrawHalfEdgePair> drawHalfEdgeList;
    
    glPushMatrix();
    
    // Draw faces (triangles, quads and polygons)
    if ( _showFaces )
    {
        glBegin( GL_TRIANGLES );
        for ( FaceList::const_iterator itr=_faceList.begin(); itr!=_faceList.end(); ++itr )
        {
            Face* face = (*itr).get();
            if ( !face->valid() ) continue;
            
            PointList ptList = face->getPoints();
            unsigned int size = ptList.size();
            switch ( size )
            {
            case 3:
                drawVertices( ptList );
                break;
            case 4:
                standbyQuads.push_back( ptList );
                break;
            default:
                if ( size>4 )
                    standbyPolygons.push_back( ptList );
                break;
            }
            
            if ( _showHalfEdges )
            {
                drawHalfEdgeList.push_back( DrawHalfEdgePair(face->getBorders(), face->computeCentroid()) );
            }
        }
        glEnd();
        
        if ( standbyQuads.size()>0 )
        {
            glBegin( GL_QUADS );
            for ( StandbyQueue::const_iterator itr=standbyQuads.begin(); itr!=standbyQuads.end(); ++itr )
            {
                drawVertices( *itr );
            }
            glEnd();
        }
        
        if ( standbyPolygons.size()>0 )
        {
            for ( StandbyQueue::const_iterator itr=standbyPolygons.begin(); itr!=standbyPolygons.end(); ++itr )
            {
                glBegin( GL_POLYGON );
                drawVertices( *itr );
                glEnd();
            }
        }
    }
    
    glPushAttrib( GL_ENABLE_BIT|GL_LINE_BIT|GL_POINT_BIT|GL_POLYGON_BIT );
    glPolygonOffset( -1.0f, -1.0f );
    glEnable( GL_POLYGON_OFFSET_LINE );
    glEnable( GL_POLYGON_OFFSET_POINT );
    glDisable( GL_LIGHTING );
    
    // Draw points
    if ( _showPoints )
    {
        glPointSize( g_displayPointSize );
        glBegin( GL_POINTS );
        glColor4fv( g_displayPointColor.ptr() );
        for ( PointSet::const_iterator itr=_pointSet.begin(); itr!=_pointSet.end(); ++itr )
        {
            glVertex3fv( (*itr)->getValue().ptr() );
        }
        glEnd();
    }
    
    // Draw edges
    if ( _showEdges )
    {
        glLineWidth( g_displayEdgeWidth );
        glBegin( GL_LINES );
        glColor4fv( g_displayEdgeColor.ptr() );
        
        // FIXME: at present the edges are rendered two times because they are half ones...
        for ( HalfEdgeMap::const_iterator itr=_halfEdgeMap.begin(); itr!=_halfEdgeMap.end(); ++itr )
        {
            HalfEdge* halfEdge = itr->second.get();
            if ( !halfEdge->getStart() || !halfEdge->getEnd() ) continue;
            
            glVertex3fv( halfEdge->getStart()->getValue().ptr() );
            glVertex3fv( halfEdge->getEnd()->getValue().ptr() );
        }
        glEnd();
    }
    
    // Draw halfedges for checking the geometry topological structure
    if ( drawHalfEdgeList.size()>0 )
    {
        for ( unsigned int i=0; i<drawHalfEdgeList.size(); ++i )
        {
            DrawHalfEdgePair& pair = drawHalfEdgeList[i];
            drawHalfEdges( pair.first, pair.second );
        }
    }
    
    glPopAttrib();
    glPopMatrix();
}

struct FunctorDrawer
{
    FunctorDrawer( osg::PrimitiveFunctor* f ) : _functor(f) {}
    osg::PrimitiveFunctor* getFunctor() { return _functor; }
    
    void drawVertices( const PointList& ptList ) const
    {
        for ( PointList::const_iterator pitr=ptList.begin(); pitr!=ptList.end(); ++pitr )
        {
            Point* pt = (*pitr).get();
            _functor->vertex( pt->getValue() );
        }
    }
    
    osg::PrimitiveFunctor* _functor;
};

void PolygonalMesh::accept( osg::PrimitiveFunctor& functor ) const
{    
    FunctorDrawer drawer(&functor);
    traversePolygonElements( drawer );
}

struct FunctorIndexDrawer
{
    FunctorIndexDrawer( osg::PrimitiveIndexFunctor* f ) : _functor(f) {}
    osg::PrimitiveIndexFunctor* getFunctor() { return _functor; }
    
    void drawVertices( const PointList& ptList ) const
    {
        for ( PointList::const_iterator pitr=ptList.begin(); pitr!=ptList.end(); ++pitr )
        {
            Point* pt = (*pitr).get();
            _functor->vertex( pt->id() );
        }
    }
    
    osg::PrimitiveIndexFunctor* _functor;
};

void PolygonalMesh::accept( osg::PrimitiveIndexFunctor& functor ) const
{
    FunctorIndexDrawer drawer(&functor);
    traversePolygonElements( drawer );
}

void PolygonalMesh::clear()
{
    for ( PointSet::iterator itr=_pointSet.begin(); itr!=_pointSet.end(); ++itr )
        (*itr)->clear();
    for ( HalfEdgeMap::iterator itr=_halfEdgeMap.begin(); itr!=_halfEdgeMap.end(); ++itr )
        itr->second->clear();
    for ( FaceList::iterator itr=_faceList.begin(); itr!=_faceList.end(); ++itr )
        (*itr)->clear();
    _pointSet.clear();
    _halfEdgeMap.clear();
    _faceList.clear();
}

#define GET_OR_CREATE_TEXCOORDS(arraytype, texcoords) \
    if ( !texcoords ) { a = new arraytype; texcoords = a; geom.setTexCoordArray(i, a); } \
    else a = static_cast<arraytype*>(texcoords.get());

bool PolygonalMesh::saveGeometry( osg::Geometry& geom )
{
    std::map<unsigned long, unsigned int> _pointIndexMap;
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Array> texcoords[8];  // Texcoords 0 - 7
    
    unsigned int index = 0;
    for ( PointSet::iterator itr=_pointSet.begin(); itr!=_pointSet.end(); ++itr )
    {
        Point* pt = (*itr).get();
        vertices->push_back( pt->getValue() );
        _pointIndexMap[pt->id()] = index++;
        
        TexCoordList& tclist = pt->getTexCoords();
        for ( unsigned int i=0; i<tclist.size(); ++i )
        {
            const osg::Vec4 vec = tclist[i].second;
            switch ( tclist[i].first )
            {
            case 1:
                {
                    osg::FloatArray* a; GET_OR_CREATE_TEXCOORDS(osg::FloatArray, texcoords[i])
                    a->push_back( vec.x() );
                }
                break;
            case 2:
                {
                    osg::Vec2Array* a; GET_OR_CREATE_TEXCOORDS(osg::Vec2Array, texcoords[i])
                    a->push_back( osg::Vec2(vec.x(), vec.y()) );
                }
                break;
            case 3:
                {
                    osg::Vec3Array* a; GET_OR_CREATE_TEXCOORDS(osg::Vec3Array, texcoords[i])
                    a->push_back( osg::Vec3(vec.x(), vec.y(), vec.z()) );
                }
                break;
            case 4:
                {
                    osg::Vec4Array* a; GET_OR_CREATE_TEXCOORDS(osg::Vec4Array, texcoords[i])
                    a->push_back( vec );
                }
                break;
            default: break;
            }
        }
    }
    geom.setVertexArray( vertices.get() );
    
    osg::ref_ptr<osg::DrawElementsUInt> de3 = new osg::DrawElementsUInt( GL_TRIANGLES );
    osg::ref_ptr<osg::DrawElementsUInt> de4 = new osg::DrawElementsUInt( GL_QUADS );
    for ( FaceList::iterator itr=_faceList.begin(); itr!=_faceList.end(); ++itr )
    {
        Face* face = (*itr).get();
        const PointList& ptList = face->getPoints();
        switch ( ptList.size() )
        {
        case 3:
            for ( unsigned int i=0; i<3; ++i )
            {
                de3->push_back( _pointIndexMap[ptList[i]->id()] );
            }
            break;
        case 4:
            for ( unsigned int i=0; i<4; ++i )
            {
                de4->push_back( _pointIndexMap[ptList[i]->id()] );
            }
            break;
        default:
            if ( ptList.size()>4 )
            {
                osg::ref_ptr<osg::DrawElementsUInt> polygon = new osg::DrawElementsUInt( GL_POLYGON );
                for ( unsigned int i=0; i<ptList.size(); ++i )
                {
                    polygon->push_back( _pointIndexMap[ptList[i]->id()] );
                }
                geom.addPrimitiveSet( polygon.get() );
            }
            break;
        }
    }
    geom.getPrimitiveSetList().clear();
    if ( de3->size()>0 ) geom.addPrimitiveSet( de3.get() );
    if ( de4->size()>0 ) geom.addPrimitiveSet( de4.get() );
    
    osgUtil::SmoothingVisitor::smooth( geom );
    osgUtil::TriStripVisitor stripper;
    stripper.stripify( geom );
    return true;
}

// PROTECTED METHODS

void PolygonalMesh::drawVertices( const PointList& ptList ) const
{
    if ( _useTexCoords )
    {
        for ( PointList::const_iterator pitr=ptList.begin(); pitr!=ptList.end(); ++pitr )
        {
            Point* pt = (*pitr).get();
            
            const TexCoordList& texcoords = pt->getTexCoords();
            for ( TexCoordList::const_iterator titr=texcoords.begin(); titr!=texcoords.end(); ++titr )
            {
                switch ( titr->first )
                {
                case 1: glTexCoord1fv( titr->second.ptr() ); break;
                case 2: glTexCoord2fv( titr->second.ptr() ); break;
                case 3: glTexCoord3fv( titr->second.ptr() ); break;
                case 4: glTexCoord4fv( titr->second.ptr() ); break;
                default: break;
                }
            }
            glNormal3fv( pt->getNormal().ptr() );
            glVertex3fv( pt->getValue().ptr() );
        }
    }
    else
    {
        for ( PointList::const_iterator pitr=ptList.begin(); pitr!=ptList.end(); ++pitr )
        {
            Point* pt = (*pitr).get();
            glNormal3fv( pt->getNormal().ptr() );
            glVertex3fv( pt->getValue().ptr() );
        }
    }
}

void PolygonalMesh::drawHalfEdges( const HalfEdgeList& edgeList, const osg::Vec3& center ) const
{
    std::vector<osg::Vec3> _points, _solidLines, _stippleLines;
    for ( HalfEdgeList::const_iterator itr=edgeList.begin(); itr!=edgeList.end(); ++itr )
    {
        HalfEdge* half = (*itr).get();
        if ( !half->valid() ) continue;
        
        osg::Vec3 start, end, nextStart, nextEnd;
        getDrawingParameters( half, center, 0.1f, start, end );
        _points.push_back( end );
        _solidLines.push_back( start );
        _solidLines.push_back( end );
        
        getDrawingParameters( half->getNext(), center, 0.1f, nextStart, nextEnd );
        _stippleLines.push_back( end );
        _stippleLines.push_back( nextStart );
        
        if ( half->getTwin()->valid() && half->getTwin()->isBoundary() )
        {
            getDrawingParameters( half->getTwin(), center, 0.1f, start, end );
            _points.push_back( end );
            _solidLines.push_back( start );
            _solidLines.push_back( end );
            
            HalfEdge* twinNext = half->getRotateNext();
            if ( twinNext->getRight()!=half->getLeft() )
            {
                Face* twinFace = twinNext->getRight();
                if ( !twinFace ) continue;
                getDrawingParameters( twinNext, twinFace->computeCentroid(), 0.1f, nextStart, nextEnd );
            }
            else
                getDrawingParameters( twinNext, center, 0.1f, nextStart, nextEnd );
            _stippleLines.push_back( end );
            _stippleLines.push_back( nextStart );
        }
    }
    
    if ( _points.size()>0 )
    {
        glBegin( GL_POINTS );
        for ( std::vector<osg::Vec3>::iterator itr=_points.begin(); itr!=_points.end(); ++itr )
        {
            glColor4fv( g_displayHalfEdgeColor.ptr() );
            glVertex3fv( itr->ptr() );
        }
        glEnd();
    }
    
    if ( _solidLines.size()>0 )
    {
        glBegin( GL_LINES );
        for ( unsigned int i=0; i<_solidLines.size(); i+=2 )
        {
            osg::Vec3& v1 = _solidLines[i], &v2 = _solidLines[i+1];
            glColor4fv( g_displayHalfEdgeColor.ptr() );
            glVertex3fv( v1.ptr() );
            glVertex3fv( v2.ptr() );
        }
        glEnd();
    }
    
    if ( _stippleLines.size()>0 )
    {
        glEnable( GL_LINE_STIPPLE );
        glLineStipple( 1, 0x00ff );
        glBegin( GL_LINES );
        for ( unsigned int i=0; i<_stippleLines.size(); i+=2 )
        {
            osg::Vec3& v1 = _stippleLines[i], &v2 = _stippleLines[i+1];
            glColor4fv( g_displayHalfEdgeColor.ptr() );
            glVertex3fv( v1.ptr() );
            glVertex3fv( v2.ptr() );
        }
        glEnd();
        glDisable( GL_LINE_STIPPLE );
    }
}

void PolygonalMesh::getDrawingParameters( HalfEdge* half, const osg::Vec3& refCenter, float offsetRatio,
                                          osg::Vec3& start, osg::Vec3& end ) const
{
    start = (*half)[0];
    end = (*half)[1];
    
    osg::Vec3 centerToLine = (start+end) * 0.5f - refCenter;
    osg::Vec3 newCenter = refCenter;
    if ( !half->isBoundary() )
    {
        centerToLine.normalize();
        newCenter += centerToLine * offsetRatio;
    }
    else
    {
        newCenter += centerToLine * 2.0f;
    }
    
    osg::Vec3 startOffset = newCenter-start, endOffset = newCenter-end;
    start += startOffset * offsetRatio;
    end += endOffset * offsetRatio;
}
