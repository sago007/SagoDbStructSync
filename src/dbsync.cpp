#include <iostream>
#include <boost/program_options.hpp>
#include "sago/dbsync.hpp"
#include <cereal/archives/json.hpp>
#include <fstream>
#include "sago/DbSyncDbOracle.hpp"
#include "sago/DbSyncDbMySql.hpp"

using std::cout;
using std::vector;

int main(int argc, const char* argv[])
{
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Print basic usage information to stdout and quits")
		("connectstring", boost::program_options::value<std::string>(), "A string for use with ")
		("schema,s", boost::program_options::value<std::string>(), "A string for use with ")
		("output-file,o", boost::program_options::value<std::string>(), "The output file. If blank stdout is used. If not set no output are generated")
		("input-file,i", boost::program_options::value<std::string>(), "The input file. If blank stdin is used. If not set no input is read")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		cout << desc << "\n";
		return 1;
	}
	std::string connectstring = "mysql:database=dbsync_test;user=testuser;password=password;opt_reconnect=1";
	if (vm.count("connectstring")) {
		connectstring = vm["connectstring"].as<std::string>();
	}
	std::ifstream inputFileStream;
	std::istream* input = &std::cin;
	bool readInput = false;
	if (vm.count("input-file")) {
		readInput = true;
		std::string inputFileName = vm["input-file"].as<std::string>();
		if (inputFileName.length() > 0) {
			inputFileStream.open(inputFileName);
			input = &inputFileStream;
		}
	}
	std::ofstream outputFileStream;
	std::ostream* output = &std::cout;
	bool writeOutput = false;
	if (vm.count("output-file")) {
		writeOutput = true;
		std::string outputFileName = vm["output-file"].as<std::string>();
		if (outputFileName.length() > 0) {
			outputFileStream.open(outputFileName);
			output = &outputFileStream;
		}
	}
	std::string schema_name = "dbsync_test";
	if (vm.count("schema")) {
		schema_name = vm["schema"].as<std::string>();
	}
	//std::shared_ptr<cppdb::session> db(new cppdb::session(connectstring));
	std::shared_ptr<cppdb::session> db = std::make_shared<cppdb::session>(connectstring);
	std::shared_ptr<sago::database::DbSyncDb> dbi;
	{
		dbi = std::shared_ptr<sago::database::DbSyncDb>(new DbSyncDbMySql(db,schema_name));
	}
	sago::database::DbDatabaseModel dbm;
	if (readInput) {
		{
			cereal::JSONInputArchive archive( *input );
			archive (cereal::make_nvp("dbm",dbm));
		}	
		sago::database::ApplyDataModel(dbm, *dbi);
	}
	if (writeOutput) {
		dbm = sago::database::ExtractDataModel(*dbi);
		{
			cereal::JSONOutputArchive archive( *output );
			archive ( cereal::make_nvp("dbm",dbm));
		}
	}
	return 0;
}
