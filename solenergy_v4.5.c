/*****Calculates Intermolecular Solvent Interaction Energies,for gromacs versions < 5.0*****/

#include	<stdio.h> /*an input-output library*/
#include	<stdlib.h> /* other functions*/
#include	<math.h> /*math library*/
#include	<string.h> /*string library*/
#define MAXSOL 100  /*Max number of Solvent Molecules*/

int main (int argc,char *argv[])
{
    // This little piece will stop execution if wrong input//
    if (argc != 10){
        printf("%s","Usage: %s (1)SimTraj(.xtc) (2)SimTop(.tpr) (3)Distance_File(.txt) (4)Nsol (5)NatomsperMol (6)SimLength(ps) (7)TimeStep(ps) (8)poreradius(nm) (9)OutFile\n");
        exit(1);
    }
	else{
        
    int i,j,k,nT,porenum,Nsol,solid,Natoms,atomid;
    float poreradius,timestep,simlength,dist;
    double LJ,Cou,etime,energy;
    Nsol = atoi(argv[4]);
    Natoms = atoi(argv[5]);
    simlength=atof(argv[6]);
    timestep=atof(argv[7]);
    poreradius=atof(argv[8]);
    nT=simlength/timestep;
    char infile[40],outfile[40],buf[80],intermol_energies[80],xtc[80],tpr[80];
    sprintf(xtc,"%s",argv[1]);
    sprintf(tpr,"%s",argv[2]);
    FILE *OUT;
    sprintf(outfile,"%s",argv[9]);
    OUT = fopen(outfile,"w");
        /*Distance Index*/
        FILE *IN;
        sprintf(infile,"%s",argv[3]);
        IN = fopen(infile,"r");
        //Distance File Headers//
        fgets(buf,180,IN);
        fgets(buf,180,IN);
        for(i=0;i<nT;i++){
        FILE *ndx;
        ndx = fopen("energy.ndx","w");
        float time = i*timestep;
        char c1[100],c2[100],c3[100];
          porenum=0;
          fprintf(ndx,"[ PORESOL ]\n");
        for(j=0;j<Nsol;j++){
            fgets(buf,180,IN);
            sscanf(buf,"%i %f",&solid,&dist);
            if (dist < poreradius){
            porenum+=1;
            sprintf(c1,"g_select -f %s -s %s -on id.ndx -select 'resnr %i' -b %f -e %f ",xtc,tpr,solid,time,time);
            system(c1);
            system("sed '1,1d' id.ndx > id.txt");
            char temp[80];
            FILE *id;
            id = fopen("id.txt","r");
                for(k=0;k<Natoms;k++){
                    fscanf(id,"%i\t",&atomid);
                    fprintf(ndx,"%i\t",atomid);
                }
            fclose(id);
            system("rm id.ndx");
            system("rm id.txt");
            }
        }
     fclose(ndx);
       
    //Get Structure and Topology//
    sprintf(c1,"sh topology.sh %s",tpr);
    system(c1);
    sprintf(c2,"sh structure.sh %s %s %f %f ",xtc,tpr,time,time);
    system(c2);
            
    system("mdrun -rerun energy.xtc -s energy.tpr -e energy.edr");
    //Use Shell Script to Noninteractively use g_energy//
    char sh1[50];
    sprintf(sh1,"sh energy.sh");
    system(sh1);
        
            //Extract Energies from .xvg file and Print to File//
        FILE *xvg;
        sprintf(intermol_energies,"%s","energy.xvg");
        xvg = fopen(intermol_energies,"r");
			for(j=0;j<21;j++){
				fgets(buf,180,xvg);
				if(j==20){
                    sscanf(buf,"%lf %lf %lf",&etime,&LJ,&Cou);
                    fprintf(OUT,"%lf %lf %lf %lf\n",etime,LJ,Cou,(LJ+Cou)/Natoms);
                    energy += (LJ+Cou)/Natoms;
                }
			}
        fclose(xvg);
        //Calculate Total Energy//
            //Delete Unwanted Files//
            system("rm -f energy.edr");
            system("rm -f energy.ndx");
            system("rm -f energy.tpr");
            system("rm -f energy.xvg");
            system("rm -f energy.xtc");
            system("rm -f md.log");
            system("rm -f traj.trr");
            system("sh clean.sh");
            
    }
        fclose(IN);
        /******End of Energy Calcularion*****/
        energy/=nT;
        //Print Total Average Intermolecular Energy at End of File//
        fprintf(OUT,"Average Interaction Energy(kJ/mol) = %f\n",energy);
        fprintf(OUT,"Average Interaction Energy(kcal/mol) = %f\n",energy*0.239);
        
        fclose(OUT);
        
    }


}


