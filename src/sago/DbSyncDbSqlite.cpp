/*
  Its is under the MIT license, to encourage reuse by cut-and-paste.

  Copyright (c) 2022 Poul Sander

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

#include "DbSyncDbSqlite.hpp"

namespace sago {
	namespace database {

		DbSyncDbSqlite::DbSyncDbSqlite(std::shared_ptr<cppdb::session>& sql) : sql(sql) {
		}

		DbSyncDbSqlite::DbSyncDbSqlite(const DbSyncDbSqlite& orig) {
		}

		DbSyncDbSqlite::~DbSyncDbSqlite() {
		}

		bool DbSyncDbSqlite::TableExists(const std::string& tablename) {
			cppdb::result res = *sql << "SELECT name FROM sqlite_master WHERE type='table' AND name=?;" << tablename;
			if (res.next()) {
				return true;
			}
			return false;
		}

		bool DbSyncDbSqlite::UniqueConstraintExists(const std::string& tablename, const std::string& name) {
			try {
				cppdb::result res = *sql << "SELECT name FROM sqlite_master WHERE type= 'index' and tbl_name = ? and name = ?;" << tablename << name;
				while (res.next()) {
					std::string indexname;
					res >> indexname;
					if (indexname == name) {
						return true;
					}
				}
				return false;
			} catch (std::exception& e) {
				throw DbException(e.what(), "DbSyncDbSqlite::UniqueConstraintExists failed", name, tablename);
			}
		}

		bool DbSyncDbSqlite::ForeignKeyExists(const std::string& tablename, const std::string& name) {
			cppdb::result res = *sql << "PRAGMA foreign_key_list(" + tablename + ");";
			while (res.next()) {
				std::string indexname;
				res >> indexname;
				if (indexname == name) {
					return true;
				}
			}
			return false;
		}

		bool DbSyncDbSqlite::SchemaExists(const std::string& schemaname) {
			return false;
		}

		bool DbSyncDbSqlite::ColumnExists(const std::string& tablename, const std::string& columnname) {
			try {
				cppdb::result res = *sql << "PRAGMA table_info(" + tablename + ")";
				while (res.next()) {
					std::string name;
					name = res.get<std::string>("name");
					if (name == columnname) {
						return true;
					}
				}
				return false;
			} catch (std::exception& e) {
				throw DbException(e.what(), "DbSyncDbSqlite::ColumnExists failed", columnname, tablename);
			}
		}

		void DbSyncDbSqlite::CreateTable(const sago::database::DbTable& t, const std::vector<DbForeignKeyConstraint>& foreign_keys) {
			if (TableExists(t.tablename)) {
				for (const sago::database::DbColumn& c : t.columns) {
					if (!ColumnExists(t.tablename, c.name)) {
						CreateColumn(t.tablename, c);
					}
				}
				return;
			}
			std::string sqlStr = "CREATE TABLE " + t.tablename + " (";
			bool first = true;
			for (const auto& col : t.columns) {
				if (!first) {
					sqlStr += ", ";
				}
				first = false;
				sqlStr += col.name + " ";
				switch (col.type) {
				case SagoDbType::TEXT:
					sqlStr += "TEXT";
					break;
				case SagoDbType::NUMBER:
					sqlStr += "DOUBLE";
					break;
				case SagoDbType::DATE:
					sqlStr += "DATE";
					break;
				case SagoDbType::BLOB:
					sqlStr += "BLOB";
					break;
				case SagoDbType::CLOB:
					sqlStr += "CLOB";
					break;
				case SagoDbType::FLOAT:
					sqlStr += "FLOAT";
					break;
				case SagoDbType::DOUBLE:
					sqlStr += "DOUBLE";
					break;
				case SagoDbType::TIMESTAMP:
					sqlStr += "TIMESTAMP";
					break;
				case SagoDbType::NONE:
					throw DbException("DbSyncDbSqlite::CreateTable", "Column type is NONE", col.name, t.tablename);
				}
			}
			for (const auto& fk : foreign_keys) {
				if (fk.tablename != t.tablename) {
					continue;
				}
				sqlStr += ", FOREIGN KEY(";
				first = true;
				for (const auto& col : fk.columnnames) {
					if (!first) {
						sqlStr += ", ";
					}
					first = false;
					sqlStr += col;
				}
				sqlStr += ") REFERENCES " + fk.foreigntablename + "(";
				first = true;
				for (const auto& col : fk.foreigntablecolumnnames) {
					if (!first) {
						sqlStr += ", ";
					}
					first = false;
					sqlStr += col;
				}
				sqlStr += ")";
			}
			sqlStr += ");";
			std::cout << sqlStr << std::endl;
			cppdb::statement st = *sql << sqlStr;
			try {
				st.exec();
			} catch (std::exception& e) {
				std::cerr << "Failed: " << sqlStr << "\n";
				throw;
			}
		}

		void DbSyncDbSqlite::CreateColumn(const std::string& tablename, const sago::database::DbColumn& c) {
			if (ColumnExists(tablename, c.name)) {
				return;
			}
			std::string sqlStr = "ALTER TABLE " + tablename + " ADD COLUMN " + c.name + " ";
			switch (c.type) {
			case SagoDbType::TEXT:
				sqlStr += "TEXT";
				break;
			case SagoDbType::NUMBER:
				sqlStr += "DOUBLE";
				break;
			case SagoDbType::DATE:
				sqlStr += "DATE";
				break;
			case SagoDbType::BLOB:
				sqlStr += "BLOB";
				break;
			case SagoDbType::CLOB:
				sqlStr += "CLOB";
				break;
			case SagoDbType::FLOAT:
				sqlStr += "FLOAT";
				break;
			case SagoDbType::DOUBLE:
				sqlStr += "DOUBLE";
				break;
			case SagoDbType::TIMESTAMP:
				sqlStr += "TIMESTAMP";
				break;
			case SagoDbType::NONE:
				throw DbException("DbSyncDbSqlite::CreateColumn", "Column type is NONE", c.name, tablename);
			}
			sqlStr += ";";
			std::cout << sqlStr << std::endl;
			cppdb::statement st = *sql << sqlStr;
			try {
				st.exec();
			} catch (std::exception& e) {
				std::cerr << "Failed: " << sqlStr << "\n";
				throw;
			}
		}

		static bool str_starts_with(const std::string& str, const std::string& prefix) {
			if (str.length() < prefix.length()) {
				return false;
			}
			return str.substr(0, prefix.length()) == prefix;
		}

		void DbSyncDbSqlite::CreateUniqueConstraint(const sago::database::DbUniqueConstraint& c) {
			std::string indexName = c.name;
			if ( !str_starts_with(indexName, c.tablename + "_") ){
				indexName = c.tablename + "_" + indexName;
			}
			if (UniqueConstraintExists(c.tablename, indexName)) {
				return;
			}
			std::string sqlStr = "CREATE UNIQUE INDEX " + indexName + " ON " + c.tablename + " (";
			bool first = true;
			for (const auto& col : c.columns) {
				if (!first) {
					sqlStr += ", ";
				}
				first = false;
				sqlStr += col;
			}
			sqlStr += ");";
			std::cout << sqlStr << std::endl;
			cppdb::statement st = *sql << sqlStr;
			try {
				st.exec();
			} catch (std::exception& e) {
				std::cerr << "Failed: " << sqlStr << "\n";
				throw;
			}
		}

		void DbSyncDbSqlite::CreateForeignKeyConstraint(const sago::database::DbForeignKeyConstraint& c) {
			std::cerr << "Warning: FK " << c.name << " on table " << c.tablename << " cannot be automatically validated. SQLite does not support named FKs.\n";
		}

	} //namespace database
} //namespace sago
