#include <stdio.h>
#include "../GradientProcessor.h"
#include <iosfwd>
#include <string>
#include <mt/common/XmlConfig.h>
#include <mt/logger/LoggerFactory.h>
#include "../GradientProcessorQuad.h"

const std::string help = "usage:\n-i <input file> - mandatory option\n-p <persistence> - default 0\n-o <output file>"
		" - original image with msComplexes\n-f <fiel file> - file with gradient fild";

int main(int argc, char* argv[]) {

	int aflag = 0;
	int bflag = 0;
	char *cvalue = NULL;
	int index;
	int c;
	opterr = 0;

	uint32_t persistence = 0;
	std::string inputFile;
	std::string gradField;
	std::string outputFile;
	bool isFits = false;

	while ((c = getopt(argc, argv, "p:f:o:i:s:")) != -1)
		switch (c) {
		case 'p':
			persistence = atoi(optarg);
			break;
		case 'f':
			gradField = optarg;
			break;
		case 'o':
			outputFile = optarg;
			break;
		case 'i':
			inputFile = optarg;
			break;
		case 's':
			inputFile = optarg;
			isFits = true;
			break;
		default:
			std::cout << help << std::endl;
			break;
		}

	if (inputFile.empty()) {
		std::cout << help << std::endl;
		return -1;
	}
	std::string loggerCfg = "<logger><logfile path='./debug.log' default='true' level='TRACE' stdout='false'/></logger>";
	mt::XmlConfigPtr cfg = new mt::XmlConfig(loggerCfg.c_str());
	mt::LoggerFactory::instance()->init(cfg);
//	GradientProcessorTriangle* proc = new GradientProcessorTriangle();
	GradientProcessorQuad* proc = new GradientProcessorQuad();
	proc->m_persistence = persistence;
	proc->m_gradFieldFile = gradField;
	proc->m_outputFile = outputFile;
	if (isFits)
		proc->loadFitsData(inputFile);
	else
		proc->loadImageData(inputFile);
	proc->run();

	return 0;
}
