#include <Packages/Uintah/CCA/Components/ICE/Advection/FirstOrderCEAdvector.h>
#include <Packages/Uintah/Core/Grid/CellIterator.h>
#include <Core/Geometry/IntVector.h>
#include <Packages/Uintah/CCA/Ports/DataWarehouse.h>
#include <Packages/Uintah/Core/Grid/VarLabel.h>
#include <Packages/Uintah/Core/Grid/Patch.h>
#include <Packages/Uintah/Core/Exceptions/OutFluxVolume.h>
#include <Core/Malloc/Allocator.h>
#include <Core/Util/Endian.h>
#include <iostream>

using namespace Uintah;
/* ---------------------------------------------------------------------
References:
    "Compatible Fluxes for van Leer Advection" W.B VanderHeyden and 
    B.A. Kashiwa, Journal of Computational Physics, 146, 1-28, (1998) 
 ---------------------------------------------------------------------  */
FirstOrderCEAdvector::FirstOrderCEAdvector()
{
  OFE_CCLabel = 0;
  OFC_CCLabel = 0;
}

FirstOrderCEAdvector::FirstOrderCEAdvector(DataWarehouse* new_dw, 
                                          const Patch* patch)
  :   d_advector(new_dw,patch)
{


  OFE_CCLabel = VarLabel::create("OFE_CC",
                             CCVariable<eflux>::getTypeDescription());
  OFC_CCLabel = VarLabel::create("OFC_CC",
                             CCVariable<cflux>::getTypeDescription());
  new_dw->allocateTemporary(d_OFE,  patch, Ghost::AroundCells,1);
  new_dw->allocateTemporary(d_OFC,  patch, Ghost::AroundCells,1);
}


FirstOrderCEAdvector::~FirstOrderCEAdvector()
{
  VarLabel::destroy(OFE_CCLabel);
  VarLabel::destroy(OFC_CCLabel);
}

FirstOrderCEAdvector* FirstOrderCEAdvector::clone(DataWarehouse* new_dw,
                                   const Patch* patch)
{
  return scinew FirstOrderCEAdvector(new_dw,patch);
}

/* ---------------------------------------------------------------------
 Function~  influxOutfluxVolume--
 Purpose~   calculate the individual outfluxes for each cell.
            This includes the slabs, edges and corners
 Steps for each cell:  
 1) calculate the volume for each outflux
 2) test to see if the total outflux > cell volume

Implementation notes:
The outflux of volume is calculated in each cell in the computational domain
+ one layer of extra cells  surrounding the domain.The face-centered velocity 
needs to be defined on all faces for these cells 

See schematic diagram at bottom of ice.cc for del* definitions
 ---------------------------------------------------------------------  */

void FirstOrderCEAdvector::inFluxOutFluxVolume(
                        const SFCXVariable<double>& uvel_FC,
                        const SFCYVariable<double>& vvel_FC,
                        const SFCZVariable<double>& wvel_FC,
                        const double& delT, 
                        const Patch* patch,
                        const int&   indx,
                        const bool& bulletProof_test)

{
  Vector dx = patch->dCell();
  double vol = dx.x()*dx.y()*dx.z();
  double delY_top, delY_bottom,delX_right, delX_left, delZ_front, delZ_back;
  double delX_tmp, delY_tmp,   delZ_tmp;

  // Compute outfluxes 
  const IntVector gc(1,1,1);
  double error_test = 0.0;
  int    num_cells = 0;
  
  for(CellIterator iter = patch->getCellIterator(gc); !iter.done(); iter++){
    IntVector c = *iter;
    delY_top    = std::max(0.0, (vvel_FC[c+IntVector(0,1,0)] * delT));
    delY_bottom = std::max(0.0,-(vvel_FC[c                 ] * delT));
    delX_right  = std::max(0.0, (uvel_FC[c+IntVector(1,0,0)] * delT));
    delX_left   = std::max(0.0,-(uvel_FC[c                 ] * delT));
    delZ_front  = std::max(0.0, (wvel_FC[c+IntVector(0,0,1)] * delT));
    delZ_back   = std::max(0.0,-(wvel_FC[c                 ] * delT));
    
    delX_tmp    = dx.x() - delX_right - delX_left;
    delY_tmp    = dx.y() - delY_top   - delY_bottom;
    delZ_tmp    = dx.z() - delZ_front - delZ_back;
    
    //__________________________________
    //   SLAB outfluxes
    d_advector.d_OFS[c].d_fflux[TOP]   = delY_top   * delX_tmp * delZ_tmp;
    d_advector.d_OFS[c].d_fflux[BOTTOM]= delY_bottom* delX_tmp * delZ_tmp;
    d_advector.d_OFS[c].d_fflux[RIGHT] = delX_right * delY_tmp * delZ_tmp;
    d_advector.d_OFS[c].d_fflux[LEFT]  = delX_left  * delY_tmp * delZ_tmp;
    d_advector.d_OFS[c].d_fflux[FRONT] = delZ_front * delX_tmp * delY_tmp;
    d_advector.d_OFS[c].d_fflux[BACK]  = delZ_back  * delX_tmp * delY_tmp;

    // Edge flux terms
    d_OFE[c].d_eflux[TOP_R]     = delY_top      * delX_right * delZ_tmp;
    d_OFE[c].d_eflux[TOP_FR]    = delY_top      * delX_tmp   * delZ_front;
    d_OFE[c].d_eflux[TOP_L]     = delY_top      * delX_left  * delZ_tmp;
    d_OFE[c].d_eflux[TOP_BK]    = delY_top      * delX_tmp   * delZ_back;
    
    d_OFE[c].d_eflux[BOT_R]     = delY_bottom   * delX_right * delZ_tmp;
    d_OFE[c].d_eflux[BOT_FR]    = delY_bottom   * delX_tmp   * delZ_front;
    d_OFE[c].d_eflux[BOT_L]     = delY_bottom   * delX_left  * delZ_tmp;
    d_OFE[c].d_eflux[BOT_BK]    = delY_bottom   * delX_tmp   * delZ_back;
    
    d_OFE[c].d_eflux[RIGHT_BK]  = delY_tmp      * delX_right * delZ_back;
    d_OFE[c].d_eflux[RIGHT_FR]  = delY_tmp      * delX_right * delZ_front;
    
    d_OFE[c].d_eflux[LEFT_BK]   = delY_tmp      * delX_left  * delZ_back;
    d_OFE[c].d_eflux[LEFT_FR]   = delY_tmp      * delX_left  * delZ_front;
    
    //__________________________________
    //   Corner flux terms
    d_OFC[c].d_cflux[TOP_R_BK]  = delY_top      * delX_right * delZ_back;
    d_OFC[c].d_cflux[TOP_R_FR]  = delY_top      * delX_right * delZ_front;
    d_OFC[c].d_cflux[TOP_L_BK]  = delY_top      * delX_left  * delZ_back;
    d_OFC[c].d_cflux[TOP_L_FR]  = delY_top      * delX_left  * delZ_front;
    
    d_OFC[c].d_cflux[BOT_R_BK]  = delY_bottom   * delX_right * delZ_back;
    d_OFC[c].d_cflux[BOT_R_FR]  = delY_bottom   * delX_right * delZ_front;
    d_OFC[c].d_cflux[BOT_L_BK]  = delY_bottom   * delX_left  * delZ_back;
    d_OFC[c].d_cflux[BOT_L_FR]  = delY_bottom   * delX_left  * delZ_front;

    //__________________________________
    //  Bullet proofing
    double total_fluxout = 0.0;
    for(int face = TOP; face <= BACK; face++ )  {
      total_fluxout  += d_advector.d_OFS[c].d_fflux[face];
    }
    for(int edge = TOP_R; edge <= LEFT_FR; edge++ )  {
      total_fluxout  += d_OFE[c].d_eflux[edge];
    }
    for(int corner = TOP_R_BK; corner <= BOT_L_FR; corner++ )  {
      total_fluxout  += d_OFC[c].d_cflux[corner];
    }
    num_cells++;
    error_test +=(vol - total_fluxout)/fabs(vol- total_fluxout);
  }  // cell iterator
  //__________________________________
  // if total_fluxout > vol then 
  // find the cell and throw an exception.  
  if (fabs(error_test - num_cells) > 1.0e-2 && bulletProof_test) {
    for(CellIterator iter = patch->getCellIterator(gc); !iter.done(); iter++){
      IntVector c = *iter; 
      double total_fluxout = 0.0;
      for(int face = TOP; face <= BACK; face++ )  {
        total_fluxout  += d_advector.d_OFS[c].d_fflux[face];
      }
      for(int edge = TOP_R; edge <= LEFT_FR; edge++ )  {
        total_fluxout  += d_OFE[c].d_eflux[edge];
      }
      for(int corner = TOP_R_BK; corner <= BOT_L_FR; corner++ )  {
        total_fluxout  += d_OFC[c].d_cflux[corner];
      }
      if (vol - total_fluxout < 0.0) {
        throw OutFluxVolume(*iter,total_fluxout, vol, indx);
      }
    }  // cell iter
  }  // if total_fluxout > vol
}

/* ---------------------------------------------------------------------
 Function~ advectQ
_____________________________________________________________________*/
//     D O U B L E
void FirstOrderCEAdvector::advectQ(const CCVariable<double>& q_CC,
                             const Patch* patch,
                             CCVariable<double>& q_advected,
				 DataWarehouse* /*new_dw*/)
{ 
  advectCE<double>(q_CC,patch,q_advected, 
                      d_notUsedX, d_notUsedY, d_notUsedZ, 
                      ignoreFaceFluxesD);
}
//__________________________________
//  S P E C I A L I Z E D   D O U B L E 
//  needed by implicit solve
void FirstOrderCEAdvector::advectQ(const CCVariable<double>& q_CC,
                             const Patch* patch,
                             CCVariable<double>& q_advected,
                             SFCXVariable<double>& q_XFC,
                             SFCYVariable<double>& q_YFC,
                             SFCZVariable<double>& q_ZFC,
				 DataWarehouse* /*new_dw*/)
{
  advectCE<double>(q_CC,patch,q_advected,  
                      q_XFC, q_YFC, q_ZFC, saveFaceFluxes);
}
//__________________________________
//     V E C T O R
void FirstOrderCEAdvector::advectQ(const CCVariable<Vector>& q_CC,
                             const Patch* patch,
                             CCVariable<Vector>& q_advected,
				 DataWarehouse* /*new_dw*/)
{
  advectCE<Vector>(q_CC,patch,q_advected, 
                      d_notUsedX, d_notUsedY, d_notUsedZ, 
                      ignoreFaceFluxesV);
}

/* ---------------------------------------------------------------------
 Function~  Advect--  driver program that does the advection  
_____________________________________________________________________*/
template <class T, typename F> 
  void FirstOrderCEAdvector::advectCE(const CCVariable<T>& q_CC,
                                  const Patch* patch,                   
                                  CCVariable<T>& q_advected,
                                  SFCXVariable<double>& q_XFC,
                                  SFCYVariable<double>& q_YFC,
                                  SFCZVariable<double>& q_ZFC,
                                  F save_q_FC)  // function is passed in
{

  for(CellIterator iter = patch->getCellIterator(); !iter.done(); iter++) { 
    double oneThird = 1.0/3.0;
    IntVector c = *iter;
    //__________________________________
    //   all faces
    T q_face_flux[6];
    double faceVol[6];
         
    for(int f = TOP; f <= BACK; f++ )  {    
      double slab_vol = 0.0;
      T q_slab_flux = T(0.0);
      
      //__________________________________
      //   S L A B S
      IntVector ac = c + S_ac[f];     // slab adjacent cell
      double outfluxVol = d_advector.d_OFS[c ].d_fflux[OF_slab[f]];
      double influxVol  = d_advector.d_OFS[ac].d_fflux[IF_slab[f]];

      q_slab_flux  = - q_CC[c]  * outfluxVol + q_CC[ac] * influxVol;             
      slab_vol    +=  outfluxVol +  influxVol;                 

      //__________________________________
      //   E D G E S  
      T q_edge_flux = T(0.0);
      double edge_vol = 0.0;

      for(int e = 0; e < 4; e++ ) {
        int OF = OF_edge[f][e];        // cleans up the equations
        int IF = IF_edge[f][e];

        IntVector ac = c + E_ac[f][e]; // adjcent cell
        outfluxVol = 0.5 * d_OFE[c ].d_eflux[OF];
        influxVol  = 0.5 * d_OFE[ac].d_eflux[IF];

        q_edge_flux += -q_CC[c] * outfluxVol
                    +  q_CC[ac] * influxVol;
        edge_vol    += outfluxVol + influxVol;
      }                

      //__________________________________
      //   C O R N E R S
      T q_corner_flux = T(0.0);
      double corner_vol = 0.0;

      for(int crner = 0; crner < 4; crner++ ) {
        int OF = OF_corner[f][crner];      // cleans up the equations
        int IF = IF_corner[f][crner];

        IntVector ac = c + C_ac[f][crner]; // adjcent cell
        outfluxVol = oneThird * d_OFC[c ].d_cflux[OF];
        influxVol  = oneThird * d_OFC[ac].d_cflux[IF];

        q_corner_flux += -q_CC[c] * outfluxVol 
                      +  q_CC[ac] * influxVol; 
        corner_vol    += outfluxVol + influxVol;
      }  //  corner loop
      
      q_face_flux[f] = q_slab_flux + q_edge_flux + q_corner_flux;
      faceVol[f]     = slab_vol + edge_vol + corner_vol; 
    }  // face loop 
       
    //__________________________________
    //  sum up all the contributions
    q_advected[c] = T(0.0);
    for(int f = TOP; f <= BACK; f++ )  {
      q_advected[c] += q_face_flux[f];
    }
    
    //__________________________________
    //  inline function to compute q_FC
    save_q_FC(c, q_XFC, q_YFC, q_ZFC, faceVol, q_face_flux);
  }
}
//______________________________________________________________________
//  
namespace Uintah {

  static MPI_Datatype makeMPI_eflux()
  {
    ASSERTEQ(sizeof(FirstOrderCEAdvector::eflux), sizeof(double)*12);
    MPI_Datatype mpitype;
    MPI_Type_vector(1, 12, 12, MPI_DOUBLE, &mpitype);
    MPI_Type_commit(&mpitype);
    return mpitype;
  }
  
  const TypeDescription* fun_getTypeDescription(FirstOrderCEAdvector::eflux*)
  {
    static TypeDescription* td = 0;
    if(!td){
      td = scinew TypeDescription(TypeDescription::Other,
                              "FirstOrderCEAdvector::eflux", true, 
                              &makeMPI_eflux);
    }
    return td;
  }
  
  static MPI_Datatype makeMPI_cflux()
  {
    ASSERTEQ(sizeof(FirstOrderCEAdvector::cflux), sizeof(double)*8);
    MPI_Datatype mpitype;
    MPI_Type_vector(1, 8, 8, MPI_DOUBLE, &mpitype);
    MPI_Type_commit(&mpitype);
    return mpitype;
  }
  
  const TypeDescription* fun_getTypeDescription(FirstOrderCEAdvector::cflux*)
  {
    static TypeDescription* td = 0;
    if(!td){
      td = scinew TypeDescription(TypeDescription::Other,
                              "FirstOrderCEAdvector::cflux", true, 
                              &makeMPI_cflux);
    }
    return td;
  }
  
}

namespace SCIRun {

void swapbytes( Uintah::FirstOrderCEAdvector::eflux& e) {
  double *p = e.d_eflux;
  SWAP_8(*p); SWAP_8(*++p); SWAP_8(*++p);
  SWAP_8(*++p); SWAP_8(*++p); SWAP_8(*++p);
  SWAP_8(*++p); SWAP_8(*++p); SWAP_8(*++p);
  SWAP_8(*++p); SWAP_8(*++p); SWAP_8(*++p);
}
  
void swapbytes( Uintah::FirstOrderCEAdvector::cflux& c) {
  double *p = c.d_cflux;
  SWAP_8(*p); SWAP_8(*++p); SWAP_8(*++p); SWAP_8(*++p);
  SWAP_8(*++p); SWAP_8(*++p); SWAP_8(*++p); SWAP_8(*++p);
}

} // namespace SCIRun
