/*
  Its is under the MIT license, to encourage reuse by cut-and-paste.

  Copyright (c) 2018 Poul Sander

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <iostream>
#include <boost/program_options.hpp>
#include "sago/dbsync.hpp"
#include <fstream>
#include "sago/DbSyncDbOracle.hpp"
#include "sago/DbSyncDbMySql.hpp"
#include "sago/DbSyncDbPostgres.hpp"
#include "sago/DbSyncDbSqlite.hpp"
#include "sago/DbSyncValidator.hpp"

using std::cout;
using std::vector;

struct CommandArguments {
	bool validate = true;
	int validateLength = 100;
};

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif

const char* const mysqlConnectString = "mysql:database=dbsync_test;user=testuser;password=password;opt_reconnect=1";
const char* const SAGO_CONNECTION_STRING = "SAGO_CONNECTION_STRING";

int main(int argc, const char* argv[]) {
	CommandArguments commandArguments;
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("version", "Print version information and quit")
		("help,h", "Print basic usage information to stdout and quits")
		("config,c", boost::program_options::value<std::vector<std::string> >(), "Read a config file with the values. Can be given multiple times")
		("connectstring", boost::program_options::value<std::string>(), "The connect string to use. If not set the env \"SAGO_CONNECTION_STRING\" will be used.")
		("schema,s", boost::program_options::value<std::string>(), "The schema to import to or export from")
		("output-file,o", boost::program_options::value<std::string>(), "The output file. If blank stdout is used. If not set no output are generated")
		("input-file,i", boost::program_options::value<std::string>(), "The input file. If blank stdin is used. If not set no input is read")
		("validate", boost::program_options::value<int>(), "If 0 do not validate. If 1 perform validation.")
		("validate-name-length", boost::program_options::value<int>(), "Validate that length of names are less than this number. 0 to disable")
		;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("version")) {
		std::cout << "dbsync " << VERSIONNUMBER << "\n";
		return 0;
	}
	if (vm.count("help")) {
		cout << desc << "\n";
		cout << "An example connectstring for mysql: " << mysqlConnectString << "\n";
		if (getenv(SAGO_CONNECTION_STRING)) {
			std::cout << "The environment " << SAGO_CONNECTION_STRING << " is currently set\n";
		}
		return 0;
	}
	if (vm.count("config")) {
		std::vector<std::string> config_filenames = vm["config"].as<std::vector<std::string> >();
		for ( const std::string& s : config_filenames) {
			std::ifstream config_file(s);
			store(parse_config_file(config_file, desc), vm);
			notify(vm);
		}
	}
	if (vm.count("validate")) {
		commandArguments.validate = vm["validate"].as<int>();
	}
	if (vm.count("validate-name-length")) {
		commandArguments.validateLength = vm["validate-name-length"].as<int>();
	}
	std::string connectstring = mysqlConnectString;
	const char* connectstring_env = getenv(SAGO_CONNECTION_STRING);
	if (connectstring_env) {
		connectstring = connectstring_env;
	}
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
			if (!inputFileStream.is_open()) {
				std::cerr << "failed to open: "<< inputFileName << "\n";
				exit(1);
			}
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
	std::string schema_name = "";
	if (vm.count("schema")) {
		schema_name = vm["schema"].as<std::string>();
	}
	//std::shared_ptr<cppdb::session> db(new cppdb::session(connectstring));
	std::shared_ptr<cppdb::session> db = std::make_shared<cppdb::session>(connectstring);
	std::shared_ptr<sago::database::DbSyncDb> dbi;
	std::string database_driver = db->driver();
	if (database_driver == "mysql") {
		dbi = std::shared_ptr<sago::database::DbSyncDb>(new sago::database::DbSyncDbMySql(db, schema_name));
	}
	else if (database_driver == "sqlite3") {
		dbi = std::shared_ptr<sago::database::DbSyncDb>(new sago::database::DbSyncDbSqlite(db));
	}
	else if (database_driver == "postgresql") {
		dbi = std::shared_ptr<sago::database::DbSyncDb>(new sago::database::DbSyncDbPostgres(db, schema_name));
	}
	else {
		throw std::runtime_error((database_driver+" not implemented").c_str());
	}
	sago::database::DbDatabaseRoot dbm;
	sago::database::DbSyncValidator validator;
	if (commandArguments.validateLength) {
		validator.nameMaxLength = commandArguments.validateLength;
		validator.checkNamesLength = true;
	}
	if (readInput) {
		{
			std::string s(std::istreambuf_iterator<char>(*input), {});
			JS::ParseContext context(s);
			try {
				context.parseTo(dbm);
			} catch (std::exception& e) {
				std::cerr << e.what() << "\n";
				throw std::runtime_error("failed to find a root element named \"databasemodel\" in json");
			}
		}
		sago::database::ApplyDataModel(dbm.databasemodel, *dbi);
	}
	if (writeOutput) {
		dbm.databasemodel = sago::database::ExtractDataModel(*dbi);
		{
			std::string pretty_json = JS::serializeStruct(dbm);
			*output << pretty_json;
		}
	}
	if (commandArguments.validate) {
		validator.ValidateModel(dbm.databasemodel);
	}
	return 0;
}
