
#pragma once
#include "ChemOperator.h"
#include "Parameters.h"

class SSControledChemOperator{

  public:

    void apply(Concentrations *C, double hour);
    SSControledChemOperator(ChemistryOperator *O, int active=1);
    ~SSControledChemOperator();

  private:

    double exit_time;
    ChemistryOperator O;

  };