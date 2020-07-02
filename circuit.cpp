#include "circuit.hpp"


bool compareLevel(CGate* g1, CGate* g2) {
	return (g1->getLevel() < g2->getLevel());
}

////////////////////////////////////////
//
//Value 0:	Low
//Value 1:	High
//Value 2: 	Unknown
//Value 3: 	good 1 / faulty 0
//Value 4:	good 0 / faulty 1
//
///////////////////////////////////////

const int NotMap[5] = {1, 0, 2, 4, 3};

const int AndMap[5][5] = {
	{0, 0, 0, 0, 0},
	{0, 1, 2, 3, 4},
	{0, 2, 2, 2, 0},
	{0, 3, 2, 3, 0},
	{0, 4, 0, 0, 4}
}; 

const int OrMap[5][5] = {
	{0, 1, 2, 3, 4},
	{1, 1, 1, 1, 1},
	{2, 1, 2, 1, 2},
	{3, 1, 1, 3, 1},
	{4, 1, 2 ,1 ,4}
};

const int XorMap[5][5] = {
	{0, 1, 2, 3, 4},
	{1, 0, 2, 4, 3},
	{2, 2, 2, 2, 2},
	{3, 4, 2, 0, 1},
	{4, 3, 2, 1, 0}
};

///////////////CGate Member Functions//////////////////
int CGate::evalNot(int v) {return NotMap[v];}

int CGate::evalAnd(int v1, int v2) {return AndMap[v1][v2];}
 
int CGate::evalOr(int v1, int v2) {return OrMap[v1][v2];}

int CGate::evalXor(int v1, int v2) {return XorMap[v1][v2];}

int CGate::eval() {
	for(CWire* w:_vFanin) {
		if(w->getValue() == -1) {
			cout << "Gate:" << _name << ",level:" << _level << ",Fanin:" << w->getName() << endl;
			assert(false);
		}
	}
	int finalValue = -1;
	// AND
	if(_type == "AND") {
		finalValue = evalAnd(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		for(int i = 2; i < _vFanin.size(); ++i) 
			finalValue = evalAnd(finalValue, _vFanin[i]->getValue());
	}
	// OR
	else if(_type == "OR") {
		finalValue = evalOr(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		for(int i = 2; i < _vFanin.size(); ++i) 
			finalValue = evalOr(finalValue, _vFanin[i]->getValue());
	}
	//NAND
	else if(_type == "NAND") {
		finalValue = evalAnd(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		for(int i = 2; i < _vFanin.size(); ++i) 
			finalValue = evalAnd(finalValue, _vFanin[i]->getValue());
		finalValue = evalNot(finalValue);	
	}
	//NOR
	else if(_type == "NOR") {
		finalValue = evalOr(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		for(int i = 2; i < _vFanin.size(); ++i) 
			finalValue = evalOr(finalValue, _vFanin[i]->getValue());
		finalValue = evalNot(finalValue);
	}
	//AOI21
	else if(_type == "AOI21") {
		finalValue = evalAnd(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		finalValue = evalOr(finalValue, _vFanin[2]->getValue());
		finalValue = evalNot(finalValue);
	}
	//AOI22
	else if(_type == "AOI22") {
		int v1 = evalAnd(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		int v2 = evalAnd(_vFanin[2]->getValue(), _vFanin[3]->getValue());
		finalValue = evalOr(v1, v2);
		finalValue = evalNot(finalValue);
	}
	//AOI211
	else if(_type == "AOI211") {
		finalValue = evalAnd(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		finalValue = evalOr(finalValue, _vFanin[2]->getValue());		
		finalValue = evalOr(finalValue, _vFanin[3]->getValue());		
		finalValue = evalNot(finalValue);
	}
	//AOI221
	else if(_type == "AOI221") {
		int v1 = evalAnd(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		int v2 = evalAnd(_vFanin[2]->getValue(), _vFanin[3]->getValue());
		finalValue = evalOr(v2, _vFanin[4]->getValue());
		finalValue = evalOr(v1, finalValue);
		finalValue = evalNot(finalValue);
	}
	//AOI222
	else if(_type == "AOI222") {
		int v1 = evalAnd(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		int v2 = evalAnd(_vFanin[2]->getValue(), _vFanin[3]->getValue());
		int v3 = evalAnd(_vFanin[5]->getValue(), _vFanin[4]->getValue());
		int v4 = evalOr(v1, v2);
		finalValue = evalOr(v3, v4);
		finalValue = evalNot(finalValue);
	}
	//OAI21
	else if(_type == "OAI21") {
		int v1 = evalOr(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		finalValue = evalAnd(v1, _vFanin[2]->getValue());
		finalValue = evalNot(finalValue);
	}
	//OAI22
	else if(_type == "OAI22") {
		int v1 = evalOr(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		int v2 = evalOr(_vFanin[2]->getValue(), _vFanin[3]->getValue());
		finalValue = evalAnd(v1, v2);
		finalValue = evalNot(finalValue);
	}
	//OAI211
	else if(_type == "OAI211") {
		int v1 = evalOr(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		finalValue = evalAnd(v1, _vFanin[2]->getValue());
		finalValue = evalAnd(finalValue, _vFanin[3]->getValue());
		finalValue = evalNot(finalValue);
	}
	//OAI221
	else if(_type == "OAI221") {
		int v1 = evalOr(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		int v2 = evalOr(_vFanin[2]->getValue(), _vFanin[3]->getValue());
	  int v3 = evalAnd(v2, _vFanin[4]->getValue());
		finalValue = evalAnd(v1, v3);
		finalValue = evalNot(finalValue);
	}
	//OAI222
	else if(_type == "OAI222") {
		int v1 = evalOr(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		int v2 = evalOr(_vFanin[2]->getValue(), _vFanin[3]->getValue());
	  int v3 = evalOr(_vFanin[4]->getValue(), _vFanin[5]->getValue());
		finalValue = evalAnd(v1, v2);
		finalValue = evalAnd(v3, finalValue);
		finalValue = evalNot(finalValue);
	}
	//OAI33
	else if(_type == "OAI33") {
		int v1 = evalOr(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		int v2 = evalOr(_vFanin[2]->getValue(), v1);
	  int v3 = evalOr(_vFanin[3]->getValue(), _vFanin[4]->getValue());
		int v4 = evalOr(v3, _vFanin[5]->getValue());
		finalValue = evalAnd(v2, v4);
		finalValue = evalNot(finalValue);
	}
	//MUX
	else if(_type == "MUX") {
		int v1 = evalNot(_vFanin[2]->getValue());
		int v2 = evalAnd(v1, _vFanin[0]->getValue());
		int v3 = evalAnd(_vFanin[1]->getValue(), _vFanin[2]->getValue());
		finalValue = evalOr(v2, v3);	
	}
	//FA
	else if(_type == "FA") {
		int v1 = evalOr(_vFanin[0]->getValue(), _vFanin[1]->getValue());	
		int v2 = evalAnd(_vFanin[2]->getValue(),v1);
		int v3 = evalAnd(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		finalValue = evalOr(v2, v3);	
		int v4 = evalXor(_vFanin[0]->getValue(), _vFanin[1]->getValue());
		v4 = evalXor(v4, _vFanin[2]->getValue());
		_QN->setValue(v4);
	}
	//XOR
	else if(_type == "XOR") {
		finalValue = evalXor(_vFanin[0]->getValue(), _vFanin[1]->getValue());	
	}
	//XNOR
	else if(_type == "XNOR") {
		finalValue = evalXor(_vFanin[0]->getValue(), _vFanin[1]->getValue());	
		finalValue = evalNot(finalValue);
	}
	//INV
	else if(_type == "INV") finalValue = evalNot(_vFanin[0]->getValue());
	else {
		if(_type != "BUF" && _type != "CLKBUF" && _type != "Dummy") cout << _type << endl;
		finalValue = _vFanin[0]->getValue();
	}	
	_fanout->setValue(finalValue);
	_eval = true;
	return finalValue;
}


float CGate::probNot(float p) {return (1-p);}

float CGate::probAnd(float p1, float p2) {return (p1*p2);}

float CGate::probOr(float p1, float p2) {return (p1 + ((1-p1)*p2));}

float CGate::probXor(float p1, float p2) {return ((p1*(1-p2)) + ((1-p1)*p2));}


void CGate::evalProb() {
	for(CWire* w:_vFanin) {
		if(w->getProb() == -1) {
			cout << "Gate:" << _name << ",level:" << _level << ",Fanin:" << w->getName() << endl;
			assert(false);
		}
	}
	float finalValue = -1;
	// AND
	if(_type == "AND") {
		finalValue = probAnd(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		for(float i = 2; i < _vFanin.size(); ++i) 
			finalValue = probAnd(finalValue, _vFanin[i]->getProb());
	}
	// OR
	else if(_type == "OR") {
		finalValue = probOr(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		for(float i = 2; i < _vFanin.size(); ++i) 
			finalValue = probOr(finalValue, _vFanin[i]->getProb());
	}
	//NAND
	else if(_type == "NAND") {
		finalValue = probAnd(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		for(float i = 2; i < _vFanin.size(); ++i) 
			finalValue = probAnd(finalValue, _vFanin[i]->getProb());
		finalValue = probNot(finalValue);	
	}
	//NOR
	else if(_type == "NOR") {
		finalValue = probOr(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		for(float i = 2; i < _vFanin.size(); ++i) 
			finalValue = probOr(finalValue, _vFanin[i]->getProb());
		finalValue = probNot(finalValue);
	}
	//AOI21
	else if(_type == "AOI21") {
		finalValue = probAnd(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		finalValue = probOr(finalValue, _vFanin[2]->getProb());
		finalValue = probNot(finalValue);
	}
	//AOI22
	else if(_type == "AOI22") {
		float v1 = probAnd(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		float v2 = probAnd(_vFanin[2]->getProb(), _vFanin[3]->getProb());
		finalValue = probOr(v1, v2);
		finalValue = probNot(finalValue);
	}
	//AOI211
	else if(_type == "AOI211") {
		finalValue = probAnd(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		finalValue = probOr(finalValue, _vFanin[2]->getProb());		
		finalValue = probOr(finalValue, _vFanin[3]->getProb());		
		finalValue = probNot(finalValue);
	}
	//AOI221
	else if(_type == "AOI221") {
		float v1 = probAnd(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		float v2 = probAnd(_vFanin[2]->getProb(), _vFanin[3]->getProb());
		finalValue = probOr(v2, _vFanin[4]->getProb());
		finalValue = probOr(v1, finalValue);
		finalValue = probNot(finalValue);
	}
	//AOI222
	else if(_type == "AOI222") {
		float v1 = probAnd(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		float v2 = probAnd(_vFanin[2]->getProb(), _vFanin[3]->getProb());
		float v3 = probAnd(_vFanin[5]->getProb(), _vFanin[4]->getProb());
		float v4 = probOr(v1, v2);
		finalValue = probOr(v3, v4);
		finalValue = probNot(finalValue);
	}
	//OAI21
	else if(_type == "OAI21") {
		float v1 = probOr(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		finalValue = probAnd(v1, _vFanin[2]->getProb());
		finalValue = probNot(finalValue);
	}
	//OAI22
	else if(_type == "OAI22") {
		float v1 = probOr(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		float v2 = probOr(_vFanin[2]->getProb(), _vFanin[3]->getProb());
		finalValue = probAnd(v1, v2);
		finalValue = probNot(finalValue);
	}
	//OAI211
	else if(_type == "OAI211") {
		float v1 = probOr(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		finalValue = probAnd(v1, _vFanin[2]->getProb());
		finalValue = probAnd(finalValue, _vFanin[3]->getProb());
		finalValue = probNot(finalValue);
	}
	//OAI221
	else if(_type == "OAI221") {
		float v1 = probOr(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		float v2 = probOr(_vFanin[2]->getProb(), _vFanin[3]->getProb());
	  float v3 = probAnd(v2, _vFanin[4]->getProb());
		finalValue = probAnd(v1, v3);
		finalValue = probNot(finalValue);
	}
	//OAI222
	else if(_type == "OAI222") {
		float v1 = probOr(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		float v2 = probOr(_vFanin[2]->getProb(), _vFanin[3]->getProb());
	  float v3 = probOr(_vFanin[4]->getProb(), _vFanin[5]->getProb());
		finalValue = probAnd(v1, v2);
		finalValue = probAnd(v3, finalValue);
		finalValue = probNot(finalValue);
	}
	//OAI33
	else if(_type == "OAI33") {
		float v1 = probOr(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		float v2 = probOr(_vFanin[2]->getProb(), v1);
	  float v3 = probOr(_vFanin[3]->getProb(), _vFanin[4]->getProb());
		float v4 = probOr(v3, _vFanin[5]->getProb());
		finalValue = probAnd(v2, v4);
		finalValue = probNot(finalValue);
	}
	//MUX
	else if(_type == "MUX") {
		float v1 = probNot(_vFanin[2]->getProb());
		float v2 = probAnd(v1, _vFanin[0]->getProb());
		float v3 = probAnd(_vFanin[1]->getProb(), _vFanin[2]->getProb());
		finalValue = probOr(v2, v3);	
	}
	//FA
	else if(_type == "FA") {
		float v1 = probOr(_vFanin[0]->getProb(), _vFanin[1]->getProb());	
		float v2 = probAnd(_vFanin[2]->getProb(),v1);
		float v3 = probAnd(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		finalValue = probOr(v2, v3);	
		float v4 = probXor(_vFanin[0]->getProb(), _vFanin[1]->getProb());
		v4 = probXor(v4, _vFanin[2]->getProb());
		_QN->setProb(v4);
	}
	//XOR
	else if(_type == "XOR") {
		finalValue = probXor(_vFanin[0]->getProb(), _vFanin[1]->getProb());	
	}
	//XNOR
	else if(_type == "XNOR") {
		finalValue = probXor(_vFanin[0]->getProb(), _vFanin[1]->getProb());	
		finalValue = probNot(finalValue);
	}
	//INV
	else if(_type == "INV") finalValue = probNot(_vFanin[0]->getProb());
	else {
		if(_type != "BUF" && _type != "CLKBUF" && _type != "Dummy") cout << _type << endl;
		finalValue = _vFanin[0]->getProb();
	}	
	_fanout->setProb(finalValue);
}






/////////////////////CCircuit Member functions//////////////////////
void CCircuit::levelize() {
	_maxlevel = -1;
	for(CWire* w : _vPi) {
		int level = 1;
		levelize_dft(w, level);
	} 
	for(int i = 0; i < _vvScanChain.size(); ++i) {
		for(CGate* g : _vvScanChain[i]) {
			int l = 1;
			levelize_dft(g->getFanout(), l);
			if(g->getQN() != 0) levelize_dft(g->getQN(),l);
		}
	}
	for(CGate*g : _vGate) {
		if(g->getLevel() == -1 && g->getType()!="SDFF") {
			cout << "Gate: Udie" << g->getDie() << "/" <<g->getName() << "(" << g->getType() << ")" <<"has not been levelized\n";
			cout << "\t" << g->getFanin(0)->getName() << "," << g->getFanout()->getName() << endl;
		}
	}
	sort(_vCompute.begin(), _vCompute.end(), compareLevel);
}

void CCircuit::levelize_dft(CWire* w, int level) {
	for(int i = 0; i < w->getFanoutNum(); ++i) {
		if(w->getFanout(i)->getType() == "SDFF") continue;
		if(w->getFanout(i)->getLevel() < level) {
			w->getFanout(i)->setLevel(level);
			_maxlevel = max(level, _maxlevel);
			levelize_dft(w->getFanout(i)->getFanout(), level+1);	
			if(w->getFanout(i)->getQN()!=0) levelize_dft(w->getFanout(i)->getQN(), level+1); //For FA .S 
		}			
	}
}

void CCircuit::function() {
	for(CWire* w : _vWire) w->setValue(0);
	for(CGate* g : _vGate) {
		if(g->getType() == "SDFF" && g->getQN() != 0) g->getQN()->setValue(1);
	}
	evaluate();
	renewPPIs();
	for(int i = 0; i < 2048; ++i) {
		string name = "coded_block_" + to_string(i) + "_";
		CWire* w = getWireFromName(name);
		if(i<22) w->setValue(1);
		else w->setValue(0);
	}
	int count = 0;
	CWire* de = getWireFromName("decoded");
	cout << "decoded: " << de->getValue() << endl;
	while(de->getValue()!=1 && de->getValue()!=3 && count < 20) {
		cout << count << " decoded value: "<< de->getValue() <<endl;
		evaluate();
		renewPPIs();
		++count;
	}
}

void CCircuit::test(string pattern) {

	ifstream file(pattern);
	ofstream reshape("leon_reshape.stil");
	string c;
	bool first = true;
	bool final_pat = false;
	bool optimized = true;
	int pat_cnt = 0;
	vector<int> vWSA;
	vector<int> miv;
	vector<int> vdie0;
	vector<int> vdie1;
	while(getline(file,c)) {
		reshape << c << endl;
		if(c == "   Macro \"test_setup\";") break;
	}
	while(getline(file,c)) { // Ann {}
		if(optimized) reshape << c << endl;
		srand(time(NULL));
		cout << "Start Pattern " << pat_cnt << endl;
		bool Xs = false;
		if(pat_cnt != 1) {
			getline(file,c); // pattern #
			if(c.find("end") != string::npos ) reshape << "   \"end " << pat_cnt << " unload\": Call \"load_unload\" {\n";
			else if(optimized) reshape << c << endl;
		}
		if(c.find("end") != string::npos) final_pat = true;
		if(!first) { // unload
			string outval;
			for(int i = 0; i < _vvScanChain.size(); ++i) {
				outval.clear();
				while(getline(file,c)) {
					if(pat_cnt == 1) reshape << c << endl;
					outval += c;
					if(c.find(";") != string::npos) break;
				}
				if(pat_cnt == 1) continue;
				//if(!optimized) continue;
				reshape << outval.substr(0, outval.find("=")+1) << endl;
				outval = outval.substr(outval.find("=")+1, outval.find(";")-outval.find("=")-1);
				assert(outval.size() == _vvScanChain[i].size());

				for(int j = 0; j < _vvScanChain[i].size(); ++j) {
					CGate* g = _vvScanChain[i][_vvScanChain[i].size()-1-j];
					if(outval[j] == 'X') {
						if(g->getFanin(0)->getValue() == 0) reshape << "L";
						else if(g->getFanin(0)->getValue() == 1) reshape << "H";
						else reshape << "X";
					}
					else if(outval[j] == 'H') {
						if(g->getFanin(0)->getValue() != 1 && g->getFanin(0)->getValue() != 3){
							cout << "H! ScanChain: " << i << ", #" << j << " Failed!!!\n";
							cout << "\t" << g->getName() << "Value: " << g->getFanin(0)->getValue() <<endl;
						}
						reshape << "H";
					}
					else {
						if(g->getFanin(0)->getValue() != 0 && g->getFanin(0)->getValue() != 4){
							cout << "L! ScanChain: " << i << ", #" << j << " Failed!!!\n";
							cout << "\t" << g->getName() << "Value: "<< g->getFanin(0)->getValue() <<endl;
						}
						reshape << "L";
					}
					if(j%1024==1023) reshape << endl;
				}
				reshape << ";" << endl;
			}
		}
		
		if(final_pat) {
			reshape << " }" << endl << "}" << endl << endl;
			reshape << "// Patterns reference\n";
			break;
		}
		reset(); 
/*		getWireFromName("net8")->setValue(0);	
		getWireFromName("net8")->setProb(0);	
		getWireFromName("net1")->setValue(0);	
		getWireFromName("net1")->setProb(0);	
	*/	
	
		
		for(int i = 0; i < _vvScanChain.size(); ++i) { // load
			string val;
			while(getline(file,c)) {
				if(first) reshape << c << endl;
				val += c;
				if(c.find(";") != string::npos) break;
			}
			//getline(file,c);
			size_t start = val.find("=")+1;
			val = val.substr(val.find("=")+1, val.find(";")-val.find("=")-1);
			assert(val.size() == _vvScanChain[i].size());
			for(int j = 0; j < _vvScanChain[i].size(); ++j) {
				CGate* g = _vvScanChain[i][_vvScanChain[i].size()-1-j];
				if(val[j] == 'N') {
					int r = rand()%2;
					g->getFanout()->setValue(2);
					if(g->getQN()!=0) g->getQN()->setValue(2);	
					continue;
				}
				int v = val[j]-'0';
				assert(v==0 || v==1);
				g->getFanout()->setValue(v);
				if(g->getQN() != 0) {
					if(v==1) g->getQN()->setValue(0);
					else g->getQN()->setValue(1);
				}
			}
		}
		//Skip Multi cpature//
		if(!first) {	
		  while(getline(file,c)) {
		  		if(c.find(";") != string::npos) break;
		  }
		}	
		/////////////////////

		getline(file,c); // Call capture
		string first_capture = c;
		if(first) reshape << c << endl;
		//getline(file,c); // _pi
		string pi;
		while(getline(file,c)) {
			if(first) reshape << c << endl;
			pi += c;
			if(c.find(";") != string::npos) break;
		}
		pi = pi.substr(pi.find("=")+1, pi.find(";")-pi.find("=")-1);
		assert(pi.size() == _vPi.size());
		string pi_next;
		string second_capture;
		bool reset_pulse = false;
		bool oneclock_first = false;
		if(!first) {
			if(c.find("Xs") != string::npos) {getline(file,c);Xs=true;}
			getline(file,c); // Call capture
			second_capture = c;
			while(getline(file,c)) {
				pi_next += c;
				if(c.find(";") != string::npos) break;
			}
			pi_next = pi_next.substr(pi_next.find("=")+1, pi_next.find(";")-pi_next.find("=")-1);
					


			
			int v, v2;
			if(true) {
				for(int i = 0; i < pi.size(); ++i) {
					if(pi[i] == 'N') {
						int r = rand()%2;
						_vPi[i]->setValue(r);
					}
					else{
						if(pi[i] == 'P') _vPi[i]->setValue(0);
						else _vPi[i]->setValue(pi[i]-'0');
					}
				}
		/*
				Xfill();
				for(vector<CGate*> vG: _vvScanChain) {
					for(CGate* g: vG) {
						if(g->getFanout()->getProb() == 0.5) {
							assert(g->getFanout()->getValue() == 2);
						//	cout << g->getName() << " " << g->getFanin(0)->getProb() << endl;
							if(g->getFanin(0)->getProb() > 0.5) {g->getFanout()->setValue(1);if(g->getQN()!=0)g->getQN()->setValue(0);}
							else if(g->getFanin(0)->getProb() < 0.5) {g->getFanout()->setValue(0);if(g->getQN()!=0)g->getQN()->setValue(1);}
							else {
								int r = rand()%2;
								g->getFanout()->setValue(r);
								if(g->getQN() != 0) g->getQN()->setValue(1-r);
							}
						}
					}
				}*/
				evaluate();
				for(CWire* w : _vWire) w->setTimeValue(w->getValue(),0);
				//if(pat_cnt == 5) checkILP();
				renewPPIs();
			}
			for(int i = 0; i < pi.size(); ++i) {
				//continue;
				int r = rand()%2;
				if(pi_next[i] == 'P') _vPi[i]->setValue(1);
				else {
					if(pi_next[i] == 'N') continue;//_vPi[i]->setValue(2);
					else _vPi[i]->setValue(pi_next[i]-'0');
					//else if(pi[i] == 'N') _vPi[i]->setValue(pi_next[i]-'0');
				}
			}
		}
		else {
			for(int i = 0; i < pi.size(); ++i) {
				if(pi[i] == 'N') {_vPi[i]->setValue(0);continue;}
				int v = pi[i]-'0';
				_vPi[i]->setValue(v);
			}	
		}
		if(reset_pulse) {
			cout << "RESET!!!\n";
			for(CGate* g: _vGate) {
				g->getFanout()->setValue(0);
				if(g->getQN() != 0) g->getQN()->setValue(1);
			}
		}
		evaluate();
		for(CWire* w : _vWire) w->setTimeValue(w->getValue(),1);
		//dumpSTA(first, pat_cnt);
		
		if(first) {
			while(getline(file,c)) {
				for(int i = 0; i < c.size(); ++i) {
					if(c[i] == 'N') reshape << "0";
					else reshape << c[i];	
				}
				reshape << endl;
				if(c.find("Ann")!=string::npos) break;
			}
			++pat_cnt;
			first = false;
			continue;
		}
		if(pat_cnt <= 252) optimized = false;
		else{
			dumpILP();
			optimized = gurobi();
		}
		if(!optimized) {
			//if(pat_cnt <= 200) {
  			while(getline(file,c)) {
  				if(c.find(";") != string::npos) break;
  			}
  			++pat_cnt;
  			continue;
			/*}
			for(CWire* w: _vWire) w->setValue(w->getTimeValue(0));
			Xfill();
			for(vector<CGate*> vG: _vvScanChain) {
				for(CGate* g: vG) {
					if(g->getFanout()->getProb() == 0.5) {
						assert(g->getFanout()->getValue() == 2);
						if(g->getFanin(0)->getProb() > 0.5) {
							g->getFanout()->setValue(1);
							g->getFanout()->setTimeValue(1,0);
							if(g->getQN()!=0){
								g->getQN()->setValue(0);
								g->getQN()->setTimeValue(0,0);
							}
						}
						else if(g->getFanin(0)->getProb() < 0.5) {
							g->getFanout()->setValue(0);
							g->getFanout()->setTimeValue(0,0);
							if(g->getQN()!=0) {
								g->getQN()->setValue(1);
								g->getQN()->setTimeValue(1,0);
							}
						}
						else {
							int r = rand()%2;
							g->getFanout()->setValue(r);
							g->getFanout()->setTimeValue(r,0);
							if(g->getQN() != 0) {
								g->getQN()->setValue(1-r);
								g->getQN()->setTimeValue(1-r, 0);
							}
						}
					}
				}
			}*/
		}
	/*	int WSA = 0;
		int WSA_die0 = 0;
		int WSA_die1 = 0;
		for(CGate* g: _vGate) {
			if(g->getType() == "Dummy") continue;
			if(g->getFanout()->getValue() > 2) {
				WSA += (1+g->getFanout()->getFanoutNum());
				if (g->getDie() == 0) {
					 WSA_die0 += (1+g->getFanout()->getFanoutNum());
				}
				else {
					 WSA_die1 += (1+g->getFanout()->getFanoutNum());
				}
				if(g->getQN() !=0 && g->getQN()->getValue() > 2) {
					WSA += (1+g->getQN()->getFanoutNum());
			  	if (g->getDie() == 0) {
			  		 WSA_die0 += (1+g->getQN()->getFanoutNum());
			  	}
			  	else {
			  		 WSA_die1 += (1+g->getQN()->getFanoutNum());
			  	}
				}	
			}
		}
		vWSA.push_back(WSA);
		vdie0.push_back(WSA_die0);
		vdie1.push_back(WSA_die1);
		int weight = 0;
		for(CWire* w: _vWire) {
			if(w->getValue() > 2) {
				int d1 = 0;
				if(w->getType() > 0) d1 = w->getFanin()->getDie();
				for(int i = 0; i < w->getFanoutNum();++i) {
					if(w->getFanout(i)->getDie() != d1) ++weight;
				}
			}
		}
		miv.push_back(weight);
		cout << "WSA: " << WSA << endl;
		cout << "WSA_die0: " << WSA_die0 << endl;
*/
		
		/////////////////ReEvaluate for dump STIL///////////////
		evaluate();
		renewPPIs();
		for(int i = 0; i < pi.size(); ++i) {
			if(pi[i] == 'P') _vPi[i]->setValue(0);
			else {
				if(pi_next[i] == 'N') continue;//_vPi[i]->setValue(2);
				else _vPi[i]->setValue(pi_next[i]-'0');
			}
		}
		for(CWire* w: _vPi) w->setTimeValue(w->getValue(),1);	
		evaluate();
		/////////////////End ReEvaluate////////////////////////
		////////////////Start Dump ScanChain///////////////////
		for(int i = 0; i < _vvScanChain.size(); ++i) {
			reshape << "      \"test_si" << i+1 << "\"=" << endl;
			for(int j = 0; j < _vvScanChain[i].size(); ++j) {
				CGate* g = _vvScanChain[i][_vvScanChain[i].size()-1-j];
				reshape << g->getFanout()->getTimeValue(0);
				if(j%1024==1023) reshape << endl;
			}
			
			if(i==_vvScanChain.size()-1) reshape << "; }\n";
			else reshape << ";\n";
		}	
		////////////Start Dump Pi//////////////////////////////
		//reshape << first_capture << endl;
		reshape << "   Call \"multiclock_capture\" {" << endl;
		reshape << "      \"_pi\"=" << endl;
		for(int i = 0; i < _vPi.size(); ++i) {
			if(false) reshape << "0";
			else if(_vPi[i]->getTimeValue(0) == 2) reshape << "N";
			else reshape << _vPi[i]->getTimeValue(0);

			if(i%1024 == 1023) reshape << endl;	
		}
		reshape << "\n; }\n";

		//LOC
		reshape << "   Call \"allclock_launch\" {" << endl;
		reshape << "      \"_pi\"=" << endl;
		for(int i = 0; i < _vPi.size(); ++i) {
			if(pi[i] == 'P') reshape << "P";
			else if(_vPi[i]->getTimeValue(0) == 2) reshape << "N";
			else reshape << _vPi[i]->getTimeValue(0);

			if(i%1024 == 1023) reshape << endl;	
		}
		reshape << "\n; }\n";

		if(Xs) {reshape << "   Ann {* Xs_are_explicit *}" << endl;Xs=false;}
		//
		//reshape << second_capture << endl;
		reshape << "   Call \"allclock_capture\" {" << endl;
		reshape << "      \"_pi\"=" << endl;
		for(int i = 0; i < _vPi.size(); ++i) {
			if(pi_next[i] == 'P') reshape << "P";
			else if(_vPi[i]->getTimeValue(1) == 2) reshape << "N";
			else reshape << _vPi[i]->getTimeValue(1);

			if(i%1024 == 1023) reshape << endl;	
		}
		reshape << "\n;\n";
		////////////////////////End Pi/////////////////////////
		reshape << "      \"_po\"=" << endl;
		string po;
		while(getline(file,c)) {
			po += c;
			if(c.find(";") != string::npos) break;
		}
		po = po.substr(po.find("=")+1, po.find(";")-po.find("=")-1);
		assert(po.size() == _vPo.size());

	
		for(int i = 0; i < po.size(); ++i) {
			if(po[i] == 'X') {
			//	if(_vPo[i]->getValue() != 2) {
			//		cout << "Po: " << _vPo[i]->getName() << " Failed!!\n" << "i=" << i << ",po[i]=X, _vPo[i]=" << _vPo[i]->getValue() << endl;
			//	}
				if(_vPo[i]->getValue() == 0) reshape << "L";
				else if(_vPo[i]->getValue() == 1) reshape << "H";
				else reshape << "X";
				//continue;
			}
			else if(po[i] == 'H') {
				if(_vPo[i]->getValue() != 1 && _vPo[i]->getValue() != 3) {
					cout << "Po: " << _vPo[i]->getName() << " Failed!!\n" << "i=" << i << ",po[i]=H, _vPo[i]=" << _vPo[i]->getValue() << endl;
				}
				reshape << "H";
			}
			else {
				if(_vPo[i]->getValue() != 0 && _vPo[i]->getValue() != 4) {
					cout << "Po: " << _vPo[i]->getName() << " Failed!!\n" << "i=" << i << ",po[i]=L, _vPo[i]=" << _vPo[i]->getValue() << endl;
				}
				reshape << "L";
			}

			if(i%1024==1023) reshape << endl;		
		}
		reshape << endl << "; }\n";

		
		//if(pat_cnt == 4) break;
		++pat_cnt;
	}
	file.close();
/*
	ofstream outfile1("leon_prefer.csv");
	for(int i = 0; i < vWSA.size(); ++i) {
		outfile1 << i << "," << vWSA[i] << "," << vdie0[i] << "," << vdie1[i] << "," << miv[i] << "\n"; 
	}*/
}

void CCircuit::reset() {
	for(CWire* w: _vWire) {
		if(w->getName() == "1'b0" || w->getName() == "1'b1") continue;
		w->setValue(-1);
		w->setProb(-1);
		w->setTimeValue(-1,0);
		w->setTimeValue(-1,1);
	}
}

void CCircuit::dumpSTA(bool first, int cnt) {
	ofstream file("to324.tcl", std::ofstream::out|std::ofstream::app);
	if(first) {
		file.clear();
  	file << endl;	
  	file << "set target_library \"/home/sh528/nangate45nm/2d_db/NG45nm_tt_ecsm.db\"" << endl;	
  	file << "set link_library \"* $target_library\"" << endl;
  	file << "read_verilog die0.v" << endl;
  	file << "read_verilog die1.v" << endl;
  	file << "read_verilog top.v" << endl;
  	file << "current_design top\n";
		file << "link\n";
		file << "source leon3mp_fast.sdc\n";
		file << "read_parasitic top.spef\n";
		file << "read_parasitic die0_noOpt.spef -path Udie0\n";
		file << "read_parasitic die1_noOpt.spef -path Udie1\n";
	}
	file << "# Pattern " << cnt << endl;
	file << "remove_case_analysis -all\n";
/*	vector<string> zero;
	vector<string> one;
	vector<string> rising;
	vector<string> falling;*/
	string zero = "set_case_analysis 0 {";
	string one = "set_case_analysis 1 {";
	//string rising = "set_case_analysis rising {";
	//string falling = "set_case_analysis falling {";

	for(CWire* w: _vPi) {
		if(w->getName() == "test_se" || w->getName() == "ispd_clk") continue;
		switch (w->getValue()) {
			case 0:
				//zero.push_back(w->getName());
				zero += w->getName() + " ";
				break;
			case 1:
				//one.push_back(w->getName());
				one += w->getName() + " ";
				break;
			/*case 3:
				//rising.push_back(w->getName());
				rising += w->getName() + " ";
				break;
			case 4:
				//falling.push_back(w->getName());
				falling += w->getName() + " ";
				break;*/
			default:
				break;
		}
	}

	file << zero << "}\n";
	file << one << "}\n";
	file << "set_case_analysis rising ispd_clk\n";
	//file << falling << "}\n";

	vector<string> zeroFF;
	vector<string> oneFF;
	vector<string> risingFF;
	vector<string> fallingFF;


	for(vector<CGate*> vG: _vvScanChain) {
		for(CGate* g: vG) {
			CWire* w = g->getFanout();
			switch (w->getValue()) {
				case 0:
					zeroFF.push_back(g->getName());
					break;
				case 1:
					oneFF.push_back(g->getName());
					break;
				case 3:
					risingFF.push_back(g->getName());
					break;
				case 4:
					fallingFF.push_back(g->getName());
					break;
			}
		}
	}
	
	// 0
	if(zeroFF.size()) {
		file << "set_case_analysis 0 {";
		for(string name: zeroFF) {
			CGate* g = getGateFromName(name);
			file << "Udie" << g->getDie() << "/" << name << "/Q ";
		}	
		file << "}" << endl;
		file << "set_case_analysis 1 {";
		for(string name: zeroFF) {
			CGate* g = getGateFromName(name);
			if(g->getQN()!=0) file << "Udie" << g->getDie() << "/" << name << "/QN ";
		}	
		file << "}" << endl;
	}

	// 1
	if(oneFF.size()) {
		file << "set_case_analysis 1 {";
		for(string name: oneFF) {
			CGate* g = getGateFromName(name);
			file << "Udie" << g->getDie() << "/" << name << "/Q ";
		}		
		file << "}" << endl;
		file << "set_case_analysis 0 {";
		for(string name: oneFF) {
			CGate* g = getGateFromName(name);
			if(g->getQN()!=0) file << "Udie" << g->getDie() << "/" << name << "/QN ";
		}		
		file << "}" << endl;
	}

	// rising
	if(risingFF.size()){
		file << "set_case_analysis rising {";
		for(string name: risingFF) {
			CGate* g = getGateFromName(name);
			file << "Udie" << g->getDie() << "/" << name << "/Q ";
		}	
		file << "}" << endl;
		file << "set_case_analysis falling {";
		for(string name: risingFF) {
			CGate* g = getGateFromName(name);
			if(g->getQN()!=0) file << "Udie" << g->getDie() << "/" << name << "/QN ";
		}	
		file << "}" << endl;
	}

	// falling
	if(fallingFF.size()) {
		file << "set_case_analysis falling {";
		for(string name: fallingFF) {
			CGate* g = getGateFromName(name);
			file << "Udie" << g->getDie() << "/" << name << "/Q ";
		}	
		file << "}" << endl;
		file << "set_case_analysis rising {";
		for(string name: fallingFF) {
			CGate* g = getGateFromName(name);
			if(g->getQN()!=0) file << "Udie" << g->getDie() << "/" << name << "/QN ";
		}	
		file << "}" << endl;
	}
//	for(CWire* w: _vPi) {
//		if(w->getName() == "clock") {
//			file << "set_case_analysis rising clock\n";
//			continue;
//		}
//		file << "set_case_analysis ";
//		if(w->getValue() < 2) file <<  w->getValue() << " " << w->getName() << endl;
//		else if (w->getValue() == 3) file << "rising " << w->getName() << endl;
//		else if (w->getValue() == 4) file << "falling " << w->getName() << endl;
//		//else cout << "Dont care\n";		
//	}
//
//	for(vector<CGate*> vG: _vvScanChain) {
//		for(CGate* g: vG) {
//			CWire* w = g->getFanout();
//			file << "set_case_analysis ";
//			if(w->getValue() < 2) file <<  w->getValue() << " Udie" << g->getDie() << "/" <<g->getName() << "/Q" << endl;
//			else if (w->getValue() == 3) file << "rising Udie" << g->getDie() << "/" <<g->getName() << "/Q" << endl;
//			else if (w->getValue() == 4) file << "falling Udie" << g->getDie() << "/" <<g->getName() << "/Q" << endl;
//			//else cout << "Dont care\n";		
//			
//			if(g->getQN() != 0) {
//				w = g->getQN();
//				file << "set_case_analysis ";
//				if(w->getValue() < 2) file <<  w->getValue() << " Udie" << g->getDie() << "/" <<g->getName() << "/QN" << endl;
//				else if (w->getValue() == 3) file << "rising Udie" << g->getDie() << "/" <<g->getName() << "/QN" << endl;
//				else if (w->getValue() == 4) file << "falling Udie" << g->getDie() << "/" <<g->getName() << "/QN" << endl;
//				//else cout << "Dont care\n";		
//			}
//		}
//	}
	if(first)	file << "redirect timing.rpt {report_timing -significant_digits 4}\n";		
	else file << "redirect -append timing.rpt {report_timing -significant_digits 4}\n";
}

void CCircuit::assignInputs() {
	_vPi[0]->setValue(2);
	_vPi[1]->setValue(2);
	_vPi[2]->setValue(2);
	_vPi[3]->setValue(2);
	_vPi[4]->setValue(2);
	_vPi[5]->setValue(2);
	_vPi[6]->setValue(2);
	_vPi[7]->setValue(2);
	_vPi[8]->setValue(0);

	_vvScanChain[0][2]->getFanout()->setValue(0);
	_vvScanChain[0][1]->getFanout()->setValue(2);
	_vvScanChain[0][0]->getFanout()->setValue(0);

	evaluate();
	dumpILP();
}

void CCircuit::renewPPIs() {
	vector< vector<int> > store;
	for(vector<CGate*> vG : _vvScanChain) {
		vector<int> tmp;
		for(CGate* g : vG) {
			int v = g->getFanin(0)->getValue();
			int v2 = g->getFanout()->getValue();
			//assert(v2 != 2);
			if(v==v2) tmp.push_back(v);
			else if(v == 2) tmp.push_back(2);
			else if(v2 == 2) tmp.push_back(v);
			else if(v2 == 0 && v == 1) tmp.push_back(1);
			else if(v2 == 1 && v == 0) tmp.push_back(0);	
			else assert(false);
		}
		store.push_back(tmp);
	}


	int cnt = 0;	
	for(int i = 0; i < store.size(); ++i) {
		for(int j = 0; j < _vvScanChain[i].size(); ++j) {
			CGate*g = _vvScanChain[i][j];
			int v = store[i][j];
			if(v == 0) {
				g->getFanout()->setValue(0);
				if(g->getQN() != 0) g->getQN()->setValue(1);
			}
			else if(v == 1) {
				g->getFanout()->setValue(1);
				if(g->getQN() != 0) g->getQN()->setValue(0);
			}
			else if(v == 2) {
				g->getFanout()->setValue(2);
				if(g->getQN() != 0) g->getQN()->setValue(2);
			}
			else if(v == 3) {
				++cnt;
				g->getFanout()->setValue(3);
				if(g->getQN() != 0) g->getQN()->setValue(4);
			}
			else if(v == 4) {
				++cnt;
				g->getFanout()->setValue(4);
				if(g->getQN() != 0) g->getQN()->setValue(3);
			}
			else assert(false);
		}
	}

//			if(v == v2) continue;
//			else if(v == 2) {
//				g->getFanout()->setValue(2);
//				if(g->getQN() != 0) g->getQN()->setValue(2);
//			}
//			else if(v2 == 2) {
//				g->getFanout()->setValue(v);
//				assert(v<=1);
//				if(g->getQN() != 0) g->getQN()->setValue(1-v);
//			}
//			else if(v2 == 0 && v == 1) {
//				g->getFanout()->setValue(3);
//				if(g->getQN() != 0) g->getQN()->setValue(4);
//			}
//			else {
//				g->getFanout()->setValue(4);
//				if(g->getQN() != 0) g->getQN()->setValue(3);
//			}
//		}
//	}
}

void CCircuit::renewPPIs_for_testonly() {
	vector< vector<int> > store;
	for(vector<CGate*> vG : _vvScanChain) {
		vector<int> tmp;
		for(CGate* g : vG) {
			int v = g->getFanin(0)->getValue();
			int v2 = g->getFanout()->getValue();
			//assert(v2 != 2);
			if(v==v2) tmp.push_back(v);
			else if(v == 2) tmp.push_back(2);
			else if(v2 == 2) tmp.push_back(v);
			else if(v2 == 0 && v == 1) tmp.push_back(3);
			else if(v2 == 1 && v == 0) tmp.push_back(4);	
			else assert(false);
		}
		store.push_back(tmp);
	}


	int cnt = 0;	
	for(int i = 0; i < store.size(); ++i) {
		for(int j = 0; j < _vvScanChain[i].size(); ++j) {
			CGate*g = _vvScanChain[i][j];
			int v = store[i][j];
			if(v == 0) {
				g->getFanout()->setValue(0);
				if(g->getQN() != 0) g->getQN()->setValue(1);
			}
			else if(v == 1) {
				g->getFanout()->setValue(1);
				if(g->getQN() != 0) g->getQN()->setValue(0);
			}
			else if(v == 2) {
				g->getFanout()->setValue(2);
				if(g->getQN() != 0) g->getQN()->setValue(2);
			}
			else if(v == 3) {
				++cnt;
				g->getFanout()->setValue(3);
				if(g->getQN() != 0) g->getQN()->setValue(4);
			}
			else if(v == 4) {
				++cnt;
				g->getFanout()->setValue(4);
				if(g->getQN() != 0) g->getQN()->setValue(3);
			}
			else assert(false);
		}
	}
}

void CCircuit::evaluate() {
//	int nowlevel = 1;
//	while(1) {
//		for(CGate* g: _vGate) {
//			if(g->getLevel() == nowlevel) {
//				g->eval();
//			}
//		}
//		++nowlevel;
//		if(nowlevel > _maxlevel) break;
//	}
	for(CGate* g : _vCompute) {
		if(g->getType() == "SDFF") continue;
		g->eval();
	}
}

void CCircuit::writeOutput() {
	ifstream file1("correct.txt");
	ofstream file("result2.txt");
	string c;
	for(int i = 0; i < _vPo.size(); ++i) {
		if(_vPo[i]->getValue() == 1 || _vPo[i]->getValue() == 3) file << "H";
		else file << "L";
		if(i == 1023 || i == 2047) file << endl;			
	}	
	file << endl;
	for(int i = 0; i < 3; ++i) getline(file1,c);
	
	for(int i = 0; i < _vvScanChain.size(); ++i) {
		getline(file1,c);
		int count = 0;
		for(int j = _vvScanChain[i].size()-1; j >= 0; --j) {
			CGate* g = _vvScanChain[i][j];
			if(g->getFanin(0)->getValue() == 0) {
				if(i==1) {
					file << "L";
				}
				else {
					file << "H";
				}
			}
			else if(g->getFanin(0)->getValue() == 1) {
				if(i==1) {
					file << "H";
				}
				else {
					file << "L";
				}
			}	
			else if(g->getFanin(0)->getValue() == 3) {
				if(i==1) { file << "H";
				} 
				else {
					file << "L";
				} 
			}
			else {
				if(i==1) {
					file << "L"; 
				}
				else {
					file << "H";
				}
			}
			++count;
		}	
		file << endl;
	}
}

void CCircuit::Xfill() {
	for(CWire* w: _vPi){
		if(w->getValue() == 1 || w->getValue() == 3) w->setProb(1);
		else if(w->getValue() == 0 || w->getValue() == 4) w->setProb(0);
		else w->setProb(0.5);
	}
	for(vector<CGate*> vG: _vvScanChain) {
		for(CGate* g: vG) {
			CWire* w = g->getFanout();
			if(w->getValue() == 1 || w->getValue() == 3) {
				w->setProb(1);
				if(g->getQN() != 0) g->getQN()->setProb(0);
			}
			else if(w->getValue() == 0 || w->getValue() == 4) {
				w->setProb(0);
				if(g->getQN() != 0) g->getQN()->setProb(1);
			}
			else {
				w->setProb(0.5);
				if(g->getQN() != 0) g->getQN()->setProb(0.5);
			}
		}
	}
	
	for(CGate* g : _vCompute) {
		if(g->getType() == "SDFF") continue;
		g->evalProb();
	}
}

void CCircuit::dumpILP() {
	ofstream file("leonxfill.lp");
	vector<string> variable;
	_constraint = 0;
	
	for(CWire* w: _vWire) w->setIlp(false);
	

	////Objective////
	file << "Minimize\n" << "	";
	/*bool first_obj = true;
	for(vector<CGate*> vG: _vvScanChain) {
		for(CGate* g: vG) {
			if(g->getFanout()->getValue() != 2) continue;
			if(first_obj) {
				file << g->getFanout()->getFanoutNum() << " " << g->getFanout()->getName();
				first_obj = false;
			}
			file << " + " << g->getFanout()->getFanoutNum() << " " << g->getFanout()->getName();
		}
	}*/
	bool first_obj = true;
	for(CGate* g: _vGate) {
		if(g->getDie() == 1) continue;
		CWire* w = g->getFanout();
		if(!first_obj) {
			file << " + ";
		}
		file << w->getFanoutNum()+1 << " " << w->getName() << "_toggle";
		if(g->getQN() != 0) {
			file << " + ";
			file << g->getQN()->getFanoutNum()+1 << " " << g->getQN()->getName() << "_toggle";
		}
		first_obj = false;
	}
	file << "\n";

	file << "Subject To\n";


/////////////////TimeFrame1///////////////
	for(CGate* g: _vCompute) {
		if(g->getType() == "SDFF") continue;
		else if(g->getFanout()->getTimeValue(0) != 2) continue;

		CWire* w = g->getFanout();
		vector<string> fanin;
		for(int i = 0; i < g->getFaninNum(); ++i) {
			/*if(g->getFanin(i)->getValue() == 2) {
				fanin.push_back(g->getFanin(i)->getName());
				g->getFanin(i)->setIlp(true);
			}
			else fanin.push_back(to_string(g->getFanin(i)->getValue()));*/
			if(g->getFanin(i)->getTimeValue(0) != 2) {
				file << " c" << _constraint++ << ": " << g->getFanin(i)->getName() << " = " << g->getFanin(i)->getTimeValue(0) << endl;
			}
			fanin.push_back(g->getFanin(i)->getName());
		}
		g->getFanout()->setIlp(true);

		vector<string> nonfill;
		// AND
		if(g->getType() == "AND") {
			for(int i = 0; i < g->getFaninNum()-1; ++i) {
				file << " c" << _constraint++ << ": " << w->getName() << " - " << fanin[i] << " <= 0\n";
			}
			file << " c" << _constraint++ << ": " << w->getName();
			for(string name: fanin) {
				file << " - " << name;
			} 
			file << " + " << g->getFaninNum() << " >= 0\n";
			/*for(int i = 0; i < g->getFaninNum(); ++i) {
				if(g->getFanin(i)->getValue() == 1) continue;
				nonfill.push_back(g->getFanin(i)->getName());
			}
			/////Buffer////
			if(nonfill.size() == 1) {
				file << "	c" << _constraint++ << ": "; 
				file << w->getName() << " = " << nonfill[0] << "\n"; 
			}
			///////////////
			else {
				for(string name: nonfill) {
					file << "	c" << _constraint++ << ": "; 
					file << w->getName() << " <= " << name << "\n";
				} 
				file << "	c" << _constraint++ << ": "; 
				file << w->getName() << " >= ";
				for(int i = 0; i < g->getFaninNum(); ++i) {
					if(g->getFanin(i)->getValue() == 1) file << "1";
					else file << g->getFanin(i)->getName();

					if(i == g->getFaninNum()-1) file << " - " << i << "\n";
					else file << " + ";
				}
			}*/
		}
		// OR
		else if(g->getType() == "OR") {
			for(int i = 0; i < g->getFaninNum()-1; ++i) {
				file << " c" << _constraint++ << ": " << w->getName() << " - " << fanin[i] << " >= 0\n";
			}
			file << " c" << _constraint++ << ": " << w->getName();
			for(string name: fanin) {
				file << " - " << name;
			} 
			file << " <= 0\n";
/*
			for(int i = 0; i < g->getFaninNum(); ++i) {
				if(g->getFanin(i)->getValue() == 0) continue;
				nonfill.push_back(g->getFanin(i)->getName());
			}
			if(nonfill.size() == 1) {
				file << w->getName() << " = " << nonfill[0] << ";\n";
				continue;
			}

			for(string name: nonfill) file << w->getName() << " >= " << name << ";\n"; 
			file << w->getName() << " <= ";
			for(int i = 0; i < g->getFaninNum(); ++i) {
				if(g->getFanin(i)->getValue() == 0) file << "0";
				else file << g->getFanin(i)->getName();

				if(i == g->getFaninNum()-1) file << ";\n";
				else file << " + ";
			}*/
		}
		//NAND
		else if(g->getType() == "NAND") {
			for(int i = 0; i < g->getFaninNum()-1; ++i) {
				file << " c" << _constraint++ << ": - " << w->getName() << " - " << fanin[i] << " <= -1\n";
			}
			file << " c" << _constraint++ << ": - " << w->getName();
			for(string name: fanin) {
				file << " - " << name;
			} 
			file << " + " << g->getFaninNum() << " >= -1\n";
/*
			for(int i = 0; i < g->getFaninNum(); ++i) {
				if(g->getFanin(i)->getValue() == 1) continue;
				nonfill.push_back(g->getFanin(i)->getName());
			}

			if(nonfill.size() == 1) {
				file << w->getName() << " = 1 - " << nonfill[0] << ";\n";
				continue;
			}

			
			for(string name: nonfill) file << "1 - " << w->getName() << " <= " << name << ";\n"; 
			file << "1 - " << w->getName() << " >= ";
			for(int i = 0; i < g->getFaninNum(); ++i) {
				if(g->getFanin(i)->getValue() == 1) file << "1";
				else file << g->getFanin(i)->getName();

				if(i == g->getFaninNum()-1) file << " - " << i << ";\n";
				else file << " + ";
			}*/
		}
		//NOR
		else if(g->getType() == "NOR") {
			for(int i = 0; i < g->getFaninNum()-1; ++i) {
				file << " c" << _constraint++ << ": - " << w->getName() << " - " << fanin[i] << " >= -1\n";
			}
			file << " c" << _constraint++ << ": - " << w->getName();
			for(string name: fanin) {
				file << " - " << name;
			} 
			file << " <= -1\n";
/*
			for(int i = 0; i < g->getFaninNum(); ++i) {
				if(g->getFanin(i)->getValue() == 0) continue;
				nonfill.push_back(g->getFanin(i)->getName());
			}
			if(nonfill.size() == 1) {
				file << w->getName() << " = 1 - " << nonfill[0] << ";\n";
				continue;
			}

			for(string name: nonfill) file << "1 - " <<  w->getName() << " >= " << name << ";\n"; 
			file << "1 - " <<  w->getName() << " <= ";
			for(int i = 0; i < g->getFaninNum(); ++i) {
				if(g->getFanin(i)->getValue() == 0) file << "0";
				else file << g->getFanin(i)->getName();

				if(i == g->getFaninNum()-1) file << ";\n";
				else file << "+";
			}*/
		}
		//AOI21
		else if(g->getType() == "AOI21") {
			
			string v1 = g->getName() + "_v1";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			// //string out = "1 - " + w->getName();
			IlpOr(v1,fanin[2],w->getName(),file, 1);
			
		}
		//AOI22
		else if(g->getType() == "AOI22") {
			string v1 = g->getName() + "_v1";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpAnd(fanin[2],fanin[3],v2,file, 0);
			variable.push_back(v2);

			////string out = "1 - " + w->getName();
			IlpOr(v1,v2,w->getName(),file, 1);
		}
		//AOI211
		else if(g->getType() == "AOI211") {
			string v1 = g->getName() + "_v1";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpOr(v1,fanin[2],v2,file, 0);
			variable.push_back(v2);

			//string out = "1 - " + w->getName();
			IlpOr(v2,fanin[3],w->getName(),file, 1);
		}
		//AOI221
		else if(g->getType() == "AOI221") {
			string v1 = g->getName() + "_v1";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpAnd(fanin[2],fanin[3],v2,file, 0);
			variable.push_back(v2);

			string v3 = g->getName() + "_v3";
			IlpOr(v2,fanin[4],v3,file, 0);
			variable.push_back(v3);
			

			//string out = "1 - " + w->getName();
			IlpOr(v1,v3,w->getName(),file, 1);
		}
		//AOI222
		else if(g->getType() == "AOI222") {
			string v1 = g->getName() + "_v1";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpAnd(fanin[2],fanin[3],v2,file, 0);
			variable.push_back(v2);

			string v3 = g->getName() + "_v3";
			IlpAnd(fanin[5],fanin[4],v3,file, 0);
			variable.push_back(v3);

			string v4 = g->getName() + "_v4";
			IlpOr(v1,v2,v4,file, 0);
			
			//string out = "1 - " + w->getName();
			IlpOr(v3,v4,w->getName(),file, 1);
		}
		//OAI21
		else if(g->getType() == "OAI21") {
			string v1 = g->getName() + "_v1";
			IlpOr(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			//string out = "1 - " + w->getName();
			IlpAnd(v1,fanin[2],w->getName(),file, 1);
		}
		//OAI22
		else if(g->getType() == "OAI22") {
			string v1 = g->getName() + "_v1";
			IlpOr(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpOr(fanin[2],fanin[3],v2,file, 0);
			variable.push_back(v2);

			//string out = "1 - " + w->getName();
			IlpAnd(v1,v2,w->getName(),file, 1);
		}
		//OAI211
		else if(g->getType() == "OAI211") {
			string v1 = g->getName() + "_v1";
			IlpOr(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpAnd(v1,fanin[2],v2,file, 0);
			variable.push_back(v2);

			//string out = "1 - " + w->getName();
			IlpAnd(v2,fanin[3],w->getName(),file, 1);
		}
		//OAI221
		else if(g->getType() == "OAI221") {
			string v1 = g->getName() + "_v1";
			IlpOr(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpOr(fanin[2],fanin[3],v2,file, 0);
			variable.push_back(v2);

			string v3 = g->getName() + "_v3";
			IlpAnd(v2,fanin[4],v3,file, 0);
			variable.push_back(v3);
			

			//string out = "1 - " + w->getName();
			IlpAnd(v1,v3,w->getName(),file, 1);
		}
		//OAI222
		else if(g->getType() == "OAI222") {
			string v1 = g->getName() + "_v1";
			IlpOr(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpOr(fanin[2],fanin[3],v2,file, 0);
			variable.push_back(v2);

			string v3 = g->getName() + "_v3";
			IlpOr(fanin[5],fanin[4],v3,file, 0);
			variable.push_back(v3);

			string v4 = g->getName() + "_v4";
			IlpAnd(v1,v2,v4,file, 0);
			
			//string out = "1 - " + w->getName();
			IlpAnd(v3,v4,w->getName(),file, 1);
		}
		//OAI33
		else if(g->getType() == "OAI33") {
			string v1 = g->getName() + "_v1";
			IlpOr(fanin[0],fanin[1],v1,file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpOr(fanin[2],v1,v2,file, 0);
			variable.push_back(v2);

			string v3 = g->getName() + "_v3";
			IlpOr(fanin[3],fanin[4],v3,file, 0);
			variable.push_back(v3);

			string v4 = g->getName() + "_v4";
			IlpOr(v3,fanin[5],v4,file, 0);
			variable.push_back(v4);

			//string out = "1 - " + w->getName();
			IlpAnd(v2,v4,w->getName(),file, 1);
		}
		//MUX
		else if(g->getType() == "MUX") {
			string v1;
			v1 = g->getName() + "_v1";
			file << " c" << _constraint++ << ": " << v1 << " + " << fanin[2] << " = 1\n";
/*
			string v1;
			if(fanin[2] == "0") v1 = "1";
			else if(fanin[2] == "1") v1 = "0";
			else v1 = "1 - " + fanin[2];*/
			string v2 = g->getName() + "_v2";
			IlpAnd(fanin[0], v1, v2, file, 0);
			variable.push_back(v2);

			string v3 = g->getName() + "_v3";
			IlpAnd(fanin[1],fanin[2],v3,file, 0);
			variable.push_back(v3);

			IlpOr(v2, v3, w->getName(), file, 0);
		}
		//FA
		else if(g->getType() == "FA") {
			string v1 = g->getName() + "_v1";
			IlpOr(fanin[0], fanin[1], v1, file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2";
			IlpAnd(fanin[2], v1, v2, file, 0);
			variable.push_back(v2);

			string v3 = g->getName() + "_v3";
			IlpAnd(fanin[0], fanin[1], v3, file, 0);
			variable.push_back(v3);

			IlpOr(v2,v3,w->getName(),file, 0);

		
			string v4 = g->getName() + "_v4";
			IlpXor(fanin[0], fanin[1], v4, file, 0);
			variable.push_back(v4);

			IlpXor(v4,fanin[2],g->getQN()->getName(),file, 0);
		}
		//XOR
		else if(g->getType() == "XOR") {
			IlpXor(fanin[0],fanin[1],w->getName(),file, 0);
		}
		//XNOR
		else if(g->getType() == "XNOR") {
			//string out = "1 - " + w->getName();
			IlpXor(fanin[0],fanin[1],w->getName(),file, 1);
		}
		//INV
		else if(g->getType() == "INV") {
			if(w->getName() != g->getFanin(0)->getName())
				file << " c" << _constraint++ << ": " << w->getName() << " + " << g->getFanin(0)->getName() << " = 1\n";
		}
		else {
			assert(g->getType() == "BUF" || g->getType() == "CLKBUF" || g->getType() == "Dummy");
			file << " c" << _constraint++ << ": " << w->getName() << " - " << g->getFanin(0)->getName() << " = 0\n";
		}	
	}
////////////////End TimeFrame1/////////////////////////


/////////////////TimeFrame2///////////////
	for(CGate* g: _vCompute) {
		if(g->getFanout()->getTimeValue(1) != 2) continue;
		if(g->getType() == "SDFF") {
			file << " c" << _constraint++ << ": " << g->getFanin(0)->getName() << " - " << g->getFanout()->getName() << "_2 = 0\n";
			if(g->getQN() != 0) {
				file << " c" << _constraint++ << ": " << g->getFanin(0)->getName() << " + " << g->getQN()->getName() << "_2 = 1\n";
			}
				continue;
		}

		CWire* w = g->getFanout();
		vector<string> fanin;
		for(int i = 0; i < g->getFaninNum(); ++i) {
			if(g->getFanin(i)->getTimeValue(1) != 2) {
				file << " c" << _constraint++ << ": " << g->getFanin(i)->getName() << "_2 = " << g->getFanin(i)->getTimeValue(1) << endl;
			}
			string n = g->getFanin(i)->getName() + "_2";
			fanin.push_back(n);
		}

		// AND
		if(g->getType() == "AND") {
			for(int i = 0; i < g->getFaninNum()-1; ++i) {
				file << " c" << _constraint++ << ": " << w->getName() << "_2 - " << fanin[i] << " <= 0\n";
			}
			file << " c" << _constraint++ << ": " << w->getName() << "_2";
			for(string name: fanin) {
				file << " - " << name;
			} 
			file << " + " << g->getFaninNum() << " >= 0\n";
		}
		// OR
		else if(g->getType() == "OR") {
			for(int i = 0; i < g->getFaninNum()-1; ++i) {
				file << " c" << _constraint++ << ": " << w->getName() << "_2 - " << fanin[i] << " >= 0\n";
			}
			file << " c" << _constraint++ << ": " << w->getName() << "_2";
			for(string name: fanin) {
				file << " - " << name;
			} 
			file << " <= 0\n";
		}
		//NAND
		else if(g->getType() == "NAND") {
			for(int i = 0; i < g->getFaninNum()-1; ++i) {
				file << " c" << _constraint++ << ": - " << w->getName() << "_2 - " << fanin[i] << " <= -1\n";
			}
			file << " c" << _constraint++ << ": - " << w->getName() << "_2";
			for(string name: fanin) {
				file << " - " << name;
			} 
			file << " + " << g->getFaninNum() << " >= -1\n";
		}
		//NOR
		else if(g->getType() == "NOR") {
			for(int i = 0; i < g->getFaninNum()-1; ++i) {
				file << " c" << _constraint++ << ": - " << w->getName() << "_2 - " << fanin[i] << " >= -1\n";
			}
			file << " c" << _constraint++ << ": - " << w->getName() << "_2";
			for(string name: fanin) {
				file << " - " << name;
			} 
			file << " <= -1\n";
		}
		//AOI21
		else if(g->getType() == "AOI21") {
			
			string v1 = g->getName() + "_v1_2";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);

			string out = w->getName() + "_2";
			IlpOr(v1,fanin[2],out,file, 1);
			
		}
		//AOI22
		else if(g->getType() == "AOI22") {
			string v1 = g->getName() + "_v1_2";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);

			string v2 = g->getName() + "_v2_2";
			IlpAnd(fanin[2],fanin[3],v2,file, 0);

			string out = w->getName() + "_2";
			IlpOr(v1,v2,out,file, 1);
		}
		//AOI211
		else if(g->getType() == "AOI211") {
			string v1 = g->getName() + "_v1_2";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);

			string v2 = g->getName() + "_v2_2";
			IlpOr(v1,fanin[2],v2,file, 0);

			string out = w->getName() + "_2";
			IlpOr(v2,fanin[3],out,file, 1);
		}
		//AOI221
		else if(g->getType() == "AOI221") {
			string v1 = g->getName() + "_v1_2";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);

			string v2 = g->getName() + "_v2_2";
			IlpAnd(fanin[2],fanin[3],v2,file, 0);

			string v3 = g->getName() + "_v3_2";
			IlpOr(v2,fanin[4],v3,file, 0);
			

			string out = w->getName() + "_2";
			IlpOr(v1,v3,out,file, 1);
		}
		//AOI222
		else if(g->getType() == "AOI222") {
			string v1 = g->getName() + "_v1_2";
			IlpAnd(fanin[0],fanin[1],v1,file, 0);

			string v2 = g->getName() + "_v2_2";
			IlpAnd(fanin[2],fanin[3],v2,file, 0);

			string v3 = g->getName() + "_v3_2";
			IlpAnd(fanin[5],fanin[4],v3,file, 0);

			string v4 = g->getName() + "_v4_2";
			IlpOr(v1,v2,v4,file, 0);
			
			string out = w->getName() + "_2";
			IlpOr(v3,v4,out,file, 1);
		}
		//OAI21
		else if(g->getType() == "OAI21") {
			string v1 = g->getName() + "_v1_2";
			IlpOr(fanin[0],fanin[1],v1,file, 0);

			string out = w->getName() + "_2";
			IlpAnd(v1,fanin[2],out,file, 1);
		}
		//OAI22
		else if(g->getType() == "OAI22") {
			string v1 = g->getName() + "_v1_2";
			IlpOr(fanin[0],fanin[1],v1,file, 0);

			string v2 = g->getName() + "_v2_2";
			IlpOr(fanin[2],fanin[3],v2,file, 0);

			string out = w->getName() + "_2";
			IlpAnd(v1,v2,out,file, 1);
		}
		//OAI211
		else if(g->getType() == "OAI211") {
			string v1 = g->getName() + "_v1_2";
			IlpOr(fanin[0],fanin[1],v1,file, 0);

			string v2 = g->getName() + "_v2_2";
			IlpAnd(v1,fanin[2],v2,file, 0);

			string out = w->getName() + "_2";
			IlpAnd(v2,fanin[3],out,file, 1);
		}
		//OAI221
		else if(g->getType() == "OAI221") {
			string v1 = g->getName() + "_v1_2";
			IlpOr(fanin[0],fanin[1],v1,file, 0);

			string v2 = g->getName() + "_v2_2";
			IlpOr(fanin[2],fanin[3],v2,file, 0);

			string v3 = g->getName() + "_v3_2";
			IlpAnd(v2,fanin[4],v3,file, 0);
			

			string out = w->getName() + "_2";
			IlpAnd(v1,v3,out,file, 1);
		}
		//OAI222
		else if(g->getType() == "OAI222") {
			string v1 = g->getName() + "_v1_2";
			IlpOr(fanin[0],fanin[1],v1,file, 0);

			string v2 = g->getName() + "_v2_2";
			IlpOr(fanin[2],fanin[3],v2,file, 0);

			string v3 = g->getName() + "_v3_2";
			IlpOr(fanin[5],fanin[4],v3,file, 0);

			string v4 = g->getName() + "_v4_2";
			IlpAnd(v1,v2,v4,file, 0);
			
			string out = w->getName() + "_2";
			IlpAnd(v3,v4,out,file, 1);
		}
		//OAI33
		else if(g->getType() == "OAI33") {
			string v1 = g->getName() + "_v1_2";
			IlpOr(fanin[0],fanin[1],v1,file, 0);

			string v2 = g->getName() + "_v2_2";
			IlpOr(fanin[2],v1,v2,file, 0);

			string v3 = g->getName() + "_v3_2";
			IlpOr(fanin[3],fanin[4],v3,file, 0);

			string v4 = g->getName() + "_v4_2";
			IlpOr(v3,fanin[5],v4,file, 0);

			string out = w->getName() + "_2";
			IlpAnd(v2,v4,out,file, 1);
		}
		//MUX
		else if(g->getType() == "MUX") {
			string v1;
			v1 = g->getName() + "_v1_2";
			file << " c" << _constraint++ << ": " << v1 << " + " << fanin[2] << " = 1\n";
			string v2 = g->getName() + "_v2_2";
			IlpAnd(fanin[0], v1, v2, file, 0);

			string v3 = g->getName() + "_v3_2";
			IlpAnd(fanin[1],fanin[2],v3,file, 0);

			string out = w->getName() + "_2";
			IlpOr(v2, v3, out, file, 0);
		}
		//FA
		else if(g->getType() == "FA") {
			string v1 = g->getName() + "_v1_2";
			IlpOr(fanin[0], fanin[1], v1, file, 0);
			variable.push_back(v1);

			string v2 = g->getName() + "_v2_2";
			IlpAnd(fanin[2], v1, v2, file, 0);
			variable.push_back(v2);

			string v3 = g->getName() + "_v3_2";
			IlpAnd(fanin[0], fanin[1], v3, file, 0);

			string out = w->getName() + "_2";
			IlpOr(v2,v3,out,file, 0);

		
			string v4 = g->getName() + "_v4_2";
			IlpXor(fanin[0], fanin[1], v4, file, 0);

			string qn = g->getQN()->getName() + "_2";
			IlpXor(v4,fanin[2],qn,file, 0);
		}
		//XOR
		else if(g->getType() == "XOR") {
			string out = w->getName() + "_2";
			IlpXor(fanin[0],fanin[1],out,file, 0);
		}
		//XNOR
		else if(g->getType() == "XNOR") {
			string out = w->getName() + "_2";
			IlpXor(fanin[0],fanin[1],out,file, 1);
		}
		//INV
		else if(g->getType() == "INV") {
			if(w->getName() != g->getFanin(0)->getName())
				file << " c" << _constraint++ << ": " << w->getName() << "_2 + " << g->getFanin(0)->getName() << "_2 = 1\n";
		}
		else {
			assert(g->getType() == "BUF" || g->getType() == "CLKBUF" || g->getType() == "Dummy");
			file << " c" << _constraint++ << ": " << w->getName() << "_2 - " << g->getFanin(0)->getName() << "_2 = 0\n";
		}	
	}
////////////////End TimeFrame2/////////////////////////

/////////////////Unknown Input/////////////////////////
	for(CWire*w: _vPi) {
		if(w->getValue() != 2) continue;
		file << " c" << _constraint++ << ": " << w->getName() << " - " << w->getName() << "_2 = 0\n";
	}


//////////Toggle of Die0///////////////////
	for(CGate* g: _vGate) {
		if(g->getDie() == 1) continue;
		CWire* w = g->getFanout();
		if(w->getTimeValue(0) != 2) file << " c" << _constraint++ << ": " << w->getName() << " = " << w->getTimeValue(0) << "\n";
		if(w->getTimeValue(1) != 2) file << " c" << _constraint++ << ": " << w->getName() << "_2 = " << w->getTimeValue(1) << "\n";

		string v1 = w->getName();
		string v2 = w->getName() + "_2";
		string out = w->getName() + "_toggle";

		IlpXor(v1,v2,out,file,0);

		if(g->getQN() != 0) {
			w = g->getQN();
			if(w->getTimeValue(0) != 2) file << " c" << _constraint++ << ": " << w->getName() << " = " << w->getTimeValue(0) << "\n";
			if(w->getTimeValue(1) != 2) file << " c" << _constraint++ << ": " << w->getName() << "_2 = " << w->getTimeValue(1) << "\n";

			v1 = w->getName();
			v2 = w->getName() + "_2";
			out = w->getName() + "_toggle";

			IlpXor(v1,v2,out,file,0);
		}
	}
////////////End Toggle///////////////


	
	file << "Binary\n" << " ";
	for(CWire* w: _vWire) {
		if(w->getTimeValue(0) == 2) file << " " << w->getName();
		if(w->getTimeValue(1) == 2) file << " " << w->getName() << "_2";
	}
	file << endl << "End\n";

	file.close();	
}

void CCircuit::IlpAnd(string n1, string n2, string nout, std::ofstream& file, bool inverse) {
	if(!inverse) {
		file << " c" << _constraint++ << ": " <<nout << " - " << n1 << " <= 0\n";
		file << " c" << _constraint++ << ": " <<nout << " - " << n2 << " <= 0\n";
		file << " c" << _constraint++ << ": " <<nout << " - " << n1 << " - " << n2 << " >= -1\n";
	}
	else {
		file << "	c" << _constraint++ << ": - " <<nout << " - " << n1 << " <= -1\n";
		file << "	c" << _constraint++ << ": - " <<nout << " - " << n2 << " <= -1\n";
		file << "	c" << _constraint++ << ": - " <<nout << " - " << n1 << " - " << n2 << " >= -2\n";
	}
}

void CCircuit::IlpOr(string n1, string n2, string nout, std::ofstream& file, bool inverse) {
	if(!inverse) {
		file << "	c" << _constraint++ << ": " <<nout << " - " << n1 << " >= 0\n";
		file << "	c" << _constraint++ << ": " <<nout << " - " << n2 << " >= 0\n";
		file << "	c" << _constraint++ << ": " <<nout << " - " << n1 << " - " << n2 << " <= 0\n";
	}
	else {
		file << "	c" << _constraint++ << ": - " <<nout << " - " << n1 << " >= -1\n";
		file << "	c" << _constraint++ << ": - " <<nout << " - " << n2 << " >= -1\n";
		file << "	c" << _constraint++ << ": - " <<nout << " - " << n1 << " - " << n2 << " <= -1\n";
	}
}

void CCircuit::IlpXor(string n1, string n2, string nout, std::ofstream& file, bool inverse) {
	if(!inverse) {
		file << "	c" << _constraint++ << ": " <<nout << " - " << n1 << " - " << n2 << " <= 0\n";
		file << "	c" << _constraint++ << ": " <<nout << " - " << n1 << " + " << n2 << " >= 0\n";
		file << "	c" << _constraint++ << ": " <<nout << " + " << n1 << " - " << n2 << " >= 0\n";
		file << "	c" << _constraint++ << ": " <<nout << " + " << n1 << " + " << n2 << " <= 2\n";
	}
	else {
		file << "	c" << _constraint++ << ": - " <<nout << " - " << n1 << " - " << n2 << " <= -1\n";
		file << "	c" << _constraint++ << ": - " <<nout << " - " << n1 << " + " << n2 << " >= -1\n";
		file << "	c" << _constraint++ << ": - " <<nout << " + " << n1 << " - " << n2 << " >= -1\n";
		file << "	c" << _constraint++ << ": - " <<nout << " + " << n1 << " + " << n2 << " <= 1\n";
	}
/*file << nout << " <= " << n1 << " + " << n2 << ";\n";
	file << nout << " >= " << n1 << " - " << n2 << ";\n";
	file << nout << " >= " << n2 << " - " << n1 << ";\n";
	file << nout << " <= " << "2 - " << n1 << " - " << n2 << ";\n";*/	
}
/*
void CCircuit::IlpNot(string n1, string nout, std::ofstream& file, 0) {
	file << "	c" << _constraint++ << ": " <<nout << " + " << n1 << " = 1\n"; 
}*/

void CCircuit::checkILP() {
	FILE* f;
	char name[128];
	int v;
	/*
	f = fopen("newxfill.sol", "r");
	while(fscanf(f, "%s %d", name, &v) != EOF) {
		string n = name;
		if(n.size() < 3) continue;
		if(n.find("toggle") != string::npos) continue;
		else if(n.substr(n.size()-3,2) == "_v") continue;
		else if(n.substr(n.size()-2) == "_2") continue;

		CWire* w = getWireFromName(n);
		w->setValue(v);	
	}*/	

	ofstream file("ilp_pat4.stil");
	int i = 1;
	for(vector<CGate*> vG: _vvScanChain) {
		file << "      \"test_si" << i++ << "\"=";
		for(int j = vG.size()-1; j>=0; --j) {
			file << vG[j]->getFanout()->getValue();
		}
		file << ";\n";
	}
}

bool CCircuit::gurobi() {
  try {
    GRBEnv env = GRBEnv();
    GRBModel model = GRBModel(env, "leonxfill.lp");

		model.set("MIPGapAbs", "20.0");
		//model.set("TimeLimit", "500.0");
    model.optimize();

    int optimstatus = model.get(GRB_IntAttr_Status);

    if (optimstatus == GRB_INF_OR_UNBD) {
      model.set(GRB_IntParam_Presolve, 0);
			//model.set("MIPGapAbs", "10.0");
      model.optimize();
      optimstatus = model.get(GRB_IntAttr_Status);
    }

    if (optimstatus == GRB_OPTIMAL) {
      double objval = model.get(GRB_DoubleAttr_ObjVal);
      cout << "Optimal objective: " << objval << endl;

			for(CWire* w: _vPi) {
				if(w->getTimeValue(0) != 2) {
					w->setValue(w->getTimeValue(0));
					continue;
				}
				GRBVar v = model.getVarByName(w->getName());
				int value = (int)v.get(GRB_DoubleAttr_X);
				assert(value==0 || value==1);
				w->setTimeValue(value,0);	
				w->setValue(value);
			}
			
			for(vector<CGate*> vG:_vvScanChain) {
				for(CGate* g: vG) {
					if(g->getFanout()->getTimeValue(0) == 2) {
						GRBVar v = model.getVarByName(g->getFanout()->getName());
						int value = (int)v.get(GRB_DoubleAttr_X);
						assert(value==0 || value==1);
						g->getFanout()->setTimeValue(value,0);
						if(g->getQN()!=0) g->getQN()->setTimeValue(1-value,0);
					}
					g->getFanout()->setValue(g->getFanout()->getTimeValue(0));
					if(g->getQN()!=0) g->getQN()->setValue(g->getQN()->getTimeValue(0));
				}
			}
			return true;
			//GRBVar* vars = model.getVars();
			//cout << vars[0].get(GRB_DoubleAttr_X);
			//cin.get();
    } else if (optimstatus == GRB_INFEASIBLE) {
      cout << "Model is infeasible" << endl;

      // compute and write out IIS

      //model.computeIIS();
      //model.write("model.ilp");
    } else if (optimstatus == GRB_UNBOUNDED) {
      cout << "Model is unbounded" << endl;
    } else {
      cout << "Optimization was stopped with status = "
           << optimstatus << endl;
    }

  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  } catch (...) {
    cout << "Error during optimization" << endl;
		cin.get();
  }
	return false;
}

void CCircuit::testonly(string pattern) {

	ifstream file(pattern);
	string c;
	bool first = true;
	bool final_pat = false;
	int pat_cnt = 0;
	vector<int> vWSA;
	vector<int> miv;
	vector<int> vdie0;
	vector<int> vdie1;
	while(getline(file,c)) {
		if(c == "   Macro \"test_setup\";") break;
	}
	while(getline(file,c)) { // Ann {}
		cout << "Start Pattern " << pat_cnt << endl;
		getline(file,c); // pattern #
		
		if(c.find("end") != string::npos) final_pat = true;
		if(!first) { // unload
			string outval;
			for(int i = 0; i < _vvScanChain.size(); ++i) {
				outval.clear();
				while(getline(file,c)) {
					outval += c;
					if(c.find(";") != string::npos) break;
				}
				if(pat_cnt == 1) continue;
				outval = outval.substr(outval.find("=")+1, outval.find(";")-outval.find("=")-1);
				assert(outval.size() == _vvScanChain[i].size());

				for(int j = 0; j < _vvScanChain[i].size(); ++j) {
					CGate* g = _vvScanChain[i][_vvScanChain[i].size()-1-j];
					if(outval[j] == 'X') {
						continue;
					}
					else if(outval[j] == 'H') {
						if(g->getFanin(0)->getValue() != 1 && g->getFanin(0)->getValue() != 3){
							cout << "H! ScanChain: " << i << ", #" << j << " Failed!!!\n";
							cout << "\t" << g->getName() << "Value: " << g->getFanout()->getValue() <<endl;
						}
					}
					else {
						if(g->getFanin(0)->getValue() != 0 && g->getFanin(0)->getValue() != 4){
							cout << "L! ScanChain: " << i << ", #" << j << " Failed!!!\n";
							cout << "\t" << g->getName() << "Value: "<< g->getFanin(0)->getValue() <<endl;
						}
					}
				}
			}
		}
		
		if(final_pat) break;
		reset(); 
	
		
		for(int i = 0; i < _vvScanChain.size(); ++i) { // load
			string val;
			while(getline(file,c)) {
				val += c;
				if(c.find(";") != string::npos) break;
			}
			//getline(file,c);
			size_t start = val.find("=")+1;
			val = val.substr(val.find("=")+1, val.find(";")-val.find("=")-1);
			assert(val.size() == _vvScanChain[i].size());
			for(int j = 0; j < _vvScanChain[i].size(); ++j) {
				CGate* g = _vvScanChain[i][_vvScanChain[i].size()-1-j];
				if(val[j] == 'N') {
					int r = rand()%2;
					g->getFanout()->setValue(2);
					if(g->getQN()!=0) g->getQN()->setValue(2);	
					continue;
				}
				int v = val[j]-'0';
				assert(v==0 || v==1);
				g->getFanout()->setValue(v);
				if(g->getQN() != 0) {
					if(v==1) g->getQN()->setValue(0);
					else g->getQN()->setValue(1);
				}
			}
		}
		//Skip Multi cpature//
		if(!first) {	
		  while(getline(file,c)) {
		  		if(c.find(";") != string::npos) break;
		  }
		}	
		/////////////////////

		getline(file,c); // Call capture
		//getline(file,c); // _pi
		string pi;
		while(getline(file,c)) {
			pi += c;
			if(c.find(";") != string::npos) break;
		}
		pi = pi.substr(pi.find("=")+1, pi.find(";")-pi.find("=")-1);
		assert(pi.size() == _vPi.size());
		string pi_next;
		bool reset_pulse = false;
		bool oneclock_first = false;
		if(!first) {
			getline(file,c); // Call capture
			if(c.find("Xs") != string::npos) getline(file,c);
			while(getline(file,c)) {
				pi_next += c;
				if(c.find(";") != string::npos) break;
			}
			pi_next = pi_next.substr(pi_next.find("=")+1, pi_next.find(";")-pi_next.find("=")-1);
					


			
			int v, v2;
			for(int i = 0; i < pi.size(); ++i) {
				if(pi[i] == 'P') _vPi[i]->setValue(0);
				else if(pi[i] == 'N') {
					int r = rand()%2;
					_vPi[i]->setValue(2);
				}
				else{
					_vPi[i]->setValue(pi[i]-'0');
				}
			}
			evaluate();
			renewPPIs_for_testonly();
		}
		else {
			for(int i = 0; i < pi.size(); ++i) {
				if(pi[i] == 'N') {_vPi[i]->setValue(2);continue;}
				else if(pi[i] == 'P') {_vPi[i]->setValue(0);continue;}
				int v = pi[i]-'0';
				_vPi[i]->setValue(v);
			}	
		}
		if(reset_pulse) {
			cout << "RESET!!!\n";
			for(CGate* g: _vGate) {
				g->getFanout()->setValue(0);
				if(g->getQN() != 0) g->getQN()->setValue(1);
			}
		}
		evaluate();
		dumpSTA(first, pat_cnt);
		
		int WSA = 0;
		int WSA_die0 = 0;
		int WSA_die1 = 0;
		for(CGate* g: _vGate) {
			if(g->getType() == "Dummy") continue;
			if(g->getFanout()->getValue() > 2) {
				WSA += (1+g->getFanout()->getFanoutNum());
				if (g->getDie() == 0) {
					 WSA_die0 += (1+g->getFanout()->getFanoutNum());
				}
				else {
					 WSA_die1 += (1+g->getFanout()->getFanoutNum());
				}
				if(g->getQN() !=0 && g->getQN()->getValue() > 2) {
					WSA += (1+g->getQN()->getFanoutNum());
			  	if (g->getDie() == 0) {
			  		 WSA_die0 += (1+g->getQN()->getFanoutNum());
			  	}
			  	else {
			  		 WSA_die1 += (1+g->getQN()->getFanoutNum());
			  	}
				}	
			}
		}
		vWSA.push_back(WSA);
		vdie0.push_back(WSA_die0);
		vdie1.push_back(WSA_die1);
		int weight = 0;
		for(CWire* w: _vWire) {
			if(w->getValue() > 2) {
				int d1 = 0;
				if(w->getType() > 0) d1 = w->getFanin()->getDie();
				for(int i = 0; i < w->getFanoutNum();++i) {
					if(w->getFanout(i)->getDie() != d1) ++weight;
				}
			}
		}
		miv.push_back(weight);
		cout << "WSA: " << WSA << endl;
		cout << "WSA_die0: " << WSA_die0 << endl;

		string po;
		while(getline(file,c)) {
			po += c;
			if(c.find(";") != string::npos) break;
		}
		po = po.substr(po.find("=")+1, po.find(";")-po.find("=")-1);
		assert(po.size() == _vPo.size());

		
		for(int i = 0; i < po.size(); ++i) {
			if(po[i] == 'X') {
				continue;
			}
			else if(po[i] == 'H') {
				if(_vPo[i]->getValue() != 1 && _vPo[i]->getValue() != 3) {
					cout << "Po: " << _vPo[i]->getName() << " Failed!!\n" << "i=" << i << ",po[i]=H, _vPo[i]=" << _vPo[i]->getValue() << endl;
				}
			}
			else {
				if(_vPo[i]->getValue() != 0 && _vPo[i]->getValue() != 4) {
					cout << "Po: " << _vPo[i]->getName() << " Failed!!\n" << "i=" << i << ",po[i]=L, _vPo[i]=" << _vPo[i]->getValue() << endl;
				}
			}
		}
		first = false;
		++pat_cnt;
	}
	file.close();

	ofstream outfile1("toggle.csv");
	for(int i = 0; i < vWSA.size(); ++i) {
		outfile1 << i << "," << vWSA[i] << "," << vdie0[i] << "," << vdie1[i] << "," << miv[i] << "\n"; 
	}
}
