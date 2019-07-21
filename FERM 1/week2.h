#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

using namespace std;

namespace week2 {

	void question1() {
		/*
		A major lottery advertises that it pays the winner $10 million. However this prize money is paid at the rate of $500,000 each year
		(with the first payment being immediate) for a total of 20 payments. What is the present value of this prize at 10% interest compounded annually?
		*/

		vector<double> disc_factors(20);
		double r = 0.1;
		vector<int> t(20);
		//start with zero as the first payment is immediate
		iota(t.begin(), t.end(), 0);
		//get all discount factors
		transform(t.begin(), t.end(), disc_factors.begin(), [&r](int t) {return 1 / pow((1 + r), t); });
		//accumulate needs 0.0 (not 0) for it to return double
		double sum = accumulate(disc_factors.begin(), disc_factors.end(), 0.0);
		//cout << "PV = " << sum * 500000.0 << endl;
		cout << fixed;
		cout.precision(2);
		cout << "Q1 Answer:" << sum * 500000.0 / 1000000.0 << endl;
	}

	void question2() {
		/*
		A young couple has made a deposit of the first month's rent (equal to
		$1,000) on a 6-month apartment lease. The deposit is refundable at
		the end of six months if they stay until the end of the lease.
		The next day they find a different apartment that they like just as well,
		but its monthly rent is only $900. And they would again have to put a
		deposit of $900 refundable at the end of 6 months.
		They plan to be in the apartment only 6 months. Should they switch to the new apartment? Assume
		an (admittedly unrealistic!) interest rate of 12% per month compounded monthly.
		*/

		// Compute PV of cash flows in both cases. First case, deposit of $1000 has already been made but second case, requires a new deposit of $900

		vector<double> cashflows{ -1000.0, -1000.0, -1000.0, -1000.0, -1000.0, -1000.0, 1000.0 }; // 6 monthly payments plus one deposit back
		vector<double> payments(7);
		double r = 0.12;
		vector<int> t(7);
		//start with zero as the first payment is immediate
		iota(t.begin(), t.end(), 0);
		//get all discount factors
		transform(t.begin(), t.end(), cashflows.begin(), payments.begin(), [&r](int t, double cf) {return cf / pow((1 + r), t); });
		double total_payments = accumulate(begin(payments), end(payments), 0.0) + cashflows.back() / pow((1 + r), t.back());
		cout << "Total Payments for first apartment:" << total_payments << endl;

		cashflows = { -900.0, -900.0, -900.0, -900.0, -900.0, -900.0, 900.0 };
		transform(t.begin(), t.end(), cashflows.begin(), payments.begin(), [&r](int t, double cf) {return cf / pow((1 + r), t); });
		total_payments = -900.0 + accumulate(begin(payments), end(payments), 0.0) + cashflows.back() / pow((1 + r), t.back());
		//cout << "Total Payments for second apartment:" << total_payments << endl;
		cout << "Q2 Answer:They should stay in the first apartment" << endl;

	}

	void question3() {
		/*
		Suppose the spot rates for 1 and 2 years are s_1 = 6.3% and s_2 =6.9% with annual compounding.
		Recall that in this course interest rates are always quoted on an annual basis unless otherwise specified.
		What is the discount rate d(0,2)?
		*/
		cout << fixed;
		cout.precision(3);
		cout << "Q3 Answer:" << 1.0 / pow((1 + 0.069), 2) << endl;
	}

	void question4() {
		/*
		Suppose the spot rates for 1 and 2 years are s_1 = 6.3% and s_2 = 6.9% with annual compounding.
		Recall that in this course interest rates are always quoted on an annual basis unless otherwise specified.
		What is the forward rate, f (1,2) assuming annual compounding?

		(1 + S2) ^ 2 = (1 + S1) * (1 + f(1,2) )
		f(1,2) = ( (1 + S2) ^2 / (1 + S1) ) - 1
		*/
		cout << fixed;
		cout.precision(1);
		cout << "Q4 Answer:" << ((pow((1 + 0.069), 2) / (1 + 0.063)) - 1) * 100.0 << endl;

	}

	void question5() {
		/*
		The current price of a stock is $400 per share and it pays no dividends.
		Assuming a constant interest rate of 8% per year compounded quarterly, what is the stock's theoretical forward price for delivery in 9 months?
		F = S0 / disc_factor
		*/

		auto disc_factor = [](double r, int m) { return 1.0 / pow(1 + (r / 4), m); };
		cout << fixed;
		cout.precision(2);
		cout << "Q5 Answer:" << 400 / disc_factor(0.08, 3) << endl;

	}

	void question6() {
		/*
		Bounds using different lending and borrowing rate Suppose the borrowing rate r_Borrow = 10% compounded annually.
		However,the lending rate (or equivalently, the interest rate on deposits) is only 8% compounded annually.
		Compute the difference between the upper and lower bounds on the price of an perpetuity that pays A=10,000$ per year.

		The upper bound on price of annuity is A/R_lending
		The lower bound on price of Annuity is A/R_borrowing

		*/

		cout << fixed;
		cout.precision(0);
		cout << "Q6 Answer:" << 10000.0 / 0.08 - 10000.0 / 0.1 << endl;
	}

	void question7() {

		/*
		Value of a Forward contract at an intermediate time
		Suppose we hold a forward contract on a stock with expiration 6 months from now.
		We entered into this contract 6 months ago so that when we entered into the contract, the expiration was T=1 year.
		The stock price 6 months ago was S_0 = 100, the current stock price is 125 and the current interest rate is r =10%
		compounded semi-annually. (This is the same rate that prevailed 66 months ago.) What is the current value of our forward contract?

		The forward price F_0 at t = 0 is given by
		F_0 = S_0/d(0,T)

		The forward price F_t at t = t is given by
		F_t = S_t/d(t,T)

		The value of Forward at t is given by
		(F_t - F_0) * d(t,T)

		*/
		auto F = [](double S, double r, int m) { return S / (1.0 / pow(1 + (r / 2.0), m)); };

		auto F_0 = F(100.0, 0.1, 2);
		auto F_t = F(125, 0.1, 1);
		auto d_t_T = (1.0 / pow(1 + (0.1 / 2), 1));
		cout << fixed;
		cout.precision(1);
		cout << "Q7 Answer:" << (F_t - F_0) *  d_t_T << endl;

	}

	template<typename Cashflow_Times, typename Cashflow_Amounts, typename R>
	auto cash_flow_pv_discrete(
		const Cashflow_Times& cflow_times,
		const Cashflow_Amounts& cflow_amounts,
		const R& r) {
		double PV = 0.0;
		for (int t = 0; t < cflow_times.size(); t++) {
			PV += cflow_amounts[t] / pow(1.0 + r, cflow_times[t]);
		}
		return PV;
	}
	struct Generator {
		Generator() : m_value(0) { }
		int operator()() { m_value = m_value + 1; return m_value; }
		int m_value;
	};

	void PS_11() {
		/*
		PS1.1: Fair Value of Bond
		Maxwell owns a bond and wants to sell it to Joan for $875. The bond has a face value of $1000 with a six year maturity,
		but the bond is already two years old and thus has 4 years remaining. The bond pays a quarterly coupon of $9 which Joan will collect
		starting in three months (if she makes the purchase). Maxwell would keep the coupon payments already made.
		The final payment to Joan will include the face value and final coupon.
		The current annual interest rate is 7.3%.
		What is the present value of this bond, rounded to dollars to two decimals places?
		If Joan purchases the bond, what will be Maxwell’s profit or loss on the transaction, ignoring any commission and overhead costs,
		rounded to dollars to two decimal places?
		*/
		vector<double> C(16);
		fill(C.begin(), C.end(), 9.0);
		C.back() = C.back() + 1000.0;

		vector<int> times(16);
		generate(begin(times), end(times), Generator());
		auto r = 0.073 / 4.0;
		auto pv = cash_flow_pv_discrete(times, C, r);

		cout << fixed;
		cout.precision(2);
		cout << "PS_11 Answer: PV of bond:" << pv << endl;
		cout << endl;
		cout << "PS_11 Maxwell's profit:" << 875.0 - pv << endl;

	}

	

}


