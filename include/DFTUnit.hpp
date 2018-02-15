//Author: Francois Leduc-Primeau (francoislp@gmail.com)

#ifndef DFTUnit_hpp_
#define DFTUnit_hpp_

#include <vector>
#include <map>
#include <stdexcept>
#include <cmath>
#include <ostream>

#include "types.hpp"

/**
 * Objects that compute a fixed-size DFT on complex vectors.
 *
 *@tparam T      Type of input and output data.
 *@tparam WORK_T Data type used for internal computations. The assignment 
 *               operator must be defined from T to WORK_T and from WORK_T 
 *               to T.
 *@tparam TWID_T Data type used to represent twiddle factors.
 */
template <typename T, typename WORK_T, typename TWID_T>
class DFTUnit {
public:

	DFTUnit() { _radix= 0; }
	
	DFTUnit(unsigned int radix);

	/**
	 * Compute a DFT.
	 */
	void step(std::vector<T>& input, std::vector<T>& output);

	unsigned int latency() const { return 0; }

	/**
	 * Returns the radix of this block.
	 */
	unsigned int radix() const { return _radix; }

	/**
	 * Prints the twiddle factors and the corresponding value of k*n to
	 * the output stream provided.
	 */
	void printTwiddle(std::ostream* os) const;

private:

	TWID_T twiddle(double);

	// ---------- Data Members ----------
	
	std::map<int, TWID_T> _twiddleTable;
	
	unsigned int _radix;
};

// --------------- Templatized Implementation ---------------

template<typename T, typename WORK_T, typename TWID_T>
inline
DFTUnit<T,WORK_T,TWID_T>::DFTUnit(unsigned int radix) {
	if(radix<2) throw std::runtime_error("radix must be at least 2");
	_radix= radix;

	// populate twiddle factor table
	for(int i=0; i<_radix; i++) {
		for(int j=0; j<_radix; j++) {
			_twiddleTable[i*j]=  twiddle(static_cast<double>(i*j) /
			                             static_cast<double>(_radix));
		}
	}
}

//Note: This implementation is only valid for fixed-point types,
//      because of the use of the right-shift operator.
template<typename T, typename WORK_T, typename TWID_T>
inline
void DFTUnit<T,WORK_T,TWID_T>::step(std::vector<T>& in, std::vector<T>& out) {
	if(in.size() != _radix)
		throw std::runtime_error("Size of input vector does not match radix");

	out.resize(_radix);

	// assign inputs to the work type
	std::vector<WORK_T> work;
	work.resize(_radix);
	for(int i=0; i<_radix; i++) {
		work[i]= WORK_T::fromCFixed(in[i]);
	}
	
	// hardcoded expressions for DFT-3 and DFT-5
	if(_radix==3) {
		out[0]= T::fromCFixed(work[0] + work[1] + work[2]);

		TWID_T twTmp(0.5, sqrt(3.0)/2.0); //Note: The multiplication by 0.5
                                    //is implemented as a right shift.
		WORK_T tw1= WORK_T::fromCFixed(twTmp); // convert to work type
		auto curSumR= work[0].real();
		curSumR-= (work[1].real()+work[2].real()) >> 1;
		auto curTerm= work[1].imag()-work[2].imag();
		curTerm*= tw1.imag();
		curSumR+= curTerm;
		
		auto curSumI= work[0].imag();
		curSumI-= (work[1].imag()+work[2].imag()) >> 1;
		curTerm= work[2].real()-work[1].real();
		curTerm*= tw1.imag();
		curSumI+= curTerm;

		out[1]= T::fromCFixed(WORK_T(curSumR, curSumI));

		curSumR= work[0].real();
		curSumR-= (work[1].real()+work[2].real()) >> 1;
		curTerm= work[2].imag()-work[1].imag();
		curTerm*= tw1.imag();
		curSumR+= curTerm;

		curSumI= work[0].imag();
		curSumI-= (work[1].imag()+work[2].imag()) >> 1;
		curTerm= work[1].real()-work[2].real();
		curTerm*= tw1.imag();
		curSumI+= curTerm;

		out[2]= T::fromCFixed(WORK_T(curSumR, curSumI));
	}
	else if(_radix==5) {
		out[0]= T::fromCFixed(work[0] + work[1] + work[2] + work[3] + work[4]);

		TWID_T twTmp1((sqrt(5.0)-1.0)/4.0, sqrt((5.0+sqrt(5.0))/8.0));
		TWID_T twTmp2((sqrt(5.0)+1.0)/4.0, sqrt((5.0-sqrt(5.0))/8.0));
		WORK_T tw1= WORK_T::fromCFixed(twTmp1);
		WORK_T tw2= WORK_T::fromCFixed(twTmp2);
		out[1]= T::fromCFixed(WORK_T(work[0].real()
		          + (work[1].real()+work[4].real()) * tw1.real()
		          - (work[2].real()+work[3].real()) * tw2.real()
		          + (work[1].imag()-work[4].imag()) * tw1.imag()
		          + (work[2].imag()-work[3].imag()) * tw2.imag(),
		          work[0].imag()
		          + (work[1].imag()+work[4].imag()) * tw1.real()
		          - (work[2].imag()+work[3].imag()) * tw2.real()
		          - (work[1].real()-work[4].real()) * tw1.imag()
		          - (work[2].real()-work[3].real()) * tw2.imag()
		                             ));

		out[2]= T::fromCFixed(WORK_T(work[0].real()
		          + (work[3].real() + work[2].real()) * tw1.real()
		          - (work[1].real() + work[4].real()) * tw2.real()
		          + (work[3].imag() - work[2].imag()) * tw1.imag()
		          + (work[1].imag() - work[4].imag()) * tw2.imag(),
		          work[0].imag()
		          + (work[3].imag() + work[2].imag()) * tw1.real()
		          - (work[1].imag() + work[4].imag()) * tw2.real()
		          - (work[3].real() - work[2].real()) * tw1.imag()
		          - (work[1].real() - work[4].real()) * tw2.imag()
		                             ));

		out[3]= T::fromCFixed(WORK_T(work[0].real()
		          + (work[2].real()+work[3].real()) * tw1.real()
		          - (work[4].real()+work[1].real()) * tw2.real()
		          + (work[2].imag()-work[3].imag()) * tw1.imag()
		          + (work[4].imag()-work[1].imag()) * tw2.imag(),
		          work[0].imag()
		          + (work[2].imag() + work[3].imag()) * tw1.real()
		          - (work[4].imag() + work[1].imag()) * tw2.real()
		          - (work[2].real() - work[3].real()) * tw1.imag()
		          - (work[4].real() - work[1].real()) * tw2.imag()
		                             ));

		out[4]= T::fromCFixed(WORK_T(work[0].real()
		          + (work[4].real() + work[1].real()) * tw1.real()
		          - (work[3].real() + work[2].real()) * tw2.real()
		          + (work[4].imag() - work[1].imag()) * tw1.imag()
		          + (work[3].imag() - work[2].imag()) * tw2.imag(),
		          work[0].imag()
		          + (work[4].imag() + work[1].imag()) * tw1.real()
		          - (work[3].imag() + work[2].imag()) * tw2.real()
		          - (work[4].real() - work[1].real()) * tw1.imag()
		          - (work[3].real() - work[2].real()) * tw2.imag()
		                             ));
	}
	else {
		for(int i=0; i<_radix; i++) {
			WORK_T curSum(0.0,0.0);
			for(int j=0; j<_radix; j++) {
				WORK_T curTerm(work[j]);
				WORK_T curTw= WORK_T::fromCFixed(_twiddleTable.at(i*j));
				curTerm*= curTw;
				curSum+= curTerm;
			}
			out[i]= T::fromCFixed(curSum);
		}
	}
}

// Specialization for std::complex<double>
template<>
void DFTUnit<std::complex<double>,std::complex<double>,std::complex<double>>::
step(std::vector<std::complex<double> >& in,
     std::vector<std::complex<double> >& out) {
	if(in.size() != _radix)
		throw std::runtime_error("Size of input vector does not match radix");

	out.resize(_radix);
	
	for(int i=0; i<_radix; i++) {
		std::complex<double> curSum(0.0,0.0);
		for(int j=0; j<_radix; j++) {
			std::complex<double> curTerm(in[j]);
			curTerm*= _twiddleTable.at(i*j);
			curSum+= curTerm;
		}
		out[i]= curSum;
	}
}

template<typename T, typename WORK_T, typename TWID_T>
inline
void DFTUnit<T,WORK_T,TWID_T>::printTwiddle(std::ostream* os) const {
	(*os) << "# k*n  real  imag" << std::endl;
	for(auto it= _twiddleTable.cbegin(); it != _twiddleTable.cend(); it++) {
		(*os) << it->first << "  " << it->second.real().toString() << "  ";
		(*os) << it->second.imag().toString() << std::endl;
	}
}

// ---------- (private) ----------

template<typename T, typename WORK_T, typename TWID_T>
inline
TWID_T DFTUnit<T,WORK_T,TWID_T>::twiddle(double angle) {
	return TWID_T(cos(-angle*2*M_PI), sin(-angle*2*M_PI));
}

#endif
