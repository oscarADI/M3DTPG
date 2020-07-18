/**CPPFile**************************************************************

  FileName    [main.cpp]

  SystemName  [LuLuTPG: M3D Test Pattern Generation.]

  PackageName [main: Main Function.]

  Synopsis    [Main]

  Author      [Shao-Chun Hung]
  
  Affiliation [Duke University]

  Date        [24, Feb., 2020.]

***********************************************************************/
#include "parser.hpp"

using namespace std;

int main(int argc, char** argv) {
	CCircuit circuit;
	CParser parse(circuit);

	cout << "Begin Parsing\n";
	parse.parseMerge(argv[1]);
	circuit.CountLogicGate();
	//parse.parseSTIL(argv[4]);
	//parse.partition(argv[2], argv[3]);
	cout << "End Parsing\n";

	cout << "Begin Levelize\n";
	//circuit.levelize();
	cout << "End Levelize\n";
	
	//circuit.assignInputs();

	cout << "Begin Test\n";
	//circuit.test(argv[4]);
	cout << "End Test2\n";

	cout << "Begin Validate\n";
	//circuit.testonly(argv[4]);
	cout << "End Validate\n";
//	circuit.renewPPIs();
//	circuit.evaluate();
//	circuit.printInputOutput();
//	circuit2.printInputOutput();
	

//	circuit.writeOutput();
	//parse.parsePattern();
	return 0;
}
