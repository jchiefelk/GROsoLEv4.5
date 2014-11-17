/*****Calculates Intermolecular Solvent Interaction Energies,for gromacs versions < 5.0*****/

#include	<stdio.h> /*an input-output library*/
#include	<stdlib.h> /* other functions*/
#include	<math.h> /*math library*/
#include	<string.h> /*string library*/
#define MAXSOL 100  /*Max number of Solvent Molecules*/

int main (int argc,char *argv[])
{
    // This little piece will stop execution if wrong input//
    if (argc != 8){
        printf("%s","Usage: %s Distance_File(.txt) Nsol NatomsperMol SimLength(ps) TimeStep(ps) poreradius(nm) OutFile\n");
        exit(1);
    }
	else{
        
    int i,j,k,nT,porenum,Nsol,solid,Natoms,atomid;
    float poreradius,timestep,simlength,dist;
    double LJ,Cou,etime,energy;
    Nsol = atoi(argv[2]);
    Natoms = atoi(argv[3]);
    simlength=atof(argv[4]);
    timestep=atof(argv[5]);
    poreradius=atof(argv[6]);
    nT=simlength/timestep;
    char infile[40],outfile[40],buf[80],intermol_energies[80];
    FILE *OUT;
    sprintf(outfile,"%s",argv[7]);
    OUT = fopen(outfile,"w");
        /*Distance Index*/
        FILE *IN;
        sprintf(infile,"%s",argv[1]);
        IN = fopen(infile,"r");
        for(i=0;i<nT;i++){
        FILE *ndx;
        ndx = fopen("energy.ndx","w");
        float time = i*timestep;
        char c1[100],c2[100],c3[100];
        //Distance File Headers//
          fgets(buf,180,IN);
          fgets(buf,180,IN);
          porenum=0;
          fprintf(ndx,"[ PORESOL ]\n");
        for(j=0;j<Nsol;j++){
            fgets(buf,180,IN);
            sscanf(buf,"%i %f",&solid,&dist);
            if (dist < poreradius){
            porenum+=1;
            sprintf(c1,"g_select -f full.xtc -s full.tpr -on id.ndx -select 'resnr %i' -b %f -e %f ",solid,time,time);
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
            
    sprintf(c2,"trjconv -f full.xtc -s full.tpr -n energy.ndx -o energy.xtc -b %f -e %f ", time,time);
    system(c2);
    sprintf(c3,"sh topology.sh");
    system(c3);
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
                    fprintf(OUT,"%lf %lf %lf\n",etime,LJ,Cou);
                    energy += (LJ+Cou);
                }
			}
        fclose(xvg);
        //Calculate Total Energy//
        energy/=nT;
            
    }
        fclose(IN);
        /******End of Energy Calcularion*****/
        //Delete Unwanted Files//
        system("rm -f energy.edr");
        system("rm -f energy.ndx");
        system("rm -f energy.tpr");
        system("rm -f energy.xvg");
        system("rm -f energy.xtc");
        system("rm -f md.log");
        system("rm -f traj.trr");
        system("sh clean.sh");
        
        //Print Total Average Intermolecular Energy at End of File//
        fprintf(OUT,"Average Interaction Energy = %f",energy);
        fclose(OUT);
        
    }


}


