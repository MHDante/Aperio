//#define VTK_MIN_SUPERQUADRIC_THICKNESS  1e-4

class MySuperquadric 
{
public:
  // Description
  // Construct with superquadric radius of 0.5, toroidal off, center at 0.0,
  // scale (1,1,1), size 0.5, phi roundness 1.0, and theta roundness 0.0.

  // ImplicitFunction interface
  double EvaluateFunction(double x[3]);
  double EvaluateFunction(double x, double y, double z)
    {return EvaluateFunction(x, y, z); } ;
  void EvaluateGradient(double x[3], double g[3]);

  void SetPhiRoundness(double e);
  void SetThetaRoundness(double e);

protected:
  MySuperquadric();
  ~MySuperquadric() {};

  int Toroidal;
  double Thickness;
  double Size;
  double PhiRoundness;
  double ThetaRoundness;
  double Center[3];
  double Scale[3];
private:
  MySuperquadric(const MySuperquadric&);  // Not implemented.
  void operator=(const MySuperquadric&);  // Not implemented.
};


