#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>

using namespace std;


namespace week5 {

	vector<vector<double>> short_rate_lattice(double r00, double u, double d, int num_periods) {
		
		vector<vector<double>> short_rates(num_periods + 1);

		short_rates[0].push_back(r00);
		for (int i = 1; i <= num_periods; ++i) {
			for (int j = 0; j < i; ++j) {
				short_rates[i].push_back(d * short_rates[i - 1][j]);
			}
			short_rates[i].push_back(u * short_rates[i - 1][i - 1]);
		}

		return short_rates;
	}

	vector<vector<double>> elementary_lattice(const vector<vector<double>>& short_rates, double q) {
		vector<vector<double>> elementary_prices(short_rates.size()+1);
		elementary_prices[0].push_back(1.0);

		for (int i = 1; i <= short_rates.size(); ++i) {
			for (int j = 0; j <= i; ++j) {
				if (j == 0) {
					elementary_prices[i].push_back(q * elementary_prices[i - 1][j] / (1.0 + short_rates[i - 1][j]));
				}
				else if (j == i) {
					elementary_prices[i].push_back(q * elementary_prices[i - 1][j-1] / (1.0 + short_rates[i - 1][j-1]));
				}
				else {
					elementary_prices[i].push_back((q * elementary_prices[i - 1][j - 1] / (1.0 + short_rates[i - 1][j - 1])) + ((1 - q) * elementary_prices[i - 1][j] / (1.0 + short_rates[i - 1][j])));
				}
			}
		}
		return elementary_prices;

	}


	vector<vector<double>> zcb_price_lattice(const vector<vector<double>>& short_rates, int zcb_maturity, double q) {

		vector<vector<double>> zcb_prices(zcb_maturity + 1);
		vector<double> prices(short_rates[zcb_maturity].size());
		fill(prices.begin(), prices.end(), 100.0);
		zcb_prices[zcb_maturity] = prices;
		prices.clear();
		for (int step = zcb_maturity - 1; step >= 0; --step) {
			for (int i = 0; i <= step; ++i) {
				prices.push_back((q * zcb_prices[step + 1][i + 1] + (1 - q) * zcb_prices[step + 1][i]) / (1 + short_rates[step][i]));
			}
			zcb_prices[step] = prices;
			prices.clear();
		}
		return zcb_prices;
	}

	vector<vector<double>> cb_price_lattice(const vector<vector<double>>& short_rates, double coupon, int cb_maturity, double q) {

		vector<vector<double>> cb_prices(cb_maturity + 1);
		vector<double> prices(cb_maturity + 1);
		fill(prices.begin(), prices.end(), 100.0 + (100.0 * coupon));
		cb_prices[cb_maturity] = prices;
		prices.clear();
		for (int step = cb_maturity - 1; step >= 0; --step) {
			for (int i = 0; i <= step; ++i) {
				prices.push_back((100.0 * coupon) + (q * cb_prices[step + 1][i + 1] + (1 - q) * cb_prices[step + 1][i]) / (1 + short_rates[step][i]));
			}
			cb_prices[step] = prices;
			prices.clear();
		}
		return cb_prices;
	}

	vector<vector<double>> option_price_lattice(const vector<vector<double>>& short_rates, const vector<vector<double>>& zcb_prices, int option_maturity, double strike, double q, bool is_call, bool is_american) {
		vector<vector<double>> option_prices(option_maturity + 1);
		int step = zcb_prices[option_maturity].size();
		for (int i = 0; i < step; ++i) {
			option_prices[option_maturity].push_back(max(0.0 , (is_call? 1.0 : -1.0) * (zcb_prices[option_maturity][i] - strike)));			
		}

		for (int step = option_maturity - 1; step >= 0; --step) {
			vector<double> payoffs;
			for (int i = 0; i <= step; ++i) {
				if (is_american) {
					payoffs.push_back(max((is_call ? 1.0 : -1.0) * (zcb_prices[step][i] - strike), (q * option_prices[step + 1][i + 1] + (1.0 - q) * (option_prices[step + 1][i]))/(1.0 + short_rates[step][i])));
				}
				else {
					payoffs.push_back(((q * option_prices[step + 1][i + 1]) + ((1.0 - q) * (option_prices[step + 1][i])))/(1.0 + short_rates[step][i]));					
				}
			}
			option_prices[step] = payoffs;
			payoffs.clear();
		}

		return option_prices;
	}

	vector<vector<double>> bond_forward_lattice(const vector<vector<double>>& short_rates, const vector<vector<double>>& cb_prices, int forward_maturity, double bond_coupon, double q) {
		vector<vector<double>> forward_prices(forward_maturity + 1);
		vector<double> prices;
		for (int i = 0; i < cb_prices[forward_maturity].size(); ++i) {
			prices.push_back(cb_prices[forward_maturity][i] - ( 100.0 * bond_coupon ));
		}
		forward_prices[forward_maturity] = prices;

		prices.clear();
		for (int step = forward_maturity - 1; step >= 0; --step) {
			for (int i = 0; i <= step; ++i) {
				prices.push_back((q * forward_prices[step + 1][i + 1] + (1 - q) * forward_prices[step + 1][i]) / (1 + short_rates[step][i]));
			}
			forward_prices[step] = prices;
			prices.clear();
		}
		return forward_prices;
	}

	vector<vector<double>> bond_future_lattice(const vector<vector<double>>& cb_prices, int future_maturity, double bond_coupon, double q) {
		vector<vector<double>> future_prices(future_maturity + 1);
		vector<double> prices;
		for (int i = 0; i < cb_prices[future_maturity].size(); ++i) {
			prices.push_back(cb_prices[future_maturity][i] - (100.0 * bond_coupon));
		}
		future_prices[future_maturity] = prices;

		prices.clear();
		for (int step = future_maturity - 1; step >= 0; --step) {
			for (int i = 0; i <= step; ++i) {
				prices.push_back((q * future_prices[step + 1][i + 1] + (1 - q) * future_prices[step + 1][i]));
			}
			future_prices[step] = prices;
			prices.clear();
		}
		return future_prices;
	}

	vector<vector<double>> caplet_lattice(const vector<vector<double>>& short_rates, int caplet_maturity, double fixed_rate, double q) {
		vector<vector<double>> caplet_prices(caplet_maturity); // no need to add 1
		vector<double> prices;
		for (int i = 0; i < short_rates[caplet_maturity-1].size(); ++i) {
			prices.push_back((short_rates[caplet_maturity-1][i] - fixed_rate)/(1.0 + short_rates[caplet_maturity - 1][i]));
		}
		caplet_prices[caplet_maturity-1] = prices;

		prices.clear();
		for (int step = caplet_maturity - 2; step >= 0; --step) {
			for (int i = 0; i <= step; ++i) {
				prices.push_back((q * caplet_prices[step + 1][i + 1] + (1 - q) * caplet_prices[step + 1][i])/(1.0 + short_rates[step][i]));
			}
			caplet_prices[step] = prices;
			prices.clear();
		}
		return caplet_prices;
	}

	vector<vector<double>> swap_lattice(const vector<vector<double>>& short_rates, int swap_maturity, double fixed_rate, double q, int payment_start_period) {
		vector<vector<double>> swap_prices(swap_maturity); // no need to add 1
		vector<double> prices;
		for (int i = 0; i < short_rates[swap_maturity - 1].size(); ++i) {
			prices.push_back((short_rates[swap_maturity - 1][i] - fixed_rate) / (1.0 + short_rates[swap_maturity - 1][i]));
		}
		swap_prices[swap_maturity - 1] = prices;

		prices.clear();
		for (int step = swap_maturity - 2; step >= 0; --step) {
			double payment = 0.0;
			for (int i = 0; i <= step; ++i) {

				if (step >= payment_start_period)
					payment = short_rates[step][i] - fixed_rate;
			
				prices.push_back((payment + ((q * swap_prices[step + 1][i + 1]) + ((1 - q) * swap_prices[step + 1][i]))) / (1.0 + short_rates[step][i]));
			}
			swap_prices[step] = prices;
			prices.clear();
		}
		return swap_prices;
	}

	vector<vector<double>> swaption_lattice(const vector<vector<double>>& short_rates, const vector<vector<double>>& swap_prices, int swaption_maturity, double strike, double q) {
		vector<vector<double>> swaption_prices(swaption_maturity + 1); 
		vector<double> prices;
		for (int i = 0; i < swap_prices[swaption_maturity].size(); ++i) {
			prices.push_back(max(0.0, swap_prices[swaption_maturity][i]));
		}
		swaption_prices[swaption_maturity] = prices;

		prices.clear();
		for (int step = swaption_maturity - 1; step >= 0; --step) {
			for (int i = 0; i <= step; ++i) {

				prices.push_back((q * swaption_prices[step + 1][i + 1] + (1 - q) * swaption_prices[step + 1][i]) / (1.0 + short_rates[step][i]));
			}
			swaption_prices[step] = prices;
			prices.clear();
		}
		return swaption_prices;
	}




	void zcb_option(double r00, double u, double d, double q, int num_periods, int zcb_maturity) {
		vector<vector<double>> short_rates = short_rate_lattice(r00, u, d, num_periods);
		vector<vector<double>> zcb_prices = zcb_price_lattice(short_rates, zcb_maturity, q);
		
		vector<vector<double>> am_put_lattice = option_price_lattice(short_rates, zcb_prices, 3, 88, 0.5, false, true); // american put
		cout << "American ZCB Put Option price: " << am_put_lattice[0][0] << endl;
		vector<vector<double>> eu_call_lattice = option_price_lattice(short_rates, zcb_prices, 2, 84, 0.5, true, false); // european call
		cout << "European ZCB Call Option Price: " << eu_call_lattice[0][0] << endl;
	}

	void cb_forward_future(double r00, double u, double d, double q, int num_periods, double coupon, int cb_maturity, int forward_maturity) {
		vector<vector<double>> short_rates = short_rate_lattice(r00, u, d, num_periods);
		vector<vector<double>> zcb_prices = zcb_price_lattice(short_rates, forward_maturity, q);
		vector<vector<double>> cb_prices = cb_price_lattice(short_rates, coupon, cb_maturity, q);
		vector<vector<double>> forward_prices = bond_forward_lattice(short_rates, cb_prices, forward_maturity, coupon, q);
		cout << "Forward Price: " << forward_prices[0][0] / (zcb_prices[0][0] / 100.0) << endl;
		vector<vector<double>> future_prices = bond_future_lattice(cb_prices, forward_maturity, coupon, q);
		cout << "Future price: " << future_prices[0][0] << endl;
	}

	void caplet(double r00, double u, double d, double q, int num_periods, int caplet_maturity, double fixed_rate) {
		vector<vector<double>> short_rates = short_rate_lattice(r00, u, d, num_periods);
		vector<vector<double>> caplet_prices = caplet_lattice(short_rates, caplet_maturity, fixed_rate, q);
		cout << "Caplet price: " << caplet_prices[0][0] << endl;
	}

	void swap_swaption(double r00, double u, double d, double q, int num_periods, int swap_maturity, double fixed_rate, int swaption_maturity, double strike) {
		vector<vector<double>> short_rates = short_rate_lattice(r00, u, d, num_periods);
		vector<vector<double>> swap_prices = swap_lattice(short_rates, swap_maturity, fixed_rate, q, 0);
		cout << "Swap Price: " << swap_prices[0][0] << endl;
		vector<vector<double>> swaption_prices = swaption_lattice(short_rates, swap_prices, swaption_maturity, strike, q);
		cout << "Swaption price: " << swaption_prices[0][0] << endl;
	}

	void elementary(double r00, double u, double d, double q, int num_periods) {
		vector<vector<double>> short_rates = short_rate_lattice(r00, u, d, num_periods);
		vector<vector<double>> elementary_prices = elementary_lattice(short_rates, q);
		vector<double> zcb_prices;
		vector<double> spot_rates;
		for (int i = 1; i < elementary_prices.size(); ++i) {
			double summ = accumulate(elementary_prices[i].begin(), elementary_prices[i].end(), 0.0);
			zcb_prices.push_back(summ);
			double spot = pow((1.0 / summ), (1.0 / i)) - 1.0;
			spot_rates.push_back(spot);
		}
		cout << "ZCB prices: " << endl;
		std::string sep1;
		for (auto x : zcb_prices) {
			cout << setprecision(5) << sep1 << x;
			sep1 = ",";
		}
		cout << endl;

		cout << "Spot Rates: " << endl;
		std::string sep2;
		for (auto x : spot_rates) {
			cout << setprecision(5) << sep2 << x;
			sep2 = ",";
		}
		cout << endl;
	}

	void question1() {
		cout << "Compute the price of a zero-coupon bond (ZCB) that matures at time t=10 and that has face value 100." << endl;
		vector<vector<double>> short_rates = short_rate_lattice(0.05, 1.1, 0.9, 10);
		vector<vector<double>> zcb_prices = zcb_price_lattice(short_rates, 10, 0.5);
		cout << "Q1 Answer: " << setprecision(2) << zcb_prices[0][0] << endl << endl;

		cout << "Compute the price of a forward contract on the same ZCB of the previous question where the forward contract matures at time t=4." << endl;
		vector<vector<double>> four_period_zcb_prices = zcb_price_lattice(short_rates, 4, 0.5);
		vector<vector<double>> forward_prices = bond_forward_lattice(short_rates, zcb_prices, 4, 0.0, 0.5);
		cout << "Q2 Answer: " << setprecision(2) << (forward_prices[0][0] / four_period_zcb_prices[0][0]) * 100.0 << endl << endl;

		cout << "Compute the initial price of a futures contract on the same ZCB of the previous two questions. The futures contract has an expiration of t = 4" << endl;
		vector<vector<double>> future_prices = bond_future_lattice(zcb_prices, 4, 0.0, 0.5);
		cout << "Q3 Answer: " << setprecision(2) << future_prices[0][0] << endl << endl;

		cout << "Compute the price of an American call option on the same ZCB of the previous three questions. The option has expiration t=6 and strike =80." << endl;
		vector<vector<double>> option_prices = option_price_lattice(short_rates, zcb_prices, 6, 80.0, 0.5, true, true);
		cout << "Q4 Answer: " << setprecision(2) << option_prices[0][0] << endl << endl;

		cout << "Compute the initial value of a forward-starting swap that begins at t=1, with maturity t=10 and a fixed rate of 4.5%. " << endl;
		cout << "(The first payment then takes place at t = 2 and the final payment takes place at t = 11 as we are assuming, as usual, that payments take place in arrears.) " << endl;
		cout << "You should assume a swap notional of 1 million and assume that you receive floating and pay fixed.)" << endl;
		vector<vector<double>> swap_prices = swap_lattice(short_rates, 11, 0.045, 0.5, 1);
		cout << "Q5 Answer: " << setprecision(0) << swap_prices[0][0] * 1000000.0 << endl << endl;

		cout << "Compute the initial price of a swaption that matures at time t=5 and has a strike of 0. The underlying swap is the same swap as described in the previous question with a notional of 1 million." << endl;
		cout << "To be clear, you should assume that if the swaption is exercised at t=5 then the owner of the swaption will receive all cash-flows from the underlying swap from times t=6 to t=11 inclusive. " << endl;
		cout << "(The swaption strike of 0 should also not be confused with the fixed rate of 4.5% on the underlying swap.)" << endl;
		vector<vector<double>> swaption_prices = swaption_lattice(short_rates, swap_prices, 5, 0.0, 0.5);
		cout << "Q6 Answer:" << setprecision(0) << swaption_prices[0][0] * 1000000.0 << endl << endl;

		
	}


}
