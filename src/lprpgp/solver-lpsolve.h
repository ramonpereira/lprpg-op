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

#ifndef SOLVERLPSOLVE_H
#define SOLVERLPSOLVE_H

#include "solver.h"

#include <lpsolve/lp_lib.h>

class MILPSolverLPSolve : public MILPSolver {

    private:
        lprec* lp;
        
        bool solvedYet;
        bool hasIntegerVariables;        

        static double * scratchW;
        static int * scratchI;        
        static int scratchSize;
        static void transferToScratch(const vector<pair<int,double> > & entries);
        
        double * tmpSolution;
        
        MILPSolverLPSolve(const MILPSolverLPSolve & c);
        
    public:
        MILPSolverLPSolve();
        virtual ~MILPSolverLPSolve();
        
        virtual MILPSolver * clone();
        
        virtual double getInfinity();
        
        virtual int getNumCols();
        virtual int getNumRows();
        
        virtual void setColName(const int & var, const string & asString);
        virtual string getColName(const int & var);
        virtual void setRowName(const int & cons, const string & asString);
        virtual string getRowName(const int & cons);
        
        //virtual void setInteger(const int & var);
        
        virtual double getColUpper(const int & var);
        virtual void setColUpper(const int & var, const double & b);
        virtual double getColLower(const int & var);
        virtual void setColLower(const int & var, const double & b);
        virtual void setColBounds(const int & var, const double & lb, const double & ub);
                        
        virtual bool isColumnInteger(const int & var);
        
        virtual double getRowUpper(const int & var);
        virtual void setRowUpper(const int & c, const double & b);
        virtual double getRowLower(const int & var);
        virtual void setRowLower(const int & c, const double & b);
        
        virtual void addCol(const vector<pair<int,double> > & entries, const double & lb, const double & ub, const ColumnType & type);
        virtual void addRow(const vector<pair<int,double> > & entries, const double & lb, const double & ub);
        virtual void setMaximiseObjective(const bool & maxim);
        
        virtual void clearObjective();
        virtual void setObjective(double * const entries);
        virtual void setObjCoeff(const int & var, const double & w);
        virtual void writeLp(const string & filename);
        virtual bool solve(const bool & skipPresolve);        
        virtual const double * getSolution();
        virtual const double * getSolutionRows();
        virtual const double * getPartialSolution(const int & from, const int & to);
        virtual double getSingleSolutionVariableValue(const int & col);
        
        virtual double getObjValue();
        
        virtual void getRow(const int & i, vector<pair<int,double> > & entries);
        
        virtual bool supportsQuadratic() const;
        
        virtual void hush();
        
        virtual bool copyBeforePresolving() const {
            return true;
        }
        
        /*virtual bool warmStartingIsUnreliable() const {
            return true;
        }*/

};

#endif
