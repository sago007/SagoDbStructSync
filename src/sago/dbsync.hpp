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

#ifndef DBSYNC_HPP
#define DBSYNC_HPP

#include <cppdb/frontend.h>
#include <vector>
#include "json_struct.h"
#include <iostream>

JS_ENUM(SagoDbType, TEXT, NUMBER, DATE, BLOB, CLOB, FLOAT, DOUBLE, TIMESTAMP, NONE);
JS_ENUM_DECLARE_STRING_PARSER(SagoDbType);

namespace sago {
	namespace database {

		class DbException : public std::exception {
		public:
			std::string header;
			std::string errmsg;
			std::string table_name;
			std::string schema_name;

			DbException(const std::string& header, const std::string& errmsg, const std::string& table_name, const std::string& schema_name)
			: header(header), errmsg(errmsg), table_name(table_name), schema_name(schema_name) {
			}

			const char* what() const noexcept override {
				return errmsg.c_str();
			}
		};


		struct DbColumn {
			std::string name;
			SagoDbType type = SagoDbType::NONE;
			int length = 0; //< Text length or number precision
			int scale = 0;
			bool nullable = false;
			bool hasDefaultValue = false;
			bool autoIncrement = false;
			std::string defaultValue = "";

			JS_OBJ(name, type, length, scale,nullable, hasDefaultValue,defaultValue, autoIncrement);
		};

		struct DbTable {
			std::string tablename;
			std::vector<DbColumn> columns;

			JS_OBJ(tablename, columns);
		};

		struct DbUniqueConstraint {
			std::string name;
			std::string tablename;
			std::vector<std::string> columns;

			JS_OBJ(name, tablename, columns);
		};

		struct DbForeignKeyConstraint {
			std::string name;
			std::string tablename;
			std::vector<std::string> columnnames;
			std::string foreigntablename;
			std::vector<std::string> foreigntablecolumnnames;

			JS_OBJ(name, tablename, columnnames, foreigntablename, foreigntablecolumnnames);
		};

		struct DbDatabaseModel {
			std::vector<DbTable> tables;
			std::vector<DbUniqueConstraint> unique_constraints;
			std::vector<DbForeignKeyConstraint> foreign_keys;

			JS_OBJ(tables, unique_constraints, foreign_keys);
		};


		void SyncTableDataModel(cppdb::session &sql, const DbTable &table);



	} //database
} // sago

#endif /* DBSYNC_HPP */

