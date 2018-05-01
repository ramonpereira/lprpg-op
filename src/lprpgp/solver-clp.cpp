/************************************************************************
 * Copyright(C) 2018: C. Piacentini, M. P. Castro, A. A. Cire, J. C. Beck
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public license as
 * published by the Free Software Foundation; either of the license, or
 * (at your option) any later version.
 *
 * This planner is currently in BETA, and is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public license for more details.
 *
 * To contact the development team, email to
 * <chiarap@mie.utoronto.ca>
 *
 * This builds on LPRPG. The following is the original LPRPG license:
 *
 * Copyright 2008, 2009, Strathclyde Planning Group,
 * Department of Computer and Information Sciences,
 * University of Strathclyde, Glasgow, UK
 * http://planning.cis.strath.ac.uk/
 *
 * Maria Fox, Richard Howey and Derek Long - Code from VAL
 * Stephen Cresswell - PDDL Parser
 * Andrew Coles, Amanda Coles - Code for LPRPG
 *
 * This file is part of LPRPG.
 *
 * LPRPG is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * LPRPG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LPRPG.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#include "solver-clp.h"
#include <OsiClpSolverInterface.hpp>
#include <CglProbing.hpp>


double * MILPSolverCLP::scratchW = 0;
int * MILPSolverCLP::scratchI = 0;
int MILPSolverCLP::scratchSize = 0;

void MILPSolverCLP::transferToScratch(const vector<pair<int,double> > & entries)
{
    const int entSize = entries.size();
    if (entSize > scratchSize) {
        delete [] scratchW;
        delete [] scratchI;
        
        scratchSize = entSize + 16;
        scratchW = new double[scratchSize];
        scratchI = new int[scratchSize];
    }
    
    for (int i = 0; i < entSize; ++i) {
        scratchI[i] = entries[i].first;
        scratchW[i] = entries[i].second;
    }
    
}
        

MILPSolverCLP::MILPSolverCLP()
{
    lp = new OsiClpSolverInterface();
    milp = 0;
    hasIntegerVariables = false;
    /*lp->setSpecialOptions();
    lp->setSpecialOptions(lp->specialOptions()|32768);*/
    solvectl = new ClpSolve();
    solvectl->setSolveType(ClpSolve::usePrimal);
    solvectl->setPresolveType(ClpSolve::presolveOn);
    
    solvedYet = false;
}

MILPSolverCLP::MILPSolverCLP(const MILPSolverCLP & c)
{
    lp = new OsiClpSolverInterface(*(c.lp));
    hasIntegerVariables = c.hasIntegerVariables;
    milp = 0;
    solvectl = new ClpSolve();
    solvectl->setSolveType(ClpSolve::usePrimal);
    solvectl->setPresolveType(ClpSolve::presolveOn);
    
    solvedYet = false;
}


MILPSolverCLP::~MILPSolverCLP()
{
    delete milp;
    delete lp;
    delete solvectl;    
}

MILPSolver * MILPSolverCLP::clone()
{
    return new MILPSolverCLP(*this);
}

double MILPSolverCLP::getInfinity()
{
    return COIN_DBL_MAX;
}

int MILPSolverCLP::getNumCols()
{
    return lp->getNumCols();
}

int MILPSolverCLP::getNumRows()
{
    return lp->getNumRows();
}

void MILPSolverCLP::setColName(const int & var, const string & asString)
{
    static int vcopy;
    static string scopy;
    
    vcopy = var;
    scopy = asString;
    
    lp->setColName(vcopy, scopy);
}

string MILPSolverCLP::getColName(const int & var)
{
    return lp->getColName(var);
}

void MILPSolverCLP::setRowName(const int & cons, const string & asString)
{
    static int vcopy;
    static string scopy;
    
    vcopy = cons;
    scopy = asString;
    
    lp->setRowName(vcopy, scopy);
}

string MILPSolverCLP::getRowName(const int & cons)
{
    return lp->getRowName(cons);
}

/*
void MILPSolverCLP::setInteger(const int & var)
{
    lp->setInteger(var);
}
*/

double MILPSolverCLP::getColUpper(const int & var)
{
    return lp->getColUpper()[var];
}

void MILPSolverCLP::setColUpper(const int & var, const double & b)
{
    lp->setColUpper(var, b);
}

double MILPSolverCLP::getRowUpper(const int & c)
{
    return lp->getRowUpper()[c];
}

void MILPSolverCLP::setRowUpper(const int & c, const double & b)
{
    lp->setRowUpper(c,b);
}

double MILPSolverCLP::getRowLower(const int & c)
{
    return lp->getRowLower()[c];
}

void MILPSolverCLP::setRowLower(const int & c, const double & b)
{
    lp->setRowLower(c,b);
}

double MILPSolverCLP::getColLower(const int & var)
{
    return lp->getColLower()[var];
}

void MILPSolverCLP::setColLower(const int & var, const double & b)
{
    lp->setColLower(var, b);
}

void MILPSolverCLP::setColBounds(const int & var, const double & lb, const double & ub)
{
    lp->setColBounds(var, lb, ub);
}

bool MILPSolverCLP::isColumnInteger(const int & c)
{
    return lp->isInteger(c);
}


void MILPSolverCLP::addCol(const vector<pair<int,double> > & entries, const double & lb, const double & ub, const double &, const ColumnType & type)
{
    if (entries.empty()) {
        lp->addCol(0, (int*)0, (double*)0, lb, ub, 0.0);
        if (type != C_REAL) {
            lp->setInteger(lp->getNumCols() - 1);
            hasIntegerVariables = true;
        }
        return;
    }
    
    transferToScratch(entries);
    lp->addCol(entries.size(), scratchI, scratchW, lb, ub, 0.0);
    if (type != C_REAL) {
        lp->setInteger(lp->getNumCols() - 1);
        hasIntegerVariables = true;
    }
}

void MILPSolverCLP::addRow(const vector<pair<int,double> > & entries, const double & lb, const double & ub)
{
    if (entries.empty()) {
        lp->addRow(0, (int*)0, (double*)0, lb, ub);
        return;
    }

    transferToScratch(entries);
    lp->addRow(entries.size(), scratchI, scratchW, lb, ub);
    
}

void MILPSolverCLP::setMaximiseObjective(const bool & maxim)
{
    if (maxim) {
        lp->setObjSense(-1);
    } else {
        lp->setObjSense(1);        
    }
}

void MILPSolverCLP::setObjective(double * const entries)
{
    lp->setObjective(entries);
}

void MILPSolverCLP::clearObjective()
{
    const int colCount = lp->getNumCols();
    for (int i = 0; i < colCount; ++i) {
        lp->setObjCoeff(i, 0.0);
    }
}
    

void MILPSolverCLP::setObjCoeff(const int & var, const double & w)
{
    lp->setObjCoeff(var,w);
}

void MILPSolverCLP::writeLp(const string & filename)
{
    lp->writeLp(filename.c_str());
}

bool MILPSolverCLP::solve(const bool & skipPresolve)
{
    if (!solvedYet) {
        if (skipPresolve) {
            solvectl->setPresolveType(ClpSolve::presolveOff);
        } else {
            solvectl->setPresolveType(ClpSolve::presolveOn);
        }
        
        lp->setSolveOptions(*solvectl);        
        lp->initialSolve();
        solvedYet = true;
        
    } else {        
        lp->resolve();
    }
    
    if (!lp->isProvenOptimal()) {
        return false;
    }
    
    if (!hasIntegerVariables) {
        return true;
    }
    
    delete milp;
    milp = 0;
    
    //lp->setHintParam(OsiDoReducePrint,true,OsiHintTry);
    
    if (!milp) {
        milp = new CbcModel(*lp);
        //milp->solver()->setHintParam(OsiDoReducePrint,true,OsiHintTry);        
    }

    CglProbing pg;
    pg.setUsingObjective(true);
    pg.setMaxPass(3);
    pg.setMaxProbe(100);
    pg.setMaxLook(50);
    pg.setRowCuts(3);
  
    milp->addCutGenerator(&pg,-1);
    if (MILPSolver::debug) {
        std::cout << "About to call CBC...";
        std::cout.flush();
    }
    
    if (MILPSolver::debug) {
        milp->setLogLevel(3);
    } else {
        milp->setLogLevel(0);
    }
    milp->branchAndBound();
    
    const bool retVal = milp->isProvenOptimal();
    if (MILPSolver::debug) {
        if (retVal) {
            std::cout << "Was proven optimal\n";
        } else {
            std::cout << "Was not proven optimal\n";
        }
        
        if (milp->isProvenInfeasible()) {
            std::cout << "Is proven infeasible\n";
        }
        
        if (milp->isInitialSolveProvenOptimal()) {
            std::cout << "Initial solved is proven optimal\n";
        }
    }
    return retVal;
}

const double * MILPSolverCLP::getSolution()
{
    if (milp) {
        return milp->solver()->getColSolution();
    } else {
        return lp->getColSolution();
    }
}

const double * MILPSolverCLP::getSolutionRows()
{
    if (milp) {
        return milp->solver()->getRowPrice();
    } else {
        return lp->getRowPrice();
    }
}

const double * MILPSolverCLP::getPartialSolution(const int & from, const int &)
{
    return &(getSolution()[from]);
}

double MILPSolverCLP::getObjValue()
{
    if (milp) {
        return milp->getObjValue();
    } else {
        return lp->getObjValue();
    }
}

bool MILPSolverCLP::supportsQuadratic() const
{
    return true;
}


void MILPSolverCLP::hush()
{
    lp->setLogLevel(0);
}

void MILPSolverCLP::getRow(const int & row, vector<pair<int,double> > & entries)
{
    const CoinPackedMatrix * const matrix = lp->getMatrixByRow();
    assert(matrix);

    map<int, map<int, double> > sparseMap;
    
    if (matrix->isColOrdered()) {
        const int lim = matrix->getMajorDim();
        const double * const elements = matrix->getElements();
        const int * const indices = matrix->getIndices();
        const int * const offsets = matrix->getVectorStarts();
        for (int col = 0; col < lim; ++col) {
            
            int index = offsets[col];
            const int limit = offsets[col + 1];
            
            for (; index < limit; ++index) {
                if (indices[index] == row) {
                    entries.push_back(std::make_pair(col, elements[index]));
                }
            }
        }
    } else {
        const double * const elements = matrix->getElements();
        const int * const indices = matrix->getIndices();
        const int * const offsets = matrix->getVectorStarts();
        
        int index = offsets[row+1];        
        const int limit = offsets[row+1];
        
        entries.reserve(limit-index);
        
        for (; index < limit; ++index) {
            entries.push_back(std::make_pair(indices[index], elements[index]));
        }        
    }
}