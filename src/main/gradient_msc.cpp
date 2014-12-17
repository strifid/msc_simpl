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

	GradientProcessorQuad* proc = new GradientProcessorQuad();

	std::string inputFile;
	GradientProcessor::DataType isFits = GradientProcessor::IMAGE;

	while ((c = getopt(argc, argv, "p:f:o:i:s:")) != -1)
		switch (c) {
		case 'p':
			proc->m_persistence = atoi(optarg);
			break;
		case 'f':
			proc->m_gradFieldFile = optarg;
			break;
		case 'o':
			proc->m_outputFile = optarg;
			break;
		case 'i':
			inputFile = optarg;
			break;
		case 's':
			inputFile = optarg;
			isFits = GradientProcessor::FITS;
			break;
		default:
			std::cout << help << std::endl;
			break;
		}

	if (inputFile.empty()) {
		std::cout << help << std::endl;
		return -1;
	}
	proc->loadData(inputFile, isFits);
	proc->run();

	return 0;
}
