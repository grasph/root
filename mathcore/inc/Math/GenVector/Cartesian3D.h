// @(#)root/mathcore:$Name:  $:$Id: Cartesian3D.h,v 1.3 2005/12/09 15:57:48 moneta Exp $
// Authors: W. Brown, M. Fischler, L. Moneta    2005  

 /**********************************************************************
  *                                                                    *
  * Copyright (c) 2005 , LCG ROOT MathLib Team 			       *
  *                    & FNAL LCG ROOT Mathlib Team  		       *
  *                                                                    *
  *                                                                    *
  **********************************************************************/

// Header file for class Cartesian3D
// 
// Created by: Lorenzo Moneta  at Mon May 30 11:16:56 2005
// Major revamp:  M. FIschler  at Wed Jun  8 2005
// 
// Last update: $ID: $
// 
#ifndef ROOT_Math_GenVector_Cartesian3D 
#define ROOT_Math_GenVector_Cartesian3D  1


#ifdef _WIN32
#define _USE_MATH_DEFINES 
#endif
#include <cmath>
#ifndef M_PI
#define M_PI        3.14159265358979323846   /* pi */
#endif

#include <limits>

#include "Math/GenVector/Polar3Dfwd.h"
#include "Math/GenVector/etaMax.h"


namespace ROOT { 

  namespace Math { 

  /** 
      Class describing a 3D cartesian coordinate system
      (x, y, z coordinates) 

      @ingroup GenVector
   */ 

template <class T = double> 
class Cartesian3D { 

public : 

  typedef T Scalar;

  /**
     Default constructor  with x=y=z=0 
   */
  Cartesian3D() : fX(0), fY(0), fZ(0) {  }

  /**
     Constructor from x,y,z coordinates
   */
  Cartesian3D(Scalar x, Scalar y, Scalar z) : fX(x), fY(y), fZ(z) {  } 

  /**
     Construct from any Vector or coordinate system implementing 
     X(), Y() and Z()
  */
  template <class CoordSystem> 
  explicit Cartesian3D(const CoordSystem & v) 
  				: fX(v.X()), fY(v.Y()), fZ(v.Z()) {  }

  // no reason for a custom destructor  ~Cartesian3D() {} and copy constructor

  /**
     Set internal data based on an array of 3 Scalar numbers
   */ 
  void SetCoordinates( const Scalar src[] ) { fX=src[0]; fY=src[1]; fZ=src[2]; }

  /**
     get internal data into an array of 3 Scalar numbers
   */ 
  void GetCoordinates( Scalar dest[] ) const 
  				{ dest[0] = fX; dest[1] = fY; dest[2] = fZ; }

  /**
     Set internal data based on 3 Scalar numbers
   */ 
  void SetCoordinates(Scalar  x, Scalar  y, Scalar  z) { fX=x; fY=y; fZ=z; }

  /**
     get internal data into 3 Scalar numbers
   */ 
  void GetCoordinates(Scalar& x, Scalar& y, Scalar& z) const {x=fX; y=fY; z=fZ;}  				

  Scalar X()     const { return fX;}
  Scalar Y()     const { return fY;}
  Scalar Z()     const { return fZ;}
  Scalar Mag2()  const { return fX*fX + fY*fY + fZ*fZ;}
  Scalar Perp2() const { return fX*fX + fY*fY ;}
  Scalar Rho()   const { return std::sqrt( Perp2());}
  Scalar R()     const { return std::sqrt( Mag2());}
  Scalar Theta() const { return (fX==0 && fY==0 && fZ==0) ? 
  						      0.0 : atan2(Rho(),Z());}
  Scalar Phi()   const { return (fX==0 && fY==0) ? 0.0 : atan2(fY,fX);}
 
  // pseudorapidity
  // T Eta() const { return -log( tan( theta()/2.));} 
  Scalar Eta() const 
  { Scalar rho = Rho();
  /* static */ const Scalar big_z_scaled = 
      std::pow(std::numeric_limits<Scalar>::epsilon(),static_cast<Scalar>(-.6)); 
    if (rho > 0) {
      Scalar z_scaled(fZ/rho);
      if (fabs(z_scaled) < big_z_scaled) {
        return std::log(z_scaled+std::sqrt(z_scaled*z_scaled+1)); 
      } else {
        return  fZ>0 ? std::log(2.0*z_scaled) : -std::log(-2.0*z_scaled);
      }
    } else if (fZ==0) {
      return 0;
    } else if (fZ>0) {
      return fZ + etaMax<Scalar>();
    }  else {
      return fZ - etaMax<Scalar>();
    }
  }

  /** 
      set the x coordinate value keeping y and z constant
   */ 
  void SetX(Scalar x) { fX = x; }

  /** 
      set the y coordinate value keeping x and z constant
   */ 
  void SetY(Scalar y) { fY = y; }

  /** 
      set the z coordinate value keeping x and y constant
   */ 
  void SetZ(Scalar z) { fZ = z; }

  /**
     scale the vector by a scalar quantity a
   */
  void Scale(Scalar a) { fX *= a; fY *= a;  fZ *= a; }

  /**
     negate the vector
   */
  void Negate() { fX = -fX; fY = -fY;  fZ = -fZ; }

  /**
     Assignment from any class implementing x(),y() and z()
     (can assign from any coordinate system) 
  */
  template <class CoordSystem> 
  Cartesian3D & operator = (const CoordSystem & v) { 
    fX = v.x();  
    fY = v.y();  
    fZ = v.z();  
    return *this;
  }

  /**
    Exact equality
   */  
  bool operator == (const Cartesian3D & rhs) const {
    return fX == rhs.fX && fY == rhs.fY && fZ == rhs.fZ;
  }
  bool operator != (const Cartesian3D & rhs) const {return !(operator==(rhs));}

  
  // ============= Compatibility section ==================
  
  // The following make this coordinate system look enough like a CLHEP
  // vector that an assignment member template can work with either
  T x() const { return X();}
  T y() const { return Y();}
  T z() const { return Z(); } 
  
  // ============= Overloads for improved speed ==================

  template <class T2>
  explicit Cartesian3D( const Polar3D<T2> & v ) : fZ (v.Z())
  {
    T rho = v.Rho(); // re-using this instead of calling v.X() and v.Y()
                     // is the speed improvement
    fX = rho * std::cos(v.Phi());
    fY = rho * std::sin(v.Phi());    
  } 
    // Technical note:  This works even though only Polar3Dfwd.h is 
    // included (and in fact, including Polar3D.h would cause circularity
    // problems). It works because any program **using** this ctor must itself
    // be including Polar3D.h.
    
  template <class T2>
  Cartesian3D & operator = (const Polar3D<T2> & v) 
  { 
    T rho = v.Rho(); 
    fX = rho * std::cos(v.Phi());
    fY = rho * std::sin(v.Phi());
    fZ = v.Z();
    return *this;
  }



#if defined(__MAKECINT__) || defined(G__DICTIONARY) 

  // ====== Set member functions for coordinates in other systems =======

  void SetR(Scalar r);
  
  void SetTheta(Scalar theta);  
  
  void SetPhi(Scalar phi); 
  
  void SetRho(Scalar rho); 
  
  void SetEta(Scalar eta);

#endif


private:

  /**
     (Contiguous) data containing the coordinates values x,y,z
   */
  T fX;
  T fY;
  T fZ; 
};


  } // end namespace Math

} // end namespace ROOT


#if defined(__MAKECINT__) || defined(G__DICTIONARY) 
// need to put here setter methods to resolve nasty cyclical dependencies 
// I need to include other coordinate systems only when Cartesian is already defined 
// since they depend on it

#include "Math/GenVector/GenVector_exception.h"
#include "Math/GenVector/CylindricalEta3D.h"
#include "Math/GenVector/Polar3D.h"

  // ====== Set member functions for coordinates in other systems =======

namespace ROOT { 

  namespace Math { 

template <class T>  
void Cartesian3D<T>::SetR(Scalar r) {  
    GenVector_exception e("Cartesian3D::SetR() is not supposed to be called");
    Throw(e);
    Polar3D<Scalar> v(*this); v.SetR(r); *this = Cartesian3D<Scalar>(v);
  }

template <class T>  
void Cartesian3D<T>::SetTheta(Scalar theta) {  
    GenVector_exception e("Cartesian3D::SetTheta() is not supposed to be called");
    Throw(e);
    Polar3D<Scalar> v(*this); v.SetTheta(theta); *this = Cartesian3D<Scalar>(v);
  }

template <class T>  
void Cartesian3D<T>::SetPhi(Scalar phi) {  
    GenVector_exception e("Cartesian3D::SetPhi() is not supposed to be called");
    Throw(e);
    Polar3D<Scalar> v(*this); v.SetPhi(phi); *this = Cartesian3D<Scalar>(v);
  }

template <class T>  
void Cartesian3D<T>::SetRho(Scalar rho) {  
    GenVector_exception e("Cartesian3D::SetRho() is not supposed to be called");
    Throw(e);
    CylindricalEta3D<Scalar> v(*this); v.SetRho(rho); 
    *this = Cartesian3D<Scalar>(v);
  }

template <class T>  
void Cartesian3D<T>::SetEta(Scalar eta) {  
    GenVector_exception e("Cartesian3D::SetEta() is not supposed to be called");
    Throw(e);
    CylindricalEta3D<Scalar> v(*this); v.SetEta(eta); 
    *this = Cartesian3D<Scalar>(v);
  }



  } // end namespace Math

} // end namespace ROOT

#endif




#endif /* ROOT_Math_GenVector_Cartesian3D  */
