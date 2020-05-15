/**HeaderFile**********************************************************

  FileName    [parser.hpp]

  SystemName  [LuLuTPG: M3D Test Pattern Generation.]

  PackageName [parser: Parser for circuit.]

  Synopsis    [parser functions declaration]

  Author      [Shao-Chun Hung]
  
  Affiliation [Duke University]

  Date        [24, Feb., 2020.]

***********************************************************************/
#include "circuit.hpp"
#include <string>


class CParser {

public:
	CParser(CCircuit& cir) : _cir(cir) {};
	//CParser() {};
	~CParser() {};

	void parseVerilog();
	void parseMerge(string);
	void partition(string, string);
	void parseSTIL(string);
	void parsePattern();
	
private:
	void connect(CGate*, string, int);

	CCircuit& _cir;
};
