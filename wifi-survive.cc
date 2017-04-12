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

	bool efi = true;
	double totResources = 10;
	bool downlink = true;
	bool uplink = true;

	std::string inputfile = "scratch/NS3Input_K1.txt";
	CommandLine cmd;
	cmd.AddValue("efi", "normal or Efi mode", efi);
	cmd.AddValue("totalResources", "Total Resources (s)", totResources);
	cmd.AddValue("downlink", "Total Resources (s)", downlink);
	cmd.AddValue("uplink", "Total Resources (s)", uplink);
	cmd.AddValue("inputfile", "Input File", inputfile);


	cmd.Parse(argc, argv);

	Experiment experiment;

	EfiTopologyReader topoReader;
	topoReader.SetFileName(inputfile);


	std::vector<std::vector<NodeSpec> > nodesList = topoReader.ReadNodeSpec();

	for(std::vector<std::vector<NodeSpec> >::iterator it = nodesList.begin(); it != nodesList.end(); it++)
	  {
		std::cout << "--------- Running new experiment ---------" << std::endl;
		std::cout.flush();

	    Experiment experiment;
	    experiment.CreateNodes(*it, efi);
	    if(downlink)
	    	experiment.Run(true, totResources);
	    if(uplink)
	    	experiment.Run(false, totResources);
	    experiment.Destroy();

	  }

	return 0;
}



