/*
  KernelEstimator.cpp
  "overturner"

  Created by Renaud Dufays on 04/04/17
  Copyright © 2017. All rights reserved.
*/

#include "Estimator.hpp"

Estimator::Estimator(int nboxy, int nboxz, double H, double L):
mNboxy(nboxy), mNboxz(nboxz), mH(H), mL(L), mEstimator(nboxy,nboxz)
{}

void Estimator::Print(std::ofstream& f, bool binary) const
{
	f.setf(std::ios::scientific); f.precision(10);
	if (binary)
		mEstimator.PrintBinary(f);
	else
		mEstimator.Print(f);
    f.close();
}

BoxEstimator::BoxEstimator(int nboxy, int nboxz, double H, double L):
Estimator(nboxy,nboxz,H,L)
{}

void BoxEstimator::Estimate(const Particles2D& particles)
{
	int i, j;
	int N = particles.mN;
	double dy = mL/mNboxy;
	double dz = mH/mNboxz;
	for (int n=0; n<N; n++)
	{
		i = std::min(int(particles.mY[n]/dy),mNboxy-1);
		j = std::min(int(particles.mZ[n]/dz),mNboxz-1);
		mEstimator(i,j) += 1.;
	}
	mEstimator /= N;
}