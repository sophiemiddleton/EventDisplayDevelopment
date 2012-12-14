/**************************************************************************
 * Copyright(c) 2005-2006, ILC Project Experiment, All rights reserved.   *
 *                                                                        *
// Author: The ILC Off-line Project. 
 // Part of the code has been developed by Alice Off-line Project. 
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//////////////////////////////////////////////////////////////////////////////
//                          Class IlcTrackPointArray                        //
//   This class contains the ESD track space-points which are used during   //
//   the alignment procedures. Each space-point consist of 3 coordinates    //
//   (and their errors) and the index of the sub-detector which contains    //
//   the space-point.                                                       //
//   cvetan.cheshkov@cern.ch 3/11/2005                                      //
//////////////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include <TMatrixDSym.h>

#include "IlcTrackPointArray.h"

//ClassImp(IlcTrackPointArray)

//______________________________________________________________________________
IlcTrackPointArray::IlcTrackPointArray()
{
  fNPoints = fSize = 0;
  fX = fY = fZ = 0;
  fVolumeID = 0;
  fCov = 0;
}

//______________________________________________________________________________
IlcTrackPointArray::IlcTrackPointArray(Int_t npoints):
  fNPoints(npoints)
{
  // Constructor
  //
  fSize = 6*npoints;
  fX = new Float_t[npoints];
  fY = new Float_t[npoints];
  fZ = new Float_t[npoints];
  fVolumeID = new UShort_t[npoints];
  fCov = new Float_t[fSize];
}

//______________________________________________________________________________
IlcTrackPointArray::IlcTrackPointArray(const IlcTrackPointArray &array):
  TObject(array)
{
  // Copy constructor
  //
  fNPoints = array.fNPoints;
  fSize = array.fSize;
  fX = new Float_t[fNPoints];
  fY = new Float_t[fNPoints];
  fZ = new Float_t[fNPoints];
  fVolumeID = new UShort_t[fNPoints];
  fCov = new Float_t[fSize];
  memcpy(fX,array.fX,fNPoints*sizeof(Float_t));
  memcpy(fY,array.fY,fNPoints*sizeof(Float_t));
  memcpy(fZ,array.fZ,fNPoints*sizeof(Float_t));
  memcpy(fVolumeID,array.fVolumeID,fNPoints*sizeof(UShort_t));
  memcpy(fCov,array.fCov,fSize*sizeof(Float_t));
}

//_____________________________________________________________________________
IlcTrackPointArray &IlcTrackPointArray::operator =(const IlcTrackPointArray& array)
{
  // assignment operator
  //
  if(this==&array) return *this;
  ((TObject *)this)->operator=(array);

  fNPoints = array.fNPoints;
  fSize = array.fSize;
  fX = new Float_t[fNPoints];
  fY = new Float_t[fNPoints];
  fZ = new Float_t[fNPoints];
  fVolumeID = new UShort_t[fNPoints];
  fCov = new Float_t[fSize];
  memcpy(fX,array.fX,fNPoints*sizeof(Float_t));
  memcpy(fY,array.fY,fNPoints*sizeof(Float_t));
  memcpy(fZ,array.fZ,fNPoints*sizeof(Float_t));
  memcpy(fVolumeID,array.fVolumeID,fNPoints*sizeof(UShort_t));
  memcpy(fCov,array.fCov,fSize*sizeof(Float_t));

  return *this;
}

//______________________________________________________________________________
IlcTrackPointArray::~IlcTrackPointArray()
{
  // Destructor
  //
  delete [] fX;
  delete [] fY;
  delete [] fZ;
  delete [] fVolumeID;
  delete [] fCov;
}


//______________________________________________________________________________
Bool_t IlcTrackPointArray::AddPoint(Int_t i, const IlcTrackPoint *p)
{
  // Add a point to the array at position i
  //
  if (i >= fNPoints) return kFALSE;
  fX[i] = p->GetX();
  fY[i] = p->GetY();
  fZ[i] = p->GetZ();
  fVolumeID[i] = p->GetVolumeID();
  memcpy(&fCov[6*i],p->GetCov(),6*sizeof(Float_t));
  return kTRUE;
}

//______________________________________________________________________________
Bool_t IlcTrackPointArray::GetPoint(IlcTrackPoint &p, Int_t i) const
{
  // Get the point at position i
  //
  if (i >= fNPoints) return kFALSE;
  p.SetXYZ(fX[i],fY[i],fZ[i],&fCov[6*i]);
  p.SetVolumeID(fVolumeID[i]);
  return kTRUE;
}

//______________________________________________________________________________
Bool_t IlcTrackPointArray::HasVolumeID(UShort_t volid) const
{
  // This method checks if the array
  // has at least one hit in the detector
  // volume defined by volid
  Bool_t check = kFALSE;
  for (Int_t ipoint = 0; ipoint < fNPoints; ipoint++)
    if (fVolumeID[ipoint] == volid) check = kTRUE;

  return check;
}

//ClassImp(IlcTrackPoint)

//______________________________________________________________________________
IlcTrackPoint::IlcTrackPoint()
{
  // Default constructor
  //
  fX = fY = fZ = 0;
  fVolumeID = 0;
  memset(fCov,0,6*sizeof(Float_t));
}


//______________________________________________________________________________
IlcTrackPoint::IlcTrackPoint(Float_t x, Float_t y, Float_t z, const Float_t *cov, UShort_t volid)
{
  // Constructor
  //
  SetXYZ(x,y,z,cov);
  SetVolumeID(volid);
}

//______________________________________________________________________________
IlcTrackPoint::IlcTrackPoint(const Float_t *xyz, const Float_t *cov, UShort_t volid)
{
  // Constructor
  //
  SetXYZ(xyz[0],xyz[1],xyz[2],cov);
  SetVolumeID(volid);
}

//______________________________________________________________________________
IlcTrackPoint::IlcTrackPoint(const IlcTrackPoint &p):
  TObject(p)
{
  // Copy constructor
  //
  SetXYZ(p.fX,p.fY,p.fZ,&(p.fCov[0]));
  SetVolumeID(p.fVolumeID);
}

//_____________________________________________________________________________
IlcTrackPoint &IlcTrackPoint::operator =(const IlcTrackPoint& p)
{
  // assignment operator
  //
  if(this==&p) return *this;
  ((TObject *)this)->operator=(p);

  SetXYZ(p.fX,p.fY,p.fZ,&(p.fCov[0]));
  SetVolumeID(p.fVolumeID);

  return *this;
}

//______________________________________________________________________________
void IlcTrackPoint::SetXYZ(Float_t x, Float_t y, Float_t z, const Float_t *cov)
{
  // Set XYZ coordinates and their cov matrix
  //
  fX = x;
  fY = y;
  fZ = z;
  if (cov)
    memcpy(fCov,cov,6*sizeof(Float_t));
}

//______________________________________________________________________________
void IlcTrackPoint::SetXYZ(const Float_t *xyz, const Float_t *cov)
{
  // Set XYZ coordinates and their cov matrix
  //
  SetXYZ(xyz[0],xyz[1],xyz[2],cov);
}

//______________________________________________________________________________
void IlcTrackPoint::GetXYZ(Float_t *xyz, Float_t *cov) const
{
  xyz[0] = fX;
  xyz[1] = fY;
  xyz[2] = fZ;
  if (cov)
    memcpy(cov,fCov,6*sizeof(Float_t));
}

//______________________________________________________________________________
Float_t IlcTrackPoint::GetResidual(const IlcTrackPoint &p, Bool_t weighted) const
{
  // This method calculates the track to space-point residuals. The track
  // interpolation is also stored as IlcTrackPoint. Using the option
  // 'weighted' one can calculate the residual either with or without
  // taking into account the covariance matrix of the space-point and
  // track interpolation. The second case the residual becomes a pull.

  Float_t res = 0;

  if (!weighted) {
    Float_t xyz[3],xyzp[3];
    GetXYZ(xyz);
    p.GetXYZ(xyzp);
    res = (xyz[0]-xyzp[0])*(xyz[0]-xyzp[0])+
          (xyz[1]-xyzp[1])*(xyz[1]-xyzp[1])+
          (xyz[2]-xyzp[2])*(xyz[2]-xyzp[2]);
  }
  else {
    Float_t xyz[3],xyzp[3];
    Float_t cov[6],covp[6];
    GetXYZ(xyz,cov);
    TMatrixDSym mcov(3);
    mcov(0,0) = cov[0]; mcov(0,1) = cov[1]; mcov(0,2) = cov[2];
    mcov(1,0) = cov[1]; mcov(1,1) = cov[3]; mcov(1,2) = cov[4];
    mcov(2,0) = cov[2]; mcov(2,1) = cov[4]; mcov(2,2) = cov[5];
    p.GetXYZ(xyzp,covp);
    TMatrixDSym mcovp(3);
    mcovp(0,0) = covp[0]; mcovp(0,1) = covp[1]; mcovp(0,2) = covp[2];
    mcovp(1,0) = covp[1]; mcovp(1,1) = covp[3]; mcovp(1,2) = covp[4];
    mcovp(2,0) = covp[2]; mcovp(2,1) = covp[4]; mcovp(2,2) = covp[5];
    TMatrixDSym msum = mcov + mcovp;
    msum.Invert();
    //    mcov.Print(); mcovp.Print(); msum.Print();
    if (msum.IsValid()) {
      for (Int_t i = 0; i < 3; i++)
	for (Int_t j = 0; j < 3; j++)
	  res += (xyz[i]-xyzp[i])*(xyz[j]-xyzp[j])*msum(i,j);
    }
  }

  return res;
}

//______________________________________________________________________________
Float_t IlcTrackPoint::GetAngle() const
{
  // The method uses the covariance matrix of
  // the space-point in order to extract the
  // orientation of the detector plane.
  // The rotation in XY plane only is calculated.

  Float_t phi= TMath::ATan2(TMath::Sqrt(fCov[0]),TMath::Sqrt(fCov[3]));
  if (fCov[1] > 0) {
    phi = TMath::Pi() - phi;
    if ((fY-fX) < 0) phi += TMath::Pi();
  }
  else {
    if ((fX+fY) < 0) phi += TMath::Pi();
  }

  return phi;

}

//_____________________________________________________________________________
IlcTrackPoint& IlcTrackPoint::Rotate(Float_t alpha) const
{
  // Transform the space-point coordinates
  // and covariance matrix from global to
  // local (detector plane) coordinate system
  // XY plane rotation only

  static IlcTrackPoint p;
  p = *this;

  Float_t xyz[3],cov[6];
  GetXYZ(xyz,cov);

  Float_t sin = TMath::Sin(alpha), cos = TMath::Cos(alpha);

  Float_t newxyz[3],newcov[6];
  newxyz[0] = cos*xyz[0] + sin*xyz[1];
  newxyz[1] = cos*xyz[1] - sin*xyz[0];
  newxyz[2] = xyz[2];

  newcov[0] = cov[0]*cos*cos+
            2*cov[1]*sin*cos+
              cov[3]*sin*sin;
  newcov[1] = cov[1]*(cos*cos-sin*sin)+
             (cov[3]-cov[0])*sin*cos;
  newcov[2] = cov[2]*cos+
              cov[4]*sin;
  newcov[3] = cov[0]*sin*sin-
            2*cov[1]*sin*cos+
              cov[3]*cos*cos;
  newcov[4] = cov[4]*cos-
              cov[2]*sin;
  newcov[5] = cov[5];

  p.SetXYZ(newxyz,newcov);
  p.SetVolumeID(GetVolumeID());

  return p;
}

//_____________________________________________________________________________
IlcTrackPoint& IlcTrackPoint::MasterToLocal() const
{
  // Transform the space-point coordinates
  // and the covariance matrix from the
  // (master) to the local (tracking)
  // coordinate system

  Float_t alpha = GetAngle();
  return Rotate(alpha);
}

//_____________________________________________________________________________
void IlcTrackPoint::Print(Option_t *) const
{
  // Print the space-point coordinates and
  // covariance matrix

  printf("VolumeID=%d\n", GetVolumeID());
  printf("X = %12.6f    Tx = %12.6f%12.6f%12.6f\n", fX, fCov[0], fCov[1], fCov[2]);
  printf("Y = %12.6f    Ty = %12.6f%12.6f%12.6f\n", fY, fCov[1], fCov[3], fCov[4]);
  printf("Z = %12.6f    Tz = %12.6f%12.6f%12.6f\n", fZ, fCov[2], fCov[4], fCov[5]);

}