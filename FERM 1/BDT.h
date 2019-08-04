#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iostream>
#include <Eigen/Dense>


#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>

using namespace std;

namespace week6 {

	vector<vector<double>> bdt_short_rate_lattice(double r00, vector<double> a, double b, int num_periods) {

		vector<vector<double>> short_rates(num_periods);

		short_rates[0].push_back(r00);
		for (int i = 1; i <= num_periods-1; ++i) {
			for (int j = 0; j <= i; ++j) {
				short_rates[i].push_back((a[i] * exp(b * j))/100.0);
			}

		}

		return short_rates;
	}

	vector<vector<double>> bdt_elementary_lattice(const vector<vector<double>>& short_rates, double q) {
		vector<vector<double>> elementary_prices(short_rates.size() + 1);
	elementary_prices[0].push_back(1.0);

		for (int i = 1; i <= short_rates.size(); ++i) {
			for (int j = 0; j <= i; ++j) {
				if (j == 0) {
					elementary_prices[i].push_back(q * elementary_prices[i - 1][j] / (1.0 + short_rates[i - 1][j]));
				}
				else if (j == i) {
					elementary_prices[i].push_back(q * elementary_prices[i - 1][j - 1] / (1.0 + (short_rates[i - 1][j - 1])));
				}
				else {
					elementary_prices[i].push_back((q * elementary_prices[i - 1][j - 1] / (1.0 + short_rates[i - 1][j - 1])) + ((1 - q) * elementary_prices[i - 1][j] / (1.0 + (short_rates[i - 1][j]))));
				}
			}
		}
		return elementary_prices;

	}

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

	struct my_functor : Functor<double>
	{
		double m_b;
		int m_num_periods;
		vector<double> m_spot_rates;

		my_functor(double b, int num_periods, vector<double> spot_rates) : Functor<double>(num_periods,num_periods),m_b(b),m_num_periods(num_periods),m_spot_rates(spot_rates) {}
		int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
		{
			vector<double> vec(x.data(), x.data() + x.rows() * x.cols());
			vector<vector<double>> short_rates = bdt_short_rate_lattice(m_spot_rates[0], vec, m_b, m_num_periods);
			vector<vector<double>> bdt_elementary_prices = bdt_elementary_lattice(short_rates, 0.5);
			vector<double> bdt_zcb_prices;
			vector<double> bdt_spot_rates;
			for (int i = 1; i <= short_rates.size(); ++i) {
				bdt_zcb_prices.push_back(accumulate(bdt_elementary_prices[i].begin(), bdt_elementary_prices[i].end(), 0.0));
				bdt_spot_rates.push_back(pow((1.0 / bdt_zcb_prices.back()), (1.0 / i)) - 1.0);
			}
			for (int i = 0; i < bdt_spot_rates.size(); ++i) {
				fvec(i) = m_spot_rates[i] - bdt_spot_rates[i];				
			}

			return 0;
		}
	};
		

	vector<vector<double>> buildBDT(vector<double>& yield_curve, vector<double>& a_guesses, double b, int num_periods, double T) {

		Eigen::VectorXd x = Eigen::Map<Eigen::VectorXd, Eigen::Unaligned>(a_guesses.data(), a_guesses.size());		

		my_functor functor(b, num_periods, yield_curve);
		Eigen::NumericalDiff<my_functor> numDiff(functor);
		Eigen::LevenbergMarquardt<Eigen::NumericalDiff<my_functor>, double> lm(numDiff);
		lm.parameters.maxfev = 2000;
		lm.parameters.xtol = 1.0e-10;
		int ret = lm.minimize(x);
		vector<double> vec(x.data(), x.data() + x.rows() * x.cols());

		vector<vector<double>> bdt_short_rates = bdt_short_rate_lattice(yield_curve[0], vec, b, num_periods);		
		return bdt_short_rates;
  	}

}
