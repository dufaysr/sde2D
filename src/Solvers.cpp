/*
  Solvers.cpp
  "overturner"

  Created by Renaud Dufays on 17/03/17.
  Copyright © 2017. All rights reserved.
*/

#include <chrono>
#include <random>
#include <iostream>
#include "Solvers.hpp"
#include "workingdirectory.hpp"

using namespace parameters;
/*-------------- Base Class Solver -----------------------*/
Solver::Solver(int N, double yStart, double zStart):
	mParticles(N,yStart,zStart), 
	seed(std::chrono::system_clock::now().time_since_epoch().count()),
	// seed(1),
	generator(seed),
	wiener(0.0,1.0)
{}

Solver::Solver(int N, double* yStart, double* zStart):
	mParticles(N,yStart,zStart), 
	seed(std::chrono::system_clock::now().time_since_epoch().count()),
	// seed(1),
	generator(seed),
	wiener(0.0,1.0)
{}

Solver::Solver(int Nloc, double* yStart, double* zStart, int n):
	mParticles(Nloc,yStart,zStart,n), 
	seed(std::chrono::system_clock::now().time_since_epoch().count()),
	generator(seed),
	wiener(0.0,1.0)
{}

Solver::Solver(int Nloc, double* yStart, double* zStart, int ny, int nz):
	mParticles(Nloc,yStart,zStart,ny,nz),
	seed(std::chrono::system_clock::now().time_since_epoch().count()),
	generator(seed),
	wiener(0.0,1.0)
{}

Solver::Solver(const Particles2D& particles):
	mParticles(particles),
	seed(std::chrono::system_clock::now().time_since_epoch().count()),
	generator(seed),
	wiener(0.0,1.0)
{}

Particles2D& Solver::Run(const AbstractAdvDiffProblem& prob)
{
	for (int i=0; i*dt < T; i++)
	{
		UpdatePosition(prob);
	}
	return mParticles;
}

Particles2D& Solver::Run(const AbstractAdvDiffProblem& prob, std::string model, int nPrint)
{
	std::ofstream fT = openOutputFile(wd::root + "out/" + model + "/time.out");
	std::ofstream fY = openOutputFile(wd::root + "out/" + model + "/Y.out");
	fY.setf(std::ios::scientific); fY.precision(10);
	std::ofstream fZ = openOutputFile(wd::root + "out/" + model + "/Z.out");
	fZ.setf(std::ios::scientific); fZ.precision(10);

	PrintParticles(fT, fY, fZ);
	int i=0;
	while (i*dt < T)
	{
		for (int j=0; j<nPrint; j++)
		{
			UpdatePosition(prob);
			i++;
		}
		PrintParticles(fT,fY,fZ); // print particles every 5 time steps
	}

	fT.close(); fY.close(); fZ.close();

	return mParticles;
}

Particles2D& Solver::RunAdim(const AbstractAdvDiffProblemAdim& prob)
{
	for (int i=0; i*dtPrime < TPrime; i++)
	{
		UpdatePositionAdim(prob);
	}
	return mParticles;
}

Particles2D& Solver::RunAdim(const AbstractAdvDiffProblemAdim& prob, std::string model, int nPrint)
{
	std::ofstream fT = openOutputFile(wd::root + "out/" + model + "/time.out");
	std::ofstream fY = openOutputFile(wd::root + "out/" + model + "/Y.out");
	fY.setf(std::ios::scientific); fY.precision(10);
	std::ofstream fZ = openOutputFile(wd::root + "out/" + model + "/Z.out");
	fZ.setf(std::ios::scientific); fZ.precision(10);

	PrintParticles(fT, fY, fZ);
	int i=0;
	while (i*dtPrime < TPrime)
	{
		for (int j=0; j<nPrint; j++)
		{
			UpdatePositionAdim(prob);
			i++;
		}
		PrintParticles(fT,fY,fZ); // print particles every 5 time steps
	}

	fT.close(); fY.close(); fZ.close();

	return mParticles;
}

void Solver::DisplayParticles() const
{
	for (int i=0; i<mParticles.mN; i++)
	{
		std::cout << i << " : (" << mParticles.mY[i] << ", " << mParticles.mZ[i] << ")" << std::endl;
	}
}

void Solver::PrintParticles(std::ofstream& fT, std::ofstream& fY, std::ofstream& fZ) const
{
	fT << mParticles.mTime << "\n";

    for(int i=0; i<mParticles.mN; i++)
    {
    	fY << mParticles.mY[i] << " ";
  	}
  	fY << "\n";

  	for(int i=0; i<mParticles.mN; i++)
    {
    	fZ << mParticles.mZ[i] << " ";
  	}
  	fZ << "\n";
}

void Solver::TestWiener()
{
	const int nrolls=10000;  // number of experiments
  	const int nstars=100;    // maximum number of stars to distribute

  	int width = 7;
  	int p[width]={};

  	for (int i=0; i<nrolls; ++i) 
  	{
    	double number = wiener(generator);
    	if ((number>-double(width)/2)&&(number<=double(width)/2))
    	{
    		++p[int(round(number))+width/2];
    	}
  	}

  	std::cout << "normal_distribution (0.0,1.0):" << std::endl;

  	for (int i=0; i<width; ++i) 
  	{
    	std::cout << std::setw(4) << i-double(width)/2 << " -> " << std::setw(4) << (i+1)-double(width)/2 << ": ";
    	std::cout << std::string(p[i]*nstars/nrolls,'*') << std::endl;
  	}
}

/*----------------- Derived class from Solver : EMSolver -----------------------*/
/*--- /!\ Does not take the derivative of the diffusivities into account /!\ ---*/

EMSolver::EMSolver(int N, double yStart, double zStart):
	Solver(N,yStart,zStart)
{}

EMSolver::EMSolver(int N, double* yStart, double* zStart):
	Solver(N,yStart,zStart)
{}

EMSolver::EMSolver(int Nloc, double* yStart, double* zStart, int n):
	Solver(Nloc,yStart,zStart,n)
{}

EMSolver::EMSolver(int Nloc, double* yStart, double* zStart, int ny, int nz):
	Solver(Nloc,yStart,zStart,ny,nz)
{}

EMSolver::EMSolver(const Particles2D& particles):
	Solver(particles)
{}

void EMSolver::UpdatePosition(const AbstractAdvDiffProblem& prob)
{
	// construct a trivial random generator engine from a time-based seed:
	for (int i=0; i<mParticles.mN; i++)
	{
		mParticles.mY[i] += prob.getV(mParticles.mY[i],mParticles.mZ[i])*prob.getdt()
							+ sqrt(2*prob.getKh(mParticles.mY[i],mParticles.mZ[i])*prob.getdt())*wiener(generator);
		mParticles.mZ[i] += prob.getW(mParticles.mY[i],mParticles.mZ[i])*prob.getdt() 
							+ sqrt(2*prob.getKv(mParticles.mY[i],mParticles.mZ[i])*prob.getdt())*wiener(generator);
	}
	mParticles.mTime += prob.getdt();
}

void EMSolver::UpdatePositionAdim(const AbstractAdvDiffProblemAdim& prob)
{
	// construct a trivial random generator engine from a time-based seed:
	for (int i=0; i<mParticles.mN; i++)
	{

		mParticles.mY[i] += prob.getVPrime(mParticles.mY[i],mParticles.mZ[i])*prob.getdtPrime()
							+ sqrt(2*prob.getPehInv(mParticles.mY[i],mParticles.mZ[i])*prob.getdtPrime())*wiener(generator);
		mParticles.mZ[i] += prob.getWPrime(mParticles.mY[i],mParticles.mZ[i])*prob.getdtPrime() 
							+ sqrt(2*prob.getPevInv(mParticles.mY[i],mParticles.mZ[i])*prob.getdtPrime())*wiener(generator);
	}
	mParticles.mTime += prob.getdtPrime();
}

/*----------- Derived class from Solver : Backward Ito (BI) Solver ------------------*/

BISolver::BISolver(int N, double yStart, double zStart):
	Solver(N,yStart,zStart)
{}

BISolver::BISolver(int N, double* yStart, double* zStart):
	Solver(N,yStart,zStart)
{}

BISolver::BISolver(int Nloc, double* yStart, double* zStart, int n):
	Solver(Nloc,yStart,zStart,n)
{}

BISolver::BISolver(int Nloc, double* yStart, double* zStart, int ny, int nz):
	Solver(Nloc,yStart,zStart,ny,nz)
{}

BISolver::BISolver(const Particles2D& particles):
	Solver(particles)
{}

void BISolver::UpdatePosition(const AbstractAdvDiffProblem& prob)
{
	double N1, N2, R1, R2, dY, dZ, y, z, v, w;
	for (int i=0; i<mParticles.mN; i++)
	{
		// position and speed of particle i at time t
		y = mParticles.mY[i];
		z = mParticles.mZ[i];
		v = prob.getV(y,z);
		w = prob.getW(y,z);
		// realisations of the noises
		R1 = wiener(generator);
		R2 = wiener(generator);
		// prediction step of the backward-Ito scheme
		dY = sqrt(2*prob.getKh(y,z)*prob.getdt())*R1;
		dZ = sqrt(2*prob.getKv(y,z)*prob.getdt())*R2;
		// amplitude of the noises
		N1 = sqrt(2*prob.getKh(y+dY,z+dZ)*prob.getdt());
		N2 = sqrt(2*prob.getKv(y+dY,z+dZ)*prob.getdt());
		/* update particles positions using backward-Ito scheme
		* 2 options for the no-flux BC : either stick to the wall or bounce on it.
		* Uncomment the one of your choice and comment the other.
		*/
		// 1. Stick to the wall
		// mParticles.mY[i] = std::min(L, std::max(mParticles.mY[i] + v*dt + N1*R1, 0.));
		// mParticles.mZ[i] = std::min(H, std::max(mParticles.mZ[i] + w*dt + N2*R2, 0.));
		// 2. Bounce on the wall
		double ynew = mParticles.mY[i] + v*prob.getdt() + N1*R1;
		double znew = mParticles.mZ[i] + w*prob.getdt() + N2*R2;
		mParticles.mY[i] = (ynew < prob.getL0()) ? 2*prob.getL0()-ynew : (ynew > prob.getL1()) ? 2*prob.getL1()-ynew : ynew;
		mParticles.mZ[i] = (znew < prob.getH0()) ? 2*prob.getH0()-znew : (znew > prob.getH1()) ? 2*prob.getH1()-znew : znew;
	}
	mParticles.mTime += prob.getdt();
}

void BISolver::UpdatePositionAdim(const AbstractAdvDiffProblemAdim& prob)
{
	double N1, N2, R1, R2, dY, dZ, y, z, v, w;
	for (int i=0; i<mParticles.mN; i++)
	{
		// position and speed of particle i at time t
		y = mParticles.mY[i];
		z = mParticles.mZ[i];
		v = prob.getVPrime(y,z);
		w = prob.getWPrime(y,z);
		// realisations of the noises
		R1 = wiener(generator);
		R2 = wiener(generator);
		// prediction step of the backward-Ito scheme
		dY = sqrt(2*prob.getPehInv(y,z)*prob.getdtPrime())*R1;
		dZ = sqrt(2*prob.getPevInv(y,z)*prob.getdtPrime())*R2;
		// amplitude of the noises
		N1 = sqrt(2*prob.getPehInv(y+dY,z+dZ)*prob.getdtPrime());
		N2 = sqrt(2*prob.getPevInv(y+dY,z+dZ)*prob.getdtPrime());
		/* update particles positions using backward-Ito scheme
		* 2 options for the no-flux BC : either stick to the wall or bounce on it.
		* Uncomment the one of your choice and comment the other.
		*/
		// 1. Stick to the wall
		// mParticles.mY[i] = std::min(1., std::max(mParticles.mY[i] + v*dtPrime + N1*R1, 0.));
		// mParticles.mZ[i] = std::min(1., std::max(mParticles.mZ[i] + w*dtPrime + N2*R2, 0.));
		// 2. Bounce on the wall
		double ynew = mParticles.mY[i] + v*prob.getdtPrime() + N1*R1;
		double znew = mParticles.mZ[i] + w*prob.getdtPrime() + N2*R2;
		mParticles.mY[i] = (ynew < 0) ? -ynew : (ynew > 1.) ? 2.-ynew : ynew;
		mParticles.mZ[i] = (znew < 0) ? -znew : (znew > 1.) ? 2.-znew : znew;
	}
	mParticles.mTime += prob.getdtPrime();
}