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
#include "cereal/cereal.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

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

		enum class DbType {
			TEXT, NUMBER, DATE, BLOB, CLOB, FLOAT, DOUBLE, TIMESTAMP, NONE
		};

		/**
		 * Converts the enum DbType into the related string
		 */
		const std::string& getTypeAsString(DbType type);
		/**
		 * Converts a given string into a DbType
		 * will be "NONE" of no match
		 */
		const DbType getTypeFromString(std::string type);

		template <class Archive> std::string save_minimal(Archive const &, DbType const& obj)
		{
			return getTypeAsString(obj);
		}

		template <class Archive> void load_minimal(Archive const &, DbType& obj, std::string const & value)
		{
			obj = getTypeFromString(value);
		}


		struct DbColumn {
			std::string name;
			DbType type = DbType::NONE;
			int length = 0; //< Text length or number precision
			int scale = 0;
			bool nullable = false;
			bool hasDefaultValue = false;
			bool autoIncrement = false;
			std::string defaultValue = "";

			template <class Archive>
			void serialize(Archive & ar) {
				ar(CEREAL_NVP(name), CEREAL_NVP(type), CEREAL_NVP(length), CEREAL_NVP(scale), CEREAL_NVP(nullable), CEREAL_NVP(hasDefaultValue), 
				CEREAL_NVP(defaultValue), CEREAL_NVP(autoIncrement));
			}

		};

		struct DbTable {
			std::string tablename;
			std::vector<DbColumn> columns;

			template <class Archive>
			void serialize(Archive & ar) {
				ar(CEREAL_NVP(tablename), CEREAL_NVP(columns));
			}
		};

		struct DbUniqueConstraint {
			std::string name;
			std::string tablename;
			std::vector<std::string> columns;

			template <class Archive>
			void serialize(Archive & ar) {
				ar(CEREAL_NVP(name), CEREAL_NVP(tablename), CEREAL_NVP(columns));
			}
		};

		struct DbForeignKeyConstraint {
			std::string name;
			std::string tablename;
			std::vector<std::string> columnnames;
			std::string foreigntablename;
			std::vector<std::string> foreigntablecolumnnames;

			template <class Archive>
			void serialize(Archive & ar) {
				ar(CEREAL_NVP(name), CEREAL_NVP(tablename), CEREAL_NVP(columnnames), CEREAL_NVP(foreigntablename), CEREAL_NVP(foreigntablecolumnnames));
			}
		};

		struct DbDatabaseModel {
			std::vector<DbTable> tables;
			std::vector<DbUniqueConstraint> unique_constraints;
			std::vector<DbForeignKeyConstraint> foreign_keys;

			template <class Archive>
			void serialize(Archive & ar) {
				ar(CEREAL_NVP(tables), CEREAL_NVP(unique_constraints), CEREAL_NVP(foreign_keys));
			}
		};


		void SyncTableDataModel(cppdb::session &sql, const DbTable &table);



	} //database
} // sago

#endif /* DBSYNC_HPP */

