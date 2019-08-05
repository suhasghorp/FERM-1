#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <Eigen/Dense>

/*#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>*/



//namespace ublas = boost::numeric::ublas;

using namespace std;
using namespace Eigen;



namespace week3 {

	void question1() {
		/*
		Suppose the current term structure of interest rates, assuming annual compounding, is as follows:
		s_1​		s_2		s_3		s_4		s_5		s_6​
		7.0%	7.3%	7.7%	8.1%	8.4%	8.8%
		What is the discount rate d(0,4)d(0,4)? (Recall that interest rates are always quoted on an annual basis unless
		stated otherwise.)

		d(0,t) = 1 / (1 + S_t) ^ t
		*/
		cout.fixed;
		cout.precision(3);
		cout << "Q1 Answer: " << 1.0 / pow((1 + 0.081), 4) << endl;

	}

	void question2() {
		/*
		Suppose a 6-year swap with a notional principal of $10 million is being configured.
		What is the fixed rate of interest that will make the value of the swap equal to zero.
		(You should use the term structure of interest rates from Question 1.)

		The value of fixed rate of the swap that will make swap value = 0 is given by
		r = (1 - d(0,T)) / (sum of d(0,t) for t = 0 to T)

		*/

		vector<double> disc_factors(6);
		vector<double> spots{ 0.07, 0.073, 0.077, 0.081, 0.084, 0.088 };
		vector<int> t(6);
		//start with zero as the first payment is immediate
		iota(t.begin(), t.end(), 1);
		//get all discount factors
		transform(t.begin(), t.end(), spots.begin(), disc_factors.begin(), [](int t, double spot) {return 1 / pow((1 + spot), t); });
		auto numerator = 1 - disc_factors.back();
		auto denominator = accumulate(disc_factors.begin(), disc_factors.end(), 0.0);
		auto fair_rate = (numerator / denominator) * 100.0;
		cout.fixed;
		cout.precision(3);
		cout << "Q2 Answer:" << fair_rate << endl;

	}

	void question3() {
		/*
		Suppose a farmer is expecting that her crop of oranges will be ready for harvest and sale as 150,000 pounds of orange juice in 3
		months time. Suppose each orange juice futures contract is for 15,000 pounds of orange juice, and the current futures price is F_0 = 118.65 cents-per-pound.
		Assuming that the farmer has enough cash liquidity to fund any margin calls, what is the risk-free price that she can guarantee herself.

		Answer: She can go short 10 futures contracts of 15,000 pounds each and she can remove all uncertainity of the spot price in 3 months time.
		She can lock the price at 118.65

		*/

		cout << "Q3 Answer:" << 118.65 << endl;
	}

	void question4() {

		/*
		Suppose a farmer is expecting that her crop of grapefruit will be ready for harvest and sale as 150,000 pounds of grapefruit juice in 3 months time.
		She would like to use futures to hedge her risk but unfortunately there are no futures contracts on grapefruit juice.
		Instead she will use orange juice futures.
		Suppose each orange juice futures contract is for 15,000 pounds of orange juice and the current futures price is F_0 = 118.65 cents-per-pound.
		The volatility, i.e. the standard deviation, of the prices of orange juice and grape fruit juice is 20% and 25%, respectively,
		and the correlation coefficient is 0.7.
		What is the approximate number of contracts she should purchase to minimize the variance of her payoff?

		Answer: Suppose the farmer goes short on h number of OJ contracts. The payoff at expiration = sale of 150,000 lbs of GFJ - 15,000 * h * (S_T_OJ - F_t_OJ)
		The minimum variance hedge is given by
		h* = (150,000/ 15,000) * (Cov(S_t_GFJ, S_t_OJ) / Var(S_t_GFJ))
		This is also written as
		h* = (Size of desired position/Size of 1 futures contract) * (Correlation Coeff * ( Volatility of Spot price / Volatility of Futures price))

		*/
		cout << "Q4 Answer:" << (150000 / 15000.0) * (0.7 * 0.25 / 0.2) << endl;

	}

	void question5() {

		/*
		Call Options
		Consider a 1-period binomial model with R=1.02, S_0 = 100, u=1/d= 1.05.
		Compute the value of a European call option on the stock with strike K=102. The stock does not pay dividends.
		*/

		double S = 100.0;
		double u = 1.05;
		double R = 1.02;
		double K = 102;
		double d = 1 / u;

		double q = ((R - d) / (u - d));

		double Cu = u * S;
		double Cd = d * S;

		//Call option, so up payoff = max(Cu - K, 0) and down payoff = max(0, Cd - K)
		double Pu = max(Cu - K, 0.0);
		double Pd = max(0.0, Cd - K);

		double price = (1 / R) * (q * Pu + (1 - q) * Pd);

		cout << "Q5 Answer:" << price << endl;

	}

	/*template <typename T, typename F = ublas::row_major>
	ublas::matrix<T, F> makeMatrix(std::size_t m, std::size_t n, const std::vector<T> & v)
	{
		if (m*n != v.size()) {
			; // Handle this case
		}
		ublas::unbounded_array<T> storage(m*n);
		std::copy(v.begin(), v.end(), storage.begin());
		return ublas::matrix<T>(m, n, storage);
	}*/


	void question6() {
		/*
		Call Options II
		When you construct the replicating portfolio for the option in the previous question,
		how many dollars do you need to invest in the cash account?

		Answer: We need to solve 2 equations in 2 unknowns

		uSx + Ry = Cu
		dSx + Ry = Cd

		x = number of shares to buy
		y = amount you need to invest in cash account
		*/

		double S = 100.0;
		double u = 1.05;
		double R = 1.02;
		double K = 102;
		double d = 1 / u;
		double Cu = u * S;
		double Cd = d * S;

		double Pu = max(Cu - K, 0.0);
		double Pd = max(0.0, Cd - K);

		Matrix2d A;
		Vector2d b;

		A << u * S, R, d * S, R;
		b << Pu, Pd;

		Vector2d x = A.colPivHouseholderQr().solve(b);

		cout << "Q6 Answer: amount needed to invest in cash account: " << x(1, 0) << endl;

		/*std::vector<double> Avec{ u * S, R, d * S, R };
		std::vector<double> bvec{ Pu, Pd };

		
		ublas::matrix<double> AB = makeMatrix(2, 2, Avec);
		ublas::matrix<double> bb = makeMatrix(2, 1, bvec);

		ublas::permutation_matrix<size_t> pm(AB.size1());
		lu_factorize(AB, pm);
		lu_substitute(AB, pm, bb);
		std::cout << bb << std::endl;*/

	}

	


	void PS2_1() {
		/*
		PS2.1: Which quant is right?
		Two companies are negotiating a 3 year swap. The S1 spot rate is 5.52%, and the S3 spot rate is 6.03% (the S2 spot rate is not available at the moment).
		Company A will take the short position on the swap while company B will take the long position.
		A senior quant from company A has calculated a fixed rate of X = 6.36% for the swap, and a junior quant from company B has computed a fixed rate of X = 6.01%.
		Which company stands to gain the advantage if the 6.36% rate is wrong and is used for the swap?
		Which company stands to gain the advantage if the 6.01% rate is wrong and is used for the swap?
		Which quant, senior or junior, is most likely to have done the calculation correctly? Why?

		Calculate the 2 discount factors first:
		D1 = 1/(1 + 0.0552) ^1  = 0.9477
		D3 = 1/(1 + 0.0603) ^3 = 0.8389
		Based on equations for Swap Fair Fixed Rate, for Company A fixed rate:
		0.0636 = (1 - 0.8389)/(0.9477 + X + 0.8389)
		X = 0.7464
		For Company B:
		0.0601 = (1 - 0.8389)/(0.9477 + X + 0.8389)
		X = 0.8939

		Company B is correct as 0.8939 sits between 0.9477 and 0.8309

		Company A - Short Position - Pay Float
		Company B - Long Position - Pay Fixed

		If 6.36% rate is used to pay fixed, A will benefit as it will receive a higher fixed rate
		If 6.01% rate is used to pay fixed, B will benefit
		*/

		cout << "PS2_1 Answer: See the comments in week3.h for PS2_1" << endl;
		
	}

	void PS2_2() {
		/*
		You are given the following discount factors for years 1 through 5 (the year 5 discount factor is currently not available):
		d(0,1) = 0.955384
		d(0,2) = 0.90496
		d(0,3) = 0.826451
		d(0,4) = 0.785256
		d(05) = n/a
		You are told that a 5 year swap was calculated using these values (the year 5 discount factor was available at the time of the calculation),
		and this resulted in a fixed rate X = 6.75%. You are asked to compute the fixed rate X value for a 4 year swap that will start in year 2,
		and for a 3 year swap that will start in year 3.

		Answer: First calculate d(0,5) using swap fair fixed rate formula :

		0.0675 = (1 - X) / (0.955384 + 0.90496 + 0.826451 + 0.785256 + X)
		X = 0.717224

		d(0,y) = 0.955384 0.90496 0.826451 0.785256 0.717224

		Now, use the formula df (t0,t2) = df(t0,t1) * df (t1,t2)

		d(1,y)  = 0.90496/0.955384, 0.826451/0.955384, 0.785256/0.955384, 0.717224/0.955384
		d(1,y) = 0.947221222 0.865045887 0.821927099 0.750718036

		Use the same technique for d(2,y)

		d(2,y) = 0.865045887/0.947221222, 0.821927099/0.947221222, 0.750718036/0.947221222
		d(2,y) = 0.913245889	0.86772454	0.792547737

		Fixed rate for 4 year swap starting in year 2 = (1 - 0.750718036)/(0.947221222 + 0.865045887 + 0.821927099 + 0.750718036) = 0.073645
		Fixed rate for 3 year swap starting in year 3 = (1 - 0.792547737)/(0.913245889 + 0.86772454 + 0.792547737) = 0.08061
		*/
		cout << "PS2_2 Answer: See the comments in week3.h for PS2_2" << endl;
	}

	/*
	Forward Contract formulas

	At Initiation:
	value of fwd contract = V0 = 0
	forward price F0 = S0(1+r)^T

	During the life of the contract:
	value of fwd contract = Vt = St - F0(1+r)^(-(T-t))

	At expiration:
	value of fwd contract VT = ST - F0

	*/

	
}
