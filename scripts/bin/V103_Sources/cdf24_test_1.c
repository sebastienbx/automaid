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
#include "cdf24.h"

/** @addtogroup TEST */
/*@{*/

#define K_DEFAULT 6UL

/** @brief the CDF[2,4] test program */
int main (int argc, char** argv) {
   int n;
   FILE* infile;
   FILE* outfile;
   size_t lx;
   int32_t x[CDF24_MAX_LENGTH];
   char filename[32];
   char* str = argv[1];
   size_t K = K_DEFAULT;
   int n0 = 1;
   int normalized = 2;

   if (argc == 1) {
      printf ("%s [K] [n] file [files...]\n", argv[0]);
      printf ("\twhere K is the number of scales (%lu per default)\n",
              K_DEFAULT);
      printf ("\tand n is 0 for no normalization, 1 normalization by sqrt of 2\n\t2 normalization by constant 2 normalization by constant 2; default - n=2\n");
      exit (EXIT_SUCCESS);
   }

	printf("argv[n0]= %s\n",argv[n0]);
   K = strtol (argv[n0], &str, 0);
   if (str == argv[n0]) {
      K = K_DEFAULT;
   } else {
      n0++;
   }
   if (strlen (argv[n0]) == 1) {
      switch (argv[n0][0]) {
//orig lines
//         case 'u':
//         case 'U':
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
      infile = fopen (argv[n], "r");
      if (NULL == infile)
	 { printf ("Can't open file %s\n", argv[n]); }
       else
	 {lx = fread (x, sizeof (int32_t), CDF24_MAX_LENGTH, infile);//if #1 start
         //lx = fread (x, sizeof (long), CDF24_MAX_LENGTH, infile);//orig line
	printf("CDF24_MAX_LENGTH=%d and the number of points read lx=%zu\n",CDF24_MAX_LENGTH,lx);
	printf("normalization = %d\n",normalized);
//	for(hh=0;hh<10;hh++) {printf("hh = %d ; x[%d] = %d\n",hh,hh,x[hh]);}//this line is just to display the first values of the signal which was read
//there is a problem with original version of cdf24_test.c-
// the number of the elements read from the binary file created by matlab using presicion int32
// is twice smaller, ie lx is twice smaller than the length of the original signal in the file
// it seems that the problem is in the specification of the function sizeof in fread commande:
// in cdf24_test, fread has sizeof(long), but x is declared as int32_t; when both sizeof statements
// in fread and fwrite were changed to sizeof(int32_t), then everything seems to be ok; the signal is
//read in correctly and then written in correctly as well;
// when the # of points in the input signal in a binary file is larger than the maximum number of points
// specified in CDF24_MAX_LENGTH, than only this # of points is read, the reading stops after this number
// of points is read
         if (cdf24 (x, lx, K, normalized) < 0) 
	     {printf ("%zu not a valid size\n", lx);}
	  else 
	  {//if #3 start
            sprintf (filename, "%s.cdf24_%zu", argv[n], K);
            outfile = fopen (filename, "w");
            if (NULL == outfile) 
			{printf ("Can't open file %s\n", filename);}
	     else
	     {//if #2 start
//		for(hh=0;hh<10;hh++) {printf("hh = %d ; x[%d] = %d\n",hh,hh,x[hh]);}
               fwrite (x, sizeof (int32_t), lx, outfile);
               //fwrite (x, sizeof (long), lx, outfile);//original line
               fclose (outfile);
//#ifdef CDF24_PARANOID
//               printf ("%sCDF24 of file %s (%lu samples) in %s, "
//                       "overflow = %lu\n", normalized?"":"Unnormalized ",
//                       argv[n], lx, filename, CDF24_OVERFLOW);
//#else
//               printf ("%sCDF24 of file %s (%zu samples) in %s\n",
//                       normalized?"":"Unnormalized ", argv[n], lx, filename);
//#endif
 	   }//if #2 end
         }//if #3 end
         fclose (infile);
      }//if #1 end
   }//for #1 end
   exit (EXIT_SUCCESS);
}

/*@}*/
