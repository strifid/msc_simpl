#include <iostream>
#include <stdexcept>
#include <mt/common/XmlConfig.h>
#include <mt/logger/LoggerFactory.h>
#include <mt/logger/Logger.h>

#include <mt/tester/CppUTest/CommandLineTestRunner.h>

int main(int argc, char* argv[]) {

	std::string logCfg = "<logger><logfile path='./test_debug.log' level='TRACE' default='true' stdout='false'></logfile><logger>";
	mt::LoggerFactory::instance()->init(new mt::XmlConfig(logCfg.c_str()));

	return CommandLineTestRunner::RunAllTests(argc, argv);
}
