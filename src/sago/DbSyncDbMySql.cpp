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

#include "DbSyncDbMySql.hpp"

using std::vector;

static std::string getCurrentSchemaName(std::shared_ptr<cppdb::session>& sql) {
	std::string ret;
	cppdb::result res = *sql << "SELECT DATABASE()";
	if (res.next()) {
		res >> ret;
	}
	return ret;
}

namespace sago {
	namespace database {

		DbSyncDbMySql::DbSyncDbMySql(std::shared_ptr<cppdb::session>& sql, const std::string& schema_input) : sql(sql), schema(schema_input) {
			if (schema.empty()) {
				schema = getCurrentSchemaName(sql);
			}
			if (!SchemaExists(schema)) {
				std::string errMsg = "Failed to find schema: \""+schema+ "\"";
				throw std::runtime_error(errMsg.c_str());
			}
		}

		DbSyncDbMySql::DbSyncDbMySql(const DbSyncDbMySql& orig) {
		}

		DbSyncDbMySql::~DbSyncDbMySql() {
		}

		bool DbSyncDbMySql::TableExists(const std::string& tablename) {
			cppdb::result res = *sql << "SELECT table_name "
				"FROM information_schema.tables "
				"WHERE table_schema = ? "
				"AND table_name = ?" << schema << tablename;
			if (res.next()) {
				return true;
			}
			return false;
		}

		bool DbSyncDbMySql::ColumnExists(const std::string& tablename, const std::string& columnname) {
			cppdb::result res = *sql << "SELECT column_name FROM INFORMATION_SCHEMA.COLUMNS "
				"WHERE table_schema = ? AND TABLE_NAME=? AND column_name=?" << schema << tablename << columnname;
			if (res.next()) {
				return true;
			}
			return false;
		}

		bool DbSyncDbMySql::UniqueConstraintExists(const std::string& tablename, const std::string& name) {
			cppdb::result res = *sql << "SELECT CONSTRAINT_NAME "
				"FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS WHERE CONSTRAINT_TYPE IN ('PRIMARY KEY', 'UNIQUE') "
				"AND table_schema = ? AND TABLE_NAME=? AND CONSTRAINT_NAME = ?" << schema << tablename << name;
			if (res.next()) {
				return true;
			}
			return false;
		}

		bool DbSyncDbMySql::ForeignKeyExists(const std::string& tablename, const std::string& name) {
			cppdb::result res = *sql << "SELECT CONSTRAINT_NAME "
				"FROM INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS WHERE CONSTRAINT_SCHEMA = ? AND TABLE_NAME = ? AND CONSTRAINT_NAME = ?" << schema << tablename << name;
			if (res.next()) {
				return true;
			}
			return false;
		}

		bool DbSyncDbMySql::SchemaExists(const std::string& schemaname) {
			cppdb::result res = *sql << "SELECT SCHEMA_NAME FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = ?" << schema;
			if (res.next()) {
				return true;
			}
			return false;
		}

		std::vector<std::string> DbSyncDbMySql::GetTableNames() {
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

		std::vector<std::string> DbSyncDbMySql::GetColoumNamesFromTable(const std::string& tablename) {
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

		std::vector<std::string> DbSyncDbMySql::GetUniqueConstraintNamesForTable(const std::string& tablename) {
			std::vector<std::string> ret;
			cppdb::result res = *sql << "SELECT CONSTRAINT_NAME FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS "
				"WHERE table_schema = ? AND TABLE_NAME = ? AND CONSTRAINT_TYPE IN ('PRIMARY KEY', 'UNIQUE')" << schema << tablename;
			while (res.next()) {
				std::string value;
				res >> value;
				ret.push_back(value);
			}
			return ret;
		}

		std::vector<std::string> DbSyncDbMySql::GetForeignKeyNamesForTable(const std::string& tablename) {
			std::vector<std::string> ret;
			cppdb::result res = *sql << "SELECT CONSTRAINT_NAME "
				"FROM INFORMATION_SCHEMA.REFERENTIAL_CONSTRAINTS WHERE CONSTRAINT_SCHEMA = ? AND TABLE_NAME = ?" << schema << tablename;
			while (res.next()) {
				std::string value;
				res >> value;
				ret.push_back(value);
			}
			return ret;
		}

		sago::database::DbColumn DbSyncDbMySql::GetColumn(const std::string& tablename, const std::string& columnname) {
			sago::database::DbColumn ret;
			cppdb::result res = *sql << "SELECT COLUMN_NAME,DATA_TYPE,CHARACTER_MAXIMUM_LENGTH,NUMERIC_PRECISION,NUMERIC_SCALE,IS_NULLABLE,COLUMN_DEFAULT IS NOT NULL,COLUMN_DEFAULT,EXTRA "
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
				std::string extra;
				res >> name >> data_type >> max_length >> numeric_precision >> numeric_scale >> nullable >> hasDefault >> defaultValue >> extra;
				ret.name = name;
				bool type_recognized = false;
				ret.hasDefaultValue = hasDefault;
				ret.defaultValue = defaultValue;
				if (data_type == "int" || data_type == "bigint" || data_type == "decimal") {
					ret.length = numeric_precision;
					ret.scale = numeric_scale;
					ret.type = sago::database::DbType::NUMBER;
					type_recognized = true;
				}
				if (data_type == "varchar" || data_type == "char") {
					ret.length = max_length;
					ret.type = sago::database::DbType::TEXT;
					type_recognized = true;
				}
				if (data_type == "datetime") {
					ret.type = sago::database::DbType::DATE;
					type_recognized = true;
				}
				if (data_type == "float") {
					ret.type = sago::database::DbType::FLOAT;
					type_recognized = true;
				}
				if (data_type == "double") {
					ret.type = sago::database::DbType::DOUBLE;
					type_recognized = true;
				}
				if (data_type == "timestamp") {
					ret.type = sago::database::DbType::TIMESTAMP;
					type_recognized = true;
				}
				if (!type_recognized) {
					std::cerr << "Failure\n";
					throw sago::database::DbException("Unregognized type", std::string("This was not regonized: ") + data_type, tablename, schema);
				}
				if (extra == "auto_increment") {
					ret.autoIncrement = true;
				}
				if (nullable == "YES") {
					ret.nullable = true;
				} else {
					ret.nullable = false;
				}
			}
			return ret;
		}

		sago::database::DbTable DbSyncDbMySql::GetTable(const std::string& tablename) {
			std::cerr << "Reading " << tablename << "\n";
			sago::database::DbTable ret;
			ret.tablename = tablename;
			std::vector<std::string> column_names = GetColoumNamesFromTable(tablename);
			for (const std::string& s : column_names) {
				ret.columns.push_back(GetColumn(tablename, s));
			}
			return ret;
		}

		sago::database::DbUniqueConstraint DbSyncDbMySql::GetUniqueConstraint(const std::string& tablename, const std::string& name) {
			sago::database::DbUniqueConstraint ret;
			ret.tablename = tablename;
			ret.name = name;
			cppdb::result res = *sql << "select COLUMN_NAME from information_schema.STATISTICS "
				"WHERE INDEX_SCHEMA = ? AND TABLE_NAME = ? and index_name = ? ORDER BY SEQ_IN_INDEX" << schema << tablename << name;
			while (res.next()) {
				std::string value;
				res >> value;
				ret.columns.push_back(value);
			}
			return ret;
		}

		sago::database::DbForeignKeyConstraint DbSyncDbMySql::GetForeignKeyConstraint(const std::string& tablename, const std::string& name) {
			sago::database::DbForeignKeyConstraint ret;
			ret.tablename = tablename;
			ret.name = name;
			cppdb::result res = *sql << "SELECT COLUMN_NAME,REFERENCED_TABLE_NAME,REFERENCED_COLUMN_NAME "
				"FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE "
				"WHERE "
				"REFERENCED_TABLE_SCHEMA = ? "
				"AND TABLE_NAME = ? "
				"AND CONSTRAINT_NAME = ? "
				"ORDER BY POSITION_IN_UNIQUE_CONSTRAINT" << schema << tablename << name;
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

		void DbSyncDbMySql::CreateTable(const sago::database::DbTable& t) {
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

		void DbSyncDbMySql::CreateColumn(const std::string& tablename, const sago::database::DbColumn& c) {
			std::string alter_table_sql = "ALTER TABLE " + tablename + " ADD `" + c.name + "`";
			switch (c.type) {
				case sago::database::DbType::NUMBER:
					{
						char buffer[200];
						snprintf(buffer, sizeof (buffer), " NUMERIC(%i,%i) ", c.length, c.scale);
						if (c.scale == 0) {
							snprintf(buffer, sizeof (buffer), " INT(%i) ", c.length);
						}
						alter_table_sql += buffer;
					}
					break;
				case sago::database::DbType::TEXT:
					{
						char buffer[200];
						snprintf(buffer, sizeof (buffer), " VARCHAR(%i) ", c.length);
						alter_table_sql += buffer;
					}
					break;
				case sago::database::DbType::DATE:
					{
						char buffer[200];
						snprintf(buffer, sizeof (buffer), " DATETIME ");
						alter_table_sql += buffer;
					}
					break;
				case sago::database::DbType::FLOAT:
					{
						alter_table_sql += " FLOAT ";
					}
					break;
				case sago::database::DbType::DOUBLE:
					{
						alter_table_sql += " DOUBLE ";
					}
					break;
				case sago::database::DbType::TIMESTAMP:
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

		void DbSyncDbMySql::CreateUniqueConstraint(const sago::database::DbUniqueConstraint& c) {
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

		void DbSyncDbMySql::CreateForeignKeyConstraint(const sago::database::DbForeignKeyConstraint& c) {
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

	} //namespace database
} //namespace sago
