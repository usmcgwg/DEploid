/*
 * pfDeconv is used for deconvoluting Plasmodium falciparum genome from
 * mix-infected patient sample.
 *
 * Copyright (C) 2016, Sha (Joe) Zhu, Jacob Almagro and Prof. Gil McVean
 *
 * This file is part of pfDeconv.
 *
 * scrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "pfDeconvIO.hpp"
#include <cassert>       // assert

PfDeconvIO::PfDeconvIO( const char plafFileName[],
              const char refFileName[],
              const char altFileName[],
              size_t kStrain ){
    plafFileName_ = string(plafFileName);
    altFileName_ = string(altFileName);
    refFileName_ = string(refFileName);
    (void)readFileLines( refFileName_.c_str(), this->refCount_);
    (void)readFileLines( altFileName_.c_str(), this->altCount_);
    (void)readFileLines( plafFileName_.c_str(), this->plaf_);
    this->nLoci_ = refCount_.size();
    assert( this->nLoci_ == this->plaf_.size() );
    assert( this->altCount_.size() == this->nLoci_ );

    this->kStrain_ = kStrain;
}


PfDeconvIO::~PfDeconvIO(){}


PfDeconvIO::PfDeconvIO(int argc, char *argv[]) {
    argv_ = std::vector<std::string>(argv + 1, argv + argc);
    init();
    this->parse();
}


void PfDeconvIO::init() {
    this->seed_set_ = false;
    this->set_seed( 0 );
    this->set_help(false);
    this->precision_ = 8;
    this->prefix_ = "pf3k-pfDeconv";
    this->kStrain_ = 5;
    this->argv_i = argv_.begin();
}


void PfDeconvIO::parse (){
    if ( argv_.size() == 0 ) {
        this->set_help(true);
        return;
    }

    do {
        if (*argv_i == "-ref") {
            this->readNextStringto ( this->refFileName_ ) ;
        } else if (*argv_i == "-alt") {
            this->readNextStringto ( this->altFileName_ ) ;
        } else if (*argv_i == "-plaf") {
            this->readNextStringto ( this->plafFileName_ ) ;
        } else if (*argv_i == "-panel") {
            this->readNextStringto ( this->panelFileName_ ) ;
        } else if (*argv_i == "-o") {
            this->readNextStringto ( this->prefix_ ) ;
        } else if ( *argv_i == "-p" ) {
            this->precision_ = readNextInput<size_t>() ;
        } else if ( *argv_i == "-k" ) {
            this->kStrain_ = readNextInput<size_t>() ;
        } else if (*argv_i == "-seed"){
            this->random_seed_ = readNextInput<size_t>() ;
            this->seed_set_ = true;
        } else if (*argv_i == "-h" || *argv_i == "-help"){
            this->set_help(true);
        } else {
            throw std::invalid_argument(std::string("unknown/unexpected argument: ") + *argv_i);
        }
    } while ( ++argv_i != argv_.end());

    (void)checkInput();

    (void)readFileLines( refFileName_.c_str(), this->refCount_);
    (void)readFileLines( altFileName_.c_str(), this->altCount_);
    (void)readFileLines( plafFileName_.c_str(), this->plaf_);
    this->nLoci_ = refCount_.size();
    assert( this->nLoci_ == this->plaf_.size() );
    assert( this->altCount_.size() == this->nLoci_ );
}


void PfDeconvIO::checkInput(){
    if ( this->refFileName_.size() == 0 )
        throw std::invalid_argument ( "Ref count file path missing!" );
    if ( this->altFileName_.size() == 0 )
        throw std::invalid_argument ( "Alt count file path missing!" );
    if ( this->plafFileName_.size() == 0 )
        throw std::invalid_argument ( "PLAF file path missing!" );
    if ( this->panelFileName_.size() == 0 )
        throw std::invalid_argument ( "Reference panel file path missing!" );
}


void PfDeconvIO::readNextStringto( string &readto ){
    string tmpFlag = *argv_i;
    ++argv_i;
    if (argv_i == argv_.end() || (*argv_i)[0] == '-' ) throw std::invalid_argument(std::string("Not enough parameters when parsing options: ") + tmpFlag);
    readto = *argv_i;
}


void PfDeconvIO::readFileLines(const char inchar[], vector <double> & out_vec){
    ifstream in_file( inchar );
    string tmp_line;
    if ( in_file.good() ){
        getline ( in_file, tmp_line ); // skip the first line, which is the header
        getline ( in_file, tmp_line );
        while ( tmp_line.size() > 0 ){
            size_t field_start = 0;
            size_t field_end = 0;
            size_t field_index = 0;

            while ( field_end < tmp_line.size() ){
                field_end = min ( tmp_line.find('\t',field_start), tmp_line.find('\n', field_start) );
                string tmp_str = tmp_line.substr( field_start, field_end - field_start );

                if ( field_index == 2 ){
                    out_vec.push_back( strtod(tmp_str.c_str(), NULL) );
                }
                field_start = field_end+1;
                field_index++;
          }
          getline ( in_file, tmp_line );
      }
    } else {
        throw std::invalid_argument("Invalid input file. " + string (inchar) );

    }
    in_file.close();
}


void PfDeconvIO::printHelp(){
    cout << " print help "<< endl;
     //pfDeconv -ref tests/PG0390_first100ref.txt -alt tests/PG0390_first100alt.txt -plaf tests/labStrains_first100_PLAF.txt -panel tests/lab_first100_Panel.txt -o tmp1
}
