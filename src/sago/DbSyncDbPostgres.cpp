/*
  Its is under the MIT license, to encourage reuse by cut-and-paste.

  Copyright (c) 2023 Poul Sander

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

#include "DbSyncDbPostgres.hpp"
#include <sstream>

static std::string getCurrentSchemaName(std::shared_ptr<cppdb::session>& sql) {
	std::string ret;
	cppdb::result res = *sql << "SELECT CURRENT_SCHEMA()";
	if (res.next()) {
		res >> ret;
	}
	return ret;
}

namespace sago {
	namespace database {

		DbSyncDbPostgres::DbSyncDbPostgres(std::shared_ptr<cppdb::session>& sql, const std::string& schema_input) : sql(sql), schema(schema_input) {
			if (schema.empty()) {
				schema = getCurrentSchemaName(sql);
			}
			if (!SchemaExists(schema)) {
				std::string errMsg = "Failed to find schema: \""+schema+ "\"";
				throw std::runtime_error(errMsg.c_str());
			}
		}

		DbSyncDbPostgres::DbSyncDbPostgres(const DbSyncDbPostgres& orig) {
		}

		DbSyncDbPostgres::~DbSyncDbPostgres() {
		}

		bool DbSyncDbPostgres::TableExists(const std::string& tablename) {
			cppdb::result res = *sql << "SELECT table_name "
				"FROM information_schema.tables "
				"WHERE table_schema = ? "
				"AND table_name = ?" << schema << tablename;
			if (res.next()) {
				return true;
			}
			return false;
		}

		bool DbSyncDbPostgres::ColumnExists(const std::string& tablename, const std::string& columnname) {
			cppdb::result res = *sql << "SELECT column_name FROM INFORMATION_SCHEMA.COLUMNS "
				"WHERE table_schema = ? AND TABLE_NAME=? AND column_name=?" << schema << tablename << columnname;
			if (res.next()) {
				return true;
			}
			return false;
		}

		bool DbSyncDbPostgres::UniqueConstraintExists(const std::string& tablename, const std::string& name) {
			cppdb::result res = *sql << "SELECT CONSTRAINT_NAME "
				"FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS WHERE CONSTRAINT_TYPE IN ('PRIMARY KEY', 'UNIQUE') "
				"AND constraint_schema = ? AND TABLE_NAME=? AND CONSTRAINT_NAME = ?" << schema << tablename << name;
			if (res.next()) {
				return true;
			}
			return false;
		}

		bool DbSyncDbPostgres::ForeignKeyExists(const std::string& tablename, const std::string& name) {
			cppdb::result res = *sql << "SELECT CONSTRAINT_NAME "
				"FROM information_schema.table_constraints tco WHERE constraint_schema = ? AND table_name = ? AND constraint_type = 'FOREIGN KEY' AND constraint_name = ?" << schema << tablename << name;
			if (res.next()) {
				return true;
			}
			return false;
		}

		bool DbSyncDbPostgres::SchemaExists(const std::string& schemaname) {
			cppdb::result res = *sql << "SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = ?" << schema;
			if (res.next()) {
				return true;
			}
			return false;
		}

		std::vector<std::string> DbSyncDbPostgres::GetTableNames() {
			std::vector<std::string> ret;
			cppdb::result res = *sql << "SELECT table_name "
				"FROM information_schema.tables "
				"WHERE table_schema = ?" << schema;
			while (res.next()) {
				std::string value;
				res >> value;
				ret.push_back(value);
			}
			std::cerr << "Found " << ret.size() << " tables!\n";
			if (ret.size() == 0) {
				std::cerr << "Nothing found. Looked in schema: \"" << schema << "\"\n";
			}
			return ret;
		}

		std::vector<std::string> DbSyncDbPostgres::GetColoumNamesFromTable(const std::string& tablename) {
			std::vector<std::string> ret;
			cppdb::result res = *sql << "SELECT column_name FROM INFORMATION_SCHEMA.COLUMNS "
				"WHERE table_schema = ? AND TABLE_NAME = ?" << schema << tablename;
			while (res.next()) {
				std::string value;
				res >> value;
				ret.push_back(value);
			}
			return ret;
		}

		std::vector<std::string> DbSyncDbPostgres::GetUniqueConstraintNamesForTable(const std::string& tablename) {
			std::vector<std::string> ret;
			cppdb::result res = *sql << "SELECT CONSTRAINT_NAME FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS "
				"WHERE constraint_schema = ? AND TABLE_NAME = ? AND CONSTRAINT_TYPE IN ('PRIMARY KEY', 'UNIQUE')" << schema << tablename;
			while (res.next()) {
				std::string value;
				res >> value;
				ret.push_back(value);
			}
			return ret;
		}

		std::vector<std::string> DbSyncDbPostgres::GetForeignKeyNamesForTable(const std::string& tablename) {
			std::vector<std::string> ret;
			cppdb::result res = *sql << "SELECT CONSTRAINT_NAME "
				"FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS WHERE constraint_type = 'FOREIGN KEY' AND CONSTRAINT_SCHEMA = ? AND TABLE_NAME = ?" << schema << tablename;
			while (res.next()) {
				std::string value;
				res >> value;
				ret.push_back(value);
			}
			return ret;
		}

		sago::database::DbColumn DbSyncDbPostgres::GetColumn(const std::string& tablename, const std::string& columnname) {
			sago::database::DbColumn ret;
			cppdb::result res = *sql << "SELECT COLUMN_NAME,DATA_TYPE,CHARACTER_MAXIMUM_LENGTH,COALESCE(NUMERIC_PRECISION,0),COALESCE(NUMERIC_SCALE,0),IS_NULLABLE,"
			" CASE"
			"  WHEN COLUMN_DEFAULT IS NOT NULL THEN 1"
			" ELSE 0"
			" END "
			"AS HAS_DEFAULT"
			",COLUMN_DEFAULT "
				"FROM INFORMATION_SCHEMA.COLUMNS "
				"WHERE table_schema = ? AND TABLE_NAME = ? AND COLUMN_NAME = ?" << schema << tablename << columnname;
			if (res.next()) {
				std::string name;
				std::string data_type;
				int max_length = 0;
				int numeric_precision = 0;
				int numeric_scale = 0;
				std::string nullable;
				int hasDefault = false;
				std::string defaultValue;
				res >> name;
				res >> data_type;
				res >> max_length;
				res >> numeric_precision;
				res >> numeric_scale;
				res >> nullable;
				res >> hasDefault;
				res >> defaultValue;
				ret.name = name;
				bool type_recognized = false;
				ret.hasDefaultValue = hasDefault;
				ret.defaultValue = defaultValue;
				if (data_type == "int" || data_type == "bigint" || data_type == "integer" || data_type == "decimal" || data_type == "numeric") {
					ret.length = numeric_precision;
					ret.scale = numeric_scale;
					ret.type = SagoDbType::NUMBER;
					type_recognized = true;
				}
				if (data_type == "character varying" || data_type == "character") {
					ret.length = max_length;
					ret.type = SagoDbType::TEXT;
					type_recognized = true;
				}
				if (data_type == "datetime") {
					ret.type = SagoDbType::DATE;
					type_recognized = true;
				}
				if (data_type == "float") {
					ret.type = SagoDbType::FLOAT;
					type_recognized = true;
				}
				if (data_type == "double") {
					ret.type = SagoDbType::DOUBLE;
					type_recognized = true;
				}
				if (data_type == "timestamp" || data_type == "timestamp without time zone") {
					ret.type = SagoDbType::TIMESTAMP;
					type_recognized = true;
				}
				if (!type_recognized) {
					std::cerr << "Failure\n";
					throw sago::database::DbException("Unregognized type", std::string("This was not regonized: ") + data_type, tablename, schema);
				}
				if (nullable == "YES") {
					ret.nullable = true;
				} else {
					ret.nullable = false;
				}
			}
			return ret;
		}

		sago::database::DbTable DbSyncDbPostgres::GetTable(const std::string& tablename) {
			std::cerr << "Reading " << tablename << "\n";
			sago::database::DbTable ret;
			ret.tablename = tablename;
			std::vector<std::string> column_names = GetColoumNamesFromTable(tablename);
			for (const std::string& s : column_names) {
				ret.columns.push_back(GetColumn(tablename, s));
			}
			return ret;
		}

		bool remove_substring(std::string& inout, const std::string& substring) {
			// Returns true if found one!
			std::string::size_type i = inout.find(substring);

			if (i != std::string::npos) {
				inout.erase(i, substring.length());
				return true;
			}
			return false;
		}

		void remove_all_substrings(std::string& inout, const std::string& substring) {
			while(remove_substring(inout, substring));
		}

		sago::database::DbUniqueConstraint DbSyncDbPostgres::GetUniqueConstraint(const std::string& tablename, const std::string& name) {
			sago::database::DbUniqueConstraint ret;
			ret.tablename = tablename;
			ret.name = name;
			cppdb::result res = *sql << "SELECT regexp_matches(indexdef, '\\((.*?)\\)') "
			"FROM pg_indexes where schemaname = ? and tablename = ? AND indexname = ?" << schema << tablename << name;
			while (res.next()) {
				std::string value;
				res >> value;
				remove_all_substrings(value,"{");
				remove_all_substrings(value,"}");
				remove_all_substrings(value,"\"");
				remove_all_substrings(value," ");
				std::stringstream ss (value);
				std::string item;

				while (std::getline (ss, item, ',')) {
					ret.columns.push_back(item);
				}

			}
			return ret;
		}

		sago::database::DbForeignKeyConstraint DbSyncDbPostgres::GetForeignKeyConstraint(const std::string& tablename, const std::string& name) {
			sago::database::DbForeignKeyConstraint ret;
			ret.tablename = tablename;
			ret.name = name;
			cppdb::result res = *sql << "SELECT"
			"  kcu.column_name,"
			"  ccu.table_name AS foreign_table_name,"
			"  ccu.column_name AS foreign_column_name"
			" FROM information_schema.table_constraints AS tc"
			" JOIN information_schema.key_column_usage AS kcu"
			"  ON tc.constraint_name = kcu.constraint_name"
			"   AND tc.table_schema = kcu.table_schema"
			" JOIN information_schema.constraint_column_usage AS ccu"
			"  ON ccu.constraint_name = tc.constraint_name"
			" WHERE tc.constraint_type = 'FOREIGN KEY'"
			"  AND tc.table_schema = ?"
			"  AND tc.table_name = ?"
			"  AND tc.constraint_name=?"
			" ORDER BY kcu.ordinal_position" << schema << tablename << name;
			while (res.next()) {
				std::string columnname;
				std::string reftablename;
				std::string refcolumnname;
				res >> columnname >> reftablename >> refcolumnname;
				ret.foreigntablename = reftablename;
				ret.columnnames.push_back(columnname);
				ret.foreigntablecolumnnames.push_back(refcolumnname);
			}
			return ret;
		}

		void DbSyncDbPostgres::CreateTable(const sago::database::DbTable& t, const std::vector<DbForeignKeyConstraint>& foreign_keys) {
			if (!TableExists(t.tablename)) {
				std::string create_table_sql = "CREATE TABLE " + t.tablename + " ( " + this->sago_id + " INT(20) NOT NULL AUTO_INCREMENT UNIQUE )";
				for (const sago::database::DbColumn& c : t.columns) {
					if (c.autoIncrement) {
						//MySQL is limited to one auto increment coloumn, so if we have one then use that.
						create_table_sql = "CREATE TABLE " + t.tablename + " ( " + c.name + " INT(20) NOT NULL AUTO_INCREMENT UNIQUE )";
					}
				}
				cppdb::statement st = *sql << create_table_sql;
				st.exec();
				std::cerr << "Created it \n";
			}
			for (const sago::database::DbColumn& c : t.columns) {
				if (!ColumnExists(t.tablename, c.name)) {
					CreateColumn(t.tablename, c);
				}
			}
			std::cout << "Creating table: " << t.tablename << "\n";
		}

		void DbSyncDbPostgres::CreateColumn(const std::string& tablename, const sago::database::DbColumn& c) {
			std::string alter_table_sql = "ALTER TABLE " + tablename + " ADD `" + c.name + "`";
			switch (c.type) {
				case SagoDbType::NUMBER:
					{
						char buffer[200];
						snprintf(buffer, sizeof (buffer), " NUMERIC(%i,%i) ", c.length, c.scale);
						if (c.scale == 0) {
							snprintf(buffer, sizeof (buffer), " INT(%i) ", c.length);
						}
						alter_table_sql += buffer;
					}
					break;
				case SagoDbType::TEXT:
					{
						char buffer[200];
						snprintf(buffer, sizeof (buffer), " VARCHAR(%i) ", c.length);
						alter_table_sql += buffer;
					}
					break;
				case SagoDbType::DATE:
					{
						char buffer[200];
						snprintf(buffer, sizeof (buffer), " DATETIME ");
						alter_table_sql += buffer;
					}
					break;
				case SagoDbType::FLOAT:
					{
						alter_table_sql += " FLOAT ";
					}
					break;
				case SagoDbType::DOUBLE:
					{
						alter_table_sql += " DOUBLE ";
					}
					break;
				case SagoDbType::TIMESTAMP:
					{
						alter_table_sql += " TIMESTAMP ";
					}
					break;
				default:
					{
						std::cerr << "Type " << static_cast<int> (c.type) << " not supported\n";
					}
					break;
			};
			if (!c.nullable) {
				alter_table_sql += " NOT NULL";
			}
			if (c.autoIncrement) {
				alter_table_sql += " AUTO_INCREMENT";
			}
			cppdb::statement st = *sql << alter_table_sql;
			try {
				st.exec();
			} catch (std::exception& e) {
				std::cerr << "Failed: " << alter_table_sql << "\n";
				throw;
			}
		}

		void DbSyncDbPostgres::CreateUniqueConstraint(const sago::database::DbUniqueConstraint& c) {
			if (c.columns.size() < 1) {
				std::cerr << "Warning: Contraint " << c.name << " on " << c.tablename << " has no columns\n";
				return;
			}
			std::string keyname_plus_type = c.name+" UNIQUE";
			if (c.name == "PRIMARY") {
				keyname_plus_type = "PRIMARY KEY";
			}
			std::string alter_table_sql = "ALTER TABLE " + c.tablename + " ADD CONSTRAINT " + keyname_plus_type + " ( " + c.columns.at(0);
			for (size_t i = 1; i < c.columns.size(); ++i) {
				alter_table_sql += ", " + c.columns.at(i);
			}
			alter_table_sql += ")";
			std::cout << alter_table_sql << "\n";
			cppdb::statement st = *sql << alter_table_sql;
			try {
				st.exec();
			} catch (std::exception& e) {
				std::cerr << "Failed: " << alter_table_sql << "\n";
			}
		}

		void DbSyncDbPostgres::CreateForeignKeyConstraint(const sago::database::DbForeignKeyConstraint& c) {
			if (c.columnnames.size() < 1) {
				std::cerr << "Warning: Foreign key " << c.name << " on " << c.tablename << " has no columns\n";
				return;
			}
			if (c.columnnames.size() != c.foreigntablecolumnnames.size()) {
				std::cerr << "Warning: Foreign key " << c.name << " on " << c.tablename << " does not have the same amount of columns on original and refered table " <<
					c.columnnames.size() << " vs " << c.foreigntablecolumnnames.size() << "\n";
				return;
			}
			std::string alter_table_sql = "ALTER TABLE " + c.tablename + " ADD CONSTRAINT " + c.name + " FOREIGN KEY ( " + c.columnnames.at(0);
			for (size_t i = 1; i < c.columnnames.size(); ++i) {
				alter_table_sql += ", " + c.columnnames.at(i);
			}
			alter_table_sql += " ) REFERENCES " + c.foreigntablename + " ( " + c.foreigntablecolumnnames.at(0);
			for (size_t i = 1; i < c.foreigntablecolumnnames.size(); ++i) {
				alter_table_sql += ", " + c.foreigntablecolumnnames.at(i);
			}
			alter_table_sql += ")";
			cppdb::statement st = *sql << alter_table_sql;
			try {
				st.exec();
			} catch (std::exception& e) {
				std::cerr << "Failed: " << alter_table_sql << "\n";
			}
		}
	}  //database
}  //sago