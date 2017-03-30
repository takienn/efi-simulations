/*
 * wifi-survive.cc
 *
 *  Created on: Jul 23, 2016
 *      Author: Taqi Ad-Din
 */

#include "experiment.h"
#include "efi-topology-reader.h"

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <ostream>

using namespace ns3;

int main (int argc, char *argv[])
{

	bool efiActive = true;
	CommandLine cmd;
	cmd.AddValue("efiActive", "normal or Efi mode", efiActive);
	Experiment experiment;

	EfiTopologyReader topoReader;
	topoReader.SetFileName("scratch/NS3Input.txt");


	std::vector<std::vector<NodeSpec> > nodesList = topoReader.ReadNodeSpec();

	for(std::vector<std::vector<NodeSpec> >::iterator it = nodesList.begin(); it != nodesList.end(); it++)
	  {
		std::cout << "--------- Running new experiment ---------" << std::endl;
		std::cout.flush();

	    Experiment experiment;
	    experiment.CreateNodes(*it, efiActive);
	    experiment.Run(false, 10);
	    experiment.Run(true);
	    experiment.Destroy();

	  }

	return 0;
}



