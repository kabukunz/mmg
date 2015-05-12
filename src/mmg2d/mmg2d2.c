/* =============================================================================
**  This file is part of the mmg software package for the tetrahedral
**  mesh modification.
**  Copyright (c) Inria - IMB (Université de Bordeaux) - LJLL (UPMC), 2004- .
**
**  mmg is free software: you can redistribute it and/or modify it
**  under the terms of the GNU Lesser General Public License as published
**  by the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  mmg is distributed in the hope that it will be useful, but WITHOUT
**  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
**  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
**  License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License and of the GNU General Public License along with mmg (in
**  files COPYING.LESSER and COPYING). If not, see
**  <http://www.gnu.org/licenses/>. Please read their terms carefully and
**  use this copy of the mmg distribution only if you accept them.
** =============================================================================
*/
#include "mmg2d.h"
#define EPSDD  1e+6

unsigned int MMG2_iare2[3][2] = { {0,1}, {0,2}, {1,2} };
unsigned int MMG2_inxt[5] = {1,2,0,1,2};
extern int ddebug;

/*remove BB triangles*/
int MMG2_removeBBtriangles(MMG5_pMesh mesh) {
  MMG5_pTria   pt;
  int     ip1,ip2,ip3,ip4,k,iadr,*adja,iadr2,*adja2;
  int     i,nd,base;
  
  /*BB vertex*/
  ip1=(mesh->np-3);
  ip2=(mesh->np-2);
  ip3=(mesh->np-1);
  ip4=(mesh->np);

  if(mesh->info.ddebug) MMG2_saveMesh(mesh,"avtdel.mesh");
  nd = 0; 
  for(k=1 ; k<=mesh->nt ; k++) {
    pt  = &mesh->tria[k];
    if(!pt->v[0]) continue; 
    if(pt->base<0) {
      iadr = 3*(k-1) + 1;                      
      adja = &mesh->adja[iadr]; 
      for(i=0 ; i<3 ; i++) {
	if(!adja[i]) continue;
	iadr2 = 3*(adja[i]/3-1) + 1;
	adja2 = &mesh->adja[iadr2];
	adja2[adja[i]%3] = 0;
      }                  
      MMG2_delElt(mesh,k);
      continue;
    } else if(!pt->base) {
      printf("UNDETERMINED TRIANGLE %d : %d %d %d\n",k,pt->v[0],pt->v[1],pt->v[2]);
      nd++;
    }
  }
  if(mesh->info.ddebug) MMG2_saveMesh(mesh,"apresdel.mesh");
           
  if(!nd) {
    MMG2_delPt(mesh,ip1);
    MMG2_delPt(mesh,ip2);
    MMG2_delPt(mesh,ip3);
    MMG2_delPt(mesh,ip4); 
  } else {
    fprintf(stdout,"PROCEDURE FAILED : %d INDETERMINED TRIANGLES\n",nd);
    MMG2_saveMesh(mesh,mesh->nameout);
    return(0);
  }
  return(1);
}

/*tag des triangles : in = base ; out = -base ; indetermine = 0*/
int MMG2_settagtriangles(MMG5_pMesh mesh,MMG5_pSol sol) {
  MMG5_pTria  pt,pt1;
  MMG5_pTria   tmp;
  double crit,cal1,cal2;
  int    base,nd,ns,iter,maxiter,k,nc,i1,i2,madj;
  int    ip1,ip2,ip3,ip4,*list,*adja,i,adjj,*adj;
  
  /*BB vertex*/
  ip1=(mesh->np-3);
  ip2=(mesh->np-2);
  ip3=(mesh->np-1);
  ip4=(mesh->np);

  base = ++mesh->base; 
  ns      = 1;             
  iter    = 0;
  maxiter = 3;
  do {
    nd = 0;
    for(k=1 ; k<=mesh->nt ; k++) {
      pt = &mesh->tria[k];
      if ( !M_EOK(pt) )  continue;
      if(!MMG2_findtrianglestate(mesh,k,ip1,ip2,ip3,ip4,base)) nd++ ;
    }       
    printf("how many undetermined triangles ? %d\n",nd);
  } while (nd && ++iter<maxiter);
  
  return(1);
}

/*insertion of the list of points inside the mesh*/
/*return 0 if pbs occur*/
int MMG2_insertpointdelone(MMG5_pMesh mesh,MMG5_pSol sol) {
  MMG5_pTria   pt,pt1;
  MMG5_pPoint  ppt;
  double  declic;
  int     list[MMG2_LONMAX],ilist,lon;
  int     k,ret;
  int kk, iadr,*adja;

  for(k=1 ; k<=mesh->np - 4 ; k++) {  

    ppt = &mesh->point[k];
    /*recherche du triangle contenant le point : lel*/
    list[0] = MMG2_findTria(mesh,k);  
    assert(list[0]);
    ilist = 1;

    lon = _MMG2_cavity(mesh,sol,k,list);
    printf("on trouve %d tria dans la cavity\n",lon);
    if ( lon < 1 ) {
      fprintf(stdout,"impossible d'inserer le point\n");
MMG2_saveMesh(mesh,"toto.mesh");
      exit(0);
    } else {
      ret = _MMG2_delone(mesh,sol,k,list,lon);
    }
    if(k==5) {
      for(kk=1 ; kk<=mesh->nt ; kk++) {
	pt = &mesh->tria[kk];
	if(!pt->v[0]) continue;
	printf("tr %d : %d %d %d\n",kk,pt->v[0],pt->v[1],pt->v[2]);
	iadr = 3*(kk-1)+1;
	adja = &mesh->adja[iadr];
	printf("adja %d %d %d\n",adja[0]/3, adja[1]/3,adja[2]/3);
      }
        MMG2_chkmsh(mesh,1);
	// MMG2_saveMesh(mesh,"toto.mesh");
	//exit(0);
    }

  }

  return(1);
}
/*insertion of the list of points inside the mesh*/
/*return 0 if pbs occur*/
int MMG2_insertpoint(MMG5_pMesh mesh,MMG5_pSol sol) {
  MMG5_pTria   pt,pt1;
  MMG5_pPoint  ppt;
  double  declic;
  int     k,nsiter,lel,iel,mel,nel,ia,ib,ic,aext0,aext1,aext2;
  int     iadr,*adja,nflat,ie1,ie2,ie3,voy,text,atext1,atext2;
  for(k=1 ; k<=mesh->np - 4 ; k++) {  
    if(1){ 
      if(ddebug) printf("------------------------------------try swap to increase mesh quality\n");     
      declic = 1.1 / ALPHA; 
      nsiter = 1;
      while (nsiter) {
        nsiter = MMG2_cendel(mesh,sol,declic,-1);  
        if ( nsiter && mesh->info.imprim < 0)
          fprintf(stdout,"     %7d SWAPPED\n",nsiter);   
      }
    }     
    ppt = &mesh->point[k];
    if(ppt->tmp==1) continue;      
    /*recherche du triangle contenant le point : lel*/
    lel = MMG2_findTria(mesh,k);    
    if(!lel) {         
      //printf("on cherche le point %d : %e %e %d\n",k,ppt->c[0],ppt->c[1],ppt->ref);
      //MMG2_saveMesh(mesh,"tmp.mesh");  
    }
    assert(lel);
    nflat = 0; // to avoid bad triangle  1, 2, 4, 3, 5, 7
    pt  = &mesh->tria[lel];
    iadr = 3*(lel-1) + 1; 
    adja = &mesh->adja[iadr];
    ia  = pt->v[0];
    ib  = pt->v[1];
    ic  = pt->v[2];
    aext0 = adja[0];
    aext1 = adja[1];                
    aext2 = adja[2];                
    if(ddebug) printf("tri trouve %d : %d %d %d\n",lel,ia,ib,ic);                
    /*creation de trois triangles*/
    mel = MMG2_newElt(mesh);
    nel = MMG2_newElt(mesh);  
    pt->v[2] = k; 
    pt->qual = MMG2_caltri(mesh,sol,pt);     
    if(pt->qual > EPSDD) {
      if(ddebug) printf("1 : on a un pbs avec le tr %d\n",lel);
      nflat = 1;             
    } else {                                                                   
      adja[0] = 3*nel + 1;
      adja[1] = 3*mel + 0; 
      if(aext2)
	(&mesh->adja[3*(aext2/3-1) + 1])[aext2%3] = 3*lel + 2;   
      //printf("adj of %d : %d %d %d -- %d(%d) : %d\n",lel,nel,mel,aext2/3,aext2/3,aext2%2,lel);      
    }
    pt  = &mesh->tria[nel];   
    pt->base = mesh->base;
    iadr = 3*(nel-1) + 1; 
    adja = &mesh->adja[iadr];
    pt->v[0] = ib;
    pt->v[1] = ic;
    pt->v[2] = k; 
    pt->qual = MMG2_caltri(mesh,sol,pt);     
    if(pt->qual > EPSDD) {
      nflat += 2;
      if(ddebug) printf("2 : on a un pbs avec le tr %d\n",nel);
    } else {    
      adja[0] = 3*mel + 1;
      adja[1] = 3*lel + 0;
      adja[2] = aext0;
      if(aext0)
	(&mesh->adja[3*(aext0/3-1) + 1])[aext0%3] = 3*nel + 2;  
      //printf("adj of %d : %d %d %d -- %d(%d) : %d\n",nel,mel,lel,aext0/3,aext0/3,aext0%3,nel);
    }    
    pt  = &mesh->tria[mel];   
    pt->base = mesh->base;
    iadr = 3*(mel-1) + 1; 
    adja = &mesh->adja[iadr];
    pt->v[0] = ic;
    pt->v[1] = ia;
    pt->v[2] = k; 
    pt->qual = MMG2_caltri(mesh,sol,pt);     
    if(pt->qual > EPSDD) {
      if(ddebug) printf("3 : on a un pbs avec le tr %d nflat : %d\n",mel,nflat); 
      nflat += 4;
      /*on coupe le tr aext1 en 2*/
      text = aext1/3; 
      voy  = aext1%3;
      pt1  = &mesh->tria[text];
      ie1  = pt1->v[voy];
      ie2  = pt1->v[MMG2_inxt[voy]];
      ie3  = pt1->v[MMG2_inxt[voy+1]];
      pt1->v[0] = ie1;
      pt1->v[1] = ie2;
      pt1->v[2] = k;
      pt1->qual = MMG2_caltri(mesh,sol,pt1);  
      if(ddebug) printf("on cree %d : %d %d %d\n",text,ie1,ie2,k);    
      pt->v[0] = ie3;
      pt->v[1] = ie1;
      pt->v[2] = k; 
      pt->qual = MMG2_caltri(mesh,sol,pt);
      if(ddebug) printf("on cree %d : %d %d %d\n",mel,ie3,ie1,k);    
      /*maj des adj*/
      /*text*/
      iadr = 3*(text-1) + 1; 
      adja = &mesh->adja[iadr]; 
      atext1 = adja[MMG2_inxt[voy]];
      atext2 = adja[MMG2_inxt[voy+1]];   
      adja[0] = 3*lel + 1;
      adja[1] = 3*mel + 0;
      adja[2] = atext2;
      if(atext2)
	(&mesh->adja[3*(atext2/3-1) + 1])[atext2%3] = 3*text + 2;
      //if(ddebug) printf("adj of %d : %d %d %d \n",text,adja[0]/3,adja[1]/3,adja[2]/3);
      /*mel*/
      iadr = 3*(mel-1) + 1; 
      adja = &mesh->adja[iadr];
      adja[0] = 3*text + 1;
      adja[1] = 3*nel + 0;
      adja[2] = atext1;
      if(atext1)
	(&mesh->adja[3*(atext1/3-1) + 1])[atext1%3] = 3*mel + 2;
      if(nflat==6 /*|| (*adj)==7*/) {//CECILE 14/04/14:*adj ne veut rien dire!!!
	adja[1] = 3*(aext0/3);
	//pas de suite sinon on perd de l'info (&mesh->adja[3*(aext0/3-1) + 1])[0] = 3*mel + 1;
	if(ddebug) printf("adj of %d : %d %d %d \n",mel,adja[0]/3,adja[1]/3,adja[2]/3);
	
      } else {
	if(ddebug) printf("adj of %d : %d %d %d \n",mel,adja[0]/3,adja[1]/3,adja[2]/3);
	/*nel*/
	iadr = 3*(nel-1) + 1; 
	adja = &mesh->adja[iadr];
	adja[0] = 3*mel + 1;
      }
      
      /*lel*/
      iadr = 3*(lel-1) + 1; 
      adja = &mesh->adja[iadr];
      adja[1] = 3*text + 0;
      
    } else {  
      adja[0] = 3*lel + 1;
      adja[1] = 3*nel + 0;
      adja[2] = aext1;
      if(aext1)
	(&mesh->adja[3*(aext1/3-1) + 1])[aext1%3] = 3*mel + 2;   
      //printf("adj of %d : %d %d %d -- %d(%d) : %d\n",mel,lel,nel,aext1/3,aext1/3,aext1%3,mel); 
      
    } 
    if(nflat==1 || nflat==3 || nflat==7 || nflat==5){
      /*on coupe le tr aext2 en 2*/
      text = aext2/3; 
      voy  = aext2%3;
      pt1  = &mesh->tria[text];
      ie1  = pt1->v[voy];
      ie2  = pt1->v[MMG2_inxt[voy]];
      ie3  = pt1->v[MMG2_inxt[voy+1]];
      pt1->v[0] = ie1;
      pt1->v[1] = ie2;
      pt1->v[2] = k;
      pt1->qual = MMG2_caltri(mesh,sol,pt1); 
      if(ddebug) printf("on cree %d : %d %d %d\n",text,ie1,ie2,k);     
      pt = &mesh->tria[lel];
      pt->v[0] = ie3;
      pt->v[1] = ie1;
      pt->v[2] = k;
      pt->qual = MMG2_caltri(mesh,sol,pt); 
      if(ddebug) printf("on cree %d : %d %d %d\n",lel,ie3,ie1,k);
      /*maj des adj*/
      /*text*/
      iadr = 3*(text-1) + 1; 
      adja = &mesh->adja[iadr]; 
      atext1 = adja[MMG2_inxt[voy]];
      atext2 = adja[MMG2_inxt[voy+1]]; 
      if(ddebug) printf("ben quoi %d %d\n",atext1/3,atext2/3);
      adja[0] = 3*nel + 1;
      adja[1] = 3*lel + 0;
      adja[2] = atext2;
      if(atext2)
	(&mesh->adja[3*(atext2/3-1) + 1])[atext2%3] = 3*text + 2;
      if(ddebug) printf("adj of %d : %d %d %d \n",text,adja[0]/3,adja[1]/3,adja[2]/3);

      /*lel*/
      iadr = 3*(lel-1) + 1; 
      adja = &mesh->adja[iadr];
      adja[0] = 3*text + 1;
      adja[1] = 3*mel + 0;
      adja[2] = atext1;
      if(atext1)
	(&mesh->adja[3*(atext1/3-1) + 1])[atext1%3] = 3*lel + 2;
      if(nflat==5 || nflat==7) {
	adja[1] = 3*(aext1/3);
	if(aext1)
	  (&mesh->adja[3*(aext1/3-1) + 1])[0] = 3*lel + 1;	
      }
      if(ddebug) printf("adj of %d : %d %d %d \n",lel,adja[0]/3,adja[1]/3,adja[2]/3);

      /*nel*/
      iadr = 3*(nel-1) + 1; 
      adja = &mesh->adja[iadr];
      adja[1] = 3*text + 0;
      
      /*mel*/
      iadr = 3*(mel-1) + 1; 
      adja = &mesh->adja[iadr]; 
      if(ddebug) printf("la change-t-on adj(%d) ? %d : %d ou %d et %d %d\n",mel,nflat,adja[0]/3,lel,adja[1]/3,adja[2]/3);
      if(!(nflat==5 || nflat==7))
        adja[0] = 3*lel + 1;        
      
    } 
    if(nflat==2 || nflat==3 || nflat==7 || nflat == 6){  
      if(ddebug) printf("ben oui on passe la\n");
      /*on coupe le tr aext0 en 2*/
      text = aext0/3; 
      voy  = aext0%3;
      pt1  = &mesh->tria[text];
      ie1  = pt1->v[voy];
      ie2  = pt1->v[MMG2_inxt[voy]];
      ie3  = pt1->v[MMG2_inxt[voy+1]];
      pt1->v[0] = ie1;
      pt1->v[1] = ie2;
      pt1->v[2] = k;
      pt1->qual = MMG2_caltri(mesh,sol,pt1);
      if(ddebug) printf("on change %d : %d %d %d\n",text,ie1,ie2,k);      
      pt = &mesh->tria[nel];
      pt->v[0] = ie3;
      pt->v[1] = ie1;
      pt->v[2] = k;  
      pt->qual = MMG2_caltri(mesh,sol,pt);       
      if(ddebug) printf("on a      %d : %d %d %d\n",nel,ie3,ie1,k);      
      /*maj des adj*/
      /*text*/
      iadr = 3*(text-1) + 1; 
      adja = &mesh->adja[iadr]; 
      atext1 = adja[MMG2_inxt[voy]];
      atext2 = adja[MMG2_inxt[voy+1]];
      adja[0] = 3*mel + 1;
      adja[1] = 3*nel + 0;
      adja[2] = atext2;
      if(atext2)
	(&mesh->adja[3*(atext2/3-1) + 1])[atext2%3] = 3*text + 2;
      if(ddebug) printf("------- adj of %d : %d %d %d == %d\n",text,adja[0]/3,adja[1]/3,adja[2]/3,atext2/3);
      /*nel*/
      iadr = 3*(nel-1) + 1; 
      adja = &mesh->adja[iadr];
      adja[0] = 3*text + 1;
      adja[1] = 3*lel + 0;
      adja[2] = atext1;
      if(atext1)
	(&mesh->adja[3*(atext1/3-1) + 1])[atext1%3] = 3*nel + 2;
      if (nflat==3 || nflat==7) {
	adja[1] = 3*(aext2/3);
	//(&mesh->adja[3*(aext2/3-1) + 1])[aext2%3] = 3*nel + 1;
      }
      if(ddebug) printf("adj of %d : %d %d %d \n",nel,adja[0]/3,adja[1]/3,adja[2]/3);
			
      /*mel*/
      iadr = 3*(mel-1) + 1; 
      adja = &mesh->adja[iadr]; 
      adja[1] = 3*text + 0;        
    }  
    if(ddebug) {
      printf("on rechk\n");
      MMG2_saveMesh(mesh,"titi.mesh");
      MMG2_chkmsh(mesh,0);
    }

  }
return(1);
}
/*put different ref on different SD*/
int MMG2_baseSD(MMG5_pMesh mesh) {
  MMG5_pTria   pt,pt1;                   
  MMG5_pEdge   ped;
  int     k,i,j,iadr,*adja,ped0,ped1,kcor,*list,ipil,ncurc,nref;
  int     kinit,nt,nsd;
  
  if ( !MMG2_hashel(mesh) )  return(0);
  
  for(k=1 ; k<=mesh->nt ; k++) mesh->tria[k].flag = mesh->base;
  list = (int*)M_calloc(mesh->nt,sizeof(int),"baseSD");
  assert(list);
  kinit = 0;
  nref  = 0;
  for(k=1 ; k<=mesh->nt ; k++) {
    if ( !mesh->tria[k].v[0] ) continue;
    mesh->tria[k].flag = mesh->base;
    mesh->tria[k].ref  = 0;
    list[k-1] = 0;    
    if((!kinit) && !(mesh->tria[k].ref)) kinit = k;  
    if(ddebug) printf("tr %d : %d %d %d\n",k,mesh->tria[k].v[0],mesh->tria[k].v[1],mesh->tria[k].v[2]);
  }  
  do {
    nref++; 
    list[0] = kinit;
    ipil = 0;
    ncurc = 0;   
    if(ddebug) printf("la %d ref %d\n",532, mesh->tria[532].ref );
    do {
      k = list[ipil]; 
      pt = &mesh->tria[k]; 
      pt->ref = nref;
      iadr = 3*(k-1) + 1;
      adja = &mesh->adja[iadr];
      for(i=0 ; i<3 ; i++)  {
        pt1 = &mesh->tria[adja[i]/3];
	if(ddebug) printf("adj of %d ref %d\n",k, mesh->tria[135].ref );

        if(pt1->ref==nref) continue;
        ped0 = pt->v[MMG2_iare[i][0]];
        ped1 = pt->v[MMG2_iare[i][1]];
        for(j=1 ; j<=mesh->na ; j++) {
          ped = &mesh->edge[j];
          if((ped->a == ped0 && ped->b==ped1) || (ped->b == ped0 && ped->a==ped1)) break;  
        }
        if(j<=mesh->na) continue;  
        if(ddebug) printf("on a cherche %d %d ....\n",ped0,ped1);

        pt1->ref = nref;  
        if(adja[i])
          list[++ncurc] = adja[i]/3; 
      }
      ++ipil ;
    } while (ipil <= ncurc) ;
    kinit = 0;  
    for(k=1 ; k<=mesh->nt ; k++) { 
      if ( !mesh->tria[k].v[0] ) continue;
      mesh->tria[k].flag = mesh->base;
      list[k-1] = 0;
      if(!kinit && !(mesh->tria[k].ref)) kinit = k;
    }
    if(ddebug) printf("kinit ? %d\n",kinit);
  } while (kinit);
  fprintf(stdout," %8d SUB-DOMAINS\n",nref);   
  
  if(mesh->info.renum) {  
    nsd = mesh->info.renum;
    nt = mesh->nt;
    for(k=1 ; k<=nt ; k++) {
      if ( !mesh->tria[k].v[0] ) continue;
      if(mesh->tria[k].ref == nsd) continue;
      MMG2_delElt(mesh,k); 
    } 
  }
  return(1);
} 

/*cherche si le  tr est dedans ou dehors ou indetermine (donc edge a forcer ou cas non convexe)*/
/*return <0 value if triangle outside ; > 0 if triangle inside*/
int MMG2_findtrianglestate(MMG5_pMesh mesh,int k,int ip1,int ip2,int ip3,int ip4,int base) {
  MMG5_pTria     pt,pt1;
  MMG5_pEdge     ped;  
  int       i,j,nb,ped0,ped1;
  int       numed[3],nbed,iadr,*adja,adj;
  
  pt = &mesh->tria[k];
  /*Count number of BB points*/
  nb = 0;
  for(i=0 ; i<3 ; i++) {
    if(pt->v[i]==ip1 || pt->v[i]==ip2 || pt->v[i]==ip3 || pt->v[i]==ip4) {
      nb++;    
    }  
  }
  /*triangle to be deleted*/
  if(nb==3 || nb==2) {
    pt->base = -base;
    pt->ref  = 3; 
    return(-base);
  } 

  /*we have to check the status of neighbour*/
  /*number of bdry edges*/
#warning to optimize
  nbed = 0;
  for(i=0 ; i<3 ; i++) {
    ped0 = pt->v[MMG2_iare[i][0]];
    ped1 = pt->v[MMG2_iare[i][1]];
    numed[i] = 0;
    for(j=1 ; j<=mesh->na ; j++) {
        ped = &mesh->edge[j];
        if((ped->a == ped0 && ped->b==ped1) || (ped->b == ped0 && ped->a==ped1)){ 
	  ped->base = -1;
	  nbed++;
	  numed[i] = j;
	  break;
	}  
    }
  }
  if(nbed<2) {
    if(nb) {
      pt->base = -base;
      pt->ref = 3;
      return(-base);
    } else {
#warning check if it is true with 1 bdry edge
      pt->base = base;
      return(base);
    }
  } else if(nbed==2) {
    if(nb) {
      pt->base = -base;
      pt->ref = 3;
      return(-base);
    } else {
      iadr = 3*(k-1) + 1;
      adja = &mesh->adja[iadr];
      for(i=0 ; i<3 ; i++) {
	adj = adja[i]/3;
	pt1 = &mesh->tria[adj];
	if(!pt1->base) continue;
	if(abs(pt1->base)<mesh->base) continue;
	if(numed[i]) {
	  pt->base = -pt1->base;
	  return(pt->base);
	} else {
	  pt->base = pt1->base;
	  return(pt->base);
	}
      }
      if(i==3) {
	pt->base = 0;
	return(0);
      }
    }
  } else {
    assert(!nb);
    fprintf(stdout,"CONNEX COMPONENT WITH ONLY ONE TRIANGLE\n");
    pt->base = base;
    return(base);
  }
 

  /*le tr est indetermine*/
  pt->base = 0;
    
  return(0);
}

/*cherche si le  tr est dedans ou dehors ou indetermine (donc edge a forcer ou cas non convexe)*/
/*return <0 value if triangle outside ; > 0 if triangle inside*/
int MMG2_findpos(MMG5_pMesh mesh,MMG5_pTria pt,int ip1,int ip2,int ip3,int ip4,int base) {
  MMG5_pEdge     ped;  
  int       i,j,nb,ped0,ped1;
  int       numed[3],nbed;
  
  /*au moins 1 point de la BB ??*/
  nb = 0;
  for(i=0 ; i<3 ; i++) {
    if(pt->v[i]==ip1 || pt->v[i]==ip2 || pt->v[i]==ip3 || pt->v[i]==ip4) {
      nb++;    
    }  
  }
  if(nb==3 || nb==2) {
    pt->base = -base;
    pt->ref  = 3; 
    return(-base);
  } else if(!nb){
    pt->base = base; 
    //pt->ref  = base; 
    return(base);
  }
  /*1 pt de la BB => on ne sait pas si le tr est dedans ou dehors*/
  /*contient un edge ?*/
  for(i=0 ; i<3 ; i++) {
    ped0 = pt->v[MMG2_iare2[i][0]];
    ped1 = pt->v[MMG2_iare2[i][1]];
    for(j=1 ; j<=mesh->na ; j++) {
        ped = &mesh->edge[j];
        if((ped->a == ped0 && ped->b==ped1) || (ped->b == ped0 && ped->a==ped1)){ 
					ped->base = -1;
	        break;
				}  
    }
    if(j<=mesh->na) break;   
  }                        
  if(i<3) { /*le tr est dehors*/ 
    //printf("on a trouve edge : %d %d\n",ped0,ped1);
    pt->base = -base; 
    pt->ref  = 3; 
    return(base);
  } 

  /*le tr est indetermine*/
  pt->base = 0;
    
  return(0);
}

/*create triangulation*/
int MMG2_mmg2d2(MMG5_pMesh mesh,MMG5_pSol sol) {  
  MMG5_pTria     pt,pt1; 
  MMG5_pTria      tmp;
  //MMG5_pEdge     ped;
  MMG5_pPoint    ppt,ppt2;  
  double    c[2],crit,cal1,cal2,dd,declic;  
  int       j,i,k,kk,ip1,ip2,ip3,ip4,jel,kel,nt,iadr,*adja,madj;  
  int       aext0,aext1,aext2,lel,nel,mel,ia,ib,ic,base,nd,iter,maxiter; 
  int       *adj,adjj,ns,i1,i2,*list,nflat,ie1,ie2,ie3,voy,text,atext1,atext2,nc;
  int       *numper,nsiter,iadr2,*adja2;
	
  mesh->base = 0;
  ddebug = 0;
  /*if existed triangle : delete them*/
  if(mesh->nt) {
    nt = mesh->nt;
    for(k=1 ; k<=nt ; k++) {
      MMG2_delElt(mesh,k);
      iadr = 3*(k-1) + 1;
      (&mesh->adja[iadr])[0] = 0; 
      (&mesh->adja[iadr])[1] = 0; 
      (&mesh->adja[iadr])[2] = 0; 
    }
  }  
  if(mesh->info.renum) { 
    /*traitement des points periodiques*/
    numper = (int*)M_calloc((mesh->np+1),sizeof(int),"mmg2");
    assert(numper);
    for(k=1 ; k<=mesh->np ; k++) {
      ppt = &mesh->point[k];
      for(kk=k ; kk<=mesh->np ; kk++) {
        if(k==kk) continue;  
        ppt2 = &mesh->point[kk];
        dd = (ppt->c[0]-ppt2->c[0])*(ppt->c[0]-ppt2->c[0])+(ppt->c[1]-ppt2->c[1])*(ppt->c[1]-ppt2->c[1]);
        if(dd<1e-6) {
          //printf("point img %d %d\n",k,kk);   
          ppt2->tmp = 1;
          if(!numper[k]) {
            numper[k] = kk;
          } else if(numper[k]!=kk){
            j = numper[k]; 
            printf("j = %d %d\n",j,numper[j]) ;
            while(numper[j] && numper[j]!=kk) {
              j = numper[j];  
              //printf("j = %d %d\n",j,numper[j]) ;
            }
            if(numper[j]!=kk) numper[j] = kk; 
          }
        }
      }       
    }
  }
  /*add bounding box vertex*/
  c[0] = -1;//mesh->info.min[0] - 1.;
  c[1] = -1.;// mesh->info.min[1] - 1.;  
  ip1 = MMG2_newPt(mesh,c);
  
  c[0] = -1.;//mesh->info.min[0] - 1.;
  c[1] = 2.;//mesh->info.max[1] + 1.;  
  ip2 = MMG2_newPt(mesh,c);

  c[0] = 2.;//mesh->info.max[0] + 1.;
  c[1] = -1.;//mesh->info.min[1] - 1.;  
  ip3 = MMG2_newPt(mesh,c);

  c[0] = 2.;//mesh->info.max[0] + 1.;
  c[1] = 2.;//mesh->info.max[1] + 1.;  
  ip4 = MMG2_newPt(mesh,c);
  
  assert(ip1==(mesh->np-3));
  assert(ip2==(mesh->np-2));
  assert(ip3==(mesh->np-1));
  assert(ip4==(mesh->np));
  /*creation des deux premiers triangles + adjacence*/
  jel  = MMG2_newElt(mesh);
  pt   = &mesh->tria[jel];
  pt->v[0] = ip1;
  pt->v[1] = ip4;
  pt->v[2] = ip2; 
  pt->base = mesh->base;

  kel  = MMG2_newElt(mesh);
  pt   = &mesh->tria[kel];
  pt->v[0] = ip1;
  pt->v[1] = ip3;
  pt->v[2] = ip4; 
  pt->base = mesh->base;
  
  iadr = 3*(jel-1) + 1;
  adja = &mesh->adja[iadr]; 
  adja[2] = 3*kel + 1; 
  
  iadr = 3*(kel-1) + 1;
  adja = &mesh->adja[iadr]; 
  adja[1] = 3*jel + 2;
  
  /*vertex insertion*/
  if(!MMG2_insertpointdelone(mesh,sol)) return(0);
  fprintf(stdout,"END OF INSERTION PHASE\n");
  
  /*bdry enforcement*/
  if(!MMG2_bdryenforcement(mesh,sol)) {
    printf("bdry enforcement failed\n");return(0);
  } 
  if(mesh->info.ddebug) MMG2_saveMesh(mesh,"bdyenforcement.mesh");	

  /*tag des triangles : in = base ; out = -base ; indetermine = 0*/
  if(!MMG2_settagtriangles(mesh,sol)) return(0);

  if(!MMG2_removeBBtriangles(mesh)) return(0);
	
  /*base SD*/
  MMG2_baseSD(mesh);
  
  return(1);
}

