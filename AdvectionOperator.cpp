
#include "CrankNicolson.h"
#include "Concentration.h"
#include "Parameters.h"
#include "AdvectionOperator.h"
#include "Utils.h"
#include <cmath>

AdvectionOperator::AdvectionOperator(MODPARAMS::CONCMAT *bc, int active) {
  this->applied = false;
  this->active = active;
  this->bc = *bc;
  initialize_diags();
}

AdvectionOperator::~AdvectionOperator(){};

void AdvectionOperator::check() {
  if (active and not applied)
    Utils::Error("Failed to apply Advection");
  if (not active and applied)
    Utils::Error("Unintended Advection Application");
}

void AdvectionOperator::apply(MODPARAMS::NVECTOR *C){
  if (active)
    applied = true;
    CrankNicolson(C, leftdiags, rightdiags, noflux_diagonal_right, noflux_diagonal_left, BC[6]);
    check();
}

void AdvectionOperator::apply(Concentrations *C){
// To apply to a full NCHEM x N matrix,
// run and slice up for each CONCMAT(n,:) term
  if (active)
    applied = true;
    MODPARAMS::NVECTOR flat_C;
    for (int n=0; n<MODPARAMS::NCHEM; n++)
      flat_C(:) = (C->values)(n,:); // Selects a portion of C corresponding to chemical n
      CrankNicolson(&flat_C, leftdiags, rightdiags); // perturbs this slice
      C->values(n,:) = flat_C; // Reassigns perturbed slice
    check(); // checks for application and errors, inherited method from Operator
    }


void AdvectionOperator::initialize_diags(){

  double rx =   PHYSPARAMS::D * 3600 * MODPARAMS::time_step/pow(MODPARAMS::del_x,2);
  double Crx =  PHYSPARAMS::U * 3600 * MODPARAMS::time_step/(MODPARAMS::del_x);
  double ry =   PHYSPARAMS::D * 3600 * MODPARAMS::time_step/pow(MODPARAMS::del_y,2);
  double Cry =  PHYSPARAMS::V * 3600 * MODPARAMS::time_step/(MODPARAMS::del_y);
  double rz =   PHYSPARAMS::D * 3600 * MODPARAMS::time_step/pow(MODPARAMS::del_z,2);
  double Crz =  PHYSPARAMS::W * 3600 * MODPARAMS::time_step/(MODPARAMS::del_z);

  leftdiags[] =  {        1 + rx + ry + rz,
                          - ry/2 + Cry/4,
                          - rx/2 - Crx/4,
                          - ry/2 + Cry/4,
                          - ry/2 - Cry/4,
                          - rz/2 + Crz/4,
                          - rz/2 - Crz/4};

  rightdiags[] = {        1 + rx - ry - rz,
                          ry/2 + Cry/4,
                          rx/2 - Crx/4,
                          ry/2 + Cry/4,
                          ry/2 - Cry/4,
                          rz/2 + Crz/4,
                          rz/2 - Crz/4]};
  }