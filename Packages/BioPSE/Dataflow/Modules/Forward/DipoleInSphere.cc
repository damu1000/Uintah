/*
  The contents of this file are subject to the University of Utah Public
  License (the "License"); you may not use this file except in compliance
  with the License.
  
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  License for the specific language governing rights and limitations under
  the License.
  
  The Original Source Code is SCIRun, released March 12, 2001.
  
  The Original Source Code was developed by the University of Utah.
  Portions created by UNIVERSITY are Copyright (C) 2001, 1994 
  University of Utah. All Rights Reserved.
*/

/*
  The contents of this file are subject to the University of Utah Public
  License (the "License"); you may not use this file except in compliance
  with the License.
  
  Software distributed under the License is distributed on an "AS IS"
  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
  License for the specific language governing rights and limitations under
  the License.
  
  The Original Source Code is SCIRun, released March 12, 2001.
  
  The Original Source Code was developed by the University of Utah.
  Portions created by UNIVERSITY are Copyright (C) 2001, 1994 
  University of Utah. All Rights Reserved.
*/

/*
 *  DipoleInSphere: Calculation of potential on 
 *                  conducting sphere due to the dipole sources
 *
 *  Written by:
 *   David Weinstein
 *   Department of Computer Science
 *   University of Utah
 *   October 1994
 *
 *  Modified by:
 *   Samsonov Alexei
 *   Department of Computer Science
 *   University of Utah
 *   March 2001
 *
 *  Copyright (C) 1994, 2001 SCI Group
 */

#include <Dataflow/Network/Module.h>
#include <Core/Datatypes/Field.h>
#include <Core/Datatypes/TriSurf.h>
#include <Core/Datatypes/DenseMatrix.h>
#include <Dataflow/Ports/FieldPort.h>
#include <Dataflow/Ports/MatrixPort.h>
#include <Core/Containers/String.h>
#include <Core/Containers/Array1.h>
#include <Core/Geometry/Point.h>
#include <Core/GuiInterface/GuiVar.h>


#include <iostream>
using std::cerr;
#include <stdio.h>
#include <math.h>

namespace BioPSE {
using namespace SCIRun;

typedef LockingHandle<TriSurf<double> > TriSurfHandle;
typedef LockingHandle<TriSurfMesh > TriSurfMeshHandle;

class DipoleInSphere : public Module {
  
  //! Private Data

  //! input ports
  MatrixIPort* iportDip_;
  FieldIPort*  iportGeom_;

  //! output port
  FieldOPort*  oportPot_;

  //! Private Methods
  // -- fills in the surface with potentials for single sphere uniform model
  void fillOneSpherePotentials(DenseMatrix&, TriSurfHandle);

public:
  
  DipoleInSphere(const clString& id);  
  virtual ~DipoleInSphere();
  virtual void execute();
};

extern "C" Module* make_DipoleInSphere(const clString& id)
{
  return new DipoleInSphere(id);
}

DipoleInSphere::DipoleInSphere(const clString& id)
: Module("DipoleInSphere", id, Filter)
{
  // Create the input ports
  iportGeom_ = new FieldIPort(this, "Sphere", FieldIPort::Atomic);
  add_iport(iportGeom_);
  
  iportDip_ = new MatrixIPort(this, "Dipoles", MatrixIPort::Atomic);
  add_iport(iportDip_);
  
  // Create the output port
  oportPot_ = new FieldOPort(this, "SphereWithPots", FieldIPort::Atomic);
  add_oport(oportPot_);
}

DipoleInSphere::~DipoleInSphere()
{
}

void DipoleInSphere::execute() {
  
  update_state(NeedData);
  
  FieldHandle field_handle;
  
  if (!iportGeom_->get(field_handle)){
    msgStream_ << "Cann't get data" << endl;
    return;
  }
  
  if (!field_handle.get_rep()) {
    msgStream_ << "Error: empty surface" << endl;
    return;
  }
 
  if (field_handle->get_type_name(0) == "TriSurf" && field_handle->get_type_name(1) == "double"){
    
    
    TriSurf<double>* pSurf = dynamic_cast<TriSurf<double>*>(field_handle.get_rep());
    TriSurfMeshHandle hMesh = new TriSurfMesh(*(pSurf->get_typed_mesh().get_rep()));
    TriSurfHandle hNewSurf = new TriSurf<double>(hMesh, Field::NODE);
    
    MatrixHandle matrix_handle;
    
    if (iportDip_->get(matrix_handle) 
	&& matrix_handle.get_rep()){
      
      if (matrix_handle->ncols()!=6){
	msgStream_ << "Error: dipoles must have 6 parameters" << endl;
	return;
      }
      
      DenseMatrix* mtrx = matrix_handle->getDense();
      
      if (!mtrx){
	msgStream_ << "Error: Dipoles should be defined in DenseMatrix" << endl;
	return;
      }
      
      update_state(JustStarted);
      fillOneSpherePotentials(*mtrx, hNewSurf);
      oportPot_->send(FieldHandle(hNewSurf.get_rep()));
    }
    else {
      msgStream_ << "No dipole info found in dipole matrix supplied" << endl;
    }
   
  }
  else {
    msgStream_ << "Error: the supplied field is not of type TriSurf<double>" << endl;
    return;
  }
}

void DipoleInSphere::fillOneSpherePotentials(DenseMatrix& dips, TriSurfHandle hSurf) {
  
  TriSurfMeshHandle hMesh = hSurf->get_typed_mesh();
  vector<double>& data = hSurf->fdata();
  data.resize(*(hMesh->node_end()), 0);
  BBox bbox = hMesh->get_bounding_box();
  
  if (!bbox.valid()){
    msgStream_ << "No valid mesh" << endl;
    return;
  }

  double R = 0.5*bbox.longest_edge();
  
  double gamma=1;
  double E[3];
  msgStream_ << "Radius= " << R << endl;
  Point p;

  if (hMesh->node_begin()!=hMesh->node_end()){ // don't want to iterate if no dipoles
    
    TriSurfMesh::node_iterator niter = hMesh->node_begin();
    
    // -- for every point
    while(niter!=hMesh->node_end()) {
      
      hMesh->get_point(p, *niter);
      
      // -- for every dipole
      int id;
      for (id = 0; id < dips.nrows(); ++id){
	
	double V = 0.0;
	E[0] = p.x();
	E[1] = p.y();
	E[2] = p.z();
	
	double rho = sqrt( pow((E[0] - dips[id][0]),2) + pow((E[1] - dips[id][1]),2) + pow((E[2] - dips[id][2]),2));
	double S = E[0]*dips[id][0] + E[1]*dips[id][1] + E[2]*dips[id][2];
	
	for(int k=0;k<3;k++) {
	  double F[3];
	  F[k] = (1/(4*M_PI*gamma*rho)) * (2*(E[k]-dips[id][k])/pow(rho,2) +
					   (1/pow(R,2)) * (E[k] + (E[k]*S/R - R*dips[id][k])/(rho+R-S/R)));
	  V += F[k]*dips[id][k+3];
	}
	data[*niter] += V;
	msgStream_ << "Point: " << p << ", pot = " << V << endl;
      }
      
      ++niter;
    }
  }    
}

} // End namespace BioPSE
