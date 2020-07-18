/**HeaderFile**********************************************************

  FileName    [circuit.hpp]

  SystemName  [LuLuTPG: M3D Test Pattern Generation.]

  PackageName [circuit: circuit classes.]

  Synopsis    [circuit functions declaration]

  Author      [Shao-Chun Hung]
  
  Affiliation [Duke University]

  Date        [26, Feb., 2020.]

***********************************************************************/
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <cassert>
#include <fstream>
#include <string>

#include "gurobi_c++.h"

using namespace std;
class CWire;
class CGate;

class CWire {
public:
	CWire(string name, int type) {
		_name = name;
		_type = type;
		_value = -1;
		_prob = -1;
		_ilp = false;
	};
	~CWire() {};
	
	void addFaninGate(CGate* g) {_fanin = g;}
	void addFanoutGate(CGate* g) {_vFanout.push_back(g);}
	string getName() {return _name;}
	CGate* getFanin() {
		return _fanin;
	}
	CGate* getFanout(int i) {
		if(i >= _vFanout.size()) {
			cout << "Invalid fanout: " << i << endl;
			return 0;
		}
		else return _vFanout[i];
	}
	int getFanoutNum() {return _vFanout.size();}
	int getType() {return _type;}
	int getValue() {return _value;}
	void setValue(int i) {_value = i;}
	void setProb(float i) {_prob = i;}
	float getProb() {return _prob;}
	void setIlp(bool i) {_ilp = i;}
	bool getIlp() {return _ilp;}
	void setTimeValue(int i, int time) {
		if(time==0) _v1 = i;
		else  _v2 = i;
	}
	int getTimeValue(int time) {
		if(time == 0) return _v1;
		else return _v2;
	}
private:
	string _name;
	int	_value; 								// Wire value
	int _type;									// 0: input 1:output 2:wire
	float _prob;								// Probability for Preferred X-filling
	CGate* _fanin;							// Fanin
	vector<CGate*> _vFanout;			// Fanouts
	bool _ilp;
	int _v1;
	int _v2;
};

class CGate {
public:
	CGate() {
		_level = -1;
		_QN = 0;
		_eval = false;
		_die = -1;
		_glitch = false;
	};
	CGate(string type) {
		_type = type;
		_level = -1;
		_QN = 0;
		_eval = false;
		_die = -1;
		_glitch = false;
	};
	~CGate() {};
	
	void addType(string type) {_type = type;}
	void addName(string name) {_name = name;}
	void addFanin(CWire* w)	{_vFanin.push_back(w);}
	void addFanout(CWire* w) {_fanout = w;}
	CWire* getFanout() {return _fanout;} 
	string getName() {return _name;}
	string getType() {return _type;}
	void addVLocation(int i) {_VLoc = i;}
	int getVLocation() {return _VLoc;}
	int getFaninNum() {return _vFanin.size();}
	CWire* getFanin(int i) {return _vFanin[i];}
	int getLevel() {return _level;}
	void setLevel(int l) {
		if (_level == -1) _level = l;
		else _level = max(_level, l);
	}
	void addQN(CWire* w) {_QN = w;}
	CWire* getQN() {return _QN;}
	bool getEval() {return _eval;}
	void setDie(int i) {_die = i;}
	int	getDie() {return _die;}

	int evalNot(int);
	int evalAnd(int, int);
	int evalOr(int, int);
	int evalXor(int, int);
	int eval();

	float probNot(float);
	float probAnd(float, float);
	float probOr(float, float);
	float probXor(float, float);
	void evalProb();
private:
	string _name;
	string _type;
	vector<CWire*> _vFanin;
	CWire* _fanout; 
	CWire* _QN;		//for SDFF
	int _VLoc;		//location in circuit.vGate
	int _level; 	//level
	bool _eval;
	int _die;			//die
	bool _glitch; //glitch
};

class CCircuit {

public:
	CCircuit() {};
	~CCircuit() {};
	
	//void addGate(CGate* g) {_vGate.push_back(g);}
	void addPi(CWire* w, string name) {
	//	pair<map<string,int>::iterator,bool> ret;
	//	ret = _mWire.insert(pair<string,int>(name, _vPi.size()));
	//	if(ret.second == false) {
	//		cout << name << "already exists with value:" << ret.first->second << endl;
	//	}
		_vPi.push_back(w);
	}
	void addPo(CWire* w, string name) {
	//	pair<map<string,int>::iterator,bool> ret;
	//	ret = _mWire.insert(pair<string,int>(name, _vPi.size()));
	//	if(ret.second == false) {
	//		cout << name << "already exists with value:" << ret.first->second << endl;
	//	}
		_vPo.push_back(w);
	}
	void addPpi(string name) {_vPpi.push_back(getWireFromName(name));}
	void addPpo(string name) {_vPpo.push_back(getWireFromName(name));}
	void addWire(CWire* w, string name) {
		pair<map<string,int>::iterator,bool> ret;
		ret = _mWire.insert(pair<string,int>(name, _vWire.size()));
		if(ret.second == false) {
			cout << name << "already exists with value:" << ret.first->second << endl;
		}
		_vWire.push_back(w);
	}
	CWire* getWire(int i) {return _vWire[i];}
	CGate* getGate(int i) {return _vGate[i];}
	CWire* getWireFromName(string name) {
		return getWire(_mWire[name]);
	}
	CGate* getGateFromName(string name) {
		return _vGate[_mGate[name]];
	}
	void addGate(CGate* g) {
		pair<map<string,int>::iterator,bool> ret;
		ret = _mGate.insert(pair<string,int>(g->getName(), _vGate.size()));
		if(ret.second == false) {
			cout << g->getName() << "already exists with value:" << ret.first->second << endl;
		}
		g->addVLocation(_vGate.size());
		_vGate.push_back(g);
		_vCompute.push_back(g);
	}
	CWire* getPo(int i) {return _vPi[i];}
	void dumpSTA(bool, int);
	void dumpILP();
	void levelize();
	void evaluate();
	void function();
	void test(string);
	void testonly(string);
	void reset();
	void assignInputs();
	void renewPPIs();
	void renewPPIs_for_testonly();
	void addScanChain(vector<CGate*> vGate) {_vvScanChain.push_back(vGate);}
	vector<CGate*> getScanChain(int i) {return _vvScanChain[i];}
	int getScanChainNum() {return _vvScanChain.size();}
	int getPiNum() {return _vPi.size();}
	int getPoNum() {return _vPo.size();}
	void Xfill();
	void IlpAnd(string, string, string, std::ofstream&, bool);
	void IlpOr(string, string, string, std::ofstream&, bool);
	void IlpXor(string, string, string, std::ofstream&, bool);
	void checkILP();
	bool gurobi();
	int CountLogicGate();
	/////////////////Debug////////////////////////
	void printWireFromName(string name) {
		CWire* w = getWireFromName(name);
		cout << "Wire:" << w->getName() << ",FaninGate:" << w->getFanin()->getName() << "(" << w->getFanin()->getLevel() << "),FanoutGate:";
		for(int i = 0;i < w->getFanoutNum(); ++i) cout << w->getFanout(i)->getName() << "(" << w->getFanout(i)->getLevel() << "),";
		cout << endl;
	}
	void printWires() {for(CWire* w: _vWire) cout << w->getName() << endl;}
	void printMap() {for(map<string,int>::iterator it=_mWire.begin(); it!=_mWire.end(); ++it) cout << it->first << " => " << it->second << endl;}
	void printGates() {
		int cnt = 0;
		for(CGate* g: _vGate) {
			if(g->getDie() != -1) continue;
			cout << "Gate: " << g->getName() << "does not partitioned\n";
			++cnt;
		}
		cout << "Ratio of non-partitioned gates: " << cnt << "/" << _vGate.size() << "=" << cnt/_vGate.size() << endl;
	}
	void printLevel() {
		for(CGate* g: _vGate) {
			cout << g->getName() << ", Type: " << g->getType() << ", Level:  " << g->getLevel() << endl;
		}
	}
	void printInputOutput() {
		cout << "PI: " << endl;
		for(CWire* w: _vPi) cout << "\t" << w->getName() << " Value: " << w->getValue() << endl;
		cout << "PO: " << endl;
		for(CWire* w: _vPo) cout << "\t" << w->getName() << " Value: " << w->getValue() << endl;
	//	for(int i = 0; i < _vvScanChain.size(); ++i) {
	//		for(int j = 0; j < _vvScanChain[i].size(); ++j) {
	//			CGate* g = _vvScanChain[i][j];
	//			CWire* w = g->getFanin(0);
	//		 	if(w->getValue() > 1) cout << "\t" << w->getName() << " Value: " << w->getValue() << endl;
	//		}
	//	}
		//for(CWire* w: _vWire) {
		// 	if(w->getValue() > 1) cout << "\t" << w->getName() << " Value: " << w->getValue() << endl;
		//}
	//	cout << "PPI: " << endl;
	//	for(CWire* w: _vPpi) cout << "\t" << w->getName() << " Value: " << w->getValue() << endl;
	//	cout << "PPO: " << endl;
	//	for(CWire* w: _vPpo) cout << "\t" << w->getName() << " Value: " << w->getValue() << endl;
	//	cout << "END PPO\n";
	//	for(CWire* w: _vWire) cout << w->getName() << " Value: " << w->getValue() << endl;	
	}
	void printFirstNet(CWire* w) {
		cout << w->getName() << "->"; 
		if(w->getType() == 1) return;
		cout << w->getFanout(0)->getName() << "(" << w->getFanout(0)->getLevel() << ")->";
		if(w->getFanout(0)->getType() == "SDFF") return;
		else printFirstNet(w->getFanout(0)->getFanout());
	}
	void printScanChain() {
		for(int i = 0; i < _vvScanChain.size(); ++i) {
			cout << "ScanChain#" << i << endl;
			for(CGate* g: _vvScanChain[i]) cout << g->getFanout()->getName() << " ";
			cout << endl;
		}
	}
	void backward(CGate* g) {
		cout << " " << g->getName() << " ";
		CWire* w = g->getFanin(0);
		cout << w->getName() << " "  << endl;
		printWireFromName(w->getName());
		return;
		if(w->getType() == 0 || g->getType() == "SDFF") return;
		else	{
			backward(w->getFanin());
		}
	}
	void writeOutput();
private:

	void levelize_dft(CWire*, int);
	int _maxlevel;
	int _constraint;
	
	vector<CWire*> _vPi;
	vector<CWire*> _vPo;
	vector<CWire*> _vPpi;
	vector<CWire*> _vPpo;
	vector<CWire*> _vWire;
	vector<CGate*> _vGate;
	map<string, int> _mWire;	
	map<string, int> _mGate;	
	vector< vector<CGate*> > _vvScanChain;
	vector<CGate*> _vCompute;
};
