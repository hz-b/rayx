#include <locale.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#if ((defined(_MSC_VER) || defined(__BORLANDC__)) && !defined(M_PI))	//_MSC_VER needs this before math.h
#define	_USE_MATH_DEFINES
#endif
#include <math.h>
#include <time.h>

#include "mgl2/addon.h"
#include "mgl2/data.h"
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	const char *args_opt="a:b:c:d:e:f:g:h:i:j:k:l:m:n:o:p:q:r:s:t:u:v:w:x:y:z:";
	std::string eqs[26];
	while(1)
	{
		int ch = getopt(argc, argv, args_opt);
		if(ch>='a' && ch<='z')	eqs[ch-'a'] = optarg;
		if(ch<0)	break;
	}
	
	double x1[3]={0,0,0},x2[3]={0,0,0},dx[3]={1,1,1};
	bool e[3]={false,false,false};
	int n=argc-optind-2;	// number of counters

	if(n<1)    // if incorrect number of arguments then print the help
	{
		printf("mgltask make output file with a set of copies of mask-file with repeatedly replaced $# by loop values. It useful for making set of initial conditions with a few parameters varied in specified range.\n");
		printf("Usage:\tmgltask [options] maskfile outputfile [min1:step1:max1 [min2:step2:max2 [min3:step3:max3]]]\n\n");
		printf("\tmask file  -- mask file in which all '$#' will be replaced by counter # value.\n");
		printf("\t\tHere # = 0 is random number in [0,1]; # = 1,2,3 are counters;\n");
		printf("\t\t# = a,b,...,z are formulas defined by options.\n");
		printf("\toutputfile -- file where result will be saved, the '-' will print in stdout;\n");
		printf("\tmin#:step#:max# -- is minimum, step increment and maximum values of counter #;\n");
		printf("\t'e'min#:step#:max# -- the same but in exponential form 10^#.\n");
		printf("\tOptions -a, -b, ..., -z define formulas for arguments $a,$b,...,$z,\n");
		printf("\t\twhich can depended on counters v0=$0,v1=$1,v2=$2,v3=$3.\n");
//		system("PAUSE");
		return 0;
	}

	FILE *fm = fopen(argv[optind],"r");
	FILE *fo = strcmp(argv[optind+1],"-") ? fopen(argv[optind+1],"w"):stdout;
	printf("mask = %s, out = %s\n",argv[optind],argv[optind+1]);
	// read parameters of loops
	for(int i=0;i<n;i++)
	{
		const char *par = argv[optind+i+2];
		if(par[0]=='e')	{	e[i] = true;	par++;	}
		int r=sscanf(par,"%lg:%lg:%lg",x1+i,dx+i,x2+i);
		if(r==2)	{	x2[i]=dx[i];	dx[i]=1;	}
		else if(r!=3)	break;	// something wrong in arguments
		printf("$%d in %g:%g:%g\n",i+1,x1[i],dx[i],x2[i]);
	}
	// for each variable
	double v1,v2,v3;
	mglData d0,d1,d2,d3;
	d0.Name("v0");	d1.Name("v1");	d2.Name("v2");	d3.Name("v3");
	double vals[26]={0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0};
	for(v3=x1[2];v3<=x2[2];v3+=dx[2])	for(v2=x1[1];v2<=x2[1];v2+=dx[1])	for(v1=x1[0];v1<=x2[0];v1+=dx[0])
	{
		d0.a[0]=mgl_rnd();	d1.a[0]=v1;	d2.a[0]=v2;	d3.a[0]=v3;
		for(int i=0;i<26;i++)	if(!eqs[i].empty())
		{
			HMDT d = mgl_formula_calc(eqs[i].c_str(), 4, &d0,&d1,&d2,&d3);
			vals[i] = d->a[0];	mgl_delete_data(d);
		}
		
		fseek(fm,0,0);
		while(!feof(fm))
		{
			int i;
			char str[1024],*buf=str;
			char *r=fgets(str,1024,fm);		// for each string
			if(!r)	break;
			while((i=mgl_chrpos(buf,'$'))!=-1)    // find '$'
			{
				char k=buf[i+1];
				buf[i]=0;
				if(k=='0')	fprintf(fo,"%s%g",buf,d0.a[0]);
				else if(k=='1')
					fprintf(fo,"%s%g",buf,e[0] ? exp(M_LN10*v1):(fabs(v1)<1e-10?0:v1));
				else if(k=='2')
					fprintf(fo,"%s%g",buf,e[1] ? exp(M_LN10*v2):(fabs(v2)<1e-10?0:v2));
				else if(k=='3')
					fprintf(fo,"%s%g",buf,e[2] ? exp(M_LN10*v3):(fabs(v3)<1e-10?0:v3));
				else if(k>='a' && k<='z')	fprintf(fo,"%s%g",buf,vals[k-'a']);
				buf = &(buf[i+2]);	// handle the last part
			}
			fprintf(fo,"%s",buf);	// write it
		}
		fprintf(fo,"\n");
	}
	fclose(fm);	fclose(fo);
	return 0;
}
//-----------------------------------------------------------------------------
