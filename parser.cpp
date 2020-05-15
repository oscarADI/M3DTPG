/**CPPFile**************************************************************

  FileName    [parser.cpp]

  SystemName  [LuLuTPG: M3D Test Pattern Generation.]

  PackageName [parser: Parser for circuit.]

  Synopsis    [parser functions definition]

  Author      [Shao-Chun Hung]
  
  Affiliation [Duke University]

  Date        [24, Feb., 2020.]

***********************************************************************/

#include <numeric>
#include <fstream>
#include <iostream>
#include <cassert>
//#include <string>
#include "parser.hpp"
//#include "circuit.hpp"
//#include "misc/util.hpp"
//#include "geo/interval_tree.hpp"
//#include <boost/functional/hash.hpp>
//#include <spdlog/spdlog.h>
//#include <spdlog/sinks/stdout_color_sinks.h>

using namespace std;

void CParser::parseMerge(string filename) {
	ifstream file(filename);
	string c;
	string name;
	string gate;

	//Input
	while(getline(file,c)) {
		if(c.find("input") != string::npos) break; 
	}
	while(1) {
		if(c.find("output") != string::npos) break;
		size_t first = 0;
		size_t last = 0;
		while(1) {
			first = c.find_first_not_of(" ", last+1);
			if(first == string::npos) break;
			last = c.find(",", first);
			if(last == string::npos) {
				if(c.find(";")==string::npos) break;
				else last = c.find(";", first);
			}
			name = c.substr(first, last-first);
			if(name.find(" ") != string::npos) name = name.substr(name.find(" ")+1); // remove input
			CWire* newWire = new CWire(name, 0);
			_cir.addWire(newWire, name);
			if(last == c.size()-1) break;
		}
		getline(file,c);
	}
	//Output
	while(1) {
		if(c.find("wire") != string::npos) break;
		size_t first = 0;
		size_t last = 0;
		while(1) {
			first = c.find_first_not_of(" ", last+1);
			if(first == string::npos) break;
			last = c.find(",", first);
			if(last == string::npos) {
				if(c.find(";")==string::npos) break;
				else last = c.find(";", first);
			}
			
			name = c.substr(first, last-first);
			if(name.find(" ") != string::npos) name = name.substr(name.find(" ")+1); // remove input
			CWire* newWire = new CWire(name, 1);
			_cir.addWire(newWire, name);
			if(last == c.size()-1) break;
			//break;
		}
		getline(file,c);
	}
	
	//Wire
	c = c.substr(c.find("wire")+4);
	bool finish = false;
	while(1) {
		if(c.find("assign") != string::npos) break;
		//if(c.find(";") != string::npos) finish = true;
		size_t first = 0;
		size_t last = 0;
		while(1) {
			first = c.find_first_not_of(" ", last+1);
			if(first == string::npos) break;
			last = c.find(",", first);
			if(last == string::npos) {
				if(c.find(";")==string::npos) break;
				else last = c.find(";", first);
			}
			
			name = c.substr(first, last-first);
			name = name.substr(name.find_first_not_of(" "), name.find_last_not_of(" ")-name.find_first_not_of(" ")+1);
			assert(name[0] != ' ' && name[name.size()-1]!= ' ');
			CWire* newWire = new CWire(name, 2);
			_cir.addWire(newWire, name);
			if(last == c.size()-1) break;
			//break;
		}
		getline(file,c);
		if(finish) break;
	}
//	CWire* newWire0 = new CWire("1'b0", 2);
//	_cir.addWire(newWire0, "1'b0");
//	newWire0->setValue(0);
//	CWire* newWire1 = new CWire("1'b1", 2);
//	_cir.addWire(newWire1, "1'b1");
//	newWire1->setValue(1);
	
	//ENDWIRE
	
	//Assign
	while(1) {
		if(c.find("assign") == string::npos) break;
		size_t first1 = 9;
		size_t last1 = c.find("=")-1;
		size_t first2 = last1+2;
		size_t last2 = c.find(";");

		string name1 = c.substr(first1, last1-first1);
		string name2 = c.substr(first2, last2-first2);		 
		CGate* newGate = new CGate("Dummy");
		newGate->addName("Dummy"+name2);
		newGate->setDie(0);
		_cir.addGate(newGate);
		connect(newGate, name2, 0);
		connect(newGate, name1, 1);

		getline(file,c);
	}

	//Gate
	gate.clear();
	while(getline(file,c)) {
		if(c.find("endmodule") != string::npos) break;
		size_t first = c.find_first_not_of(" ");
		size_t last = c.find_last_not_of(" ");
		gate += c.substr(first, last-first+1);
		if(gate.find(";") == string::npos) continue;

		if(gate.substr(0,4) == "SDFF") {
			CGate* newGate = new CGate("SDFF");
			first = gate.find(" ") + 1;
			last = gate.find(" ", first+1);
			newGate->addName(gate.substr(first,last-first));
			_cir.addGate(newGate);

			// D
			first = gate.find(".D",last+1) + 3;
			last = gate.find(")", first+1);
			connect(newGate, gate.substr(first, last-first), 0);

			// Q
			first = gate.find(".Q(") + 3;
			last = gate.find(")", first+1);
			connect(newGate, gate.substr(first, last-first), 1);
			if(gate.find(".QN") != string::npos) {
				first = gate.find(".QN(",last+1) + 4;
				last = gate.find(")", first+1);
				name = gate.substr(first, last-first);
				name = name.substr(name.find_first_not_of(" "), name.find_last_not_of(" ")-name.find_first_not_of(" ")+1);
				CWire* w = _cir.getWireFromName(name);
				if(!w) {cout << name << "is not found!" << endl; assert(false);}
				w->addFaninGate(newGate);
				newGate->addQN(w);
			}
		} // END SDFF
			
		else if(gate.substr(0,2) == "FA") {
			//assert(gate.find(".S") != string::npos);
			CGate* newGate = new CGate("FA");
			//Name
			first = gate.find(" ") + 1;
			last = gate.find(" ", first+1);
			newGate->addName(gate.substr(first,last-first));
			_cir.addGate(newGate);
	
			//Fanin
			first = gate.find("(") + 1;
			for(int i = 0; i < 5; ++i) {
				first = gate.find("(", first) + 1;
				last = gate.find(")", first);
				if(i < 3) connect(newGate, gate.substr(first,last-first), 0);
				else if(i == 3) {
					name = gate.substr(first, last-first);
					name = name.substr(name.find_first_not_of(" "), name.find_last_not_of(" ")-name.find_first_not_of(" ")+1);
					connect(newGate, name, 1);
				}
				else {
					name = gate.substr(first, last-first);
					name = name.substr(name.find_first_not_of(" "), name.find_last_not_of(" ")-name.find_first_not_of(" ")+1);
					CWire* w = _cir.getWireFromName(name);
					if(!w) {cout << name << "is not found!" << endl; assert(false);}
					w->addFaninGate(newGate);
					newGate->addQN(w);
				}
			}
		}	

		// Else
		else {
			CGate* newGate = new CGate();
			if(gate.substr(0,3) == "AND") 				newGate->addType("AND");
			else if(gate.substr(0,2) == "OR") 		newGate->addType("OR");
			else if(gate.substr(0,4) == "NAND") 	newGate->addType("NAND");
			else if(gate.substr(0,3) == "NOR") 		newGate->addType("NOR");
			else if(gate.substr(0,6) == "AOI21_") 	newGate->addType("AOI21");
			else if(gate.substr(0,6) == "AOI22_") 	newGate->addType("AOI22");
			else if(gate.substr(0,6) == "AOI211") newGate->addType("AOI211");
			else if(gate.substr(0,6) == "AOI221") newGate->addType("AOI221");
			else if(gate.substr(0,6) == "AOI222") newGate->addType("AOI222");
			else if(gate.substr(0,6) == "OAI21_") 	newGate->addType("OAI21");
			else if(gate.substr(0,6) == "OAI22_") 	newGate->addType("OAI22");
			else if(gate.substr(0,6) == "OAI211") newGate->addType("OAI211");
			else if(gate.substr(0,6) == "OAI221") newGate->addType("OAI221");
			else if(gate.substr(0,6) == "OAI222") newGate->addType("OAI222");
			else if(gate.substr(0,5) == "OAI33")  newGate->addType("OAI33");
			else if(gate.substr(0,3) == "MUX") 	 	newGate->addType("MUX");
			else if(gate.substr(0,3) == "XOR")  	newGate->addType("XOR");
			else if(gate.substr(0,4) == "XNOR") 	newGate->addType("XNOR");
			else if(gate.substr(0,3) == "INV") 		newGate->addType("INV");
			else if(gate.substr(0,3) == "BUF") 		newGate->addType("BUF");
			else if(gate.substr(0,6) == "CLKBUF") newGate->addType("CLKBUF");
			else cout << gate << endl;
		//	assert(gate.find(".Z") != string::npos);

			//Name
			first = gate.find(" ") + 1;
			last = gate.find(" ", first+1);
			newGate->addName(gate.substr(first,last-first));
			_cir.addGate(newGate);

			//Fanout
		//	size_t out = gate.find("(", gate.find(".Z")) + 1;
		//	last = gate.find(")", out);
		//	connect(newGate, gate.substr(out, last-out), 1);

			//Fanin
			size_t out = gate.find(".Z");
			assert(out != string::npos);
			first = gate.find("(") + 1;
			while(1) {
				first = gate.find("(", first) + 1;
				last = gate.find(")", first);
				if(first < out)
					connect(newGate, gate.substr(first, last-first), 0);
				else {
					connect(newGate, gate.substr(first, last-first), 1);
					break;
				}
			} 
		}		
		gate.clear();
	}
	file.close();	
}
void CParser::parseVerilog() {
	//_cir.setName("s27"); // temporary
	
	//FILE* fin = fopen("s27_syn_dft.v", "r");
	////cout << "Enter the name of an existing verilog file: ";
	//string filename;
	//cin >> 	filename;
	ifstream file("ldpc_syn_dft.v");
	//ifstream file("s27_syn_dft.v");	

	string c;
	string name;
	//INPUT for LDPC
	CWire* Reset = new CWire("reset", 0);
	//_cir.addPi(Reset, "reset");
	_cir.addWire(Reset, "reset");
	CWire* Clock = new CWire("clock", 0);
	//_cir.addPi(Clock, "clock");
	_cir.addWire(Clock, "clock");
	CWire* Load = new CWire("load", 0);
	//_cir.addPi(Load, "load");
	_cir.addWire(Load, "load");
	for(int i = 2047; i >= 0; --i) {
		name = "coded_block_" + to_string(i) + "_";
		CWire* newWire = new CWire(name, 0);
		//_cir.addPi(newWire, name);
		_cir.addWire(newWire, name);
	}
	for(int i = 1; i < 6; ++i) {
		name = "test_si" + to_string(i);
		CWire* newWire = new CWire(name, 0);
		//_cir.addPi(newWire, name);
		_cir.addWire(newWire, name);
	}
	CWire* se = new CWire("test_se", 0);
	//_cir.addPi(se, "test_se");
	_cir.addWire(se, "test_se");
	//END INPUT for LDPC
	//
	//OUTPUT for LDPC
	CWire* out = new CWire("decoded", 1);
	//_cir.addPo(out, "decoded");
	_cir.addWire(out, "decoded");
	for(int i = 2047; i >= 0; --i) {
		name = "decoded_block_" + to_string(i) + "_";
		out = new CWire(name, 1);
		//_cir.addPo(out, name);
		_cir.addWire(out, name);
	}		
	// END OUTPUT for LDPC
	//
	// Wire for LDPC
	for(int i = 1; i <= 78224; ++i) {
		name = "n" + to_string(i);
		CWire* newWire = new CWire(name, 2);
		_cir.addWire(newWire, name);
	}	
	CWire* newWire0 = new CWire("1'b0", 2);
	_cir.addWire(newWire0, "1'b0");
	newWire0->setValue(0);
	CWire* newWire1 = new CWire("1'b1", 2);
	_cir.addWire(newWire1, "1'b1");
	newWire1->setValue(1);
	// END Wire for LDPC
	while(getline(file,c)) {
		if(c.size()) {
			// INPUT
		//	if(c.substr(2,5) == "input") {
		//		size_t found = c.find(" ",3) + 1;
		//		size_t last = c.find(",", found+1);
		//		//cout << "input ";
		//		while(1) {
		//			//cout << c.substr(found, last-found) << ", ";
		//			name = c.substr(found, last-found);
		//			CWire* newWire = new CWire(newWire, name);
		//			_cir.addPi(newWire, name);
		//			_cir.addWire(newWire, name);
		//			found = c.find(" ", last+1) + 1;
		//			if(found == string::npos) break;
		//			last = c.find(",", found+1);
		//			if(last == string::npos) {
		//				last = c.find(";");
		//				//cout << c.substr(found, last-found) << "; " << endl;
		//				name = c.substr(found, last-found);
		//				CWire* newWire2 = new CWire(newWire, name);
		//				_cir.addPi(newWire2, name);
		//				break;
		//			}
		//		}
		//	} // END INPUT
		
		//	// OUTPUT
		//	else if(c.substr(2,6) == "output") {
		//		size_t found = c.find(" ",3) + 1;
		//		size_t last = c.find(",", found+1);
		//		//cout << "output ";
		//		while(1) {
		//			//cout << c.substr(found, last-found) << ", ";
		//			name = c.substr(found, last-found);
		//			CWire* newWire = new CWire(name, 1);
		//			_cir.addPo(newWire, name);
		//			_cir.addWire(newWire, name);
		//			found = c.find(" ", last+1) + 1;
		//			if(found == string::npos) break;
		//			last = c.find(",", found+1);
		//			if(last == string::npos) {
		//				last = c.find(";");
		//				//cout << c.substr(found, last-found) << "; " << endl;
		//				name = c.substr(found, last-found);
		//				CWire* newWire2 = new CWire(name, 1);
		//				_cir.addPo(newWire2, name);
		//				break;
		//			}
		//		}
		//	} //END OUTPUT
		//
		//	// WIRE
		//	else if(c.substr(2,4) == "wire") {
		//		size_t found = c.find(" ",3) + 3;
		//		size_t last = c.find(",", found+1);
		//		//cout << "wire ";
		//		while(1) {
		//			//cout << c.substr(found, last-found) << ", ";
		//			name = c.substr(found, last-found);
		//			CWire* newWire = new CWire(name, 2);
		//			_cir.addWire(newWire, name);
		//			found = c.find(" ", last+1)+1;
		//			if(found == string::npos) break;
		//			last = c.find(",", found+1);
		//			if(last == string::npos) {
		//				last = c.find(";");
		//				//cout << c.substr(found, last-found) << "; " << endl;
		//				name = c.substr(found, last-found);
		//				CWire* newWire2 = new CWire(name, 2);
		//				_cir.addWire(newWire2, name);
		//				break;
		//			}
		//		}
		//	} //END WIRE
		//if(c.substr(2,6) != "AOI211") continue;
			// AOI21 & OAI21
		  if(c.substr(2,6) == "AOI21_" || c.substr(2,6)== "OAI21_") {
				CGate* newGate;
				if(c.substr(2,6) == "AOI21_") newGate = new CGate("AOI21");
				else newGate = new CGate("OAI21");
				//cout << "AOI21 ";
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);
				// B1 input
				found = c.find(".B1")+4;
				last = c.find(")", found+1);
				//cout<< ".B1 = " << c.substr(found,last-found);
				connect(newGate, c.substr(found,last-found),0);
				
				// B2 input
				found = c.find(".B2",found+1) + 4;
				last = c.find(")", found+1);
				//cout<< ", .B2 = " << c.substr(found,last-found);
				connect(newGate, c.substr(found,last-found),0);
					
				// A input
				found = c.find(".A",found+1) + 3;
				last = c.find(")", found+1);
				//cout<< ", .A = " << c.substr(found,last-found);
				connect(newGate, c.substr(found,last-found),0);

				// ZN output
				found = c.find(".ZN",found+1) + 4;
				last = c.find(")", found+1);
				if(found == string::npos || last == string::npos) {
					getline(file,c);
					found = c.find_first_not_of(" ");
					last = c.find(")", found+1);
				}
				//cout<< ", .ZN = " << c.substr(found,last-found) << endl;
				connect(newGate, c.substr(found,last-found),1);
			} // END AOI21
		 	
			//AOI221 & OAI221
			else if(c.substr(2,6) == "AOI221" || c.substr(2,6) == "OAI221") {
				CGate* newGate;
				if(c.substr(2,6) == "AOI221") newGate = new CGate("AOI221");
				else newGate = new CGate("OAI221");
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);

				// input
				found = c.find(".");
				//cout<< ".B1 = " << c.substr(found,last-found);
				int tot = 0;
				while(1) {
					if(c.find("(", found) == string::npos) break;
					found = c.find("(", found) + 1;
					last = c.find(")", found+1);
					if(last == string::npos) {
						getline(file,c);
						//cout << c << endl;
						found = c.find_first_not_of(" ");
						last = c.find(")", found+1);
					}
					//cout << c.substr(found, last-found) << ",";
					if(tot == 5) connect(newGate, c.substr(found, last-found),1);
					else connect(newGate, c.substr(found, last-found), 0);
					++tot;
				}
				//cout << "tot= " << tot << endl;
			}	
			//AOI211 & OAI211
			else if(c.substr(2,6) == "AOI211" || c.substr(2,6) == "OAI211") {
				//cout << c << endl;
				CGate* newGate;
				if(c.substr(2,6) == "AOI211") newGate = new CGate("AOI211");
				else newGate = new CGate("OAI211");
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);

				// input
				found = c.find(".");
				//cout<< ".B1 = " << c.substr(found,last-found);
				int tot = 0;
				while(1) {
					if(c.find("(", found) == string::npos) break;
					found = c.find("(", found) + 1;
					last = c.find(")", found+1);
					if(last == string::npos) {
						getline(file,c);
						//cout << c << endl;
						found = c.find_first_not_of(" ");
						last = c.find(")", found+1);
					}
					//cout << c.substr(found, last-found) << ",";
					if(tot == 4) connect(newGate, c.substr(found, last-found),1);
					else connect(newGate, c.substr(found, last-found), 0);
					++tot;
				}
			}	
			//AOI22 & OAI22
			else if(c.substr(2,6) == "AOI22_" || c.substr(2,6) == "OAI22_") {
				//cout << c << endl;
				CGate* newGate;
				if(c.substr(2,6) == "AOI22_") newGate = new CGate("AOI22");
				else newGate = new CGate("OAI22");
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);

				// input
				found = c.find(".");
				//cout<< ".B1 = " << c.substr(found,last-found);
				int tot = 0;
				while(1) {
					if(c.find("(", found) == string::npos) break;
					found = c.find("(", found) + 1;
					last = c.find(")", found+1);
					if(last == string::npos) {
						getline(file,c);
						//cout << c << endl;
						found = c.find_first_not_of(" ");
						last = c.find(")", found+1);
					}
					//cout << c.substr(found, last-found) << ",";
					if(tot == 4) connect(newGate, c.substr(found, last-found),1);
					else connect(newGate, c.substr(found, last-found), 0);
					++tot;
				}
				//Exception
				if(newGate->getName() == "U27606") connect(newGate, "n24138", 1); 
			}	
			//OAI33
			else if(c.substr(2,6) == "OAI33_" || c.substr(2,6) == "AOI222" || c.substr(2,6) == "OAI222") {
				//cout << c << endl;
				CGate* newGate;
				if(c.substr(2,6) == "OAI33_") newGate = new CGate("OAI33");
				else if(c.substr(2,6) == "OAI222") newGate = new CGate("OAI222");
				else newGate = new CGate("AOI222");
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);

				// input
				found = c.find(".");
				//cout<< ".B1 = " << c.substr(found,last-found);
				int tot = 0;
				while(1) {
					if(c.find("(", found) == string::npos) break;
					found = c.find("(", found) + 1;
					last = c.find(")", found+1);
					if(last == string::npos) {
						getline(file,c);
						//cout << c << endl;
						found = c.find_first_not_of(" ");
						last = c.find(")", found+1);
					}
					//cout << c.substr(found, last-found) << ",";
					if(tot == 6) connect(newGate, c.substr(found, last-found),1);
					else connect(newGate, c.substr(found, last-found), 0);
					++tot;
				}
			}	
			// MUX
			else if(c.substr(2,3) == "MUX") {
				//cout << c << endl;
				CGate* newGate = new CGate("MUX");
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);

				// input
				bool z = true;
				if(c.find(".Z") == string::npos) z = false;
				found = c.find(".");
				//cout<< ".B1 = " << c.substr(found,last-found);
				int tot = 0;
				while(1) {
					if(c.find("(", found) == string::npos) break;
					found = c.find("(", found) + 1;
					last = c.find(")", found+1);
					if(last == string::npos) {
						getline(file,c);
						//cout << c << endl;
						found = c.find_first_not_of(" ");
						last = c.find(")", found+1);
					}
					//cout << c.substr(found, last-found) << ",";
					if(tot == 3) connect(newGate, c.substr(found, last-found),1);
					else connect(newGate, c.substr(found, last-found), 0);
					++tot;
				}
				if(!z) {
						getline(file,c);
						//cout << c << endl;
						found = c.find("(") + 1;
						last = c.find(")", found+1);
						connect(newGate, c.substr(found, last-found),1);
				}
			}	
			// FA
			else if(c.substr(2,2) == "FA") {
				//cout << c << endl;
				CGate* newGate = new CGate("FA");
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);

				// input
				found = c.find(".");
				//cout<< ".B1 = " << c.substr(found,last-found);
				int tot = 0;
				while(1) {
					if(c.find("(", found) == string::npos) break;
					found = c.find("(", found) + 1;
					last = c.find(")", found+1);
					if(last == string::npos) {
						getline(file,c);
						//cout << c << endl;
						found = c.find_first_not_of(" ");
						last = c.find(")", found+1);
					}
					//cout << c.substr(found, last-found) << ",";
					if(tot == 3) connect(newGate, c.substr(found, last-found),1);
					else connect(newGate, c.substr(found, last-found), 0);
					++tot;
				}
			}	
			// XOR & XNOR
			else if(c.substr(2,3) == "XOR" || c.substr(2,4) == "XNOR") {
				//cout << c << endl;
				CGate* newGate;
				if(c.substr(2,3) == "XOR") newGate = new CGate("XOR");
				else newGate = new CGate("XNOR");
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);

	
				found = c.find(".");
				//cout<< ".B1 = " << c.substr(found,last-found);
				int tot = 0;
				while(1) {
					if(c.find("(", found) == string::npos) break;
					found = c.find("(", found) + 1;
					last = c.find(")", found+1);
					if(last == string::npos) {
						getline(file,c);
						//cout << c << endl;
						found = c.find_first_not_of(" ");
						last = c.find(")", found+1);
					}
					//cout << c.substr(found, last-found) << ",";
					if(tot == 2) connect(newGate, c.substr(found, last-found),1);
					else connect(newGate, c.substr(found, last-found), 0);
					++tot;
				}
			}	
			//AOI22 
			//AOI22 
		//	else if(c.substr(2,5) == "AOI22") {
		//		CGate* newGate = new CGate("AOI22");
		//		_cir.addGate(newGate);
		//		size_t found = c.find(" ", 3) + 1;
		//		size_t last = c.find(" ", found+1);
		//		newGate->addName(c.substr(found,last-found));
		//		//cout << "AOI22 ";
		//		// A1 input
		//		found = c.find(".A1")+4;
		//		last = c.find(")", found+1);
		//		//cout<< ".A1 = " << c.substr(found,last-found);
		//		connect(newGate, c.substr(found,last-found),0);
		//		
		//		// A2 input
		//		found = c.find(".A2",found+1) + 4;
		//		last = c.find(")", found+1);
		//		//cout<< ", .A2 = " << c.substr(found,last-found);
		//		connect(newGate, c.substr(found,last-found),0);
		//			
		//		// B1 input
		//		found = c.find(".B1",found+1) + 4;
		//		last = c.find(")", found+1);
		//		//cout<< ", .B1 = " << c.substr(found,last-found);
		//		connect(newGate, c.substr(found,last-found),0);

		//		// B1 input
		//		found = c.find(".B2",found+1) + 4;
		//		last = c.find(")", found+1);
		//		//cout<< ", .B2 = " << c.substr(found,last-found);
		//		connect(newGate, c.substr(found,last-found),0);

		//		// ZN output
		//		found = c.find(".ZN",found+1) + 4;
		//		last = c.find(")", found+1);
		//		if(found == string::npos || last == string::npos) {
		//			getline(file,c);
		//			found = c.find_first_not_of(" ");
		//			last = c.find(")", found+1);
		//		}
		//		//cout<< ", .ZN = " << c.substr(found,last-found) << endl;
		//		connect(newGate, c.substr(found,last-found),1);
		//	} // END AOI22

			// INV
			else if(c.substr(2,3) == "INV" || c.substr(2,3) == "BUF" || c.substr(2,6) =="CLKBUF") {
				CGate* newGate; 
				if(c.substr(2,3) == "INV") newGate = new CGate("INV");
				else if(c.substr(2,3) == "BUF") newGate = new CGate("BUF");
				else  newGate = new CGate("CLKBUF");
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);
				//cout << "INV ";
				// A input
				found = c.find(".A") + 3;
				last = c.find(")" ,found+1);
				//cout<< ", .A = " << c.substr(found,last-found);
				connect(newGate, c.substr(found,last-found),0);
				
				// ZN output
				if(newGate->getType() == "INV"){
					found = c.find(".ZN",found+1) + 4;
					last = c.find(")", found+1);
				}
				else {
					found = c.find(".Z",found+1) + 3;
					last = c.find(")", found+1);
				}
				//cout<< ", .ZN = " << c.substr(found,last-found) << endl;
				connect(newGate, c.substr(found,last-found),1);
			} //ENDINV

		//	// NOR
		//	else if(c.substr(2,3) == "NOR") {
		//		int nfanin = c[5] - '0';
		//		//cout << "NOR" << nfanin << " ";
		//		CGate* newGate = new CGate("NOR");
		//		_cir.addGate(newGate);
		//		size_t found = c.find(" ", 3) + 1;
		//		size_t last = c.find(" ", found+1);
		//		newGate->addName(c.substr(found,last-found));

		//	  found = c.find("(");
		//	  last = found+1;
		//		for(int i=0; i<nfanin; ++i) {
		//			found = c.find("(", found+1) + 1;
		//			last = c.find(")", found+1);
		//			//cout << ",.A" << i+1 << " = " << c.substr(found,last-found);
		//			connect(newGate, c.substr(found,last-found),0);
		//		}
		//		
		//		// ZN output
		//		found = c.find(".ZN",found+1) + 4;
		//		last = c.find(")", found+1);
		//		//cout<< ", .ZN = " << c.substr(found,last-found) << endl;
		//		connect(newGate, c.substr(found,last-found),1);
		//	} // END NOR

			// SDFF
			else if(c.substr(2,4) == "SDFF") {
				CGate* newGate = new CGate("SDFF");
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);

				// D
				found = c.find(".D",last+1) + 3;
				last = c.find(")", found+1);
				//cout << c.substr(found, last-found);
				connect(newGate, c.substr(found, last-found), 0);
				_cir.addPpo(c.substr(found,last-found));

				// Q
				if(c.find("Q") == string::npos) {
					getline(file,c);
				}
				found = c.find(".Q(") + 3;
				last = c.find(")", found+1);
				connect(newGate, c.substr(found, last-found), 1);
				_cir.addPpi(c.substr(found,last-found));	
				if(c.find(".QN") != string::npos) {
					found = c.find(".QN(",last+1) + 4;
					last = c.find(")", found+1);
					CWire* w = _cir.getWireFromName(c.substr(found, last-found));
					w->addFaninGate(newGate);
					newGate->addQN(w);
				}
				// _cir.addPpi(c.substr(found,last-found));	
			} // END SDFF

			// AND & NOR
			else if(c.substr(2,3) == "AND" || c.substr(2,3) == "NOR" || c.substr(2,4) == "NAND" || c.substr(2,2) == "OR") {
				int nfanin;
				//cout << "NOR" << nfanin << " ";
				CGate* newGate;
				if(c.substr(2,3)=="AND") {
					newGate = new CGate("AND");
					nfanin = c[5] - '0';
				}
				else if(c.substr(2,3)=="NOR") {
					newGate = new CGate("NOR");
					nfanin = c[5] - '0';
				}
				else if(c.substr(2,2)=="OR") {
					newGate = new CGate("OR");
					nfanin = c[4] - '0';
				}
				else {
					newGate = new CGate("NAND");
					nfanin = c[6]-'0';
				}
				size_t found = c.find(" ", 3) + 1;
				size_t last = c.find(" ", found+1);
				newGate->addName(c.substr(found,last-found));
				_cir.addGate(newGate);

			  found = c.find("(");
			  last = found+1;
				for(int i=0; i<nfanin; ++i) {
					found = c.find("(", found+1) + 1;
					last = c.find(")", found+1);
					if(last == string::npos) {
						getline(file,c);
						found = c.find_first_not_of(" ");
						last = c.find(")", found+1);
					}
					//cout << ",.A" << i+1 << " = " << c.substr(found,last-found);
					connect(newGate, c.substr(found,last-found),0);
				}
				
				// ZN output
				found = c.find(".ZN",found+1) + 4;
				last = c.find(")", found+1);
				if(found == string::npos || last == string::npos) {
					getline(file,c);
					found = c.find_first_not_of(" ");
					last = c.find(")", found+1);
				}
				//cout<< ", .ZN = " << c.substr(found,last-found) << endl;
				connect(newGate, c.substr(found,last-found),1);
			} // END AND
		}
		//cout << c << endl;
	}
	file.close();
	//cout << "///////////DEBUG///////////////////" << endl;
  //_cir.printGates();	
	//_cir.printLevel();
}

void CParser::partition(string die0, string die1) {
	ifstream file(die0);
	ifstream file1(die1);
	string c;
	string name;
	size_t found, last;
	//die0
	while(getline(file,c)) {
		if(c.find("_X") == string::npos)  continue;
		found = c.find(" ") + 1;
		last = c.find(" ", found+1);
		name = c.substr(found, last-found);
		CGate* g = _cir.getGateFromName(name);
		g->setDie(0);
	}
	//die1
	while(getline(file1,c)) {
		if(c.find("_X") == string::npos)  continue;
		found = c.find(" ") + 1;
		last = c.find(" ", found+1);
		name = c.substr(found, last-found);
		CGate* g = _cir.getGateFromName(name);
		if(g) g->setDie(1);
	}
}
void CParser::connect(CGate* g, string name, int type) {
	name = name.substr(name.find_first_not_of(" "), name.find_last_not_of(" ")-name.find_first_not_of(" ")+1);
	CWire* w = _cir.getWireFromName(name);
	if(!w) {
		cout << "Cannout find Wire" << name << endl;
		return;
	}
	if(type == 0) {
		g->addFanin(w);
		w->addFanoutGate(g);		
	}
	else {
		g->addFanout(w);
		w->addFaninGate(g);
	}
}

void CParser::parseSTIL(string filename) {
	ifstream file(filename);
	string c;

	if(!file) {
		cout << "Cannout Open File!\n";
		assert(false);
	}
	
	while(getline(file,c)) {
		if(!c.size()) continue;
		if(c.substr(0,12) == "SignalGroups") {
			size_t found = 0;
			size_t last = 0;
			//_pi
			while(getline(file,c)) {
				last = 0;
				while(1){
					found = c.find("\"",last+1)+1;
					last = c.find("\"", found+1);
					if(c.substr(found,last-found)=="_pi") continue;
					_cir.addPi(_cir.getWireFromName(c.substr(found, last-found)), c.substr(found,last-found));
					//cout << c.substr(found,last-found) << endl;
					if(c.find("\"", last+1) == string::npos) break; 
		 		}
				if(c.find(";") != string::npos) break;	
			}
			//Skip _in
			getline(file,c);
			while(getline(file,c)) {
				if(c.find(";") != string::npos) break;
			} 
			//Skip_all_inputs
			getline(file,c);
			while(getline(file,c)) {
				if(c.find(";") != string::npos) break;
			} 
			//_po
			while(getline(file,c)) {
				last = 0;
				while(1){
					found = c.find("\"",last+1)+1;
					last = c.find("\"", found+1);
					if(c.substr(found,last-found)=="_po") continue;
					_cir.addPo(_cir.getWireFromName(c.substr(found, last-found)), c.substr(found,last-found));
					
					if(c.find("\"", last+1) == string::npos) break; 
		 		}
				if(c.find(";") != string::npos) break;	
			}
		}
		//Scan Chain
		
		else if(c.find("ScanInversion") != string::npos) {
			vector<CGate*> scanchain;
			while(getline(file,c)) {
				size_t found = 0;
				size_t last = 0;
				while(1) {
					found = c.find("\"top.",last+1)+5;
					last = c.find(".nextstate\"", found+1);
					scanchain.push_back(_cir.getGateFromName(c.substr(found, last-found)));
					if(c.find("\"top.", last+1) == string::npos) break; 
				}
				if(c.find(";") != string::npos) break;
			}
			_cir.addScanChain(scanchain);
		}
		else if(c.find("PatternBurst")!=string::npos) break;
	}
	/*
	vector<CGate*> scanchain;
	scanchain.push_back(_cir.getGateFromName("\\DFF_0/Q_reg"));
	scanchain.push_back(_cir.getGateFromName("\\DFF_1/Q_reg"));
	scanchain.push_back(_cir.getGateFromName("\\DFF_2/Q_reg"));
	_cir.addScanChain(scanchain);*/
//	vector<CGate*> scanchain;
//	string regname;
//	for(int i = 0; i < 2048; ++i) {
//		regname = "r" + to_string(i) + "_reg";		
//		scanchain.push_back(_cir.getGateFromName(regname));
//		if(i == 409 || i == 819 || i == 1229 || i == 1638 || i == 2047) {
//			_cir.addScanChain(scanchain);
//			scanchain.clear();		
//		}
//	} 	
}

void CParser::parsePattern() {
	ifstream file1("result.txt");
	ifstream file2("correct.txt");
	string c1, c2;
	int v = 0;
	while(getline(file1, c1)) {
		getline(file2, c2);
		++v;
		if(c1.size() != c2.size()) cout << "Not Equal Size!!!\n";
		for(int i=0; i<c1.size(); ++i) {
			if(c1[i] != c2[i]) {
				cout << "Line: "<< v <<", result:" << c1[i] << ", correct:" << c2[i] << ", @" << i << endl;
				vector<CGate*> vGate = _cir.getScanChain(v-4);
				CGate* g = vGate[vGate.size()-1-i];
				cout << "Gate: " << g->getName() << ".D Value" << g->getFanin(0)->getValue() << endl;
				_cir.printWireFromName(g->getFanin(0)->getName());		
			}
		}
	}
}
