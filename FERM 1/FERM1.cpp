// FERM 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "ferm1.h"
#include "week5.h"

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
	vector<double> spot_rates = { 0.073,0.0762,0.081,0.0845,0.092,0.0964,0.1012,0.1045,0.1075,0.1122,0.1155,0.1192,0.122,0.1232 };
	vector<double> a_guesses = { 7.3,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0 };
	vector<vector<double>> bdt_short_rates = week6::buildBDT(spot_rates, a_guesses, 0.005, 14, 14, 0.05);
	vector<vector<double>> swap_prices = week5::swap_lattice(bdt_short_rates, 10, 0.1165, 0.5, 0);
	cout << "Swap Price: " << setprecision(6) << swap_prices[0][0] << endl;
	vector<vector<double>> swaption_prices = week5::swaption_lattice(bdt_short_rates, swap_prices, 2, 0.0, 0.5);
	cout << "Swaption price: " << setprecision(6) << swaption_prices[0][0] << endl;

	//BDT_b = 0.01
	bdt_short_rates = week6::buildBDT(spot_rates, a_guesses, 0.01, 14, 14, 0.05);
	swap_prices = week5::swap_lattice(bdt_short_rates, 10, 0.1165, 0.5, 0);
	cout << "Swap Price: " << setprecision(6) << swap_prices[0][0] << endl;
	swaption_prices = week5::swaption_lattice(bdt_short_rates, swap_prices, 2, 0.0, 0.5);
	cout << "Swaption price: " << setprecision(6) << swaption_prices[0][0] << endl;








}

