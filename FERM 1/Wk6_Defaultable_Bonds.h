#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>
#include "functors.h"
#include "Hazard_Rate_Term_Structure.h"

using namespace std;


namespace week6 {

	void get_defaultable_bond_prices() {
		// Bonds and CDS V2 spreadsheet //
		vector<double> hazard_rates(11);
		fill(hazard_rates.begin(), hazard_rates.end(), 0.02);
		HazardRateTermStructure hazardRateTermStructure(5, 2, hazard_rates, 0.05);

		//1 Year Bond 
		DefaultableBond bond1year(1, 2, 0.05, 100);
		double price = bond1year.get_bond_price(hazardRateTermStructure, 10.0);
		cout << setprecision(4) << "Price 1 Year bond: " << price << endl;

		//2 Year bond
		DefaultableBond bond2year(2, 2, 0.02, 100);
		price = bond2year.get_bond_price(hazardRateTermStructure, 25.0);
		cout << setprecision(4) << "Price 2 Year bond: " << price << endl;

		//3 Year bond
		DefaultableBond bond3year(3, 2, 0.05, 100);
		price = bond3year.get_bond_price(hazardRateTermStructure, 50.0);
		cout << setprecision(4) << "Price 3 Year bond: " << price << endl;

		//4 Year bond
		DefaultableBond bond4year(4, 2, 0.05, 100);
		price = bond4year.get_bond_price(hazardRateTermStructure, 10.0);
		cout << setprecision(4) << "Price 4 Year bond: " << price << endl;

		//5 Year bond
		DefaultableBond bond5year(5, 2, 0.1, 100);
		price = bond5year.get_bond_price(hazardRateTermStructure, 20.0);
		cout << setprecision(4) << "Price 5 Year bond: " << price << endl;
	}

	

	vector<double> calibrate_hazard_rates(vector<double>& market_bond_prices, vector<double>& h_guesses, double r, int maturity, int frequency) {


		Eigen::VectorXd x = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(h_guesses.data(), h_guesses.size());

		hazard_functor functor(market_bond_prices,r,maturity,frequency);
		Eigen::NumericalDiff<hazard_functor> numDiff(functor);
		Eigen::LevenbergMarquardt<Eigen::NumericalDiff<hazard_functor>, double> lm(numDiff);
		lm.parameters.maxfev = 2000;
		lm.parameters.xtol = 1.0e-10;
		lm.minimize(x);
		vector<double> hazard_rates(x.data(), x.data() + x.rows() * x.cols());		
		return hazard_rates;
	}

	double calibrate_cds(const HazardRateTermStructure& hzStructure, vector<double> spread_guess, double principal, double recovery, double r, int maturity, int frequency) {

		Eigen::VectorXd x = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(spread_guess.data(), spread_guess.size());
		
		cds_functor functor(hzStructure, principal, recovery, r, maturity, frequency);
		Eigen::NumericalDiff<cds_functor> numDiff(functor);
		Eigen::LevenbergMarquardt<Eigen::NumericalDiff<cds_functor>, double> lm(numDiff);
		lm.parameters.maxfev = 2000;
		lm.parameters.xtol = 1.0e-10;
		lm.minimize(x);
		vector<double> spread(x.data(), x.data() + x.rows() * x.cols());
		return spread[0];
	}





}
