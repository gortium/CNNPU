//Author: Francois Leduc-Primeau (francoislp@gmail.com)

#include <ostream>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <functional>
#include "../ext/config/config.hpp"

#include "fi/CFixed.hpp"
//#include "fi/overflow/Throw.hpp"
#include "fi/overflow/Saturate.hpp"
#include "fi/rounding/Classic.hpp"

#include "models/DFTUnit.hpp"
#include "models/types.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
using std::ofstream;
using std::string;
using std::vector;


// Forward declarations
void setAuthorizedKeys(config& conf);


int main(int argc, char** argv) {

	// print usage if no arguments are provided
	if(argc==1) {
		cout << "Usage: ./TestDFTUnit [key1=val1] [key2=val2] [--option1] ..." << endl;
    cout << "To read the parameters from a file, use \"config=<path to config file>\""<<endl;
    cout << "To print the list of supported keys and options, use --help" <<endl;
		return 1;
	}

	config conf;
	setAuthorizedKeys(conf);
  // parse the command line arguments
  try {
    conf.initCL(argc, argv);
  } catch(syntax_exception& e) {
    cerr << "Invalid syntax in cmd-line arguments: "<<e.what()<<endl;
    return 1;
  } catch(invalidkey_exception& e) {
    cerr << "Invalid key or option: "<<e.what()<<endl;
    return 1;
  }

  if(conf.checkOption("help")) {
	  cout << "Possible configuration keys (* = mandatory):" <<endl;
	  cout << "  config:       File path to a configuration file" <<endl;	  
	  cout << "  radix*:       Size of the DFT" << endl;
	  cout << "  nb_inputs*:   Number of DFTs to be evaluated" << endl;
	  cout << "  scaling:      Random inputs in [-1,1] are divided by this value, which can be any real number (default=radix)" <<endl;
	  cout << "  in_file:      Print the input sequence to the specified file" <<endl;
	  cout << "  out_file:     Print the output sequence to the specified file (default is std out)" <<endl;
	  cout << "  twiddle_file: Print the twiddle factors to the specified file" << endl;
	  cout << "  col_size:     Truncate/pad base-10 values to a fixed number of digits when printing" << endl;
	  
	  cout << "Possible options:" <<endl;
	  cout << "  --help:     Print this message" <<endl;
	  return 0;
  }

  // if a configuration file is specified, load its content
  try {
    std::string confpath = conf.getParamString("config");
    conf.initFile(confpath);
  } catch(key_not_found& e) {
	  //cerr << "Warning: No configuration file specified, using command line arguments only." << endl;
  } catch(invalidkey_exception& e) {
    cerr << "Invalid key: "<<e.what()<<endl;
    return 1;
  } catch(file_exception& e) {
    cerr << "Error reading from file at " << e.what() << endl;
    return 1;
  } catch(syntax_exception& e) {
    cerr << "Invalid syntax: "<<e.what() << endl;
    return 1;
  }	

  int radix=0;
  int nbInputs=0;
  try {
	  radix= conf.parseParamUInt("radix");
	  nbInputs= conf.parseParamUInt("nb_inputs");
  } catch(key_not_found& e) {
	  cerr << "Key not found: " << e.what()<<endl;
	  return 1;
  }

	// print the input sequence to a file if file is specified
  ofstream* ofs_In = 0;
	try {
		string inFile= conf.getParamString("in_file");
		ofs_In= new ofstream(inFile.c_str());
		if(!ofs_In->good()) {
			cerr << "Could not open "<<inFile << endl;
			return 1;
		}
	} catch(key_not_found&) {}

	// print output sequence to a file or to std out
	ostream* os_Out;
	if(conf.keyExists("out_file")) { // output to a file
		string outFile= conf.getParamString("out_file");
		os_Out= new ofstream(outFile.c_str());
		if(!os_Out->good()) {
			cerr << "Could not open "<<outFile << endl;
			return 1;
		}
	} else { // output to std out
		os_Out= &cout;
	}

	// Ability to fix the number of digits used when printing out numbers
	int colSize= 0; // 0 means variable number of digits
	if(conf.keyExists("col_size")) {
		colSize= conf.parseParamUInt("col_size");
	}

	// instantiate RNG
	std::default_random_engine generator;
	std::uniform_real_distribution<double> distribution(-1.0,1.0);
	auto rng= std::bind(distribution, generator);	

	DFTUnit<Fx_t,FxW_t,twid_t> unit(radix);

	if(conf.keyExists("twiddle_file")) {
		string twiddleFile= conf.getParamString("twiddle_file");
		ofstream ofsTwiddle(twiddleFile.c_str());
		if(!ofsTwiddle.good()) {
			cerr << "Could not open "<< twiddleFile << endl;
			return 1;
		}
		unit.printTwiddle(&ofsTwiddle);
		ofsTwiddle.close();
	}

	// use the specified scaling factor if provided (defaults to the radix value)
	double sf= radix;
	if(conf.keyExists("scaling")) {
		sf= conf.parseParamDouble("scaling");
	}

  for(int i=0; i<nbInputs; i++) {
	  // generate random inputs
	  vector<Fx_t> curInputs;
	  for(int j=0; j<radix; j++) {
		  Fx_t x(rng()/sf, rng()/sf);
		  curInputs.push_back(x);
	  }

	  // print out current inputs if a file has been specified
	  if(ofs_In != 0) {
		  for(int j=0; j<radix; j++) {
			  if(j!=0) (*ofs_In) << "  ";
			  if(colSize==0) {
				  (*ofs_In) << curInputs.at(j).real().toString() << " ";
				  (*ofs_In) << curInputs.at(j).imag().toString();
			  } else {
				  (*ofs_In) << curInputs.at(j).real().toString(colSize) << " ";
				  (*ofs_In) << curInputs.at(j).imag().toString(colSize);
			  }
		  }
		  (*ofs_In) << endl;
	  }

	  vector<Fx_t> curOutputs;
	  unit.step(curInputs, curOutputs);

	  // print out current outputs
	  for(int j=0; j<radix; j++) {
		  if(j!=0) *os_Out << "  ";
		  if(colSize==0) {
			  (*os_Out) << curOutputs.at(j).real().toString() << " ";
			  (*os_Out) << curOutputs.at(j).imag().toString();
		  } else {
			  (*os_Out) << curOutputs.at(j).real().toString(colSize) << " ";
			  (*os_Out) << curOutputs.at(j).imag().toString(colSize);
		  }
	  }
	  (*os_Out)<< endl;
  }


  // Note: We omit the deletion of os_Out and ofs_In

  return 0;
} // end main

void setAuthorizedKeys(config& conf) {
	// key-value pairs
	conf.addValidKey("config");
	conf.addValidKey("radix");
	conf.addValidKey("N");
	conf.addValidKey("nb_inputs");
	conf.addValidKey("scaling");
	conf.addValidKey("in_file");
	conf.addValidKey("out_file");
	conf.addValidKey("twiddle_file");
	conf.addValidKey("col_size");
	// options
	conf.addValidOption("help");
}
