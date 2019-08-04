#pragma once

#include <algorithm>
#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>

using namespace std;

vector<vector<double>> hazard_rate_lattice(double a, double b, int num_periods) {

	vector<vector<double>> hazard_rates(num_periods + 1);

	hazard_rates[0].push_back(0.01);
	for (int i = 1; i <= num_periods; ++i) {
		for (int j = 0; j <= i; ++j) {
			hazard_rates[i].push_back(a * pow(b, j - (i/2.0)));
		}
		//short_rates[i].push_back(u * short_rates[i - 1][i - 1]);
	}

	return hazard_rates;
}

vector<vector<double>> defaultable_zcb_price_lattice(const vector<vector<double>>& short_rates, const vector<vector<double>>& hazard_rates, int zcb_maturity, double q, double recovery) {

	vector<vector<double>> zcb_prices(zcb_maturity + 1);
	vector<double> prices(short_rates[zcb_maturity].size());
	fill(prices.begin(), prices.end(), 100.0);
	zcb_prices[zcb_maturity] = prices;
	prices.clear();
	for (int step = zcb_maturity - 1; step >= 0; --step) {
		for (int i = 0; i <= step; ++i) {
			double first_term = (((q * (1.0 - hazard_rates[step][i]) * zcb_prices[step + 1][i + 1]) + ((1 - q) * (1.0 - hazard_rates[step][i]) * zcb_prices[step + 1][i])) / (1 + short_rates[step][i]));
			double second_term = ((q * hazard_rates[step][i] * recovery) + ((1 - q) * hazard_rates[step][i] * recovery)) / (1 + short_rates[step][i]);
			prices.push_back(first_term + second_term);
		}
		zcb_prices[step] = prices;
		prices.clear();
	}
	return zcb_prices;
}

struct HazardRateTermStructure {

	int m_maturity; // in years for example 5
	int m_frequency; // 1 = Annual, 2 = Semi-annual, 4 = Quarterly
	vector<double> m_hazard_rates; //given hazard rates
	vector<double> m_surv_probs;
	vector<double> m_default_probs;
	vector<double> m_disc_rates;
	double m_r;
	
	HazardRateTermStructure(int maturity, int frequency, vector<double> hazard_rates, double r) : m_maturity(maturity), m_frequency(frequency), m_hazard_rates(hazard_rates), m_r(r) {
		int m_periods = (maturity * frequency);
		
		m_surv_probs.push_back(1.0);
		m_disc_rates.push_back(1.0);
		m_default_probs.push_back(0.0);

		for (int i = 1; i <= m_periods; ++i) {
			m_surv_probs.push_back(m_surv_probs[i - 1] * (1.0 - m_hazard_rates[i - 1]));
			m_default_probs.push_back(m_surv_probs[i - 1] * m_hazard_rates[i - 1]);
			m_disc_rates.push_back(pow((1.0 / (1.0 + (m_r / m_frequency))), i));
		}		
	}

	double get_surv_prob (int period) const {
		return m_surv_probs[period];
	}

	double get_default_prob(int period) const {
		return m_default_probs[period];
	}

	double get_discount_factor(int period) const {
		return m_disc_rates[period];
	}

};

struct DefaultableBond {

	int m_maturity; // years
	int m_frequency; // 1 = Annual, 2 = Semi-annual, 4 = Quarterly
	double m_coupon; 
	double m_face;
	vector<int> m_times;
	vector<double> m_cash_flows;

	DefaultableBond(int maturity, int frequency, double coupon, double face) :m_maturity(maturity), m_frequency(frequency), m_coupon(coupon), m_face(face) {
		m_times.push_back(0); m_cash_flows.push_back(0.0);
		for (int i = 1; i < (m_maturity * m_frequency); ++i) {
			m_times.push_back(i);
			m_cash_flows.push_back(m_coupon * m_face); //coupon payments
		}
		m_times.push_back(m_maturity * m_frequency);
		m_cash_flows.push_back(m_face + (m_coupon * m_face)); //par amount + last coupon
	}

	double get_bond_price(const HazardRateTermStructure& hazardRateTermStructure, double recovery_rate) {
		vector<double> disc_expected_values;
		for (int period = 1; period <= (m_maturity * m_frequency); ++period) {
			double disc_factor = hazardRateTermStructure.get_discount_factor(period);
			double surv_prob = hazardRateTermStructure.get_surv_prob(period);
			double def_prob = hazardRateTermStructure.get_default_prob(period);
			disc_expected_values.push_back(disc_factor * ((m_cash_flows[period] * surv_prob) + (def_prob * recovery_rate)));
		}
		return accumulate(disc_expected_values.begin(), disc_expected_values.end(), 0.0);
	}
};

struct PremiumLeg {
	double fixed_payment;
	double expected_value;
	double present_value;
	double accrued;
	double present_value_accrued;
};

struct ProtectionLeg {
	double protection_payment;
	double present_value;
};

double sum_up_premium_legs(const vector<PremiumLeg>& vec, double PremiumLeg::*member) {
	double sum = 0;
	for (vector<PremiumLeg>::const_iterator it = vec.begin(), end = vec.end(); it != end; ++it)
		sum += (*it).*member;
	return sum;
}

double sum_up_protection_legs(const vector<ProtectionLeg>& vec, double ProtectionLeg::*member) {
	double sum = 0;
	for (vector<ProtectionLeg>::const_iterator it = vec.begin(), end = vec.end(); it != end; ++it)
		sum += (*it).*member;
	return sum;
}

struct CDS {

	double m_spread;
	double m_principal;
	double m_recovery;
	double m_r;
	int m_maturity;
	int m_frequency;
	vector<PremiumLeg> premium_legs;
	vector<ProtectionLeg> protection_legs;


	CDS(double spread, double principal, double recovery, double r, int maturity, int frequency) : m_spread(spread), m_principal(principal), m_recovery(recovery), m_r(r), m_maturity(maturity), m_frequency(frequency) {}
	
	double price_cds(const HazardRateTermStructure& hazardRateStructure) {
		for (int i = 1; i <= (m_maturity * m_frequency); ++i) {

			PremiumLeg premium_leg;
			premium_leg.fixed_payment = m_spread / m_frequency;
			premium_leg.expected_value = premium_leg.fixed_payment * hazardRateStructure.get_surv_prob(i);
			premium_leg.present_value = premium_leg.expected_value * hazardRateStructure.get_discount_factor(i) * m_principal * 0.0001;
			premium_leg.accrued = (premium_leg.fixed_payment / 2.0) * hazardRateStructure.get_default_prob(i);
			premium_leg.present_value_accrued = premium_leg.accrued * hazardRateStructure.get_discount_factor(i) * m_principal * 0.0001;
			premium_legs.push_back(premium_leg);

			ProtectionLeg protection_leg;
			protection_leg.protection_payment = hazardRateStructure.get_default_prob(i) * (1.0 - m_recovery);
			protection_leg.present_value = protection_leg.protection_payment * hazardRateStructure.get_discount_factor(i) * m_principal;
			protection_legs.push_back(protection_leg);		
		}
		double sum_premium_legs = sum_up_premium_legs(premium_legs, &PremiumLeg::present_value) + sum_up_premium_legs(premium_legs, &PremiumLeg::present_value_accrued);
		double sum_protection_legs = sum_up_protection_legs(protection_legs, &ProtectionLeg::present_value);
		return sum_protection_legs - sum_premium_legs;
	}


};


