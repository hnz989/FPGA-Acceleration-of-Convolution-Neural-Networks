
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "testdata.h"

//float conv2d1(float xx[N][RP][RC], float WW[M][N][K*K], float BB[M], float yy0[M][R][C], float* bram_x0, float* bram_x1,
              //float* bram_w00, float* bram_w01, float* bram_w10, float* bram_w11, float* bram_y0, float* bram_y1, int* hw, float* hw_float)
float conv2d1(float *xx, float *WW, float *BB, float *yy0, float* bram_x0, float* bram_x1,
              float* bram_w00, float* bram_w01, float* bram_w10, float* bram_w11, float* bram_y0, float* bram_y1, int* hw, float* hw_float)
{
   int k=0, j=0;
   int r, c, n, m, p, q;
   int Tn=2,Tm=2;

    for(int m=0; m<M; m++)
	{
		for(int r=0; r<R; r++)
		{
			for(int c=0; c<C; c++)
			{
				 yy0[m][r][c] = 0;
			}
		}
	}
   for(r=0; r<R; r+=Tr)
   {
	   for(c=0; c<C; c+=Tc)
	   {
		   for(m=0; m<M; m+=Tm)
		   {
			   for(p=0; p<Tr*Tc; p++)
			   {
				   bram_y0[p] = 0;
				   bram_y1[p] = 0;
				   //printf("Y:%d O:%d \n", bram_y[p],o);
				   //o++;bram_y0[p] = 0;
			   }

			   for(n=0; n<N; n+=(Tn+1))
			   {
				   for(k=0; k<K*K; k++)
				   {
					   bram_w00[k] = 0;
					   bram_w01[k] = 0;
					   bram_w10[k] = 0;
					   bram_w11[k] = 0;
				   }
				   for(q=0; q<TcPrime*TrPrime;q++)
				   {
					   bram_x0[q] = 0;
					   bram_x1[q] = 0;
					   //printf("%d W U %d value\n", bram_x[q],q);
				   }
				   j=0;
				   for(int rr=0; rr<TrPrime; rr++)
				   {
					   for(int cc=0; cc<TcPrime; cc++)
					   {
						   if((r*S+rr < RP) && (c*S+cc < RC))
						   {
							   bram_x0[rr*TrPrime+cc]=xx[n][r*S+rr][c*S+cc];
							   bram_x1[rr*TrPrime+cc]=xx[n+1][r*S+rr][c*S+cc];
							   //u = r*S+rr;
							   //o = c*S+cc;
							   //z = rr*TrPrime+cc;
							   //printf("R:%d C:%d N:%d M:%d X:%d value Z:%d \n", u,o,n,m,x[n][r*S+rr][c*S+cc],z);
							   //printf("R:%d C:%d N:%d M:%d X:%f value Z:%d \n", u,o,n,m,bram_x0[rr*TrPrime+cc],z);
							   //printf("R:%d C:%d N:%d M:%d X:%f value Z:%d \n", u,o,n,m,bram_x1[rr*TrPrime+cc],z);
						   }
					   }
				   }
				   for(k=0; k<K*K; k++)
				   {
					   bram_w00[j] = WW[m][n][k];
					   bram_w01[j] = WW[m][n+1][k];
					   bram_w10[j] = WW[m+1][n][k];
					   bram_w11[j] = WW[m+1][n+1][k];
					   //printf("K:%d N:%d M:%d W:%f value\n", k,n,m,W[m][n][k]);
					   j++;
				   }
				   if(n==0)
				   {
					   hw_float[1] = BB[m];
					   hw_float[2] = BB[m+1];
					   //printf("B1 Value: %f \n",hw[1]);
					   //printf("B2 Value: %f \n",hw[2]);
				   }
				   else
				   {
					   hw[1] = 0;
					   hw[2] = 0;
					   //printf("B Value: %d \n",hw[1]);
				   }

					// Assert start signal
				   hw[3] = K;hw[4] = TrPrime;hw[5] = TcPrime;hw[6] = Tr;hw[7] = Tc;hw[8] = S;
				   hw[0] = 1;

					// Wait for done signal
					while ( hw[9]== 0)
					{
						;
					}

					// Deassert start signal
						hw[0] = 0;
						for(int rr=0; rr<Tr; rr++)
						{
						   for(int cc=0; cc<Tc; cc++)
						   {
							   if(r+rr<R && c+cc<C)
							   {
								   yy0[m][r+rr][c+cc]=bram_y0[rr*Tr+cc];
								   yy0[m+1][r+rr][c+cc]=bram_y1[rr*Tr+cc];

								   //u = r+rr;
								   //o = c+cc;
								   //printf("Bram1:%f N:%d M:%d value\n", y[m][r+rr][c+cc], n, m);
								   //printf("Bram2:%f N:%d M:%d value\n", y[m+1][r+rr][c+cc], n, (m+1));
							   }
						   }
						}

					  while (hw[9] == 1)
					  {
							;
					   }
			   }
		   }
	   }
	}

	return yy0[M][R][C];
}

float ReLU1(float y[M][R][C], float y0[M][R][C])
{
	for(int m=0; m<M; m++)
	{
		for(int r=0; r<R; r++)
		{
			for(int c=0; c<C; c++)
			{
				y0[m][r][c] = (y[m][r][c]>=0) ? y[m][r][c] : 0;
			}
		}
	}
	return y0[M][R][C];
}

//float conv2d2(float xx1[M][R/2][C/2], float WW1[M2][M][K*K], float BB1[M2], float yy1[M2][R2][C2], float* bram_x0, float* bram_x1,
              //float* bram_w00, float* bram_w01, float* bram_w10, float* bram_w11, float* bram_y0, float* bram_y1, int* hw, float* hw_float)
float conv2d2(float *xx2, float *WW2, float *BB2, float *yy1, float* bram_x0, float* bram_x1,
              float* bram_w00, float* bram_w01, float* bram_w10, float* bram_w11, float* bram_y0, float* bram_y1, int* hw, float* hw_float)
{
   int k=0, j=0;
   int r, c, n, m, p, q;
   int Tn=2,Tm=2;

   for(int m=0; m<M2; m++)
   {
		for(int r=0; r<R2; r++)
		{
			for(int c=0; c<C2; c++)
			{
				 yy1[m][r][c] = 0;
			}
		}
	}
   for(r=0; r<R2; r+=Tr)
   {
	   for(c=0; c<C2; c+=Tc)
	   {
		   for(m=0; m<M2; m+=Tm)
		   {
			   for(p=0; p<Tr*Tc; p++)
			   {
				   bram_y0[p] = 0;
				   bram_y1[p] = 0;
				   //printf("Y:%d O:%d \n", bram_y[p],o);
				   //o++;bram_y0[p] = 0;
			   }
			   for(n=0; n<M; n+=(Tn+1))
			   {
				   for(k=0; k<K*K; k++)
				   {
					   bram_w00[k] = 0;
					   bram_w01[k] = 0;
					   bram_w10[k] = 0;
					   bram_w11[k] = 0;
				   }
				   for(q=0; q<TcPrime*TrPrime;q++)
				   {
					   bram_x0[q] = 0;
					   bram_x1[q] = 0;
					   //printf("%d W U %d value\n", bram_x[q],q);
				   }
				   j=0;
				   for(int rr=0; rr<TrPrime; rr++)
				   {
					   for(int cc=0; cc<TcPrime; cc++)
					   {
						   if((r*S+rr < R/2) && (c*S+cc < C/2))
						   {
							   bram_x0[rr*TrPrime+cc]=xx2[n][r*S+rr][c*S+cc];
							   bram_x1[rr*TrPrime+cc]=xx2[n+1][r*S+rr][c*S+cc];
							   //u = r*S+rr;
							   //o = c*S+cc;
							   //z = rr*TrPrime+cc;
							   //printf("R:%d C:%d N:%d M:%d X:%d value Z:%d \n", u,o,n,m,x[n][r*S+rr][c*S+cc],z);
							   //printf("R:%d C:%d N:%d M:%d X:%f value Z:%d \n", u,o,n,m,bram_x0[rr*TrPrime+cc],z);
							   //printf("R:%d C:%d N:%d M:%d X:%f value Z:%d \n", u,o,n,m,bram_x1[rr*TrPrime+cc],z);
						   }
					   }
				   }
				   for(k=0; k<K*K; k++)
				   {
					   bram_w00[j] = WW2[m][n][k];
					   bram_w01[j] = WW2[m][n+1][k];
					   bram_w10[j] = WW2[m+1][n][k];
					   bram_w11[j] = WW2[m+1][n+1][k];
					   //printf("K:%d N:%d M:%d W:%f value\n", k,n,m,W[m][n][k]);
					   j++;
				   }
				   if(n==0)
				   {
					   hw_float[1] = BB2[m];
					   hw_float[2] = BB2[m+1];
					   //printf("B1 Value: %f \n",hw[1]);
					   //printf("B2 Value: %f \n",hw[2]);
				   }
				   else
				   {
					   hw[1] = 0;
					   hw[2] = 0;
					   //printf("B Value: %d \n",hw[1]);
				   }

					// Assert start signal
				   hw[3] = K;hw[4] = TrPrime;hw[5] = TcPrime;hw[6] = Tr;hw[7] = Tc;hw[8] = S;
				   hw[0] = 1;

					// Wait for done signal
					while ( hw[9]== 0)
					{
						;
					}

					// Deassert start signal
						hw[0] = 0;
						for(int rr=0; rr<Tr; rr++)
						{
						   for(int cc=0; cc<Tc; cc++)
						   {
							   if(r+rr<R2 && c+cc<C2)
							   {
								   yy1[m][r+rr][c+cc]=bram_y0[rr*Tr+cc];
								   yy1[m+1][r+rr][c+cc]=bram_y1[rr*Tr+cc];
								   //u = r+rr;
								   //o = c+cc;
								   //printf("Bram1:%f N:%d M:%d value\n", y[m][r+rr][c+cc], n, m);
								   //printf("Bram2:%f N:%d M:%d value\n", y[m+1][r+rr][c+cc], n, (m+1));
							   }
						   }
						}

					  while (hw[9] == 1)
					  {
							;
					   }
			   }
		   }
	   }
	}

	return yy1[M2][R2][C2];
}

float MaxPooling1(float y[M][R][C], float y1[M][R/2][C/2])
{
	float max, max_00, max_01;
	for(int m=0; m<M; m++)
	{
		for(int r=0; r<R; r=r+2)
		{
			for(int c=0; c<C; c=c+2)
			{
			     max_00 = (y[m][r][c] > y[m][r][c+1]) ? y[m][r][c] : y[m][r][c+1];
				 max_01 = (y[m][r+1][c] > y[m][r+1][c+1]) ? y[m][r+1][c] : y[m][r+1][c+1];
                 max = (max_00 > max_01) ? max_00 : max_01;
				 y1[m][r/2][c/2] = max;
			}
		}
	}
    return y1[M][R/2][C/2];
}

float ReLU2(float y[M2][R2][C2], float y0[M2][R2][C2])
{
	for(int m=0; m<M2; m++)
	{
		for(int r=0; r<R2; r++)
		{
			for(int c=0; c<C2; c++)
			{
				y0[m][r][c] = (y[m][r][c]>=0) ? y[m][r][c] : 0;
			}
		}
	}
	return y0[M2][R2][C2];
}

float MaxPooling2(float y[M2][R2][C2], float y1[M2][R2/2][C2/2])
{
	float max, max_00, max_01;
	for(int m=0; m<M2; m++)
	{
		for(int r=0; r<R2; r=r+2)
		{
			for(int c=0; c<C2; c=c+2)
			{
			     max_00 = (y[m][r][c] > y[m][r][c+1]) ? y[m][r][c] : y[m][r][c+1];
				 max_01 = (y[m][r+1][c] > y[m][r+1][c+1]) ? y[m][r+1][c] : y[m][r+1][c+1];
                 max = (max_00 > max_01) ? max_00 : max_01;
				 y1[m][r/2][c/2] = max;
			}
		}
	}
    return y1[M2][R2/2][C2/2];
}

int verify_results(float expectedy[M2][R2/2][C2/2] , float y[M2][R2/2][C2/2] ) {
   int errors=0; int right=0;

   for (int m=0; m<M2; m++)
   {
      for (int r=0; r<(R2/2); r++)
      {
    	  for(int c=0; c<(C2/2); c++)
    	  {
    		 if ((expected[m][r][c]+1)>=y[m][r][c] && y[m][r][c]>=(expected[m][r][c]-1))
    		 {
                 right++;
    		 }
    		 else
    		 {
    			 errors++;
    		 }
    	  }
      }
   }
   return errors;
}

int main() {
   init_platform();
   volatile float* bram_x0 = (float*)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR;
   volatile float* bram_x1 = (float*)XPAR_AXI_BRAM_CTRL_1_S_AXI_BASEADDR;
   volatile float* bram_w00 = (float*)XPAR_AXI_BRAM_CTRL_2_S_AXI_BASEADDR;
   volatile float* bram_w01 = (float*)XPAR_AXI_BRAM_CTRL_3_S_AXI_BASEADDR;
   volatile float* bram_w10 = (float*)XPAR_AXI_BRAM_CTRL_4_S_AXI_BASEADDR;
   volatile float* bram_w11 = (float*)XPAR_AXI_BRAM_CTRL_5_S_AXI_BASEADDR;
   volatile float* bram_y0 = (float*)XPAR_AXI_BRAM_CTRL_6_S_AXI_BASEADDR;
   volatile float* bram_y1 = (float*)XPAR_AXI_BRAM_CTRL_7_S_AXI_BASEADDR;
   volatile int* hw = (int*)XPAR_CONV_11_0_S00_AXI_BASEADDR;
   float* hw_float = (float*)hw;
   // This matrix will hold the M output values for each of the C testcases.
   float y1[M][R][C];
   float y2[M][R][C];
   float y3[M][R/2][C/2];
   float y4[M2][R2][C2];
   float y5[M2][R2][C2];
   float y[M2][R2/2][C2/2];

   for(int m=0; m<M2; m++)
	{
		for(int r=0; r<R2/2; r++)
		{
			for(int c=0; c<C2/2; c++)
			{
				 y[m][r][c] = 0;
			}
		}
	}

//-------------------------------------First Convolution Layer-------------------------------------------------------------------------------------
   conv2d1(x, W, Bia, y1, bram_x0, bram_x1, bram_w00, bram_w01, bram_w10, bram_w11, bram_y0, bram_y1, hw, hw_float);
   ReLU1(y1, y2);
   MaxPooling1(y2, y3);
//-------------------------------------Second Convolution Layer-------------------------------------------------------------------------------------
   conv2d2(y3, W2, Bia2, y4, bram_x0, bram_x1, bram_w00, bram_w01, bram_w10, bram_w11, bram_y0, bram_y1, hw, hw_float);
   ReLU2(y4, y5);
   MaxPooling2(y5, y);

    // check results
	int errors = verify_results(expected, y);
	printf(" After two convolution: %d errors\n", errors);
	int accuracy = ((M2*(R2/2)*(C2/2)) - errors)/(M2*(R2/2)*(C2/2)) ;
	printf(" Total Accuracy: %d errors\n", accuracy);

	cleanup_platform();

}
