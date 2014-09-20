// This method takes in P1 (initial mouse press position) and P2 (current mouse position)
// these are 3D picked points
// The method allows a user to draw a sq similar to lengthening/orienting a line segment
// The initial sq is "square-ish" or spherical
// The user can slide it around the data surface, continuously setting its center
// When the user wants to draw a "paint brush stroke" they click a spot on data surface and then drag out the
// "stroke", simultaneously lengthening and orienting the sq
// This method would be called continuously in mouse drag callback method
// The initial 3d picked position would be set in the mouse press callback method (it is a global variableso can be
// accessed in mouse drag)
void PaintBrushStroke(const FLOATVECTOR3& P1, const FLOATVECTOR3& P2)
{
    // z direction  of sq coord system is "thickness"
	// y direction of  sq is "length"
	// x direction of  sq is "width"
    // I establish a min sq radius and max sq radius. 
	// the sq cannot be widened/narrowed that it becomes < or > these limits.
	// I have 3 sq scale variables (one each for x,y,z)
	// I have a "global" sale factor called "sqRadius"
	// So the x/y/z scale of sq is given by  xScale*sqRadius (similar for y, z)
	// I did it this way so I could control/set min and max radius? I've forgotten - but there was a good reason.
	// See the widen/thicken methods to follow
		
	// Lengthen stroke
	float yScale = ((P1-P2).length() + (sqYScale*sqMinRadius))/(sqYScale*sqMinRadius);
    
	FLOATVECTOR3 sqCenter = (P1+P2)*0.5;
		
	// Set sq params
	sq zaxis vec is set via data surface normal (or averaged normal) 
	
	sq yaxis vec is set =  P1- P2
	
	sq xaxis vec is set via cross product
	
	sq rotation matrix is built from axes

	// Then I update "initial" variables
	// these are used when widening/thickening the sq such that it appears like one half of sq is fixed and
	// other half is streched
	// They are "shadow" sq variables
	sq initialCenter    = sqCenter
	sq initialZAxis vec = sq zaxis vec
	sq initialYAxis vec = sq yaxis vec
	sq initialXAxis vec = sq xaxis vec
	
	// There is also initialXScale,initialYScale,initialZScale
	// I seem to update them (and the sq initial center again)in the mouse release callback *after* **editing** the sq width,thickness using
	// methods below
	// but I don't update them in this method. They are initially set to be equal to sq Xscale,YScale,ZScale 
}

// Edit the width of the sq such that it appears as if one half is fixed and the other half is stretched
// this is called from mouse drag callback
// same arguments as above (initial mousepress point (i.e. 3d picked position) and P2 is current mouse point (3D picked position) 
void WidenBrushStroke(const FLOATVECTOR3& P1, const FLOATVECTOR3& P2)
{
	FLOATVECTOR3 newCenter;
    float xScale;
	
	FLOATVECTOR3 dir = P2-P1;
    
	// The ^ symbol is dot product
	FLOATVECTOR3 dirProj_on_xVec   = (dir^sqXAxisVec)*sqXAxisVec;
	FLOATVECTOR3 idir              = P1-sqInitialCenter;
    	
	float r       = sqInitialXScale * sqMinRadius;
	float r_delta = dirProj_on_xVec.length();
	float r_new;
    

	// Can't remember what all of this is doing but I think it tries to figure out whether you are stretching the
	// left half or the right half
	// Then once you know what half, it figures out if you are widening or narrowing the sq
	// so, you can pick any point on sq (on either half) and widen/narrow the sq
	if ((idir^sqXAxisVec) > 0.0f)
	{
		if ((dir^sqXAxisVec) > 0.0f)
		{
			r_new = r + r_delta;
			if (r_new > sqMaxRadius)
				r_new = sqMaxRadius;
		}
		else
		{
			r_new = r - r_delta;
			if (r_new < sqMinRadius)
				r_new = sqMinRadius;
		}
		xScale    = r_new/sqMinRadius;
		newCenter = sqInitialCenter + 0.5f*dirProj_on_xVec;
	}
	else
	{
		if ((dir^blob.xVec) > 0.0f)
		{
			r_new = r - r_delta;
			if (r_new < sqMinRadius)
				r_new = sqMinRadius;
		}
		else
		{
			r_new = r + r_delta;
			if (r_new > sqMaxRadius)
				r_new = sqMaxRadius;
		}
		xScale      = r_new/sqMinRadius;
		newCenter = sqInitialCenter + 0.5f*dirProj_on_xVec;
	}
	
	sqXScale   = xScale;
	if (r_new > sqMinRadius && r_new < sqMaxRadius)
		sqCenter = newCenter;
	   
}

// This one is called to thicken the sq - i.e. into the data
// it attempts to do this such the the front (top) surfaceof the sq remains fixed - although it does move a bit
// it is called from the mouse wheel callback function
//
//float thicknessInc = ((m_bInvWheel) ? 1.0f : -1.0f) * (float)(wheelDelta)/10000.0;
//ThickenBrushStroke(FLOATVECTOR3(0,0,thicknessInc));
// the thickness increment can be positive or negative depending on wheel direction

ThickenBrushStroke(FLOATVECTOR3 tf)
{
    FLOATVECTOR3 newCenter;
    float zScale;

	FLOATVECTOR3 transProj_on_normal   = (tf.z)*sqZAxisVec;

	float r       = sqZScale * sqMinRadius;
	float r_delta = transProj_on_normal.length();
	float r_new;

	if (tf.z  > 0.0f)
	{
		r_new = r + r_delta;
		if (r_new > sqMaxRadius)
			r_new = sqMaxRadius;
	}
	else
	{
		r_new = r - r_delta;
		if (r_new < sqMinRadius)
			r_new = sqMinRadius;
	}
	
	zScale = r_new/sqMinRadius;
	
	newCenter = sqCenter - 0.5f*transProj_on_normal;
	
	if (r_new > sqMinRadius && r_new < sqMaxRadius)
	{
		sqCenter = newCenter;
		sqZScale = zScale;
	}
}