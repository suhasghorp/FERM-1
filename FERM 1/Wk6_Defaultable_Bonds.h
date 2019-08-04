#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>
#include "functor.h"
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

	struct hazard_functor : Functor<double>
	{
		vector<double> m_market_bond_prices;
		double m_r;
		int m_maturity;
		int m_frequency;

		hazard_functor(vector<double> market_bond_prices, double r, int maturity,int frequency) : Functor<double>((maturity*frequency)+1, (maturity*frequency)+1), m_market_bond_prices(market_bond_prices), m_r(r),m_maturity(maturity), m_frequency(frequency){}
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

	vector<double> calibrate_hazard_rates(vector<double>& market_bond_prices, vector<double>& h_guesses, double r, int maturity, int frequency) {


		Eigen::VectorXd x = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(h_guesses.data(), h_guesses.size());

		hazard_functor functor(market_bond_prices,r,maturity,frequency);
		Eigen::NumericalDiff<hazard_functor> numDiff(functor);
		Eigen::LevenbergMarquardt<Eigen::NumericalDiff<hazard_functor>, double> lm(numDiff);
		lm.parameters.maxfev = 2000;
		lm.parameters.xtol = 1.0e-10;
		int ret = lm.minimize(x);
		vector<double> hazard_rates(x.data(), x.data() + x.rows() * x.cols());		
		return hazard_rates;
	}

	struct cds_functor : Functor<double>
	{
		double m_notional;
		double m_recovery;
		double m_r;
		int m_maturity;
		int m_frequency;
		HazardRateTermStructure m_hzStructure;

		cds_functor(const HazardRateTermStructure& hzStructure, double notional,double recovery,double r, int maturity, int frequency) : Functor<double>(1,1), 
					m_hzStructure(hzStructure),m_notional(notional), m_recovery(recovery),m_r(r),m_maturity(maturity),m_frequency(frequency){}

		int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
		{
			vector<double> vec(x.data(), x.data() + x.rows() * x.cols()); // spread is x
			CDS cds(vec[0], m_notional, m_recovery, m_r, m_maturity, m_frequency);
			fvec(0) = cds.price_cds(m_hzStructure);
			return 0;
		}
	};

	double calibrate_cds(const HazardRateTermStructure& hzStructure, vector<double> spread_guess, double principal, double recovery, double r, int maturity, int frequency) {

		Eigen::VectorXd x = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(spread_guess.data(), spread_guess.size());
		//vector<double> spread_g = { spread_guess };
		//Eigen::VectorXd x(spread_g);
		

		cds_functor functor(hzStructure, principal, recovery, r, maturity, frequency);
		Eigen::NumericalDiff<cds_functor> numDiff(functor);
		Eigen::LevenbergMarquardt<Eigen::NumericalDiff<cds_functor>, double> lm(numDiff);
		lm.parameters.maxfev = 2000;
		lm.parameters.xtol = 1.0e-10;
		int ret = lm.minimize(x);
		vector<double> spread(x.data(), x.data() + x.rows() * x.cols());
		return spread[0];
	}





}
