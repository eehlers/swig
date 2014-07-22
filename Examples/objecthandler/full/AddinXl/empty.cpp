#include <windows.h>
#include "xlcall.h"
#include "framewrk.hpp"
#include <fstream>
using namespace std;

#define g_rgWorksheetFuncsRows 2
#define g_rgWorksheetFuncsCols 10

static LPSTR g_rgWorksheetFuncs
                [g_rgWorksheetFuncsRows][g_rgWorksheetFuncsCols] =
{
{" Junk2", " EPP", " Junk2", " Arg 1, Arg 2", " 1", " Generic Add-In", " ", " ", " Returns the product of the numbers", " "},
{" Junk3", " A"  , " Junk3", " ",             " 2", " Generic Add-In", " ", " ", " Returns the product of the numbers", " "},
};

__declspec(dllexport) int WINAPI Junk3() {
	ofstream log1;
	log1.open("c:/x.txt", ios::out);
	log1 << "000" << endl;
	log1 << "900" << endl;
	return 0;
}

__declspec(dllexport) int WINAPI xlAutoOpen(void) {
  static XLOPER xDLL;
  Excel(xlGetName, &xDLL, 0);
  for (int i=0; i<g_rgWorksheetFuncsRows; i++) {
		Excel(xlfRegister, 0, 1+ g_rgWorksheetFuncsCols,
	    	(LPXLOPER) &xDLL,
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][0]),
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][1]),
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][2]),
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][3]),
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][4]),
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][5]),
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][6]),
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][7]),
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][8]),
	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][9]));
    }
  return 1;
}

__declspec(dllexport) int WINAPI xlAutoClose(void){
    return 1;
}

double* __stdcall Junk2( LPXLOPER x1, LPXLOPER x2) {
	static double dResult;
	if ( x1->xltype==xltypeNum && x2->xltype==xltypeNum) {
		// x1 and x2 are numbers
		dResult = x1->val.num * x2->val.num;
		return &dResult;
	} else {
		//x1 and x2 are not both numbers
		return 0; // returns #NUM! to excel
	}
}