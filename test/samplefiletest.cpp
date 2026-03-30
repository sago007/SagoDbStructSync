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

#define BOOST_TEST_MAIN
#if !defined( WIN32 )
#define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/unit_test.hpp>
#include "../src/sago/dbsync.hpp"
#include "../src/sago/DbSyncValidator.hpp"
#include <fstream>
#include <cstdlib>

static std::string getFixturesDir() {
	const char* env = std::getenv("FIXTURES_DIR");
	if (env) {
		return std::string(env);
	}
	return std::string(FIXTURES_DIR);
}

static sago::database::DbDatabaseRoot parseFixture(const std::string& filename) {
	std::string path = getFixturesDir() + "/" + filename;
	std::ifstream f(path);
	BOOST_REQUIRE_MESSAGE(f.is_open(), "Could not open fixture: " + path);
	std::string content(std::istreambuf_iterator<char>(f), {});
	sago::database::DbDatabaseRoot root;
	JS::ParseContext context(content);
	context.parseTo(root);
	return root;
}

BOOST_AUTO_TEST_CASE(Parse_base_model) {
	auto root = parseFixture("01_base_model.json");
	const auto& model = root.databasemodel;
	BOOST_CHECK_EQUAL(model.tables.size(), 1u);
	BOOST_CHECK_EQUAL(model.tables[0].tablename, "test_table");
	BOOST_CHECK_EQUAL(model.tables[0].columns.size(), 3u);

	BOOST_CHECK_EQUAL(model.tables[0].columns[0].name, "item_id");
	BOOST_CHECK(model.tables[0].columns[0].type == SagoDbType::NUMBER);
	BOOST_CHECK_EQUAL(model.tables[0].columns[0].autoIncrement, true);

	BOOST_CHECK_EQUAL(model.tables[0].columns[1].name, "title");
	BOOST_CHECK(model.tables[0].columns[1].type == SagoDbType::TEXT);
	BOOST_CHECK_EQUAL(model.tables[0].columns[1].length, 50);

	BOOST_CHECK_EQUAL(model.tables[0].columns[2].name, "status");
	BOOST_CHECK(model.tables[0].columns[2].type == SagoDbType::NUMBER);

	BOOST_CHECK_EQUAL(model.unique_constraints.size(), 1u);
	BOOST_CHECK_EQUAL(model.unique_constraints[0].name, "PRIMARY");
}

BOOST_AUTO_TEST_CASE(Parse_added_fields) {
	auto root = parseFixture("02_added_fields.json");
	const auto& model = root.databasemodel;
	BOOST_CHECK_EQUAL(model.tables[0].columns.size(), 4u);
	BOOST_CHECK_EQUAL(model.tables[0].columns[3].name, "description");
	BOOST_CHECK(model.tables[0].columns[3].type == SagoDbType::TEXT);
	BOOST_CHECK_EQUAL(model.tables[0].columns[3].length, 200);
}

BOOST_AUTO_TEST_CASE(Parse_wider_varchar) {
	auto root = parseFixture("03_wider_varchar.json");
	const auto& model = root.databasemodel;
	BOOST_CHECK_EQUAL(model.tables[0].columns[1].name, "title");
	BOOST_CHECK_EQUAL(model.tables[0].columns[1].length, 100);
}

BOOST_AUTO_TEST_CASE(Parse_fewer_fields) {
	auto root = parseFixture("04_fewer_fields.json");
	const auto& model = root.databasemodel;
	BOOST_CHECK_EQUAL(model.tables[0].columns.size(), 2u);
	BOOST_CHECK_EQUAL(model.tables[0].columns[0].name, "item_id");
	BOOST_CHECK_EQUAL(model.tables[0].columns[1].name, "title");
}

BOOST_AUTO_TEST_CASE(Parse_narrower_varchar) {
	auto root = parseFixture("05_narrower_varchar.json");
	const auto& model = root.databasemodel;
	BOOST_CHECK_EQUAL(model.tables[0].columns[1].name, "title");
	BOOST_CHECK_EQUAL(model.tables[0].columns[1].length, 30);
}

BOOST_AUTO_TEST_CASE(Roundtrip_serialize) {
	auto original = parseFixture("01_base_model.json");
	std::string json = JS::serializeStruct(original);
	sago::database::DbDatabaseRoot reparsed;
	JS::ParseContext context(json);
	context.parseTo(reparsed);
	BOOST_CHECK_EQUAL(reparsed.databasemodel.tables.size(), original.databasemodel.tables.size());
	BOOST_CHECK_EQUAL(reparsed.databasemodel.tables[0].tablename, original.databasemodel.tables[0].tablename);
	BOOST_CHECK_EQUAL(reparsed.databasemodel.tables[0].columns.size(), original.databasemodel.tables[0].columns.size());
	for (size_t i = 0; i < original.databasemodel.tables[0].columns.size(); ++i) {
		BOOST_CHECK_EQUAL(reparsed.databasemodel.tables[0].columns[i].name, original.databasemodel.tables[0].columns[i].name);
		BOOST_CHECK(reparsed.databasemodel.tables[0].columns[i].type == original.databasemodel.tables[0].columns[i].type);
		BOOST_CHECK_EQUAL(reparsed.databasemodel.tables[0].columns[i].length, original.databasemodel.tables[0].columns[i].length);
	}
}

BOOST_AUTO_TEST_CASE(Validate_base_model) {
	auto root = parseFixture("01_base_model.json");
	sago::database::DbSyncValidator validator;
	BOOST_CHECK_NO_THROW(validator.ValidateModel(root.databasemodel));
}
