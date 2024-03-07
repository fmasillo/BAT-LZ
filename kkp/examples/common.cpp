////////////////////////////////////////////////////////////////////////////////
// common.cpp
//   Common routines.
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013 Juha Karkkainen, Dominik Kempa and Simon J. Puglisi
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <ctime>

#include <sys/time.h>

#include <iostream>
#include <fstream>

#include "common.h"

double elapsed(std::clock_t timestamp) {
  return ((double)std::clock() - timestamp) / CLOCKS_PER_SEC;
}

long double wclock() {
  timeval tim;
  gettimeofday(&tim, NULL);
  return tim.tv_sec + (tim.tv_usec / 1000000.0L);
}

long double welapsed(long double ts) {
  return wclock() - ts;
}

void read_text(const char *filename, unsigned char* &text, int &length) {
  std::fstream f(filename, std::fstream::in);
  if (f.fail()) {
    std::cerr << "\nError: cannot open file " << filename << "\n";
    std::exit(EXIT_FAILURE);
  }

  f.seekg(0, std::ios_base::end);
  length = f.tellg();
  f.seekg (0, std::ios_base::beg);

  text = new unsigned char[length];
  if (!text) {
    std::cerr << "\nError: allocation of " << length << " bytes failed\n";
    std::exit(EXIT_FAILURE);
  }

  std::cerr << "Reading the file " << filename << " (" << length << " bytes)... ";
  f.read((char *)text, length);
  if (!f) {
    std::cerr << "\nError: failed to read " << length << " bytes from file "
      << filename << ". Only " << f.gcount() << " could be read\n";
    std::exit(EXIT_FAILURE);
  }
  std::cerr << std::endl;
  f.close();
}

void read_sa(const char *filename, int* &sa, int length) {
  std::string fname = std::string(filename) + ".sa";
  sa = new int[length + 2];
  if (!sa) {
    std::cerr << "\nError: allocation of " << length << " words failed\n";
    std::exit(EXIT_FAILURE);
  }
  std::fstream f(fname.c_str(), std::fstream::in);
  if (f.fail()) {
    std::cerr << "\nError: cannot open file " << fname << "\n";
    std::exit(EXIT_FAILURE);
  }
  std::cerr << "Reading the suffix array... ";
  f.read((char *)sa, sizeof(int) * length);
  if (!f) {
    std::cerr << "\nError: failed to read " << sizeof(int) * length
      << " bytes from file " << fname << ". Only " << f.gcount()
      << " could be read\n";
    std::exit(EXIT_FAILURE);
  }
  std::cerr << std::endl;
  f.close();
}
