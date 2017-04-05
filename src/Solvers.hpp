/*
  Solvers.hpp
  "overturner"

  Created by Renaud Dufays on 17/03/17.
  Copyright © 2017. All rights reserved.
*/

#ifndef SOLVERSHEADERFILE
#define SOLVERSHEADERFILE

#include "Particles2D.hpp"
#include "Utilities.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

/*-------------- Abstract base class : Solver ---------------------*/
class Solver
{
	protected:
		Particles2D mParticles;
		double mFinalTime;
		double mDt;
		unsigned seed;
		std::default_random_engine generator;
		std::normal_distribution<double> wiener;

	public:
		Solver(int N, double yStart, double zStart, double dt, double T);
		Solver(int N, double* yStart, double* zStart, double dt, double T);
		Solver(Particles2D particles, double dt, double T);
		virtual void UpdatePosition() =0;		// Pure virtual function
		virtual void UpdatePositionAdim() =0;	// Pure virtual function
		Particles2D Run();
		Particles2D RunAdim();
		void DisplayParticles() const;
		void PrintParticles() const;
		void PrintParameters() const;
		void TestWiener();
};

/*--------------Derived class from Solver : Euler-Maruyama (EM) solver-----------*/
class EMSolver : public Solver
{
	public:
		EMSolver(int N, double yStart, double zStart, double dt, double T);
		EMSolver(int N, double* yStart, double* zStart, double dt, double T);
		EMSolver(Particles2D particles, double dt, double T);
		void UpdatePosition();
		void UpdatePositionAdim();
};

/*--------------Derived class from Solver : Backward Ito (BI) solver-----------*/
class BISolver : public Solver
{
	public:
		BISolver(int N, double yStart, double zStart, double dt, double T);
		BISolver(int N, double* yStart, double* zStart, double dt, double T);
		BISolver(Particles2D particles, double dt, double T);
		void UpdatePosition();
		void UpdatePositionAdim();
};

#endif