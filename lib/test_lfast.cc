/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cppunit/TextTestRunner.h>
#include <cppunit/XmlOutputter.h>

#include <gnuradio/unittests.h>
#include "qa_lfast.h"
#include <iostream>
#include <fstream>

#include <chrono>
#include "costas2_impl.h"

int largeBlockSize=8192;

using namespace gr::lfast;

void timeCostasLoop() {
	int localblocksize=largeBlockSize;

	std::cout << "Testing Costas Loop with " << localblocksize << " samples..." << std::endl;

	costas2_impl *test;
	test = new costas2_impl(0.00199,2);

	int i;
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::vector<int> ninitems;


	std::vector<gr_complex> inputItems;
	std::vector<gr_complex> outputItems;
	std::vector<gr_complex> outputItems2;
	std::vector<const void *> inputPointers;
	std::vector<void *> outputPointers;
	std::vector<void *> outputPointers2;

	gr_complex grZero(0.0,0.0);
	gr_complex newComplex(1.0,0.5);

	for (i=0;i<localblocksize;i++) {
		inputItems.push_back(gr_complex(1.0f,0.5f));
		outputItems.push_back(grZero);
		outputItems2.push_back(grZero);
	}

	inputPointers.push_back((const void *)&inputItems[0]);
	outputPointers.push_back((void *)&outputItems[0]);
	outputPointers2.push_back((void *)&outputItems2[0]);

	ninitems.push_back(localblocksize);

	int noutputitems;
	int iterations = 100;
	float elapsed_time,throughput_original,throughput;

	noutputitems = test->work_original(localblocksize,inputPointers,outputPointers);

	start = std::chrono::steady_clock::now();
	// make iterations calls to get average.
	for (i=0;i<iterations;i++) {
		noutputitems = test->work_original(localblocksize,inputPointers,outputPointers);
	}
	end = std::chrono::steady_clock::now();

	elapsed_seconds = end-start;

	elapsed_time = elapsed_seconds.count()/(float)iterations;
	throughput_original = localblocksize / elapsed_time;

	std::cout << "Original Code Run Time:   " << std::fixed << std::setw(11)
    << std::setprecision(6) << elapsed_time << " s  (" << throughput_original << " sps)" << std::endl;

	// New Code
	// -----------------------------
	start = std::chrono::steady_clock::now();
	// make iterations calls to get average.
	for (i=0;i<iterations;i++) {
		noutputitems = test->work_test(localblocksize,inputPointers,outputPointers);
	}
	end = std::chrono::steady_clock::now();

	elapsed_seconds = end-start;

	elapsed_time = elapsed_seconds.count()/(float)iterations;
	throughput = localblocksize / elapsed_time;

	std::cout << "LFAST Code Run Time:   " << std::fixed << std::setw(11)
    << std::setprecision(6) << elapsed_time << " s  (" << throughput << " sps)" << std::endl;

	float faster = (throughput / throughput_original - 1) * 100.0;
	std::cout << "Speedup:   " << std::fixed << std::setw(11)
    << std::setprecision(2) << faster << "% faster" << std::endl << std::endl;

	delete test;
}

int
main (int argc, char **argv)
{
	/*
  CppUnit::TextTestRunner runner;
  std::ofstream xmlfile(get_unittest_path("lfast.xml").c_str());
  CppUnit::XmlOutputter *xmlout = new CppUnit::XmlOutputter(&runner.result(), xmlfile);

  runner.addTest(qa_lfast::suite());
  runner.setOutputter(xmlout);

  bool was_successful = runner.run("", false);

  return was_successful ? 0 : 1;
  */

	timeCostasLoop();
	return 0;

}
