/**************************************************************************//**
 * @file cdf24_test.c
 * @version 0.99
 * @date jeu. mai 14 00:37:45 CEST 2009
 *
 * @author jf.argentino@osean.fr
 * @brief The cdf24 function test program.
 *
 * history: 
 *          -0.99: initial release
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "icdf24.h"

/** @addtogroup TEST */
/*@{*/

#define K_DEFAULT 6UL

/** @brief the CDF[2,4] test program */
int main (int argc, char** argv) {
   int n;
//   int hh;
   FILE* infile;
   FILE* outfile;
   size_t lx;
   size_t lx_temp;
//   int32_t x[CDF24_MAX_LENGTH];
//   int32_t* x;
   int32_t* xmine;
   char filename[32];
   char* str = argv[1];
   size_t K = K_DEFAULT;
   int n0 = 1;
   int normalized = 2;

   if (argc == 1) {
      printf ("%s [K] [n] file [files...]\n", argv[0]);
      printf ("\twhere K is the number of scales (%lu by default)\n",
              K_DEFAULT);
      printf ("\tand n is 0 for no normalization, 1 for normalization by sqrt of 2\n\t2 for normalization by constant 2; default - n=2\n");
      exit (EXIT_SUCCESS);
   }

   K = strtol (argv[n0], &str, 0);
   if (str == argv[n0]) {
      K = K_DEFAULT;
   } else {
      n0++;
   }
   if (strlen (argv[n0]) == 1) {
      switch (argv[n0][0]) {
	 case '0':
         normalized = 0;
         break;
//my addition
	 case '1':
         normalized = 1;
         break;
	 case '2':
	 normalized = 2;
	 break;
///////////////////////
         default:
         normalized = 2;
         break;
      }
      n0++;
   }

   for (n = n0; n < argc; n++)
   {//for #1 start

   /// block no know the number of points in the file
	xmine = (int32_t *) malloc(sizeof(int32_t)*CDF24_MAX_LENGTH);
   lx = 0;
      infile = fopen (argv[n0], "r");
      if (NULL == infile)
	 { printf ("Can't open file to read %s\n", argv[n]); }
       else
       {
         while ((lx_temp = fread (xmine, sizeof (int32_t), CDF24_MAX_LENGTH, infile))>0)
	 { lx = lx + lx_temp;}
	 printf("The number of points in the file %s is lx=%zu\n",argv[n],lx); 
	 free(xmine); // freeing the part occupied by a temporary array x
   /////////////////////////////////////////////////////////////

   // assigning the array for cdf24 transform of the length of the input file
	xmine = (int32_t *) malloc(sizeof(int32_t)*lx);
      if (NULL == xmine)
	 { printf ("Can't allocate memory space for signal from input file\n"); }
      else
	 { printf ("successfully allocated memory space for signal from file %s\n",argv[n]); }

//         fclose (infile);
//         infile = fopen (argv[n0], "r");
         rewind(infile);

	 lx = fread (xmine, sizeof(int32_t), lx, infile);
	 //printf("the number of points read lx=%zu\n",lx);
	 printf("normalization method  = %d\n",normalized);
	 printf("number of scales = %zu\n",K);
//	 for(hh=0;hh<16;hh++) {printf("hh = %d ; xmine[%d] = %d\n",hh,hh,xmine[hh]);}//this line is just to display the first values of the signal which was read
	//for(hh=lx-1;hh>(lx-11);hh--) {printf("hh = %d ; xmine[%d] = %d\n",hh,hh,xmine[hh]);}//this line is just to display the first values of the signal which was read
         if (icdf24 (xmine, lx, K, normalized) < 0) 
	     {printf ("%zu not a valid size\n", lx);}
	  else 
	  {//if #3 start
	     //printf ("successfully did cdf24 on xmine\n");
	//for(hh=0;hh<lx;hh++) {printf("hh = %d ; xmine[%d] = %d\n",hh,hh,xmine[hh]);}//this line is just to display the first values of the signal which was computed 
//      for(hh=lx-1;hh>(lx-11);hh--) {printf("hh = %d ; xmine[%d] = %d\n",hh,hh,xmine[hh]);}//this line is just to display the first values of the signal which was read
            sprintf (filename, "%s.icdf24_%zu", argv[n], K);
            outfile = fopen (filename, "w");
            if (NULL == outfile) 
			{printf ("Can't open file to write %s\n", filename);}
	     else
	     {//if #2 start
               fwrite (xmine, sizeof (int32_t), lx, outfile);
               fclose (outfile);
	  }
         }
         fclose (infile);
       }
}
   exit (EXIT_SUCCESS);
}
//abort();
////////////////////////////////////
// old part of the file cdf24.c; it is intact in cdf24.c_orig
//   for (n = n0; n < argc; n++)
//   {//for #1 start
//      infile = fopen (argv[n], "r");
//      if (NULL == infile)
//	 { printf ("Can't open file %s\n", argv[n]); }
//       else
//	 {lx = fread (x, sizeof (int32_t), CDF24_MAX_LENGTH, infile);//if #1 start
//         //lx = fread (x, sizeof (long), CDF24_MAX_LENGTH, infile);//orig line
//	printf("CDF24_MAX_LENGTH=%d and the number of points read lx=%zu\n",CDF24_MAX_LENGTH,lx);
//	printf("normalization = %d\n",normalized);
//	for(hh=0;hh<10;hh++) {printf("hh = %d ; x[%d] = %d\n",hh,hh,x[hh]);}//this line is just to display the first values of the signal which was read
////there is a problem with original version of cdf24_test.c-
//// the number of the elements read from the binary file created by matlab using presicion int32
//// is twice smaller, ie lx is twice smaller than the length of the original signal in the file
//// it seems that the problem is in the specification of the function sizeof in fread commande:
//// in cdf24_test, fread has sizeof(long), but x is declared as int32_t; when both sizeof statements
//// in fread and fwrite were changed to sizeof(int32_t), then everything seems to be ok; the signal is
////read in correctly and then written in correctly as well;
//// when the # of points in the input signal in a binary file is larger than the maximum number of points
//// specified in CDF24_MAX_LENGTH, than only this # of points is read, the reading stops after this number
//// of points is read
//         if (cdf24 (x, lx, K, normalized) < 0) 
//	     {printf ("%zu not a valid size\n", lx);}
//	  else 
//	  {//if #3 start
//            sprintf (filename, "%s.cdf24_%zu", argv[n], K);
//            outfile = fopen (filename, "w");
//            if (NULL == outfile) 
//			{printf ("Can't open file %s\n", filename);}
//	     else
//	     {//if #2 start
////		for(hh=0;hh<10;hh++) {printf("hh = %d ; x[%d] = %d\n",hh,hh,x[hh]);}
//               fwrite (x, sizeof (int32_t), lx, outfile);
//               //fwrite (x, sizeof (long), lx, outfile);//original line
//               fclose (outfile);
////#ifdef CDF24_PARANOID
////               printf ("%sCDF24 of file %s (%lu samples) in %s, "
////                       "overflow = %lu\n", normalized?"":"Unnormalized ",
////                       argv[n], lx, filename, CDF24_OVERFLOW);
////#else
////               printf ("%sCDF24 of file %s (%zu samples) in %s\n",
//                       normalized?"":"Unnormalized ", argv[n], lx, filename);
//#endif
// 	   }//if #2 end
//         }//if #3 end
//         fclose (infile);
//      }//if #1 end
//   }//for #1 end
//   exit (EXIT_SUCCESS);
//}

/*@}*/
