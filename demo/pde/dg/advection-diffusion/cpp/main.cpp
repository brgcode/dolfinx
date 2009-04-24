// Copyright (C) 2007-2008 Kristian B. Oelgaard, Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2007-06-29
// Last changed: 2008-12-23
//
// Steady state advection-diffusion equation, discontinuous
// formulation using full upwinding.

#include <dolfin.h>

#include "AdvectionDiffusion.h"
#include "Projection.h"
#include "Velocity.h"

using namespace dolfin;

// Dirichlet boundary condition
class BC : public Function
{
  void eval(double* values, const double* x) const
  {
    values[0] = sin(DOLFIN_PI*5.0*x[1]);
  }
};

// Sub domain for Dirichlet boundary condition
class DirichletBoundary : public SubDomain
{
  bool inside(const double* x, bool on_boundary) const
  {
    return std::abs(x[0] - 1.0) < DOLFIN_EPS && on_boundary;
  }
};

int main(int argc, char *argv[])
{
  error("Demo broken. Needs updating for UFL.");
/*
  // Read simple velocity field (-1.0, -0.4) defined on a 64x64 unit square 
  // mesh and a quadratic vector Lagrange element

  // Read mesh 
  Mesh mesh("../mesh.xml.gz");

  // Create velocity FunctionSpace
  VelocityFunctionSpace V_u(mesh);

  // Create velocity function
  Function velocity(V_u, "../velocity.xml.gz");

  // Diffusivity
  Constant c(0.0); 

  //Source term
  Constant f(0.0);

  // Mesh-related functions
  FacetNormal N;
  AvgMeshSize h;

  // Penalty parameter
  Constant alpha(5.0);

  // Create outflow facet function
  AdvectionDiffusionBilinearFormCoefficientSpace3 V_dg(mesh);
  IsOutflowFacet of(V_dg, velocity);

  // Create function space
  AdvectionDiffusionFunctionSpace V(mesh);

  // Create forms and attach functions
  AdvectionDiffusionBilinearForm a(V, V);
  a.b = velocity; a.n = N; a.h = h; a.of = of; a.kappa = c; a.alpha = alpha;
  AdvectionDiffusionLinearForm L(V);
  L.f = f;

  // Set up boundary condition (apply strong BCs)
  BC g;
  DirichletBoundary boundary;
  DirichletBC bc(V, g, boundary, geometric);

  // Solution function
  Function uh(V);

  // Assemble and apply boundary conditions
  Matrix A;
  Vector b;
  assemble(A, a);
  assemble(b, L);
  bc.apply(A, b);

  // Solve system
  solve(A, uh.vector(), b);

  // Define PDE for projection onto continuous P1 basis
  ProjectionFunctionSpace Vp(mesh);
  ProjectionBilinearForm ap(Vp, Vp);
  ProjectionLinearForm Lp(Vp);
  Lp.u0 = uh;
  VariationalProblem pde(ap, Lp);

  // Compute projection
  Function up;
  pde.solve(up);

  // Save projected solution in VTK format
  File file("temperature.pvd");
  file << up;

  // Plot projected solution
  plot(up);
*/
}
