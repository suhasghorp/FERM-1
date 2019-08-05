#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>
#include <Eigen/Dense>
#include "Hazard_Rate_Term_Structure.h"
#include "BDT.h"


#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>

using namespace std;

template<typename _Scalar, int NX = Eigen::Dynamic, int NY = Eigen::Dynamic>
struct Functor
{
	typedef _Scalar Scalar;
	enum {
		InputsAtCompileTime = NX,
		ValuesAtCompileTime = NY
	};
	typedef Eigen::Matrix<Scalar, InputsAtCompileTime, 1> InputType;
	typedef Eigen::Matrix<Scalar, ValuesAtCompileTime, 1> ValueType;
	typedef Eigen::Matrix<Scalar, ValuesAtCompileTime, InputsAtCompileTime> JacobianType;

	int m_inputs, m_values;

	Functor() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime) {}
	Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}

	int inputs() const { return m_inputs; }
	int values() const { return m_values; }

};

struct cds_functor : Functor<double>
{
	double m_notional;
	double m_recovery;
	double m_r;
	int m_maturity;
	int m_frequency;
	HazardRateTermStructure m_hzStructure;

	cds_functor(const HazardRateTermStructure& hzStructure, double notional, double recovery, double r, int maturity, int frequency) : Functor<double>(1, 1),
		m_hzStructure(hzStructure), m_notional(notional), m_recovery(recovery), m_r(r), m_maturity(maturity), m_frequency(frequency) {}

	int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
	{
		vector<double> vec(x.data(), x.data() + x.rows() * x.cols()); // spread is x
		CDS cds(vec[0], m_notional, m_recovery, m_r, m_maturity, m_frequency);
		fvec(0) = cds.price_cds(m_hzStructure);
		return 0;
	}
};

struct hazard_functor : Functor<double>
{
	vector<double> m_market_bond_prices;
	double m_r;
	int m_maturity;
	int m_frequency;

	hazard_functor(vector<double> market_bond_prices, double r, int maturity, int frequency) : Functor<double>((maturity*frequency) + 1, (maturity*frequency) + 1), m_market_bond_prices(market_bond_prices), m_r(r), m_maturity(maturity), m_frequency(frequency) {}
	int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
	{
		vector<double> vec(x.data(), x.data() + x.rows() * x.cols()); // hazard rates are x
		HazardRateTermStructure hazardRateTermStructure(m_maturity, m_frequency, vec, m_r);


		/*fvec(0) = 0.02;
		fvec(1) = 0.02;

		fvec(2) = m_market_bond_prices[0] - DefaultableBond(1, 2, 0.05, 100).get_bond_price(hazardRateTermStructure, 10.0);
		fvec(3) = 0;
		//fvec(3) = m_market_bond_prices[0] - DefaultableBond(1, 2, 0.05, 100).get_bond_price(hazardRateTermStructure, 10.0);

		fvec(4) = m_market_bond_prices[1] - DefaultableBond(2, 2, 0.02, 100).get_bond_price(hazardRateTermStructure, 25.0);
		fvec(5) = 0; // m_market_bond_prices[1] - DefaultableBond(2, 2, 0.02, 100).get_bond_price(hazardRateTermStructure, 25.0);

		fvec(6) = m_market_bond_prices[2] - DefaultableBond(3, 2, 0.05, 100).get_bond_price(hazardRateTermStructure, 50.0);
		fvec(7) = 0;// m_market_bond_prices[2] - DefaultableBond(3, 2, 0.05, 100).get_bond_price(hazardRateTermStructure, 50.0);

		fvec(8) = m_market_bond_prices[3] - DefaultableBond(4, 2, 0.05, 100).get_bond_price(hazardRateTermStructure, 10.0);
		fvec(9) = 0;// m_market_bond_prices[3] - DefaultableBond(4, 2, 0.05, 100).get_bond_price(hazardRateTermStructure, 10.0);

		fvec(10) = m_market_bond_prices[4] - DefaultableBond(5, 2, 0.1, 100).get_bond_price(hazardRateTermStructure, 20.0);*/


		fvec(0) = (m_market_bond_prices[0] - DefaultableBond(1, 2, 0.05, 100).get_bond_price(hazardRateTermStructure, 10.0));
		fvec(1) = (m_market_bond_prices[1] - DefaultableBond(2, 2, 0.02, 100).get_bond_price(hazardRateTermStructure, 25.0));
		fvec(2) = (m_market_bond_prices[2] - DefaultableBond(3, 2, 0.05, 100).get_bond_price(hazardRateTermStructure, 50.0));
		fvec(3) = (m_market_bond_prices[3] - DefaultableBond(4, 2, 0.05, 100).get_bond_price(hazardRateTermStructure, 10.0));
		fvec(4) = (m_market_bond_prices[4] - DefaultableBond(5, 2, 0.1, 100).get_bond_price(hazardRateTermStructure, 20.0));
		//fvec(1) = 0; fvec(2) = 0; fvec(3) = 0; fvec(4) = 0; fvec(5) = 0; fvec(6) = 0; fvec(7) = 0; fvec(8) = 0; fvec(9) = 0; fvec(10) = 0;


		return 0;
	}
};
