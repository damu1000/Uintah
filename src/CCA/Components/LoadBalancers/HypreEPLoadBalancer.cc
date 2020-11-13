/*
 * The MIT License
 *
 * Copyright (c) 1997-2020 The University of Utah
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <CCA/Components/LoadBalancers/HypreEPLoadBalancer.h>

#include <Core/Grid/Grid.h>
#include <Core/Grid/Level.h>
#include <Core/Grid/Patch.h>
#include <Core/Parallel/Parallel.h>
#include <Core/Parallel/ProcessorGroup.h>

using namespace Uintah;

//patches numbers are always assigned serially. EP assignment logic may vary.
//index into g_patch_ep indicates patch id, values indicates assigned EP.
std::vector<int> g_patch_ep;
std::vector<int> g_ep_superpatch; //ep to superpatch mapping
int g_superpatches[3];
int g_num_of_ranks3d[3];

#define OneDtoThreeD(t, p, x_patches, y_patches, z_patches)	\
	t[2] = p / (y_patches*x_patches);						\
	t[1] = (p % (y_patches*x_patches)) / x_patches;			\
	t[0] = p % x_patches;

HypreEPLoadBalancer::HypreEPLoadBalancer( const ProcessorGroup * myworld )
  : LoadBalancerCommon( myworld )
{}

int
HypreEPLoadBalancer::getPatchwiseProcessorAssignment( const Patch * patch )
{
  const Patch * realPatch = patch->getRealPatch();

#ifdef USE_MPI_EP
  int proc = m_patch_to_proc[realPatch->getID()];
#else
  long long     numProcs  = d_myworld->nRanks();
  int proc = (realPatch->getLevelIndex() * numProcs) / static_cast<long long>(realPatch->getLevel()->numPatches());
  ASSERTRANGE(proc, 0, d_myworld->nRanks());
#endif
  return proc;
}

//todo: populate m_patch_to_proc for the finest level,
//figure out usage of global variables,
//check whether g_patch_ep and g_ep_superpatch are passed to hypre
//check whether hypre's fine level patches need to have index starting from 0. In Uintah's setting it wont be 0.
//tie everything into hypresolver.cc

void
HypreEPLoadBalancer::problemSetup( ProblemSpecP & pspec, GridP & grid, const MaterialManagerP & materialManager )
{


  proc0cout << "Using HypreEP load balancer. Communication reducing patch assignment.\n";

  LoadBalancerCommon::problemSetup( pspec, grid, materialManager );

#ifdef USE_MPI_EP
  int size = Uintah::Parallel::getMPISize();
  int xthreads = Uintah::Parallel::xthreads(), ythreads = Uintah::Parallel::ythreads(), zthreads = Uintah::Parallel::zthreads();
  int num_of_threads = xthreads * ythreads * zthreads;
  int num_of_eps = size * num_of_threads; // this number of end points
  int maxLevels = grid->numLevels();
  int tot_num_patches = 0, num_coarse_patches=0;


  if(xthreads == -1 || ythreads == -1 || zthreads == -1){
	  printf("Please pass -xthreads <#xthreads> -ythreads <#ythreads> -zthreads <#zthreads> to the commandline\n");
	  exit(1);
  }


  for(int l=0; l < maxLevels; l++)
	  tot_num_patches += grid->getLevel(l)->numPatches();
  m_patch_to_proc.resize(tot_num_patches);

  for(int p=0; p<tot_num_patches; p++)
	  m_patch_to_proc[p]=-1;

  for(int l=0; l < maxLevels; l++){
	  const LevelP level = grid->getLevel(l);

	  if(l < maxLevels-1){ //assuming maxLevels-1 is the finest level. Assign simple load balancing for coarse patches.
		  for(int p=0; p<level->numPatches(); p++){
			  const Patch *patch = level->getPatch(p);
			  const Patch *realPatch = patch->getRealPatch();
			  int proc = (realPatch->getLevelIndex() * size) / static_cast<long long>(realPatch->getLevel()->numPatches()); // same as simple load balancer
			  m_patch_to_proc[realPatch->getID()] = proc;
			  num_coarse_patches++;
		  }
		  continue;
	  }

	  IntVector patchDist = level->getPatchDistribution();
	  int xpatches = patchDist[0], ypatches = patchDist[1], zpatches = patchDist[2];
	  int num_of_patches = xpatches * ypatches * zpatches;
	  int patches_per_ep = num_of_patches / num_of_eps;
	  int num_of_psets = num_of_patches / num_of_threads; //number of patch sets
	  int num_of_psets_per_rank = num_of_psets / size;
	  int xpsets = xpatches / xthreads,  ypsets = ypatches / ythreads,  zpsets = zpatches / zthreads;

	  if(num_of_eps > num_of_patches){
		  printf("Ensure that num of end points <= num of patches\n"); exit(1);
	  }

	  //bulletproofing: Ensure num_of_psets_per_rank is aligned with xpsets, ypsets and zpsets
	  g_num_of_ranks3d[0] = xpsets, g_num_of_ranks3d[1] = ypsets, g_num_of_ranks3d[2] = zpsets;
	  int temp = num_of_psets_per_rank;

	  for(int i=0; i<3; i++){
		  if(g_num_of_ranks3d[i] >= temp){ //patches fit within the given dimension. recompute and break
			  if(g_num_of_ranks3d[i] % temp != 0){ //patch dim should be multiple of patches_per_ep
				  printf("Part 1: number of psets per rank should be aligned with patch dimensions at %s:%d\n", __FILE__, __LINE__);exit(1);
			  }
			  g_num_of_ranks3d[i] = g_num_of_ranks3d[i] / temp;
			  temp = 1;
			  break;
		  }
		  else{//there can be multiple rows or planes assigned to one EP
			  if(temp % g_num_of_ranks3d[i] != 0){ //now patches_per_ep should be multiple of patch_dim
				  printf("Part 2: number of psets per rank should be aligned with patch dimensions at %s:%d\n", __FILE__, __LINE__);exit(1);
			  }
			  temp = temp / g_num_of_ranks3d[i];
			  g_num_of_ranks3d[i] = 1; //current dimension will be 1 because no division across this dimension
		  }
	  }

	  if(temp !=1 || size != g_num_of_ranks3d[0]*g_num_of_ranks3d[1]*g_num_of_ranks3d[2]){
		  printf("Part 3: Error in superpatch generation logic at %s:%d\n", __FILE__, __LINE__);exit(1);
	  }

	  g_patch_ep.resize(xpatches * ypatches * zpatches);
	  g_ep_superpatch.resize(num_of_eps);

	  //Each rank will hold patchsets from rank*num_of_psets_per_rank to rank*num_of_psets_per_rank + num_of_psets_per_rank - 1
	  //bulletproofing ensures patchset assignment among ranks is aligned and continuous. So ranks can be converted directly to 3D coordinates for odd-even logic
	  //patch assignment
	  for(int rank=0; rank<size; rank++){
		  //convert low and high patchset ids to 3d and multiply by xthreads, ythreads, zthreads to find out first and the last patch ids of the rank
		  int plow[3], phigh[3], pslownum = rank*num_of_psets_per_rank, pshighnum = rank*num_of_psets_per_rank + num_of_psets_per_rank-1;
		  OneDtoThreeD(plow, pslownum, xpsets, ypsets, zpsets);	//plow has 3d patch set id of the first patch of the rank;
		  OneDtoThreeD(phigh, pshighnum, xpsets, ypsets, zpsets);	//phigh has 3d patch set id of the last patch of the rank;
		  //Now multiply by xthreads, ythreads, zthreads to find out first and the last patch ids of the rank
		  plow[0] *= xthreads; plow[1] *= ythreads; plow[2] *= zthreads;
		  phigh[0] *= xthreads; phigh[1] *= ythreads; phigh[2] *= zthreads;
		  phigh[0] += xthreads; phigh[1] += ythreads; phigh[2] += zthreads; //Add xthreads, ythreads, zthreads to take high to the last patch within the pset
		  int tid = 0, count=0;
		  //now iterate over patches and assign EPs.
		  for(int k=plow[2]; k<phigh[2]; k++)
			  for(int j=plow[1]; j<phigh[1]; j++)
				  for(int i=plow[0]; i<phigh[0]; i++){
					  int patch = k * xpatches * ypatches + j * xpatches + i;
					  int ep = rank * num_of_threads + tid;
					  g_patch_ep[patch] = ep;
					  g_ep_superpatch[ep] = patch / patches_per_ep;
					  m_patch_to_proc[num_coarse_patches + patch] = rank; //patch this loop considered only fine level patches. Add num of patches at coarse levels.
					  count++;
					  if(count % patches_per_ep == 0) tid++;
					  //calculate super patches within ep to find out directions
				  }
	  }

	  //	if(rank==0){
	  //		printf("ranks shape: %d %d %d\n", g_num_of_ranks3d[0], g_num_of_ranks3d[1], g_num_of_ranks3d[2]);
	  //		for(int i=0; i<g_patch_ep.size(); i++)
	  //			printf("patch %d EP %d\n", i, g_patch_ep[i]);
	  //
	  //		for(int i=0; i<g_ep_superpatch.size(); i++)
	  //			printf("EP %d superpatch %d\n", i, g_ep_superpatch[i]);
	  //	}


	  //compute number of number of superpatches in each dimension. Needed to convert super patch number to 3d during comm mapping.
	  //same logic as used earlier to compute number of ranks in each direction
	  g_superpatches[0] = xpatches, g_superpatches[1] = ypatches, g_superpatches[2] = zpatches;

	  int spatches_per_ep = patches_per_ep;
	  for(int i=0; i<3; i++){
		  if(g_superpatches[i] >= spatches_per_ep){ //patches fit within the given dimension. recompute and break
			  if(g_superpatches[i] % spatches_per_ep != 0){ //patch dim should be multiple of patches_per_ep
				  printf("Part 1: number of patches per EP should be aligned with patch dimensions at %s:%d\n", __FILE__, __LINE__);exit(1);
			  }
			  g_superpatches[i] = g_superpatches[i] / spatches_per_ep;
			  spatches_per_ep = 1;
			  break;
		  }
		  else{//there can be multiple rows or planes assigned to one EP
			  if(spatches_per_ep % g_superpatches[i] != 0){ //now patches_per_ep should be multiple of patch_dim
				  printf("Part 2: number of patches per EP should be aligned with patch dimensions at %s:%d\n", __FILE__, __LINE__);exit(1);
			  }
			  spatches_per_ep = spatches_per_ep / g_superpatches[i];
			  g_superpatches[i] = 1; //current dimension will be 1 because no division across this dimension
		  }
	  }

	  if(spatches_per_ep !=1 || num_of_eps != g_superpatches[0]*g_superpatches[1]*g_superpatches[2]){
		  printf("Part 3: Error in superpatch generation logic at %s:%d\n", __FILE__, __LINE__);exit(1);
	  }

  }

  //used to create a comm map in hypre if multiple comms are used
  Uintah::Parallel::setHypreCommInfo(g_ep_superpatch.data(), g_superpatches, g_num_of_ranks3d);

//  if(Uintah::Parallel::getMPIRank()==0)
//  for(int p=0; p<tot_num_patches; p++)
//	  printf("%d %d %d\n", Uintah::Parallel::getMPIRank(), p, m_patch_to_proc[p]);
#endif

}
