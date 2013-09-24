#include "Vectors.h"

const float M_PI = 3.141592654;

class SuperQuadric
{
public:
	FLOATVECTOR3	position;
	FLOATVECTOR3	initialPosition;
	float		radius;
	FLOATVECTOR4	color;
	float		xScale;
	float           yScale;
	float		zScale;
	float		initialXScale;
	float           initialYScale;
	float		initialZScale;
	float		xyExponent;
	float		zExponent;
	FLOATVECTOR3	zOrientVec;
	FLOATVECTOR3	normal;
	FLOATVECTOR3	up;
	FLOATVECTOR3	xVec;
	FLOATMATRIX4	zRotMatrix;

	int             n; // number of vertices to use when drawing superquad

	SuperQuadric()
	{
		Init(FLOATVECTOR3(0.0f, 0.0f, 0.0f), 0.0f, FLOATVECTOR4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, 1.0f, 1.0f, 2.0f, 2.0f, FLOATVECTOR3(0,0,1));
	}

	SuperQuadric(FLOATVECTOR3 position, float radius, FLOATVECTOR4 color, 
		float xScale = 1.0f, float yScale = 1.0f, float zScale = 1.0f, 
		float xyExponent = 8.0f, float zExponent = 2.0f, FLOATVECTOR3 zOrientVec = FLOATVECTOR3(0,0,1))
	{
		Init(position, radius, color, xScale, yScale, zScale, xyExponent, zExponent, zOrientVec);
	}

	void Init(FLOATVECTOR3 position, float radius, FLOATVECTOR4 color, 
		float xScale = 1.0f, float yScale = 1.0f, float zScale = 1.0f, 
		float xyExponent = 8.0f, float zExponent = 2.0f, FLOATVECTOR3 zOrientVec = FLOATVECTOR3(0,0,1))
	{
		this->position		= position;
		this->radius		= radius;
		this->color		= color;
		this->n                 = 64;
		this->xScale		= xScale;
		this->yScale		= yScale;
		this->zScale		= zScale;
		this->initialXScale     = xScale;
		this->initialYScale     = yScale;
		this->initialZScale     = zScale;
		this->xyExponent	= xyExponent;
		this->zExponent		= zExponent;

		SetZOrientVec(zOrientVec);

	}

	void SetZOrientVec(const FLOATVECTOR3& zOrientVec)
	{
		this->zOrientVec	= this->normal	= zOrientVec;
		this->zOrientVec.normalize();

		FLOATVECTOR3 xOrientVec; // reference vector, will correct later
		if (this->zOrientVec.x == 0 && this->zOrientVec.y == 0)
		{
			xOrientVec = FLOATVECTOR3(.0f, .0f, 1.0f);

		}
		else
		{
			xOrientVec = FLOATVECTOR3(zOrientVec.z, .0f, .0f); // could be optimized
		}

		FLOATVECTOR3 yOrientVec;
		yOrientVec = this->zOrientVec % xOrientVec;
		yOrientVec.normalize();
		this->up = yOrientVec;

		this->xVec = yOrientVec % this->zOrientVec;
		this->xVec.normalize();

		zRotMatrix.m11 = this->xVec.x;
		zRotMatrix.m12 = this->xVec.y;
		zRotMatrix.m13 = this->xVec.z;

		zRotMatrix.m21 = yOrientVec.x;
		zRotMatrix.m22 = yOrientVec.y;
		zRotMatrix.m23 = yOrientVec.z;

		zRotMatrix.m31 = this->zOrientVec.x;
		zRotMatrix.m32 = this->zOrientVec.y;
		zRotMatrix.m33 = this->zOrientVec.z;
		// Not sure whyLev is doing this
		// seems like the orientation matrix is set to the inverse, in which case it should be called zRotMatrixInverse
		this->zRotMatrix = this->zRotMatrix.inverse();
	}

	// normal would be an averaged data surface normal so that superquad orientationis aligned with data surface as you slide
	// it over the surface
	// "up" would be formed by clicking a point on data surface (P1) and then sliding mouse along continuously defining a second
	//  data surface point (P2) up = P2 - P1, this is for when you are "drawing" a stretched superquad

	void SetOrientation(const FLOATVECTOR3& normal, const FLOATVECTOR3& up)
	{
		this->normal = normal;
		this->up     = up;

		this->normal.normalize();
		this->up.normalize();

		FLOATVECTOR3 xOrientVec; // reference vector, will correct later
		FLOATVECTOR3 yOrientVec;

		xOrientVec = this->up % this->normal;
		xOrientVec.normalize();
		this->xVec = xOrientVec;

		yOrientVec = this->normal % xOrientVec;
		yOrientVec.normalize();
		this->up   = yOrientVec;

		zRotMatrix.m11 = xOrientVec.x;
		zRotMatrix.m12 = xOrientVec.y;
		zRotMatrix.m13 = xOrientVec.z;

		zRotMatrix.m21 = yOrientVec.x;
		zRotMatrix.m22 = yOrientVec.y;
		zRotMatrix.m23 = yOrientVec.z;

		zRotMatrix.m31 = this->normal.x;
		zRotMatrix.m32 = this->normal.y;
		zRotMatrix.m33 = this->normal.z;

		this->zRotMatrix = this->zRotMatrix.inverse();
	}
	// Use a bunch of data surface normals inside superquad to make its orientation more robust 
	void SetAveragedOrientation(const std::vector<FLOATVECTOR3> &normalVectors,const FLOATVECTOR3& up)
	{

		for(int i=0; i<(int)normalVectors.size(); i++)
		{
			this->normal += normalVectors[i];
		}
		this->normal /= (float)(normalVectors.size()+1);
		this->up     = up;

		this->normal.normalize();
		this->up.normalize();

		FLOATVECTOR3 xOrientVec; // reference vector, will correct later
		FLOATVECTOR3 yOrientVec;

		xOrientVec = this->up % this->normal;
		xOrientVec.normalize();
		this->xVec = xOrientVec;

		yOrientVec = this->normal % xOrientVec;
		yOrientVec.normalize();
		this->up   = yOrientVec;

		zRotMatrix.m11 = xOrientVec.x;
		zRotMatrix.m12 = xOrientVec.y;
		zRotMatrix.m13 = xOrientVec.z;

		zRotMatrix.m21 = yOrientVec.x;
		zRotMatrix.m22 = yOrientVec.y;
		zRotMatrix.m23 = yOrientVec.z;

		zRotMatrix.m31 = this->normal.x;
		zRotMatrix.m32 = this->normal.y;
		zRotMatrix.m33 = this->normal.z;

		this->zRotMatrix = this->zRotMatrix.inverse();
	}

	// Implicit function to determine if point is inside or outside
	bool insideSuperquadric( const FLOATVECTOR3&	point) 
	{
		// translate to superquad to center i.e. untranslate everything
		FLOATVECTOR3 to = point - position;

		// unrotate superquad so it's back in original orientation (Lev has zRotMatrix as inverse already
		FLOATVECTOR4 toOriented = zRotMatrix * FLOATVECTOR4(to, 1.0);

		float sqDist = pow((float)pow(fabs(toOriented.x)/(radius*xScale), 2.0f/xyExponent) + 
			(float)pow(fabs(toOriented.y)/(radius*yScale), 2.0f/xyExponent), xyExponent/zExponent) + 
			(float)pow(fabs(toOriented.z)/(radius*zScale), 2.0f/zExponent);

		sqDist       = (float)pow(sqDist,zExponent/2.0f);

		if (sqDist > 1.0)
			return false;

		return true;
	}

	/* Returns the sign of x */
	static float SIGN ( float x ) {
		if ( x < 0 )
			return -1;
		if ( x > 0 )
			return 1;
		return 0;
	}

	static void EvalSuperquadric(double xyTheta, double zTheta, double xyExponent, double zExponent, double xScale, double yScale, double zScale, FLOATVECTOR3 *p)
	{
		double tmp;
		double ct1,ct2,st1,st2;
		ct1 = cos(zTheta);
		ct2 = cos(xyTheta);
		st1 = sin(zTheta);
		st2 = sin(xyTheta);
		tmp  = SIGN(ct1) * pow(fabs(ct1),zExponent);
		p->x = xScale * tmp * SIGN(ct2) * pow(fabs(ct2),xyExponent);
		p->y = yScale * tmp * SIGN(st2) * pow(fabs(st2),xyExponent);
		p->z = zScale * SIGN(st1) * pow(fabs(st1),zExponent);
	}

	// Use parametric function to draw
	void DrawSuperSuperquadric()
	{
		float theta;	// added

		int i, j;
		double phi, phiNext, phi;
		FLOATVECTOR3 p, p1, p2, en;
		double delta;

		delta = 0.01 * M_PI*2.0 / n;

		for (j=0;j<n/2;j++)
		{
			phi     =  j  * M_PI*2.0 / (double)n - M_PI/2.0;
			phiNext = (j + 1) * M_PI*2.0 / (double)n - M_PI/2.0;

			//glBegin(GL_TRIANGLE_STRIP);

			for (i=0;i<=n;i++)
			{
				if (i == 0 || i == n)
					theta = 0;
				else
					theta = i * M_PI*2.0 / n;
				
				EvalSuperquadric(theta,phiNext,xyExponent,zExponent,xScale,yScale,zScale,&p);
				EvalSuperquadric(theta,phiNext+delta,xyExponent,zExponent,xScale,yScale,zScale,&p1);
				EvalSuperquadric(theta+delta,phiNext,xyExponent,zExponent,xScale,yScale,zScale,&p2);
				//en = CalcNormal(p1,p,p2);
				//glNormal3f(en.x,en.y,en.z);
				//glVertex3f(p.x,p.y,p.z);

				EvalSuperquadric(theta,phi,xyExponent,zExponent,xScale,yScale,zScale,&p);
				EvalSuperquadric(theta,phi+delta,xyExponent,zExponent,xScale,yScale,zScale,&p1);
				EvalSuperquadric(theta+delta,phi,xyExponent,zExponent,xScale,yScale,zScale,&p2);
				//en = CalcNormal(p1,p,p2);
				//glNormal3f(en.x,en.y,en.z);
				//glVertex3f(p.x,p.y,p.z);
			}
			//glEnd();
		}
	}

	bool operator == ( const SuperQuadric& other ) const {return (other.position==position && other.radius==radius && other.color==color ); }
	bool operator != ( const SuperQuadric& other ) const {return (other.position!=position || other.radius!=radius || other.color!=color ); }
};