// FERM 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "ferm1.h"
#include "week5.h"
#include "Wk6_Defaultable_Bonds.h"


int main()
{

    //Week 2
	cout << "-----------  Week2 -------------" << endl;
	cout << endl;
	week2::question1();
	cout << endl;
	week2::question2();
	cout << endl;
	week2::question3();
	cout << endl;
	week2::question4();
	cout << endl;
	week2::question5();
	cout << endl;
	week2::question6();
	cout << endl;
	week2::question7();
	cout << endl;
	week2::PS_11();
	cout << endl;

	//Week3
	cout << "-----------  Week3 -------------" << endl;
	cout << endl;
	week3::question1();
	cout << endl;
	week3::question2();
	cout << endl;
	week3::question3();
	cout << endl;
	week3::question4();
	cout << endl;
	week3::question5();
	cout << endl;
	week3::question6();
	cout << endl;

	cout << " Additional questions posed by Scott Tuttle Week 3 Practice Problem Set PS2.1 - PS2.5 " << endl;
	cout << endl;
	week3::PS2_1();
	cout << endl;
	week3::PS2_2();
	cout << endl;

	//Week 4
	cout << "-----------  Week4 -------------" << endl;
	cout << endl;
	week4::question1(true,true);
	cout << endl;
	week4::question2and3and4(false, true);
	cout << endl;
	week4::question5();
	cout << endl;
	week4::question6();
	cout << endl;
	week4::question8(true,false);
	cout << endl;

	//Week 5
	
	cout << "-----------  Week5 -------------" << endl;
	cout << endl;
	cout << "--------   Class Spreadsheet  ------" << endl;
	cout << endl;
	week5::zcb_option(0.06, 1.25, 0.9, 0.5, 5, 4);
	week5::cb_forward_future(0.06, 1.25, 0.9, 0.5, 5, 0.1, 6, 4);
	week5::caplet(0.06, 1.25, 0.9, 0.5, 5, 6, 0.02);
	week5::swap_swaption(0.06, 1.25, 0.9, 0.5, 5, 6, 0.05, 3, 0.0);
	week5::elementary(0.06, 1.25, 0.9, 0.5, 5);
	cout << endl;

	cout << "--------   Quiz  ------" << endl;
	cout << endl;
	week5::question1();
	cout << endl;


	//Week 6 - class spreadsheet
	//BDT_b = 0.005
	cout << "-----------  Week6 -------------" << endl;
	cout << endl;
	cout << "--------   BDT model - Class Spreadsheet  ------" << endl;
	cout << endl;
	cout << "--------   b = 0.005  ------" << endl;
	cout << endl;

	vector<double> spot_rates = { 0.073,0.0762,0.081,0.0845,0.092,0.0964,0.1012,0.1045,0.1075,0.1122,0.1155,0.1192,0.122,0.1232 };
	vector<double> a_guesses = { 7.3,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0 };
	vector<vector<double>> bdt_short_rates = week6::buildBDT(spot_rates, a_guesses, 0.005, 14, 14);
	vector<vector<double>> swap_prices = week5::swap_lattice(bdt_short_rates, 10, 0.1165, 0.5, 0);
	cout << "Swap Price: " << setprecision(6) << swap_prices[0][0] << endl << endl;
	vector<vector<double>> swaption_prices = week5::swaption_lattice(bdt_short_rates, swap_prices, 2, 0.0, 0.5);
	cout << "Swaption price: " << setprecision(6) << swaption_prices[0][0] << endl << endl;

	//BDT_b = 0.01
	cout << "--------   b = 0.01  ------" << endl;
	cout << endl;
	bdt_short_rates = week6::buildBDT(spot_rates, a_guesses, 0.01, 14, 14);
	swap_prices = week5::swap_lattice(bdt_short_rates, 10, 0.1165, 0.5, 0);
	cout << "Swap Price: " << setprecision(6) << swap_prices[0][0] << endl << endl;
	swaption_prices = week5::swaption_lattice(bdt_short_rates, swap_prices, 2, 0.0, 0.5);
	cout << "Swaption price: " << setprecision(6) << swaption_prices[0][0] << endl << endl;

	//Defaultable bond
	cout << "--------   Defaultable Bond Prices  ------" << endl;
	cout << endl;
	week6::get_defaultable_bond_prices();
	cout << endl;

	cout << "--------   Calibration of Hazard Rates, this did not work as expected  ------" << endl;
	cout << " The class spreadsheet has only 5 bonds and there are 10 hazard rates to be calibrated" << endl;
	cout << " So I think a short cut was taken by setting 2 consecutive hazard rates equal " << endl;
	cout << " and that's the part which is hard to automate outside Excel" << endl;
	cout << endl;
	vector<double> market_bond_prices = { 101.23,92.63,107.39,104.12,145.90 };
	vector<double> hazard_rates_guesses(11);
	fill(hazard_rates_guesses.begin(), hazard_rates_guesses.end(), 0.01);
	vector<double> hazard_rates = week6::calibrate_hazard_rates(market_bond_prices, hazard_rates_guesses, 0.05, 5, 2);

	cout << "--------   CDS NPV by using a fair spread of 218.8895, it should be very close to zero  ------" << endl;
	//CDS - 2 years maturity, quarterly payments
	hazard_rates = { 0.01007,0.01007,0.01007,0.01007,0.00972,0.00972,0.00972,0.00972,0.00972 };
	HazardRateTermStructure hzStructure(HazardRateTermStructure(2, 4, hazard_rates, 0.01));
	CDS cds(218.88953946553, 1000000.0, 0.45, 0.01, 2, 4);
	double npv = cds.price_cds(hzStructure);
	cout << setprecision(4) << "NPV of CDS: " << npv << endl << endl;;

	//CDS calibration
	cout << "--------   Calibrating fair spread of CDS, it should be very close to 218.8895  ------" << endl;
	vector<double> spread_guess = { 100.0 };
	double spread = week6::calibrate_cds(hzStructure, spread_guess, 1000000.0, 0.45, 0.01, 2, 4);
	cout << "The CDS spread that makes NPV = 0: " << setprecision(4) << spread << endl << endl;

	cout << "--------  Week6 Quiz  ------" << endl << endl;
	spot_rates = { 0.03, 0.031, 0.032,0.033, 0.034,0.035, 0.0355, 0.036, 0.0365, 0.037 };
	a_guesses = {3.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0 };
	bdt_short_rates = week6::buildBDT(spot_rates, a_guesses, 0.05, 10, 10);
	swap_prices = week5::swap_lattice(bdt_short_rates, 10, 0.039, 0.5, 0);
	swaption_prices = week5::swaption_lattice(bdt_short_rates, swap_prices, 3, 0.0, 0.5);
	cout << "Answer Swaption price b = 0.05: " << setprecision(0) << swaption_prices[0][0] * 1000000.0 << endl << endl;

	bdt_short_rates = week6::buildBDT(spot_rates, a_guesses, 0.1, 10, 10);
	swap_prices = week5::swap_lattice(bdt_short_rates, 10, 0.039, 0.5, 0);
	swaption_prices = week5::swaption_lattice(bdt_short_rates, swap_prices, 3, 0.0, 0.5);
	cout << "Answer Swaption price b = 0.1: " << setprecision(0) << swaption_prices[0][0] * 1000000.0 << endl << endl;


	vector<vector<double>> short_lattice = week5::short_rate_lattice(0.05, 1.1, 0.9, 10);
	vector<vector<double>> hazard_lattice = hazard_rate_lattice(0.01, 1.01, 10);
	vector<vector<double>> defaultable_zcb_prices = defaultable_zcb_price_lattice(short_lattice, hazard_lattice, 10, 0.5, 20.0);
	cout << "Answer Defaultable ZCB price: " << setprecision(2) << defaultable_zcb_prices[0][0] << endl << endl;
	
	cout << " ------  Question 4 - Missing an answer ---------" << endl << endl;

	//CDS - 5 years maturity, quarterly payments
	hazard_rates.resize(5 * 4);
	fill(hazard_rates.begin(), hazard_rates.end(), 0.01);
	hzStructure = HazardRateTermStructure(5, 4, hazard_rates, 0.05);
	spread_guess = { 100.0 };
	spread = week6::calibrate_cds(hzStructure, spread_guess, 10000000.0, 0.25, 0.05, 5, 4);
	cout << "The CDS spread that makes NPV = 0: " << setprecision(2) << spread << endl << endl;

	cout << "--------  Week7 Quiz  ------" << endl << endl;
	cout << "I have not coded week 7 quiz as it is much easier to do on the spreadsheet by changing parameters" << endl;
	cout << "Answer 1: 2147.29" << endl;
	cout << "Answer 2: 171.18" << endl;
	cout << "Answer 3: 181.09" << endl;
	cout << "Answer 4: 268.15" << endl;
	cout << "Answer 5: 280.10" << endl;
	cout << "Answer 6: 133.23" << endl;
	cout << "Answer 7: 6.01" << endl;
	cout << "Answer 8: 7.17" << endl;
	cout << "I did not get this right : Answer 9: 9.58" << endl << endl;
	


}

