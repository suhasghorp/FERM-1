// FERM 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "ferm1.h"

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





}

