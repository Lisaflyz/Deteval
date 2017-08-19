/**********************************************************
 * Matrix_Inv.h
 * 
 * Matrix inversion
 **********************************************************/

#ifndef	_MATRIX__INV_H_
#define	_MATRIX__INV_H_

// From the main module
#include "CIL.h"

// From the compatibility module
#include "visualbug.h"

// From this module
#include "Vector.h"
#include "MatrixDef.h"

using namespace std;


/* ------------------------------------------------
       Code taken from the book
       NUMERICAL RECIPES IN C
       ------------------------------------------------ */
       
/*
#define TINY 1.0e-20

template <class T>
void ludcmp(Matrix<T> &A, int N, Vector<int> &INDX, float *D)
{
	int i,imax,j,k;
	float big,dum,sum,temp;
	Vector<T> VV(N);
	
	*D=1.0;
	for (i=0;i<N;i++) 
	{
		big=0.0;
		for (j=0;j<N;j++)
			if ((temp=fabs(A(i,j))) > big) 
				big=temp;
		if (big == 0.0) 
			ERR_THROW ("Singular matrix in routine ludcmp");
		VV[i]=1.0/big;
	}
	for (j=0;j<N;j++) 
	{
		for (i=0;i<j;i++) 
		{
			sum=A(i,j);
			for (k=0;k<i;k++) sum -= A(i,k)*A(k,j);
			A(i,j)=sum;
		}
		big=0.0;
		for (i=j;i<=N;i++) 
		{
			sum=A(i,j);
			for (k=0;k<j;k++)
				sum -= A(i,k)*A(k,j);
			A(i,j)=sum;
			if ((dum=VV[i]*fabs(sum)) >= big) 
			{
				big=dum;
				imax=i;
			}
		}
		if (j != imax) 
		{
			for (k=0;k<N;k++) 
			{
				dum=A(imax,k);
				A(imax,k)=A(j,k);
				A(j,k)=dum;
			}
			*D = -(*D);
			VV[imax]=VV[j];
		}
		INDX[j]=imax;
		if (A(j,j) == 0.0) A(j,j)=TINY;
		if (j != N) 
		{
			dum=1.0/(A(j,j));
			for (i=j+1;i<N;i++) A(i,j) *= dum;
		}
	}
	
}

template <class T>
void lubksb(Matrix<T> &A, int N, Vector<int> &INDX, Vector<T> &B)
{
	int i,ii=0,ip,j;
	float sum;

	for (i=0;i<N;i++) 
	{
		ip=INDX[i];
		sum=B[ip];
		B[ip]=B[i];
		if (ii)
			for (j=ii;j<=i-1;j++) 
				sum -= A(i,j)*B[j];
		else 
			if (sum) 
				ii=i;
		B[i]=sum;
	}
	for (i=N;i>=0;i--) 
	{
		sum=B[i];
		for (j=i+1;j<N;j++) 
			sum -= A(i,j)*B[j];
		B[i]=sum/A(i,i);
	}
}

*/

template <class T>
void Matrix<T>::invert()  
{
    if (noColumns < 1) 
		return;  

    if (noColumns != noRows)
    	ERR_THROW ("Internal Error in Matrix<T>::invert(): Matrix is not square!!!\n");
    	
    // The trivial case: a scalar matrix
    if (noColumns==1)
    {
    	set(0,0,1.0/get(0,0));
    	return;
    }
    	
/*    	
    int N=rows();
    Matrix<T> y(N,N);
    Vector<int> INDX (N);
    Vector<T> COL(N);
    float d;
    
    ludcmp(*this,N,INDX,&d);
    for (int j=0; j<N; ++j)
    {
    	for (int i=0; i<N; ++i)
    		COL[i]=0.0;
    	COL[j]=1.;
    	lubksb(*this,N,INDX,COL);
    	for (int i=0; i<N; ++i)
    		y(i,j)=COL[i];
    }
    *this = y;
    	
*/    	    
              
       // Code taken from Mike Dinolfo
	   // http://users.erols.com/mdinolfo/matrix.htm
    	
	// normalize row 0
    for (unsigned int i=1; i < noColumns; i++)
    	data[i][0] /= data[0][0]; 
    
    for (unsigned int i=1; i < noColumns; i++)  
	{
      for (unsigned int j=i; j < noColumns; j++)  
	  { // do a column of L
        T sum = 0.0;
        for (unsigned int k = 0; k < i; k++)
            sum += data[k][j] * data[i][k];
        data[i][j] -= sum;
        }
      if (i == noColumns-1) continue;
      for (unsigned int j=i+1; j < noColumns; j++)  {  // do a row of U
        T sum = 0.0;
        for (unsigned int k = 0; k < i; k++)
            sum += data[k][i]*data[j][k];
        data[j][i] =
           (data[j][i]-sum) / data[i][i];
        }
      }
    for (unsigned int i = 0; i < noColumns; i++ )  // invert L
      for (unsigned int j = i; j < noColumns; j++ )  {
        T x = 1.0;
        if ( i != j ) {
          x = 0.0;
          for (unsigned int k = i; k < j; k++ )
              x -= data[k][j]*data[i][k];
          }
        data[i][j] = x / data[j][j];
        }
    for (unsigned int i = 0; i < noColumns; i++ )   // invert U
      for (unsigned int j = i; j < noColumns; j++ )  {
        if ( i == j ) continue;
        T sum = 0.0;
        for (unsigned int k = i; k < j; k++ )
            sum += data[j][k]*( (i==k) ? 1.0 : data[k][i]);
        data[j][i] = -sum;
        }
    for (unsigned int i = 0; i < noColumns; i++ )   // final inversion
      for (unsigned int j = 0; j < noColumns; j++ )  {
        T sum = 0.0;
        for (unsigned int k = ((i>j)?i:j); k < noColumns; k++ )
            sum += ((j==k)?1.0:data[k][j])*data[i][k];
        data[i][j] = sum;
        }
        
}

#endif

