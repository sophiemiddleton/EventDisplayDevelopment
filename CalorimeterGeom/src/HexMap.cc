// Hexagon position map generator: 
//   tesselate a plane with hexagons starting at the center of the plane
//
// Use basis vector, l and k, defined as  
// l = up right
// k = down right
//
// so 
// l-k = up
// l+k = right 
/*
           ____
          /    \ 
     ____/ 1 -1 \____
    /    \      /    \
   / 0 -1 \____/  1 0 \   l,k coordinates
   \      /    \      /  (for whatever reason, l seems to come before k...)
    \____/  0 0 \____/   (hope you appreciate this piece of art...)
    /    \      /    \
   / -1 0 \____/ 0 1  \
   \      /    \      /
    \____/ -1 1 \____/
         \      /    
          \____/      
*/
// Tesselation algorithm: tessalate in "rings" from the center
//   for each ring, start at n+,-n (one step down right from the upmost hexagon), 
//   then go n time each step to create the ring (see example function at the end of the file)
//
// Neighbors of (l0,k0), just add +-(1,0), +-(0,1) or +-(1,-1) to (l0,k0)
//                       next ring of neighbours, add +(2,2) and go around the ring,...
//


// C++ includes
#include <iostream>
#include <map>
#include <cmath>

// Mu2e includes
#include "CalorimeterGeom/inc/HexMap.hh"

//CLHEP includes
#include "CLHEP/Vector/TwoVector.h"


namespace mu2e {

      HexMap::HexMap(void) : _step() 
      {
         _step.push_back( HexLK( 0, 1) ); //down right
	 _step.push_back( HexLK(-1, 1) ); //down
	 _step.push_back( HexLK(-1, 0) ); //down left
	 _step.push_back( HexLK( 0,-1) ); //up left
	 _step.push_back( HexLK( 1,-1) ); //up
	 _step.push_back( HexLK( 1, 0) ); //up right      
      }
                       




      CLHEP::Hep2Vector HexMap::getXYPosition(int index) const
      {        
         HexLK lk = getLK(index);
         double x = (lk._l+lk._k)*sqrt(3.0)/2.0;
         double y = (lk._l-lk._k)/2.0;
         return CLHEP::Hep2Vector(x,y);
      }
  

      int HexMap::getIndexFromXY(double x0, double y0) const
      {        
         int l0 = int( x0/sqrt(3.0)+y0+0.5 );
         int k0 = int( x0/sqrt(3.0)-y0+0.5 );
	 
         int lf(l0-3),kf(k0-3);
	 double dist0(100);
	 
	 for (int l=l0-3;l<l0+4;++l){
	   for (int k=k0-3;k<k0+4;++k){
	      double dx    = (l+k)*sqrt(3.0)/2.0 - x0;
              double dy    = (l-k)/2.0 - y0;
	      double dist = sqrt(dx*dx+dy*dy);
	      if (dist < dist0) {dist0=dist;lf=l;kf=k;}	 
           }	 
	 }
	 
	 HexLK lk(lf,kf);	 
	 return getIndex(lk);
      }



      std::vector<int> HexMap::getNeighbors(int index, int level)  const
      {	 
	 std::vector<int> neighbors;
	 neighbors.reserve(100);

         HexLK init = getLK(index);
	 HexLK lk(init._l + level, init._k - level);	 

         for (unsigned int i=0;i<_step.size();++i){       
	     for (int iseg=0;iseg<level;++iseg){	  

		lk += _step[i];
		neighbors.push_back(getIndex(lk));	     
	     }
	 }
	 
         return neighbors;
      }





     

      HexLK HexMap::getLK(int index) const
      {
         
	 if (index==0) return HexLK(0,0);

	 int nRing = int(0.5+sqrt(0.25+(float(index)-1.0)/3.0));
         int nSeg  = (index -1 -3*nRing*(nRing-1))/nRing;
         int nPos  = (index -1 -3*nRing*(nRing-1))%nRing;
	 
	 int l = nRing+(nPos+1)*_step[nSeg]._l;
	 int k = -nRing+(nPos+1)*_step[nSeg]._k;
	 
	 for (int i=0;i<nSeg;++i) {
	    l += _step[i]._l*nRing;
	    k += _step[i]._k*nRing;
	 }
	 	 	 
	 return HexLK(l,k);
      } 


      int HexMap::getIndex(HexLK& lk) const 
      {
         if (lk._l==0 && lk._k==0) return 0;
	 
	 int nring = getRing(lk);
	 int pos = (nring>0) ? 1+3*nring*(nring-1): 0;

	 //find segment along the ring
	 int segment(0);	 
	 if ( std::abs(lk._l+lk._k) == nring && lk._k!=0) segment += 1;  
	 if ( std::abs(lk._k) == nring && lk._l!=0)        segment += 2; 
	 if ( (lk._l+lk._k) <=0 && lk._k<nring)           segment += 3; 
	 pos += segment*nring;
	 
	 //add position le long du segment	 
	 if (segment==0 || segment==3)  pos += nring - std::abs(lk._k)-1;
	 if (segment==1 || segment==4)  pos += nring - std::abs(lk._l)-1;
	 if (segment==2 || segment==5)  pos += std::abs(lk._l)-1;
	
	 return pos;      
      }
           
      int HexMap::getRing(HexLK& lk) const 
      {         
	  if (lk._l*lk._k > 0) return std::abs(lk._l+lk._k);
	  if ( std::abs(lk._l) > std::abs(lk._k) ) return std::abs(lk._l);
	  return std::abs(lk._k);
      }





     /*
     // This function illustrates how the map is generated, storing each lk values in a vector
     // In this version, getIndex gives the index in the _position vector given lk values, eliminating
     // the need for this vector
     void HexMap::generate(int nring)
      { 
 
         _positions.push_back(HexLK(0,0));
         for(int iring=1;iring<nring;iring++){  

            HexLK cp(iring,-iring);         
            for(unsigned int istep=0; istep<_step.size(); istep++){
               for(int iseg=0;iseg<iring;iseg++){
                  cp += _step[istep];
                  _positions.push_back(cp);                  
               }
            }
         }

         return;
      } 
      */


}


             