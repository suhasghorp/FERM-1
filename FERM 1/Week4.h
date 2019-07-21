#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>

using namespace std;


namespace week4 {

	/* Quiz Instructions
	Questions 1-8 should be answered by building a 15-period binomial model whose parameters should be calibrated to a Black-Scholes geometric Brownian motion model 
	with: T = .25 years, S0=100, r=2%, sigma = 30% and a dividend yield of c=1%.
	*/

	template<typename S0, typename K, typename T, typename NumOfPeriods, typename Sigma, typename Rate, typename DividendRate, typename IsCall, typename IsAmerican>
	auto Option_Payoff_Tree(
		const S0& S0, const K& K, const T& T, const NumOfPeriods& num_periods, const Sigma& sigma, const Rate& r, const DividendRate& c, const IsCall& is_call, const IsAmerican& is_american) {

		double u = exp(sigma * sqrt(T / num_periods));
		double d = 1 / u;

		double r_inverse = exp(-r * T / num_periods);
		double uu = u * u;
		double q = (exp((r - c)* T / num_periods) - d) / (u - d);
		vector<double> prices(num_periods + 1);
		vector<double> payoffs(num_periods + 1);
		vector < vector <double> > prices_store(num_periods + 1);
		vector < vector <double> > payoffs_store(num_periods + 1);
		prices[0] = S0 * pow(d, num_periods);
		for (int i = 1; i <= num_periods; ++i) {
			prices[i] = uu * prices[i - 1];
		}
		prices_store.push_back(prices);

		for (int i = 0; i <= num_periods; ++i) {
			if (is_call) {
				payoffs[i] = max(0.0, (prices[i] - K));
			}
			else {
				payoffs[i] = max(0.0, (K - prices[i]));
			}
		}
		payoffs_store.push_back(payoffs);

		for (int step = num_periods - 1; step >= 0; --step) {
			for (int i = 0; i <= step; ++i) {
				prices[i] = d * prices[i + 1];
				payoffs[i] = (q * payoffs[i + 1] + (1 - q) * payoffs[i]) * r_inverse;
				if (is_american) {
					double early_exercise = 0.0;
					if (is_call) {
						early_exercise = prices[i] - K;
					}
					else {
						early_exercise = K - prices[i];
					}
					if (early_exercise > payoffs[i]) {
						cout << "It is optimal to exercise this American " << (is_call ? "Call" : "Put") << " option at step " << step << endl;
					}
					payoffs[i] = max(payoffs[i], early_exercise);
				}
			}

			prices_store.insert(prices_store.begin(), vector<double>(prices.begin(), prices.begin() + step + 1));
			payoffs_store.insert(payoffs_store.begin(), vector<double>(payoffs.begin(), payoffs.begin() + step + 1));

		}
		return payoffs_store;
	}	

	void question1(bool p_is_call, bool p_is_american) {
		/*Compute the price of an American call option with strike K=110 and maturity T=.25 years.*/
		double S0 = 100.0;
		double T = 0.25;
		int num_periods = 15;
		double sigma = 0.3;
		double r = 0.02;
		double c = 0.01;
		double K = 110.0;
		bool is_call = p_is_call;
		bool is_american = p_is_american;

		vector<vector<double>> payoff_tree = Option_Payoff_Tree(S0, K, T, num_periods, sigma, r, c, is_call, is_american);

		cout << "Q1 Answer : " << (is_american ? "American" : "European") << " " << (is_call ? "Call" : "Put") << " K=110 and T=.25 : " << payoff_tree[0][0] << endl;

	}

	void question2and3and4(bool p_is_call, bool p_is_american) {
		/**Compute the price of an American put option with strike K=110 and maturity T=.25 years.
		The output shows it is optimal to early exercise the put option in question2.
		The earliest time period for optimal exercise is 5
		*/
		question1(p_is_call, p_is_american);
	}

	void question5() {
		/* Check put-call parity
		C + K*e^(-rT) = P + S
		C = price of European Call option
		K*e(-rT) = present value of Strike price discounted from expiration date
		P = price of European Put option
		S = Spot price
		American options DO NOT allow put-call parity so the answer is NO	
		But American options do satify
		S - K <= C + P <= S - Ke^-rT
		100.0 - 110.0 <= 2.60408 + 12.3598 <= 100.0 - (110.0 * exp(-0.02 * 0.25 / 4.0));
		*/
		cout << "Q5 Answer:" << "American options DO NOT allow put-call parity so the answer is NO" << endl;		
	}

	void question6() {

		/*Compute the fair value of an American call option with strike K=110 and maturity n=10 periods where the option is written on a futures contract that expires after
		15 periods. The futures contract is on the same underlying security of the previous questions.
		The strategy to use :
		build stock price tree, the up parameter uses 15 periods and down = 1/up
		build future price tree, the risk neutral probability q uses 10 periods in its calculation
		build american call payoff tree, discount the rate using 10 periods		
		*/

		double S0 = 100.0;
		double T = 0.25;
		int num_periods_future = 15;
		int num_periods_option = 10;
		double sigma = 0.3;
		double u = exp(sigma * sqrt(T / num_periods_future));
		double d = 1 / u;
		double r = 0.02;
		double c = 0.01;
		double K = 110.0;
		bool is_call = true;
		bool is_american = true;


		double q = (exp((r - c)* T / num_periods_future) - d) / (u - d);
		vector<vector<double>> stock_prices;
		vector<vector<double>> future_prices;
		vector<vector<double>> payoffs(num_periods_option + 1);


		for (int i = 1; i <= num_periods_future + 1; ++i) {
			vector<double> S(i);
			for (int j = 0; j < i; ++j) {
				S[j] = S0 * pow(u, j)*pow(d, i - j - 1);
			};
			stock_prices.push_back(S);
		}

		future_prices.insert(future_prices.begin(), stock_prices[num_periods_future]);

		for (int step = num_periods_future - 1; step >= 0; --step) {
			vector<double> temp(step + 1);
			for (int i = 0; i <= step; ++i) {
				temp[i] = (q * stock_prices[step + 1][i + 1] + (1 - q) * stock_prices[step + 1][i]);
			}
			future_prices.insert(future_prices.begin(), temp);
		}

		// fill in only the last period first
		vector<double> temp(num_periods_option + 1);
		for (int i = 0; i <= num_periods_option; ++i) {
			temp[i] = max((is_call ? 1 : -1)*(future_prices[num_periods_option][i] - K), 0.0);
		}
		payoffs[num_periods_option] = temp;

		//calculate payoffs for the option for the rest of the steps
		for (int step = num_periods_option - 1; step >= 0; --step) {
			vector<double> future_payoff(step + 1);
			vector<double> stock_payoff(step + 1);
			vector<double> temp(step + 1);
			for (int i = 0; i <= step; ++i) {
				future_payoff[i] = exp(-r * T / num_periods_future) * (q * payoffs[step + 1][i + 1] + (1 - q) * payoffs[step + 1][i]);
				//stock_payoff[i] = (is_call ? 1 : -1) * (stock_prices[step][i] - K);
				stock_payoff[i] = (is_call ? 1 : -1) * (future_prices[step][i] - K);
				if (is_american) {
					if (stock_payoff[i] > future_payoff[i]) {
						cout << "It is optimal to exercise this American " << (is_call ? "Call" : "Put") << " option at step " << step << endl;
					}
					temp[i] = max(future_payoff[i], stock_payoff[i]);
				}
				else {
					temp[i] = max(future_payoff[i], 0.0);
				}
			}
			payoffs[step] = temp;
		}
		cout << "Q6 Answer: " << payoffs[0][0] << endl;
		cout << endl;
		cout << "Q7 I could not get the answer to Question 7 yet" << endl;

		/*for (int step = num_periods_option - 1; step >= 0; --step) {
			vector<double> future_payoff(step + 1);
			vector<double> stock_payoff(step + 1);
			vector<double> temp(step + 1);
			for (int i = 0; i <= step; ++i) {
				stock_payoff[i] = (is_call ? 1 : -1) * (future_prices[step][i] - K);
				cout << "Step: " << step << "Check:" << stock_payoff[i] - payoffs[step][i] << endl;
			}
		}*/

		/*cout << setiosflags(ios::fixed) << setprecision(3);
		for (int step = 0; step <= num_periods_option; step++) {
			for (int i = stock_prices[step].size()-1; i >= 0; --i) {
				cout << setw(10) << stock_prices[step][i] << endl;
			}
			cout << "---------------------------" << endl;
		}
		cout << endl;*/

		/*cout << setiosflags(ios::fixed) << setprecision(3);
		for (int step = 0; step <= num_periods_option; step++) {
			for (int i = future_prices[step].size() - 1; i >= 0; --i) {
				cout << setw(10) << future_prices[step][i] << endl;
			}
			cout << "---------------------------" << endl;
		}
		cout << endl;*/

		/*cout << setiosflags(ios::fixed) << setprecision(3);
		for (int step = 0; step <= num_periods_option; step++) {
			for (int i = payoffs[step].size() - 1; i >= 0; --i) {
				cout << setw(10) << payoffs[step][i] << endl;
			}
			cout << "---------------------------" << endl;
		}
		cout << endl;*/
	}

	void question8(bool p_is_call, bool p_is_american) {
		
		double S0 = 100.0;
		double T = 0.25;
		int num_periods = 15;
		int num_periods_chooser = 10;
		double sigma = 0.3;
		double u = exp(sigma * sqrt(T / num_periods));
		double d = 1 / u;
		double r = 0.02;
		double c = 0.01;
		double K = 100.0;
		bool is_american = false;
		double q = (exp((r - c)* T / num_periods) - d) / (u - d);

		//compute call and put payoffs
		vector<vector<double>> call_payoffs = Option_Payoff_Tree(S0, K, T, num_periods, sigma, r, c, true, is_american);
		vector<vector<double>> put_payoffs = Option_Payoff_Tree(S0, K, T, num_periods, sigma, r, c, false, is_american);
		//extract period 10 payoffs from both
		vector<double> call_10_payoffs = call_payoffs[num_periods_chooser];
		vector<double> put_10_payoffs = put_payoffs[num_periods_chooser];
		vector<vector<double>> chooser_payoffs(num_periods_chooser+1);
		vector<double> temp = call_10_payoffs; // we are going to transform temp in-place for chooser_payoffs below

		std::transform(temp.begin(), temp.end(), put_10_payoffs.begin(), temp.begin(),
			[](float call_payoff, float put_payoff) {
			return max(call_payoff, put_payoff);
		});

		chooser_payoffs[num_periods_chooser] = temp;

		for (int step = num_periods_chooser - 1; step >= 0; --step) {
			vector<double> temp(step + 1);
			for (int i = 0; i <= step; ++i) {
				//note we discount back using 15 periods
				temp[i] = exp(-r * T / num_periods) * (q * chooser_payoffs[step + 1][i + 1] + (1 - q) * chooser_payoffs[step + 1][i]);
			}
			chooser_payoffs[step] = temp;
		}
		cout << "Q8 Answer: Chooser option payoff: " << chooser_payoffs[0][0] << endl;
	}

}

