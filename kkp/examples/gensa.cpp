////////////////////////////////////////////////////////////////////////////////
// gensa.cpp
//   Computes the suffix array of a given file.
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

#include <iostream>
#include <fstream>

#include <cstdlib>
#include <ctime>

#include "divsufsort.h"
#include "common.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "usage: " << argv[0] << " infile outfile           \n\n"
      << "Computes the suffix array of infile and stores into outfile.\n";
    std::exit(EXIT_FAILURE);
  }

  // Read the text.
  unsigned char *text;
  int length;
  read_text(argv[1], text, length);

  // Alocate and compute the suffix array.  
  int *sa = new int[length];
  if (!sa) {
    std::cerr << "\nError: allocating " << length << " words failed\n";
    std::exit(EXIT_FAILURE);
  }
  std::cerr << "Computing suffix array... ";
  std::clock_t timestamp = std::clock();
  divsufsort(text, sa, length);
  std::cerr << elapsed(timestamp) << " secs\n";

  // Write the output on standard output.
  std::cerr << "Writing the output to " << argv[2] << "... ";
  std::fstream outfile(argv[2], std::fstream::out);
  outfile.write((char *)sa, sizeof(int) * length);
  outfile.close();
  std::cerr << std::endl;

  // Clean up.
  delete[] text;
  delete[] sa;
  return EXIT_SUCCESS;
}

