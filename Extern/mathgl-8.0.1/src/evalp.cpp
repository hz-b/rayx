/***************************************************************************
 * evalp.cpp is part of Math Graphic Library
 * Copyright (C) 2007-2016 Alexey Balakin <mathgl.abalakin@gmail.ru>       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License  as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <time.h>
#include <ctype.h>
#include <wchar.h>
#include "mgl2/base.h"
#include "mgl2/parser.h"
#if MGL_HAVE_GSL
#include <gsl/gsl_sf.h>
#include <gsl/gsl_errno.h>
#endif
//-----------------------------------------------------------------------------
std::wstring mgl_trim_ws(const std::wstring &str);
HMDT MGL_NO_EXPORT mglFormulaCalc(const std::wstring &string, mglParser *arg, const std::vector<mglDataA*> &head);
HADT MGL_NO_EXPORT mglFormulaCalcC(const std::wstring &string, mglParser *arg, const std::vector<mglDataA*> &head);
HMDT MGL_NO_EXPORT mglFormulaCalcA(std::wstring string, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns);
HADT MGL_NO_EXPORT mglFormulaCalcAC(std::wstring string, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns);
//-----------------------------------------------------------------------------
HMDT MGL_EXPORT mgl_formula_calc(const char *str, long n, ...)
{
	if(n<1)	return NULL;
	std::vector<mglDataA*> head;
	va_list vl;	va_start(vl,n);
	for(long i=0;i<n;i++)	head.push_back(va_arg(vl,mglDataA*));
	va_end(vl);
	return mglFormulaCalc(str, head);
}
//-----------------------------------------------------------------------------
HADT MGL_EXPORT mgl_formula_calc_c(const char *str, long n, ...)
{
	if(n<1)	return NULL;
	std::vector<mglDataA*> head;
	va_list vl;	va_start(vl,n);
	for(long i=0;i<n;i++)	head.push_back(va_arg(vl,mglDataA*));
	va_end(vl);
	return mglFormulaCalcC(str, head);
}
//-----------------------------------------------------------------------------
HMDT MGL_EXPORT mglFormulaCalc(const char *str, const std::vector<mglDataA*> &head)
{
	if(!str || *str==0)	return NULL;
	std::wstring s;
	for(long i=0;str[i];i++)	s.push_back(str[i]);
	return mglFormulaCalc(s,0,head);
}
//-----------------------------------------------------------------------------
HADT MGL_EXPORT mglFormulaCalcC(const char *str, const std::vector<mglDataA*> &head)
{
	if(!str || *str==0)	return NULL;
	std::wstring s;
	for(long i=0;str[i];i++)	s.push_back(str[i]);
	return mglFormulaCalcC(s,0,head);
}
//-----------------------------------------------------------------------------
HMDT MGL_NO_EXPORT mglApplyFunc(const std::wstring &str, mglParser *arg, const std::vector<mglDataA*> &head, double (*func)(double), const std::vector<std::wstring> &fns)
{
	HMDT d = mglFormulaCalcA(str, arg, head, fns);
	long n = d->GetNN();	mreal *dd=d->a;
#pragma omp parallel for
	for(long i=0;i<n;i++)	dd[i] = func(dd[i]);
	return d;
}
//-----------------------------------------------------------------------------
#if MGL_HAVE_GSL
HMDT MGL_NO_EXPORT mglApplyFuncGSL(const std::wstring &str, mglParser *arg, const std::vector<mglDataA*> &head, double (*func)(double, gsl_mode_t), const std::vector<std::wstring> &fns)
{
	HMDT d = mglFormulaCalcA(str, arg, head, fns);
	long n = d->GetNN();	mreal *dd=d->a;
#pragma omp parallel for
	for(long i=0;i<n;i++)	dd[i] = func(dd[i],GSL_PREC_SINGLE);
	return d;
}
#endif
//-----------------------------------------------------------------------------
HMDT MGL_NO_EXPORT mglApplyOper(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, double (*func)(double,double), const std::vector<std::wstring> &fns)
{
	HMDT a = mglFormulaCalcA(a1,arg,head, fns), b = mglFormulaCalcA(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	mreal va=a->a[0], vb=b->a[0], *aa=a->a, *bb=b->a, *cc=r->a;
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = func(aa[i], bb[i]);
	else if(na==1)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = func(va, bb[i]);
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = func(aa[i], vb);
	mgl_delete_data(d);	return r;
}
//-----------------------------------------------------------------------------
HMDT MGL_NO_EXPORT mglApplyOperAdd(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
	HMDT a = mglFormulaCalcA(a1,arg,head,fns), b = mglFormulaCalcA(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	mreal *aa=r->a, *bb=d->a, v=bb[0];
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	aa[i] += bb[i];
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	aa[i] += v;
	mgl_delete_data(d);	return r;
}
//-----------------------------------------------------------------------------
HMDT MGL_NO_EXPORT mglApplyOperSub(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
	HMDT a = mglFormulaCalcA(a1,arg,head,fns), b = mglFormulaCalcA(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	mreal va=a->a[0], vb=b->a[0], *aa=a->a, *bb=b->a, *cc=r->a;
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = aa[i]-bb[i];
	else if(na==1)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = va-bb[i];
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = aa[i]-vb;
	mgl_delete_data(d);	return r;
}
//-----------------------------------------------------------------------------
HMDT MGL_NO_EXPORT mglApplyOperMul(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
	HMDT a = mglFormulaCalcA(a1,arg,head,fns), b = mglFormulaCalcA(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	mreal *aa=r->a, *bb=d->a, v=bb[0];
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	aa[i] *= bb[i];
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	aa[i] *= v;
	mgl_delete_data(d);	return r;
}
//-----------------------------------------------------------------------------
HMDT MGL_NO_EXPORT mglApplyOperDiv(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
	HMDT a = mglFormulaCalcA(a1,arg,head,fns), b = mglFormulaCalcA(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	mreal va=a->a[0], vb=b->a[0], *aa=a->a, *bb=b->a, *cc=r->a;
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = bb[i]!=0?aa[i]/bb[i]:NAN;
	else if(na==1)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = bb[i]!=0?va/bb[i]:NAN;
	else if(vb!=0)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = aa[i]/vb;
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = NAN;
	mgl_delete_data(d);	return r;
}
//-----------------------------------------------------------------------------
HADT MGL_NO_EXPORT mglApplyFuncC(const std::wstring &str, mglParser *arg, const std::vector<mglDataA*> &head, dual (*func)(dual), const std::vector<std::wstring> &fns)
{
	HADT d = mglFormulaCalcAC(str, arg, head,fns);
	long n = d->GetNN();	dual *dd=d->a;
#pragma omp parallel for
	for(long i=0;i<n;i++)	dd[i] = func(dd[i]);
	return d;
}
//-----------------------------------------------------------------------------
HADT MGL_NO_EXPORT mglApplyOperC(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, dual (*func)(dual,dual), const std::vector<std::wstring> &fns)
{
	HADT a = mglFormulaCalcAC(a1,arg,head,fns), b = mglFormulaCalcAC(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	dual va=a->a[0], vb=b->a[0], *aa=a->a, *bb=b->a, *cc=r->a;
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = func(aa[i], bb[i]);
	else if(na==1)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = func(va, bb[i]);
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = func(aa[i], vb);
	mgl_delete_datac(d);	return r;
}
//-----------------------------------------------------------------------------
HADT MGL_NO_EXPORT mglApplyOperAddC(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
	HADT a = mglFormulaCalcAC(a1,arg,head,fns), b = mglFormulaCalcAC(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	dual *aa=r->a, *bb=d->a, v=bb[0];
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	aa[i] += bb[i];
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	aa[i] += v;
	mgl_delete_datac(d);	return r;
}
//-----------------------------------------------------------------------------
HADT MGL_NO_EXPORT mglApplyOperSubC(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
	HADT a = mglFormulaCalcAC(a1,arg,head,fns), b = mglFormulaCalcAC(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	dual va=a->a[0], vb=b->a[0], *aa=a->a, *bb=b->a, *cc=r->a;
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = aa[i]-bb[i];
	else if(na==1)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = va-bb[i];
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = aa[i]-vb;
	mgl_delete_datac(d);	return r;
}
//-----------------------------------------------------------------------------
HADT MGL_NO_EXPORT mglApplyOperMulC(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
	HADT a = mglFormulaCalcAC(a1,arg,head,fns), b = mglFormulaCalcAC(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	dual *aa=r->a, *bb=d->a, v=bb[0];
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	aa[i] *= bb[i];
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	aa[i] *= v;
	mgl_delete_datac(d);	return r;
}
//-----------------------------------------------------------------------------
HADT MGL_NO_EXPORT mglApplyOperDivC(const std::wstring &a1, const std::wstring &a2, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
	HADT a = mglFormulaCalcAC(a1,arg,head,fns), b = mglFormulaCalcAC(a2,arg,head,fns), r,d;
	long na = a->GetNN(), nb = b->GetNN(), nn;
	if(na!=1)	{	r=a;	d=b;	nn=na;	}
	else		{	r=b;	d=a;	nn=nb;	}
	dual va=a->a[0], vb=b->a[0], *aa=a->a, *bb=b->a, *cc=r->a;
	if(na==nb)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = bb[i]!=mreal(0)?aa[i]/bb[i]:NAN;
	else if(na==1)
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = bb[i]!=mreal(0)?va/bb[i]:NAN;
	else if(vb!=mreal(0))
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = aa[i]/vb;
	else
#pragma omp parallel for
		for(long i=0;i<nn;i++)	cc[i] = NAN;
	mgl_delete_datac(d);	return r;
}
//-----------------------------------------------------------------------------
bool MGL_LOCAL_PURE mglCheck(std::wstring str)
{
	long s = 0,i,n=str.length();
	for(i=0;i<n;i++)
	{
		if(str[i]=='(')	s++;
		if(str[i]==')') s--;
		if(s<0)	return false;
	}
	return (s==0) ? true : false;
}
//-----------------------------------------------------------------------------
long MGL_LOCAL_PURE mglFindInText(const std::wstring &str,const char *lst, int num=0)
{
	long l=0,r=0,ls=0,rs=0;
	for(long i=str.length()-1;i>=0;i--)
	{
		if(str[i]=='(') l++;
		if(str[i]==')') r++;
		if(str[i]=='[') ls++;
		if(str[i]==']') rs++;
		if(l==r && ls==rs && strchr(lst,str[i]))
		{	num--;	if(num<0)	return i;	}
	}
	return -1;
}
//-----------------------------------------------------------------------------
double MGL_LOCAL_CONST cand(double a,double b);//	{return a&&b?1:0;}
double MGL_LOCAL_CONST cor(double a,double b);//	{return a||b?1:0;}
double MGL_LOCAL_CONST ceq(double a,double b);//	{return a==b?1:0;}
double MGL_LOCAL_CONST clt(double a,double b);//	{return a<b?1:0;}
double MGL_LOCAL_CONST cgt(double a,double b);//	{return a>b?1:0;}
double MGL_LOCAL_CONST stp(double a);//	{return a>0?1:0;}
double MGL_LOCAL_CONST sgn(double a);//	{return a>0?1:(a<0?-1:0);}
double MGL_LOCAL_CONST ipw(double a,double b);//	{return mgl_ipow(a,int(b));}
double MGL_LOCAL_CONST llg(double a,double b);//	{return log(a)/log(b);}
//double MGL_LOCAL_CONST asinh(double x);//	{	return log(x+sqrt(x*x+1));	}
//double MGL_LOCAL_CONST acosh(double x);//	{	return x>1 ? log(x+sqrt(x*x-1)) : NAN;	}
//double MGL_LOCAL_CONST atanh(double x);//	{	return fabs(x)<1 ? log((1+x)/(1-x))/2 : NAN;	}
double MGL_LOCAL_CONST gslEllE(double a,double b);//	{return gsl_sf_ellint_E(a,b,GSL_PREC_SINGLE);}
double MGL_LOCAL_CONST gslEllF(double a,double b);//	{return gsl_sf_ellint_F(a,b,GSL_PREC_SINGLE);}
double MGL_LOCAL_CONST gslLegP(double a,double b);//	{return gsl_sf_legendre_Pl(int(a),b);}
double MGL_LOCAL_CONST mgl_asinh(double x);
double MGL_LOCAL_CONST mgl_acosh(double x);
double MGL_LOCAL_CONST mgl_atanh(double x);
double MGL_LOCAL_CONST mgl_fmin(double a,double b);
double MGL_LOCAL_CONST mgl_fmax(double a,double b);
double MGL_LOCAL_CONST mgl_fmod(double a, double m);
//-----------------------------------------------------------------------------
// It seems that standard wcstombs() have a bug. So, I replace by my own.
void MGL_EXPORT mgl_wcstombs(char *dst, const wchar_t *src, int size)
{
	int j;
	for(j=0;j<size-1 && src[j]!=0;j++)
		dst[j] = src[j]<0x7f ? src[j] : ' ';
	dst[j] = 0;
}
//-----------------------------------------------------------------------------
MGL_LOCAL_PURE const mglDataA *FindVar(const std::vector<mglDataA*> &head, const std::wstring &name)
{
	for(size_t i=0;i<head.size();i++)
		if(head[i] && !wcscmp(head[i]->Name(),name.c_str()))	// bypass std::string comparison warning
			return head[i];
	return 0;
}
//-----------------------------------------------------------------------------
void MGL_EXPORT mgl_wcslwr(wchar_t *str)
{
	size_t l=mgl_wcslen(str);
	for(size_t k=0;k<l;k++)
		str[k] = (str[k]>='A' && str[k]<='Z') ? str[k]+'a'-'A' : str[k];
}
//-----------------------------------------------------------------------------
mreal MGL_NO_EXPORT mgl_gettime(const std::wstring &s)
{
	mreal t=NAN;
	tm a;	memset(&a,0,sizeof(tm));
	if(swscanf(s.c_str(),L"%u-%u-%u_%u.%u.%d", &a.tm_hour,&a.tm_min,&a.tm_sec, &a.tm_mday,&a.tm_mon,&a.tm_year)==6)
	{	a.tm_year-=1900;	a.tm_mon -= 1;
		if(a.tm_hour<24 && a.tm_min<60 && a.tm_sec<60 && a.tm_mday>0 && a.tm_mday<32 && a.tm_mon<12)
			t = mktime(&a);
	}
	else if(swscanf(s.c_str(),L"%d.%d.%d", &a.tm_mday,&a.tm_mon,&a.tm_year)==3)
	{	a.tm_year-=1900;	a.tm_mon -= 1;
		if(a.tm_mday>0 && a.tm_mday<32 && a.tm_mon<12)
			t = mktime(&a);
	}
	else if(swscanf(s.c_str(),L"%d-%d-%d", &a.tm_hour,&a.tm_min,&a.tm_sec)==3)
	{	a.tm_mday=1;	a.tm_mon=0;	a.tm_year=70;
		if(a.tm_hour<24 && a.tm_min<60 && a.tm_sec<60)
			t = mktime(&a);
	}
	return t;
}
//-----------------------------------------------------------------------------
double MGL_NO_EXPORT mgl_jac_sn(double a, double m)
{
	double sn=0, cn=0, dn=0;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return sn;
}
double MGL_NO_EXPORT mgl_jac_sc(double a, double m)
{
	double sn=0, cn=1, dn=0;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return sn/cn;
}
double MGL_NO_EXPORT mgl_jac_sd(double a, double m)
{
	double sn=0, cn=0, dn=1;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return sn/dn;
}

double MGL_NO_EXPORT mgl_jac_cn(double a, double m)
{
	double sn=1, cn=0, dn=0;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return cn;
}
double MGL_NO_EXPORT mgl_jac_cs(double a, double m)
{
	double sn=1, cn=0, dn=0;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return cn/sn;
}
double MGL_NO_EXPORT mgl_jac_cd(double a, double m)
{
	double sn=0, cn=0, dn=1;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return cn/dn;
}

double MGL_NO_EXPORT mgl_jac_dn(double a, double m)
{
	double sn=0, cn=0, dn=0;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return dn;
}
double MGL_NO_EXPORT mgl_jac_ds(double a, double m)
{
	double sn=1, cn=0, dn=0;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return dn/sn;
}
double MGL_NO_EXPORT mgl_jac_dc(double a, double m)
{
	double sn=0, cn=1, dn=0;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return dn/cn;
}

double MGL_NO_EXPORT mgl_jac_nd(double a, double m)
{
	double sn=0, cn=0, dn=1;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return 1./dn;
}
double MGL_NO_EXPORT mgl_jac_ns(double a, double m)
{
	double sn=1, cn=0, dn=0;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return 1./sn;
}
double MGL_NO_EXPORT mgl_jac_nc(double a, double m)
{
	double sn=0, cn=1, dn=0;
#if MGL_HAVE_GSL
	gsl_sf_elljac_e(a,m, &sn, &cn, &dn);
#endif
	return 1./cn;
}
//-----------------------------------------------------------------------------
/// Parse string and substitute the script argument
// All numbers are presented as mglData(1). Do boundary checking.
// NOTE: In any case where number is required the mglData::a[0] is used.
// String flag is binary 0x1 -> 'x', 0x2 -> 'y', 0x4 -> 'z'
HMDT MGL_NO_EXPORT mglFormulaCalc(const std::wstring &str, mglParser *arg, const std::vector<mglDataA*> &head)
{
	if(str.empty())	return NULL;
	std::vector<std::wstring> fns = mgl_wcs_args(str,'\\');
	return mglFormulaCalcA(fns[0],arg,head,fns);
}
HMDT MGL_NO_EXPORT mglFormulaCalcA(std::wstring str, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
#if MGL_HAVE_GSL
	gsl_set_error_handler_off();
#endif
	if(str.empty())	return new mglData;	// nothing to parse
	str = mgl_trim_ws(str);
	mreal tval = mgl_gettime(str);
	if(mgl_isnum(tval))
	{	mglData *r=new mglData;	r->a[0] = tval;	return r;	}

	long n,len=str.length();
	if(str[0]=='(' && mglCheck(str.substr(1,len-2)))	// remove braces
	{	str = str.substr(1,len-2);	len-=2;	}
	if(str[0]==':' && str[1]!=0)		//	this data file
	{
		size_t l=str.length()+1;
		char *buf = new char[l];	memset(buf,0,l);
		for(size_t i=1;str[i]!=0 && str[i]!=':' && i<l;i++)	buf[i-1]=str[i];
		HMDT res = new mglData(buf);	delete []buf;
		return res;
	}
	if(str[0]=='[')	// this is manual subdata
	{
		long i, j, br=0,k;
		bool ar=true,mt=false;
		HMDT res=0;
		for(i=1,j=1;i<len-1;i++)
		{
			if(str[i]=='[')	br++;
			if(str[i]==']' && br>0)	br--;
			if(str[i]==',' && !br)
			{
				HMDT a1=mglFormulaCalc(str.substr(j,i-j), arg, head);
				if(j==1)
				{	res = a1;	ar = (a1->nx==1);	mt = (a1->nx>1 && a1->ny==1);	}
				else
				{
					if(ar)		// res 1d array
					{	k = res->nx;	res->Insert('x',k);	mgl_data_put_dat(res,a1,k,-1,-1);	}
					else if(mt)	// res 2d array
					{	k = res->ny;	res->Insert('y',k);	mgl_data_put_dat(res,a1,-1,k,-1);	}
					else		// res 3d array
					{	k = res->nz;	res->Insert('z',k);	mgl_data_put_dat(res,a1,-1,-1,k);	}
					mgl_delete_data(a1);
				}
				j=i+1;
			}
		}
		HMDT a1=mglFormulaCalc(str.substr(j,i-j), arg, head);
		if(j==1)
		{	res = a1;	ar = (a1->nx==1);	mt = (a1->nx>1 && a1->ny==1);	}
		else
		{
			if(ar)		// res 1d array
			{	k = res->nx;	res->Insert('x',k);	mgl_data_put_dat(res,a1,k,-1,-1);	}
			else if(mt)	// res 2d array
			{	k = res->ny;	res->Insert('y',k);	mgl_data_put_dat(res,a1,-1,k,-1);	}
			else		// res 3d array
			{	k = res->nz;	res->Insert('z',k);	mgl_data_put_dat(res,a1,-1,-1,k);	}
			mgl_delete_data(a1);
		}
		return res;
	}

	n=mglFindInText(str,"&|");	// lowest priority -- logical
	if(n>=0)
		return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, str[n]=='|'?cor:cand,fns);
	n=mglFindInText(str,"<>=");	// low priority -- conditions
	if(n>=0)
		return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, str[n]=='<'?clt:(str[n]=='>'?cgt:ceq),fns);
	n=mglFindInText(str,"+-");	// normal priority -- additions
	if(n>=0 && (n<2 || !strchr("eE",str[n-1]) || (str[n-2]!='.' && !isdigit(str[n-2])) ))
		return str[n]=='+'? mglApplyOperAdd(str.substr(0,n),str.substr(n+1),arg, head,fns) : 
				mglApplyOperSub(str.substr(0,n),str.substr(n+1),arg, head,fns);
	n=mglFindInText(str,"*/%");	// high priority -- multiplications
	if(n>=0)
		return str[n]=='*'? mglApplyOperMul(str.substr(0,n),str.substr(n+1),arg, head,fns) : 
			(str[n]=='/'? mglApplyOperDiv(str.substr(0,n),str.substr(n+1),arg, head,fns) :
				mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_fmod,fns));
	n=mglFindInText(str,"@");	// high priority -- combine
	if(n>=0)
	{
		HMDT a1 = mglFormulaCalc(str.substr(0,n),arg, head);
		HMDT a2 = mglFormulaCalc(str.substr(n+1),arg, head);
		HMDT res = mgl_data_combine(a1,a2);
		mgl_delete_data(a1);	mgl_delete_data(a2);
		return res?res:new mglData;
	}
	n=mglFindInText(str,"^");	// highest priority -- power
	if(n>=0)	return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, ipw,fns);
	n=mglFindInText(str,":");	// highest priority -- array
	if(n>=0 && str.compare(L":"))
	{
		HMDT a1=mglFormulaCalc(str.substr(0,n), arg, head);
		HMDT a2=mglFormulaCalc(str.substr(n+1), arg, head);
		HMDT res = new mglData(abs(int(a2->a[0]+0.5)-int(a1->a[0]+0.5))+1);
		res->Fill(a1->a[0], a2->a[0]);
		mgl_delete_data(a1);	mgl_delete_data(a2);
		return res;
	}
	n=mglFindInText(str,".");				// highest priority -- suffixes
	wchar_t c0 = str[n+1];
	if(n>=0  && c0>='a' && c0!='e' && c0!='E' && !isdigit(c0))
	{
		mreal x,y,z,k,v=NAN;
		HMDT d = mglFormulaCalc(str.substr(0,n), arg, head);
		long ns[3] = {d->nx-1, d->ny-1, d->nz-1};
		const std::wstring &p=str.substr(n+1);
		wchar_t ch = p[1];
		if(c0=='a')
		{
			if(ch==0)	v = d->a[0];
			else
			{
				d->Momentum(ch,x,y);
				if(ch=='a')	v = x;
				else if(ch>='x' && ch<='z')	v = x/ns[ch-'x'];
			}
		}
		else if(c0=='n')
		{
			if(ch>='x' && ch<='z')	v = ns[p[1]-'x']+1;
			else if(!p.compare(L"nmax"))	{	v=d->MaximalNeg();	}
			else if(!p.compare(L"nmin"))	{	v=d->Minimal();	v = v<0?v:0;	}
		}
		else if(c0=='k')
		{	d->Momentum(ch,x,y,z,k);	v=k;	}
		else if(c0=='w')
		{
			d->Momentum(ch,x,y);
			if(ch=='a')	v = y;
			else if(ch>='x' && ch<='z')	v = y/ns[ch-'x'];
		}
		else if(c0=='m')
		{
			if(ch=='a' && p[2]=='x')	v = d->Maximal();
			else if(ch=='i' && p[2]=='n')	v = d->Minimal();
			else if(ch=='x' && p[2]=='f')	v = d->Maximal('x',0)/mreal(ns[0]);
			else if(ch=='x' && p[2]=='l')	v = d->Maximal('x',-1)/mreal(ns[0]);
			else if(ch=='x')	{	d->Maximal(x,y,z);	v = x/ns[0];	}
			else if(ch=='y' && p[2]=='f')	v = d->Maximal('y',0)/mreal(ns[1]);
			else if(ch=='y' && p[2]=='l')	v = d->Maximal('y',-1)/mreal(ns[1]);
			else if(ch=='y')	{	d->Maximal(x,y,z);	v = y/ns[1];	}
			else if(ch=='z' && p[2]=='f')	v = d->Maximal('z',0)/mreal(ns[2]);
			else if(ch=='z' && p[2]=='l')	v = d->Maximal('z',-1)/mreal(ns[2]);
			else if(ch=='z')	{	d->Maximal(x,y,z);	v = z/ns[2];	}
		}
		else if(c0=='s')
		{
			if(ch=='u' && p[2]=='m')	v = d->Momentum('x',x,y);
			else if(ch=='a' || (ch>='x' && ch<='z'))
			{	d->Momentum(ch,x,y,z,k);	v = z;	}
		}
		else if(!p.compare(L"fst"))	{	long i=-1,j=-1,l=-1;	v = d->Find(0,i,j,l);	}
		else if(!p.compare(L"lst"))	{	long i=-1,j=-1,l=-1;	v = d->Last(0,i,j,l);	}
		else if(!p.compare(L"pmax"))	{	v=d->Maximal();	v = v>0?v:0;	}
		else if(!p.compare(L"pmin"))	{	v=d->MinimalPos();	}
		delete d;
		// if this is valid suffix when finish parsing (it can be mreal number)
		if(mgl_isfin(v))
		{	HMDT res = new mglData;	res->a[0]=v;	return res;	}
	}
	if(str[0]=='`')
	{
		HMDT res = mglFormulaCalc(str.substr(1), arg, head);
		res->Transpose();	return res;
	}
	for(n=0;n<len;n++)	if(str[n]=='(')	break;
	if(n>=len)		// this is number or variable
	{
		HCDT v = (str!=L"#$mgl")?FindVar(head, str):0;
		if(v)	return new mglData(v);
		const mglNum *f = arg?arg->FindNum(str.c_str()):0;
		if(f)	{	HMDT res = new mglData;	res->a[0] = f->d;	return res;	}
		else if(!str.compare(L"rnd"))
		{
			v=FindVar(head, L"#$mgl");
			HMDT res = v?new mglData(v->GetNx(),v->GetNy(),v->GetNz()) : new mglData;
			for(long i=0;i<res->GetNN();i++)	res->a[i] = mgl_rnd();
			return res;
		}
		else
		{
			HMDT res = new mglData;
			wchar_t ch = str[0];
			if(ch<':') res->a[0] = wcstod(str.c_str(),0);	// this is number
			else if(!str.compare(L"pi"))	res->a[0] = M_PI;
			else if(ch==':')	res->a[0] = -1;
			else if(!str.compare(L"nan"))	res->a[0] = NAN;
			else if(!str.compare(L"inf"))	res->a[0] = INFINITY;
			return res;
		}
	}
	else
	{
		std::wstring nm = str.substr(0,n);
		str = str.substr(n+1,len-n-2);	len -= n+2;
		HCDT v = FindVar(head, nm);
		HMDT tmp = 0;
//		mglVar *v = arg->FindVar(nm.c_str());
		if(!v && !nm.compare(0,7,L"jacobi_"))	nm = nm.substr(7);
		if(!v && nm.empty())
		{
			long m=mglFindInText(str,")");
			if(m>1)
			{	nm = str.substr(0,m);	str = str.substr(m+2);
				tmp = mglFormulaCalc(nm,arg,head);	v = tmp;	}
		}
		n = mglFindInText(str,",");
		if(v)	// subdata
		{
			if(str[0]=='\'' && str[len-1]=='\'')	// this is column call
			{
				char *buf = new char[len];
				mgl_wcstombs(buf, str.substr(1).c_str(), len-1);	buf[len-1]=0;
				HMDT res = mgl_data_column(v,buf);
				if(tmp)	mgl_delete_data(tmp);
				delete []buf;	return res?res:new mglData;
			}
			else
			{
				HMDT a1=0, a2=0, a3=0;
				if(n>0)
				{
					long m=mglFindInText(str.substr(0,n),",");
					if(m>0)
					{
						str[m]=0;
						a1 = mglFormulaCalc(str.substr(0,m), arg, head);
						a2 = mglFormulaCalc(str.substr(m+1,n-m-1), arg, head);
						a3 = mglFormulaCalc(str.substr(n+1), arg, head);
					}
					else
					{
						a1 = mglFormulaCalc(str.substr(0,n), arg, head);
						a2 = mglFormulaCalc(str.substr(n+1), arg, head);
					}
				}
				else	a1 = mglFormulaCalc(str, arg, head);
				HMDT res = mgl_data_subdata_ext(v,a1,a2,a3);
				if(tmp)	mgl_delete_data(tmp);
				mgl_delete_data(a1);	mgl_delete_data(a2);
				mgl_delete_data(a3);	return res?res:new mglData;
			}
			if(tmp)	mgl_delete_data(tmp);
		}
		else if(nm[0]=='a')	// function
		{
			if(!nm.compare(L"asin"))		return mglApplyFunc(str, arg, head, asin,fns);
			else if(!nm.compare(L"acos"))	return mglApplyFunc(str, arg, head, acos,fns);
			else if(!nm.compare(L"atan"))	return mglApplyFunc(str, arg, head, atan,fns);
			else if(!nm.compare(L"asinh"))	return mglApplyFunc(str, arg, head, mgl_asinh,fns);
			else if(!nm.compare(L"acosh"))	return mglApplyFunc(str, arg, head, mgl_acosh,fns);
			else if(!nm.compare(L"atanh"))	return mglApplyFunc(str, arg, head, mgl_atanh,fns);
			else if(!nm.compare(L"arg"))
			{
				if(n>0)	return mglApplyOper(str.substr(n+1),str.substr(0,n),arg, head, atan2,fns);
				else
				{
					HADT a1 = mglFormulaCalcC(str, arg, head);
					HMDT res = mgl_datac_arg(a1);
					mgl_delete_datac(a1);	return res;
				}
			}
			else if(!nm.compare(L"abs"))
			{
				if(n>0)	return mglApplyOper(str.substr(n+1),str.substr(0,n),arg, head, hypot,fns);
				else
				{
					HADT a1 = mglFormulaCalcC(str, arg, head);
					HMDT res = mgl_datac_abs(a1);
					mgl_delete_datac(a1);	return res;
				}
			}
#if MGL_HAVE_GSL
			else if(!nm.compare(L"ai") || !nm.compare(L"airy_ai"))
				return mglApplyFuncGSL(str, arg, head, gsl_sf_airy_Ai,fns);
			else if(!nm.compare(L"airy_dai"))
				return mglApplyFuncGSL(str, arg, head, gsl_sf_airy_Ai_deriv,fns);
			else if(!nm.compare(L"airy_bi"))
				return mglApplyFuncGSL(str, arg, head, gsl_sf_airy_Bi,fns);
			else if(!nm.compare(L"airy_dbi"))
				return mglApplyFuncGSL(str, arg, head, gsl_sf_airy_Bi_deriv,fns);
		}
		else if(nm[0]=='b')
		{
			if(!nm.compare(L"beta") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_beta,fns);
			else if(!nm.compare(L"bi"))
				return mglApplyFuncGSL(str, arg, head, gsl_sf_airy_Bi,fns);
			else if(!nm.compare(L"bessel_i") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_bessel_Inu,fns);
			else if(!nm.compare(L"bessel_j") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_bessel_Jnu,fns);
			else if(!nm.compare(L"bessel_k") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_bessel_Knu,fns);
			else if(!nm.compare(L"bessel_y") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_bessel_Ynu,fns);
#endif
		}
		else if(nm[0]=='c')
		{
			if(!nm.compare(L"cos"))	return mglApplyFunc(str, arg, head, cos,fns);
			else if(!nm.compare(L"cosh") || !nm.compare(L"ch"))	return mglApplyFunc(str, arg, head, cosh,fns);
			else if(!nm.compare(L"conj"))
			{
				HADT a1 = mglFormulaCalcC(str, arg, head);
				HMDT res = mgl_datac_real(a1);
				mgl_delete_datac(a1);	return res;
			}
#if MGL_HAVE_GSL
			else if(!nm.compare(L"ci"))	return mglApplyFunc(str, arg, head, gsl_sf_Ci,fns);
			else if(!nm.compare(L"cn") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_cn,fns);
			else if(!nm.compare(L"cs") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_cs,fns);
			else if(!nm.compare(L"cd") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_cd,fns);
#endif
		}
		else if(nm[0]=='e')
		{
			if(!nm.compare(L"exp"))	return mglApplyFunc(str, arg, head, exp,fns);
#if MGL_HAVE_GSL
			else if(!nm.compare(L"erf"))	return mglApplyFunc(str, arg, head, gsl_sf_erf,fns);
			else if(!nm.compare(L"ee") || !nm.compare(L"elliptic_ec"))
				return mglApplyFuncGSL(str, arg, head, gsl_sf_ellint_Ecomp,fns);
			else if(!nm.compare(L"ek") || !nm.compare(L"elliptic_kc"))
				return mglApplyFuncGSL(str, arg, head, gsl_sf_ellint_Kcomp,fns);
			else if((!nm.compare(L"e") || !nm.compare(L"elliptic_e")) && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gslEllE,fns);
			else if(!nm.compare(L"elliptic_f"))
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gslEllF,fns);

			else if(!nm.compare(L"ei"))	return mglApplyFunc(str, arg, head, gsl_sf_expint_Ei,fns);
			else if(!nm.compare(L"e1"))	return mglApplyFunc(str, arg, head, gsl_sf_expint_E1,fns);
			else if(!nm.compare(L"e2"))	return mglApplyFunc(str, arg, head, gsl_sf_expint_E2,fns);
			else if(!nm.compare(L"eta"))	return mglApplyFunc(str, arg, head, gsl_sf_eta,fns);
			else if(!nm.compare(L"ei3"))	return mglApplyFunc(str, arg, head, gsl_sf_expint_3,fns);
#endif
		}
		else if(nm[0]=='l')
		{
			if(!nm.compare(L"log") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, llg,fns);
			else if(!nm.compare(L"lg"))	return mglApplyFunc(str, arg, head, log10,fns);
			else if(!nm.compare(L"ln"))	return mglApplyFunc(str, arg, head, log,fns);
#if MGL_HAVE_GSL
			else if(!nm.compare(L"li2"))	return mglApplyFunc(str, arg, head, gsl_sf_dilog,fns);
			else if(!nm.compare(L"legendre") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gslLegP,fns);
#endif
		}
		else if(nm[0]=='s')
		{
			if(!nm.compare(L"sqrt"))	return mglApplyFunc(str, arg, head, sqrt,fns);
			else if(!nm.compare(L"sin"))	return mglApplyFunc(str, arg, head, sin,fns);
			else if(!nm.compare(L"step"))	return mglApplyFunc(str, arg, head, stp,fns);
			else if(!nm.compare(L"sign"))	return mglApplyFunc(str, arg, head, sgn,fns);
			else if(!nm.compare(L"sinh") || !nm.compare(L"sh"))	return mglApplyFunc(str, arg, head, sinh,fns);
#if MGL_HAVE_GSL
			else if(!nm.compare(L"si"))		return mglApplyFunc(str, arg, head, gsl_sf_Si,fns);
			else if(!nm.compare(L"sinc"))	return mglApplyFunc(str, arg, head, gsl_sf_sinc,fns);
			else if(!nm.compare(L"sn") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_sn,fns);
			else if(!nm.compare(L"sc") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_sc,fns);
			else if(!nm.compare(L"sd") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_sd,fns);
#endif
			else if(!nm.compare(L"sum") && n>0)
			{
				HMDT a=NULL, b=mglFormulaCalcA(str.substr(n+1), arg, head, fns);
				long m = long(b->a[0]+0.5);
				const char *s = head.size()>0?head[head.size()-1]->s.s:"";
				if(m>0)
				{
					std::vector<mglDataA*> hh(head);
					int in=0;
					if(s[0]=='_' && s[1]>='i' && s[1]<'z') in = s[1]-'i'+1;
					char name[3] = {'_',char('i'+in),0};
					b->s = name;	b->Create(1,1,1);	hh.push_back(b);
					a = mglFormulaCalcA(str.substr(0,n), arg, hh, fns);
					long nn = a->GetNN();
					for(long i=1;i<m;i++)
					{
						b->a[0] = i;
						HMDT c = mglFormulaCalcA(str.substr(0,n), arg, hh, fns);
						for(long j=0;j<nn;j++)	a->a[j] += c->a[j];
						mgl_delete_data(c);
					}
					mgl_delete_data(b);
				}
				else
				{	a = new mglData;	a->a[0]=NAN;	}
				return a;
			}
			else if(!nm.compare(L"spline"))
			{
				HMDT a = mglFormulaCalcA(str.substr(0,n), arg, head, fns);
				long n1 = mglFindInText(str,",",1), n2 = mglFindInText(str,",",2);
				HMDT ix, iy=NULL, iz=NULL;
				if(n1>0)
				{
					ix = mglFormulaCalcA(str.substr(n+1,n1), arg, head, fns);
					if(n2>0)
					{
						iy = mglFormulaCalcA(str.substr(n1+1,n2), arg, head, fns);
						iz = mglFormulaCalcA(str.substr(n2+1), arg, head, fns);
					}
					else	iy = mglFormulaCalcA(str.substr(n1+1), arg, head, fns);

				}
				else	ix = mglFormulaCalcA(str.substr(n+1), arg, head, fns);
				HMDT res = mgl_data_evaluate(a, ix,iy,iz, false);
				if(a)	mgl_delete_data(a);
				if(ix)	mgl_delete_data(ix);
				if(iy)	mgl_delete_data(iy);
				if(iz)	mgl_delete_data(iz);
				return res;
			}
		}
		else if(nm[0]=='t')
		{
			if(!nm.compare(L"tg") || !nm.compare(L"tan"))
				return mglApplyFunc(str, arg, head, tan,fns);
			else if(!nm.compare(L"tanh") || !nm.compare(L"th"))
				return mglApplyFunc(str, arg, head, tanh,fns);
		}
		else if(!nm.compare(L"pow") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, pow,fns);
		else if(nm[0]=='m')
		{
			if(!nm.compare(L"mod") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_fmod,fns);
			else if(!nm.compare(L"min") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_fmin,fns);
			else if(!nm.compare(L"max") && n>0)
				return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_fmax,fns);
		}
		else if(!nm.compare(L"int"))	return mglApplyFunc(str, arg, head, floor,fns);
		else if(!nm.compare(L"random"))
		{	HMDT res=mglFormulaCalc(str, arg, head);	mreal *a = res->a;
			for(long i=0;i<res->GetNN();i++)	a[i] = mgl_rnd();
			return res;	}
		else if(!nm.compare(L"real"))
		{
			HADT a1 = mglFormulaCalcC(str, arg, head);
			HMDT res = mgl_datac_real(a1);
			mgl_delete_datac(a1);	return res;
		}
		else if(!nm.compare(L"imag"))
		{
			HADT a1 = mglFormulaCalcC(str, arg, head);
			HMDT res = mgl_datac_imag(a1);
			mgl_delete_datac(a1);	return res;
		}
		else if(!nm.compare(L"norm"))
		{
			HADT a1 = mglFormulaCalcC(str, arg, head);
			HMDT res = mgl_datac_norm(a1);
			mgl_delete_datac(a1);	return res;
		}
#if MGL_HAVE_GSL
		else if(!nm.compare(L"dn") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_dn,fns);
		else if(!nm.compare(L"ds") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_ds,fns);
		else if(!nm.compare(L"dc") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_dc,fns);
		else if(!nm.compare(L"nc") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_nc,fns);
		else if(!nm.compare(L"ns") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_ns,fns);
		else if(!nm.compare(L"nd") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, mgl_jac_nd,fns);
		else if(!nm.compare(L"i") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_bessel_Inu,fns);
		else if(!nm.compare(L"j") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_bessel_Jnu,fns);
		else if(!nm.compare(L"k") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_bessel_Knu,fns);
		else if(!nm.compare(L"y") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_bessel_Ynu,fns);
		else if(!nm.compare(L"f") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gslEllF,fns);
		else if(!nm.compare(L"hypot") && n>0)
			return mglApplyOper(str.substr(n+1),str.substr(0,n),arg, head, hypot,fns);
		else if(!nm.compare(L"gamma"))	return mglApplyFunc(str, arg, head, gsl_sf_gamma,fns);
		else if(!nm.compare(L"gamma_inc") && n>0)
			return mglApplyOper(str.substr(0,n),str.substr(n+1),arg, head, gsl_sf_gamma_inc,fns);
		else if(!nm.compare(L"w0"))	return mglApplyFunc(str, arg, head, gsl_sf_lambert_W0,fns);
		else if(!nm.compare(L"w1"))	return mglApplyFunc(str, arg, head, gsl_sf_lambert_Wm1,fns);
		else if(!nm.compare(L"psi"))	return mglApplyFunc(str, arg, head, gsl_sf_psi,fns);
		else if(!nm.compare(L"zeta"))	return mglApplyFunc(str, arg, head, gsl_sf_zeta,fns);
		else if(!nm.compare(L"z"))	return mglApplyFunc(str, arg, head, gsl_sf_dawson,fns);
#endif
		else if(!nm.compare(L"value"))
		{
			HMDT a = mglFormulaCalcA(str.substr(0,n), arg, head, fns);
			long n1 = mglFindInText(str,",",1), n2 = mglFindInText(str,",",2);
			HMDT ix, iy=NULL, iz=NULL;
			if(n1>0)
			{
				ix = mglFormulaCalcA(str.substr(n+1,n1), arg, head, fns);
				if(n2>0)
				{
					iy = mglFormulaCalcA(str.substr(n1+1,n2), arg, head, fns);
					iz = mglFormulaCalcA(str.substr(n2+1), arg, head, fns);
				}
				else	iy = mglFormulaCalcA(str.substr(n1+1), arg, head, fns);

			}
			else	ix = mglFormulaCalcA(str.substr(n+1), arg, head, fns);
			HMDT res = mgl_data_subdata_ext(a, ix,iy,iz);
			if(a)	mgl_delete_data(a);
			if(ix)	mgl_delete_data(ix);
			if(iy)	mgl_delete_data(iy);
			if(iz)	mgl_delete_data(iz);
			return res;
		}
		else if(!nm.compare(L"dsum") && n>0)
		{
			HMDT a=NULL, b=mglFormulaCalcA(str.substr(n+1), arg, head, fns);
			long m = long(b->a[0]+0.5);
			const char *s = head.size()>0?head[head.size()-1]->s.s:"";
			if(m>0)
			{
				std::vector<mglDataA*> hh(head);
				int in=0, zn=1;
				if(s[0]=='_' && s[1]>='i' && s[1]<'z') in = s[1]-'i'+1;
				char name[3] = {'_',char('i'+in),0};
				b->s = name;	b->Create(1,1,1);	hh.push_back(b);
				a = mglFormulaCalcA(str.substr(0,n), arg, hh, fns);
				long nn = a->GetNN();
				for(long i=1;i<m;i++)
				{
					b->a[0] = i;	zn *= -1;
					HMDT c = mglFormulaCalcA(str.substr(0,n), arg, hh, fns);
					for(long j=0;j<nn;j++)	a->a[j] += zn*c->a[j];
					mgl_delete_data(c);
				}
				mgl_delete_data(b);
			}
			else
			{	a = new mglData;	a->a[0]=NAN;	}
			return a;
		}
		else if(!nm.compare(L"prod") && n>0)
		{
			HMDT a=NULL, b=mglFormulaCalcA(str.substr(n+1), arg, head, fns);
			long m = long(b->a[0]+0.5);
			const char *s = head.size()>0?head[head.size()-1]->s.s:"";
			if(m>0)
			{
				std::vector<mglDataA*> hh(head);
				int in=0;
				if(s[0]=='_' && s[1]>='i' && s[1]<'z') in = s[1]-'i'+1;
				char name[3] = {'_',char('i'+in),0};
				b->s = name;	b->Create(1,1,1);	hh.push_back(b);
				a = mglFormulaCalcA(str.substr(0,n), arg, hh, fns);
				long nn = a->GetNN();
				for(long i=1;i<m;i++)
				{
					b->a[0] = i;
					HMDT c = mglFormulaCalcA(str.substr(0,n), arg, hh, fns);
					for(long j=0;j<nn;j++)	a->a[j] *= c->a[j];
					mgl_delete_data(c);
				}
				mgl_delete_data(b);
			}
			else
			{	a = new mglData;	a->a[0]=NAN;	}
			return a;
		}
		else if(nm[0]=='f' && nm[1]=='n' && fns.size()>1)
		{
			HMDT a=NULL, b=NULL, r=NULL;
			std::vector<mglDataA*> hh(head);
			std::vector<std::wstring> tmp;	// disable recursion
			if(n>0)
			{
				a = mglFormulaCalcA(str.substr(0,n), arg, head, fns);
				if(a)	{	a->s = "_1";	hh.push_back(a);	}
				b = mglFormulaCalcA(str.substr(n+1), arg, head, fns);
				if(b)	{	b->s = "_2";	hh.push_back(b);	}
			}
			else
			{
				a = mglFormulaCalcA(str, arg, head, fns);
				if(a)	{	a->s = "_1";	hh.push_back(a);	}
			}
			if(!nm.compare(L"fn1") && fns.size()>1)	r = mglFormulaCalcA(fns[1], arg, hh, tmp);
			if(!nm.compare(L"fn2") && fns.size()>2)	r = mglFormulaCalcA(fns[2], arg, hh, tmp);
			if(!nm.compare(L"fn3") && fns.size()>3)	r = mglFormulaCalcA(fns[3], arg, hh, tmp);
			if(!nm.compare(L"fn4") && fns.size()>4)	r = mglFormulaCalcA(fns[4], arg, hh, tmp);
			if(!nm.compare(L"fn5") && fns.size()>5)	r = mglFormulaCalcA(fns[5], arg, hh, tmp);
			if(!nm.compare(L"fn6") && fns.size()>6)	r = mglFormulaCalcA(fns[6], arg, hh, tmp);
			if(!nm.compare(L"fn7") && fns.size()>7)	r = mglFormulaCalcA(fns[7], arg, hh, tmp);
			if(!nm.compare(L"fn8") && fns.size()>8)	r = mglFormulaCalcA(fns[8], arg, hh, tmp);
			if(!nm.compare(L"fn9") && fns.size()>9)	r = mglFormulaCalcA(fns[9], arg, hh, tmp);
			if(a)	mgl_delete_data(a);
			if(b)	mgl_delete_data(b);
			if(!r)	{	r = new mglData;	r->a[0]=NAN;	}
			return r;
		}
	}
	HMDT res = new mglData;	res->a[0]=NAN;	return res;
}
//-----------------------------------------------------------------------------
dual MGL_LOCAL_CONST ceqc(dual a,dual b);	//{return a==b?1:0;}
dual MGL_LOCAL_CONST cltc(dual a,dual b);	//{return real(a-b)<0?1:0;}
dual MGL_LOCAL_CONST cgtc(dual a,dual b);	//{return real(a-b)>0?1:0;}
dual MGL_LOCAL_CONST ipwc(dual a,dual b);	//{return mgl_ipowc(a,int(b.real()));}
dual MGL_LOCAL_CONST powc(dual a,dual b);	//{return exp(b*log(a));	}
dual MGL_LOCAL_CONST llgc(dual a,dual b);	//{return log(a)/log(b);	}
dual MGL_LOCAL_CONST cmplxc(dual a,dual b);	//{return a+dual(0,1)*b;	}
dual MGL_LOCAL_CONST expi(dual a);	//{	return exp(dual(0,1)*a);	}
dual MGL_LOCAL_CONST expi(double a);	//{	return dual(cos(a),sin(a));	}
//-----------------------------------------------------------------------------
dual MGL_LOCAL_CONST hypotc(dual x, dual y);	//{	return sqrt(x*x+y*y);	}
dual MGL_LOCAL_CONST asinhc(dual x);	//{	return log(x+sqrt(x*x+mreal(1)));	}
dual MGL_LOCAL_CONST acoshc(dual x);	//{	return log(x+sqrt(x*x-mreal(1)));	}
dual MGL_LOCAL_CONST atanhc(dual x);	//{	return log((mreal(1)+x)/(mreal(1)-x))/mreal(2);	}
dual MGL_LOCAL_CONST conjc(dual x);	//{	return dual(real(x),-imag(x));	}
dual MGL_LOCAL_CONST sinc(dual x);	//{	return sin(x);	}
dual MGL_LOCAL_CONST cosc(dual x);	//{	return cos(x);	}
dual MGL_LOCAL_CONST tanc(dual x);	//{	return tan(x);	}
dual MGL_LOCAL_CONST sinhc(dual x);	//{	return sinh(x);	}
dual MGL_LOCAL_CONST coshc(dual x);	//{	return cosh(x);	}
dual MGL_LOCAL_CONST tanhc(dual x);	//{	return tanh(x);	}
dual MGL_LOCAL_CONST asinc(dual x);	//{	return log(ic*x+sqrt(mreal(1)-x*x))/ic;	}
dual MGL_LOCAL_CONST acosc(dual x);	//{	return log(x+sqrt(x*x-mreal(1)))/ic;	}
dual MGL_LOCAL_CONST atanc(dual x);	//{	return log((ic-x)/(ic+x))/(mreal(2)*ic);	}
dual MGL_LOCAL_CONST expc(dual x);	//{	return exp(x);	}
dual MGL_LOCAL_CONST sqrtc(dual x);	//{	return sqrt(x);	}
dual MGL_LOCAL_CONST logc(dual x);	//{	return log(x);	}
dual MGL_LOCAL_CONST absc(dual x);	//{	return abs(x);	}
dual MGL_LOCAL_CONST argc(dual x);	//{	return arg(x);	}
dual MGL_LOCAL_CONST lgc(dual x);	//{	return log10(x);}
dual MGL_LOCAL_CONST realc(dual x);	//{	return real(x);	}
dual MGL_LOCAL_CONST imagc(dual x);	//{	return imag(x);	}
dual MGL_LOCAL_CONST normc(dual x);	//{	return norm(x);	}
//-----------------------------------------------------------------------------
/// Parse string and substitute the script argument
// All numbers are presented as mglData(1). Do boundary checking.
// NOTE: In any case where number is required the mglData::a[0] is used.
// String flag is binary 0x1 -> 'x', 0x2 -> 'y', 0x4 -> 'z'
HADT MGL_NO_EXPORT mglFormulaCalcC(const std::wstring &str, mglParser *arg, const std::vector<mglDataA*> &head)
{
	if(str.empty())	return NULL;
	std::vector<std::wstring> fns = mgl_wcs_args(str,'\\');
	return mglFormulaCalcAC(fns[0],arg,head,fns);
}
HADT MGL_NO_EXPORT mglFormulaCalcAC(std::wstring str, mglParser *arg, const std::vector<mglDataA*> &head, const std::vector<std::wstring> &fns)
{
#if MGL_HAVE_GSL
	gsl_set_error_handler_off();
#endif
	if(str.empty())	return new mglDataC;	// nothing to parse
	str = mgl_trim_ws(str);
	mreal tval = mgl_gettime(str);
	if(mgl_isnum(tval))
	{	mglDataC *r=new mglDataC;	r->a[0] = tval;	return r;	}

	long n,len=str.length();
	if(str[0]=='(' && mglCheck(str.substr(1,len-2)))	// remove braces
	{	str = str.substr(1,len-2);	len-=2;	}
	if(str[0]=='[')	// this is manual subdata
	{
		long i, j, br=0,k;
		bool ar=true,mt=false;
		HADT res=0;
		for(i=1,j=1;i<len-1;i++)
		{
			if(str[i]=='[')	br++;
			if(str[i]==']' && br>0)	br--;
			if(str[i]==',' && !br)
			{
				HADT a1=mglFormulaCalcC(str.substr(j,i-j), arg, head);
				if(j==1)
				{	res = a1;	ar = (a1->nx==1);	mt = (a1->nx>1 && a1->ny==1);	}
				else
				{
					if(ar)		// res 1d array
					{	k = res->nx;	res->Insert('x',k);	mgl_datac_put_dat(res,a1,k,-1,-1);	}
					else if(mt)	// res 2d array
					{	k = res->ny;	res->Insert('y',k);	mgl_datac_put_dat(res,a1,-1,k,-1);	}
					else		// res 3d array
					{	k = res->nz;	res->Insert('z',k);	mgl_datac_put_dat(res,a1,-1,-1,k);	}
					mgl_delete_datac(a1);
				}
				j=i+1;
			}
		}
		HADT a1=mglFormulaCalcC(str.substr(j,i-j), arg, head);
		if(j==1)
		{	res = a1;	ar = (a1->nx==1);	mt = (a1->nx>1 && a1->ny==1);	}
		else
		{
			if(ar)		// res 1d array
			{	k = res->nx;	res->Insert('x',k);	mgl_datac_put_dat(res,a1,k,-1,-1);	}
			else if(mt)	// res 2d array
			{	k = res->ny;	res->Insert('y',k);	mgl_datac_put_dat(res,a1,-1,k,-1);	}
			else		// res 3d array
			{	k = res->nz;	res->Insert('z',k);	mgl_datac_put_dat(res,a1,-1,-1,k);	}
			mgl_delete_datac(a1);
		}
		return res;
	}

	n=mglFindInText(str,"<>=");	// low priority -- conditions
	if(n>=0)
		return mglApplyOperC(str.substr(0,n),str.substr(n+1),arg, head, str[n]=='<'?cltc:(str[n]=='>'?cgtc:ceqc),fns);
	n=mglFindInText(str,"+-");	// normal priority -- additions
	if(n>=0 && (n<2 || !strchr("eE",str[n-1]) || (str[n-2]!='.' && !isdigit(str[n-2]))))
		return str[n]=='+'? mglApplyOperAddC(str.substr(0,n),str.substr(n+1),arg, head,fns) : mglApplyOperSubC(str.substr(0,n),str.substr(n+1),arg, head,fns);
	n=mglFindInText(str,"*/");	// high priority -- multiplications
	if(n>=0)
		return str[n]=='*'? mglApplyOperMulC(str.substr(0,n),str.substr(n+1),arg, head,fns) : mglApplyOperDivC(str.substr(0,n),str.substr(n+1),arg, head,fns);
	n=mglFindInText(str,"@");	// high priority -- combine
	if(n>=0)
	{
		HADT a1 = mglFormulaCalcC(str.substr(0,n),arg, head);
		HADT a2 = mglFormulaCalcC(str.substr(n+1),arg, head);
		HADT res = mgl_datac_combine(a1,a2);
		mgl_delete_datac(a1);	mgl_delete_datac(a2);
		return res?res:new mglDataC;
	}
	n=mglFindInText(str,"^");				// highest priority -- power
	if(n>=0)
		return mglApplyOperC(str.substr(0,n),str.substr(n+1),arg, head, ipwc,fns);
	n=mglFindInText(str,":");				// highest priority -- array
	if(n>=0 && str.compare(L":"))
	{
		HMDT a1=mglFormulaCalc(str.substr(0,n), arg, head);
		HMDT a2=mglFormulaCalc(str.substr(n+1), arg, head);
		HADT res = new mglDataC(abs(int(a2->a[0]+0.5)-int(a1->a[0]+0.5))+1);
		res->Fill(a1->a[0], a2->a[0]);
		mgl_delete_data(a1);	mgl_delete_data(a2);
		return res;
	}
	n=mglFindInText(str,".");				// highest priority -- suffixes
	wchar_t c0 = str[n+1];
	if(n>=0  && c0>='a' && c0!='e' && c0!='E' && !isdigit(c0))
	{
		dual v=NAN;
		HADT d = mglFormulaCalcC(str.substr(0,n), arg, head);
		long ns[3] = {d->nx-1, d->ny-1, d->nz-1};
		const std::wstring &p=str.substr(n+1);
		wchar_t ch = p[1];
		if(c0=='a')
		{
			if(ch==0)	v = d->a[0];
			else
			{
				mreal x,y;
				d->Momentum(ch,x,y);
				if(ch=='a')	v = x;
				else if(ch>='x' && ch<='z')	v = x/ns[ch-'x'];
			}
		}
		else if(c0=='n' && ch>='x' && ch<='z')	v = ns[ch-'x']+1;
		else if(c0=='k')
		{
			mreal x,y,z,k;
			d->Momentum(ch,x,y,z,k);
			if(ch=='a')	v = k;
			else if(ch>='x' && ch<='z')	v = k/ns[ch-'x'];
		}
		else if(c0=='w')
		{
			mreal x,y;
			d->Momentum(ch,x,y);
			if(ch=='a')	v = y;
			else if(ch>='x' && ch<='z')	v = y/ns[ch-'x'];
		}
		else if(c0=='m')
		{
			mreal x,y,z;
			if(ch=='a' && p[2]=='x')	v = d->Maximal();
			else if(ch=='i' && p[2]=='n')	v = d->Minimal();
			else if(ch=='x' && p[2]=='f')	v = d->Maximal('x',0)/mreal(ns[0]);
			else if(ch=='x' && p[2]=='l')	v = d->Maximal('x',-1)/mreal(ns[0]);
			else if(ch=='x')	{	d->Maximal(x,y,z);	v = x/ns[0];	}
			else if(ch=='y' && p[2]=='f')	v = d->Maximal('y',0)/mreal(ns[1]);
			else if(ch=='y' && p[2]=='l')	v = d->Maximal('y',-1)/mreal(ns[1]);
			else if(ch=='y')	{	d->Maximal(x,y,z);	v = y/ns[1];	}
			else if(ch=='z' && p[2]=='f')	v = d->Maximal('z',0)/mreal(ns[2]);
			else if(ch=='z' && p[2]=='l')	v = d->Maximal('z',-1)/mreal(ns[2]);
			else if(ch=='z')	{	d->Maximal(x,y,z);	v = z/ns[2];	}
		}
		else if(c0=='s')
		{
			mreal x,y,z,k;
			if(ch=='u' && p[2]=='m')	v = d->Momentum('x',x,y);
			else if(ch=='a')
			{	d->Momentum(ch,x,y,z,k);	v = z;	}
			else if(ch>='x' && ch<='z')
			{	d->Momentum(ch,x,y,z,k);	v = z/ns[ch-'x'];	}
		}
		else if(!p.compare(L"fst"))	{	long i=-1,j=-1,l=-1;	v = d->Find(0,i,j,l);	}
		else if(!p.compare(L"lst"))	{	long i=-1,j=-1,l=-1;	v = d->Last(0,i,j,l);	}
		delete d;
		// if this is valid suffix when finish parsing (it can be mreal number)
		if(mgl_isfin(v))
		{	HADT res = new mglDataC;	res->a[0]=v;	return res;	}
	}
	if(str[0]=='`')
	{
		HADT res = mglFormulaCalcC(str.substr(1), arg, head);
		res->Transpose();	return res;
	}
	for(n=0;n<len;n++)	if(str[n]=='(')	break;
	if(n>=len)		// this is number or variable
	{
		HCDT v = (str!=L"#$mgl")?FindVar(head, str):0;
		if(v)	return new mglDataC(v);
		const mglNum *f = arg?arg->FindNum(str.c_str()):0;
		if(f)	{	HADT res = new mglDataC;	res->a[0] = f->c;	return res;	}
		else if(!str.compare(L"rnd"))
		{
			v=FindVar(head, L"#$mgl");
			HADT res = v?new mglDataC(v->GetNx(),v->GetNy(),v->GetNz()) : new mglDataC;
			for(long i=0;i<res->GetNN();i++)
				res->a[i] = dual(mgl_rnd(), mgl_rnd());
			return res;
		}
		else
		{
			HADT res = new mglDataC;
			wchar_t ch = str[0];
			if(ch<':')	// this is real number
				res->a[0] = (str[str.length()-1]=='i') ? dual(0,wcstod(str.c_str(),0)) :  mreal(wcstod(str.c_str(),0));
			else if(ch=='i')	// this is imaginary number
				res->a[0] = dual(0,(str[1]>='0' && str[1]<='9')?wcstod(str.c_str()+1,0):1);
			else if(!str.compare(L"pi"))	res->a[0] = M_PI;
			else if(ch==':')	res->a[0] = -1;
			else if(!str.compare(L"nan"))	res->a[0] = NAN;
			else if(!str.compare(L"inf"))	res->a[0] = INFINITY;
			return res;
		}
	}
	else
	{
		std::wstring nm = str.substr(0,n);
		str = str.substr(n+1,len-n-2);	len -= n+2;
		HCDT v = FindVar(head, nm);
		HADT tmp = 0;
//		mglVar *v = arg->FindVar(nm.c_str());
		if(!v && !nm.compare(0,7,L"jacobi_"))	nm = nm.substr(7);
		if(!v && nm.empty())	
		{
			long m=mglFindInText(nm,")");
			if(m>1)
			{	nm = str.substr(0,m);	str = str.substr(m+2);
				tmp = mglFormulaCalcC(nm,arg,head);	v = tmp;	}
		}
		n = mglFindInText(str,",");
		if(v)	// subdata
		{
			if(str[0]=='\'' && str[len-1]=='\'')	// this is column call
			{
				char *buf = new char[len];
				mgl_wcstombs(buf, str.substr(1).c_str(), len-1);	buf[len-1]=0;
				HADT res = mgl_datac_column(v,buf);
				if(tmp)	mgl_delete_datac(tmp);
				delete []buf;	return res?res:new mglDataC;
			}
			else
			{
				HMDT a1=0, a2=0, a3=0;
				if(n>0)
				{
					long m=mglFindInText(str.substr(0,n),",");
					if(m>0)
					{
						str[m]=0;
						a1 = mglFormulaCalc(str.substr(0,m), arg, head);
						a2 = mglFormulaCalc(str.substr(m+1,n-m-1), arg, head);
						a3 = mglFormulaCalc(str.substr(n+1), arg, head);
					}
					else
					{
						a1 = mglFormulaCalc(str.substr(0,n), arg, head);
						a2 = mglFormulaCalc(str.substr(n+1), arg, head);
					}
				}
				else	a1 = mglFormulaCalc(str, arg, head);
				HADT res = mgl_datac_subdata_ext(v,a1,a2,a3);
				if(tmp)	mgl_delete_datac(tmp);
				mgl_delete_data(a1);	mgl_delete_data(a2);
				mgl_delete_data(a3);	return res?res:new mglDataC;
			}
				if(tmp)	mgl_delete_datac(tmp);
		}
		else if(nm[0]=='a')	// function
		{
			if(!nm.compare(L"asin"))	return mglApplyFuncC(str, arg, head, asinc,fns);
			else if(!nm.compare(L"acos"))	return mglApplyFuncC(str, arg, head, acosc,fns);
			else if(!nm.compare(L"atan"))	return mglApplyFuncC(str, arg, head, atanc,fns);
			else if(!nm.compare(L"asinh"))	return mglApplyFuncC(str, arg, head, asinhc,fns);
			else if(!nm.compare(L"acosh"))	return mglApplyFuncC(str, arg, head, acoshc,fns);
			else if(!nm.compare(L"atanh"))	return mglApplyFuncC(str, arg, head, atanhc,fns);
			else if(!nm.compare(L"arg"))	return mglApplyFuncC(str, arg, head, argc,fns);
			else if(!nm.compare(L"abs"))	return mglApplyFuncC(str, arg, head, absc,fns);
		}
		else if(nm[0]=='c')
		{
			if(!nm.compare(L"cos"))	return mglApplyFuncC(str, arg, head, cosc,fns);
			else if(!nm.compare(L"cosh") || !nm.compare(L"ch"))	return mglApplyFuncC(str, arg, head, coshc,fns);
			else if(!nm.compare(L"conj"))	return mglApplyFuncC(str, arg, head, conjc,fns);
			else if(!nm.compare(L"cmplx") && n>0)
				return mglApplyOperC(str.substr(0,n),str.substr(n+1),arg, head, cmplxc,fns);
		}
		else if(!nm.compare(L"exp"))	return mglApplyFuncC(str, arg, head, expc,fns);
		else if(nm[0]=='l')
		{
			if(!nm.compare(L"log") || !nm.compare(L"ln"))	return mglApplyFuncC(str, arg, head, logc,fns);
			else if(!nm.compare(L"lg"))	return mglApplyFuncC(str, arg, head, lgc,fns);
		}
		else if(nm[0]=='s')
		{
			if(!nm.compare(L"sqrt"))	return mglApplyFuncC(str, arg, head, sqrtc,fns);
			else if(!nm.compare(L"sin"))	return mglApplyFuncC(str, arg, head, sinc,fns);
			else if(!nm.compare(L"sinh") || !nm.compare(L"sh"))	return mglApplyFuncC(str, arg, head, sinhc,fns);
			else if(!nm.compare(L"sum") && n>0)
			{
				HADT a=NULL;
				HMDT b=mglFormulaCalcA(str.substr(n+1), arg, head, fns);
				long m = long(b->a[0]+0.5);
				const char *s = head.size()>0?head[head.size()-1]->s.s:"";
				if(m>0)
				{
					std::vector<mglDataA*> hh(head);
					int in=0;
					if(s[0]=='_' && s[1]>='i' && s[1]<'z') in = s[1]-'i'+1;
					char name[3] = {'_',char('i'+in),0};
					b->s = name;	b->Create(1,1,1);	hh.push_back(b);
					a = mglFormulaCalcAC(str.substr(0,n), arg, hh, fns);
					long nn = a->GetNN();
					for(long i=1;i<m;i++)
					{
						b->a[0] = i;
						HADT c = mglFormulaCalcAC(str.substr(0,n), arg, hh, fns);
						for(long j=0;j<nn;j++)	a->a[j] += c->a[j];
						mgl_delete_datac(c);
					}
					mgl_delete_data(b);
				}
				else
				{	a = new mglDataC;	a->a[0]=NAN;	}
				return a;
			}
			else if(!nm.compare(L"spline"))
			{
				HADT a = mglFormulaCalcAC(str.substr(0,n), arg, head, fns);
				long n1 = mglFindInText(str,",",1), n2 = mglFindInText(str,",",2);
				HMDT ix, iy=NULL, iz=NULL;
				if(n1>0)
				{
					ix = mglFormulaCalcA(str.substr(n+1,n1), arg, head, fns);
					if(n2>0)
					{
						iy = mglFormulaCalcA(str.substr(n1+1,n2), arg, head, fns);
						iz = mglFormulaCalcA(str.substr(n2+1), arg, head, fns);
					}
					else	iy = mglFormulaCalcA(str.substr(n1+1), arg, head, fns);

				}
				else	ix = mglFormulaCalcA(str.substr(n+1), arg, head, fns);
				HADT res = mgl_datac_evaluate(a, ix,iy,iz, false);
				if(a)	mgl_delete_datac(a);
				if(ix)	mgl_delete_data(ix);
				if(iy)	mgl_delete_data(iy);
				if(iz)	mgl_delete_data(iz);
				return res;
			}
		}
		else if(nm[0]=='t')
		{
			if(!nm.compare(L"tg") || !nm.compare(L"tan"))	return mglApplyFuncC(str, arg, head, tanc,fns);
			else if(!nm.compare(L"tanh") || !nm.compare(L"th"))	return mglApplyFuncC(str, arg, head, tanhc,fns);
		}
		else if(!nm.compare(L"pow") && n>0)
			return mglApplyOperC(str.substr(0,n),str.substr(n+1),arg, head, powc,fns);
		else if(!nm.compare(L"random"))
		{	HADT res=mglFormulaCalcC(str, arg, head);	dual *a = res->a;
			for(long i=0;i<res->GetNN();i++)	a[i] = dual(mgl_rnd(), mgl_rnd());
			return res;	}
		else if(!nm.compare(L"hypot"))
			return mglApplyOperC(str.substr(0,n),str.substr(n+1),arg, head, hypotc,fns);
		else if(!nm.compare(L"real"))	return mglApplyFuncC(str, arg, head, realc,fns);
		else if(!nm.compare(L"imag"))	return mglApplyFuncC(str, arg, head, imagc,fns);
		else if(!nm.compare(L"norm"))	return mglApplyFuncC(str, arg, head, normc,fns);
		else if(!nm.compare(L"value"))
		{
			HADT a = mglFormulaCalcAC(str.substr(0,n), arg, head, fns);
			long n1 = mglFindInText(str,",",1), n2 = mglFindInText(str,",",2);
			HMDT ix, iy=NULL, iz=NULL;
			if(n1>0)
			{
				ix = mglFormulaCalcA(str.substr(n+1,n1), arg, head, fns);
				if(n2>0)
				{
					iy = mglFormulaCalcA(str.substr(n1+1,n2), arg, head, fns);
					iz = mglFormulaCalcA(str.substr(n2+1), arg, head, fns);
				}
				else	iy = mglFormulaCalcA(str.substr(n1+1), arg, head, fns);

			}
			else	ix = mglFormulaCalcA(str.substr(n+1), arg, head, fns);
			HADT res = mgl_datac_subdata_ext(a, ix,iy,iz);
			if(a)	mgl_delete_datac(a);
			if(ix)	mgl_delete_data(ix);
			if(iy)	mgl_delete_data(iy);
			if(iz)	mgl_delete_data(iz);
			return res;
		}
		else if(!nm.compare(L"dsum") && n>0)
		{
			HADT a=NULL;
			HMDT b=mglFormulaCalcA(str.substr(n+1), arg, head, fns);
			long m = long(b->a[0]+0.5);
			const char *s = head.size()>0?head[head.size()-1]->s.s:"";
			if(m>0)
			{
				std::vector<mglDataA*> hh(head);
				int in=0, zn=1;
				if(s[0]=='_' && s[1]>='i' && s[1]<'z') in = s[1]-'i'+1;
				char name[3] = {'_',char('i'+in),0};
				b->s = name;	b->Create(1,1,1);	hh.push_back(b);
				a = mglFormulaCalcAC(str.substr(0,n), arg, hh, fns);
				long nn = a->GetNN();
				for(long i=1;i<m;i++)
				{
					b->a[0] = i;	zn *= -1;
					HADT c = mglFormulaCalcAC(str.substr(0,n), arg, hh, fns);
					for(long j=0;j<nn;j++)	a->a[j] += mreal(zn)*c->a[j];
					mgl_delete_datac(c);
				}
				mgl_delete_data(b);
			}
			else
			{	a = new mglDataC;	a->a[0]=NAN;	}
			return a;
		}
		else if(!nm.compare(L"prod") && n>0)
		{
			HADT a=NULL;
			HMDT b=mglFormulaCalcA(str.substr(n+1), arg, head, fns);
			long m = long(b->a[0]+0.5);
			const char *s = head.size()>0?head[head.size()-1]->s.s:"";
			if(m>0)
			{
				std::vector<mglDataA*> hh(head);
				int in=0;
				if(s[0]=='_' && s[1]>='i' && s[1]<'z') in = s[1]-'i'+1;
				char name[3] = {'_',char('i'+in),0};
				b->s = name;	b->Create(1,1,1);	hh.push_back(b);
				a = mglFormulaCalcAC(str.substr(0,n), arg, hh, fns);
				long nn = a->GetNN();
				for(long i=1;i<m;i++)
				{
					b->a[0] = i;
					HADT c = mglFormulaCalcAC(str.substr(0,n), arg, hh, fns);
					for(long j=0;j<nn;j++)	a->a[j] *= c->a[j];
					mgl_delete_datac(c);
				}
				mgl_delete_data(b);
			}
			else
			{	a = new mglDataC;	a->a[0]=NAN;	}
			return a;
		}
		else if(nm[0]=='f' && nm[1]=='n' && fns.size()>1)
		{
			HADT a=NULL, b=NULL, r=NULL;
			std::vector<mglDataA*> hh(head);
			std::vector<std::wstring> tmp;	// disable recursion
			if(n>0)
			{
				a = mglFormulaCalcAC(str.substr(0,n), arg, head, fns);
				if(a)	{	a->s = "_1";	hh.push_back(a);	}
				b = mglFormulaCalcAC(str.substr(n+1), arg, head, fns);
				if(b)	{	b->s = "_2";	hh.push_back(b);	}
			}
			else
			{
				a = mglFormulaCalcAC(str, arg, head, fns);
				if(a)	{	a->s = "_1";	hh.push_back(a);	}
			}
			if(!nm.compare(L"fn1") && fns.size()>1)	r = mglFormulaCalcAC(fns[1], arg, hh, tmp);
			if(!nm.compare(L"fn2") && fns.size()>3)	r = mglFormulaCalcAC(fns[2], arg, hh, tmp);
			if(!nm.compare(L"fn3") && fns.size()>3)	r = mglFormulaCalcAC(fns[3], arg, hh, tmp);
			if(!nm.compare(L"fn4") && fns.size()>4)	r = mglFormulaCalcAC(fns[4], arg, hh, tmp);
			if(!nm.compare(L"fn5") && fns.size()>5)	r = mglFormulaCalcAC(fns[5], arg, hh, tmp);
			if(!nm.compare(L"fn6") && fns.size()>6)	r = mglFormulaCalcAC(fns[6], arg, hh, tmp);
			if(!nm.compare(L"fn7") && fns.size()>7)	r = mglFormulaCalcAC(fns[7], arg, hh, tmp);
			if(!nm.compare(L"fn8") && fns.size()>8)	r = mglFormulaCalcAC(fns[8], arg, hh, tmp);
			if(!nm.compare(L"fn9") && fns.size()>9)	r = mglFormulaCalcAC(fns[9], arg, hh, tmp);
			if(a)	mgl_delete_datac(a);
			if(b)	mgl_delete_datac(b);
			if(!r)	{	r = new mglDataC;	r->a[0]=NAN;	}
			return r;
		}
	}
	HADT res = new mglDataC;	res->a[0]=NAN;	return res;
}
//-----------------------------------------------------------------------------
