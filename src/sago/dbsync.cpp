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

#include "dbsync.hpp"
#include "dbsync_db.hpp"


namespace sago {
	namespace database {

		const std::string& getTypeAsString(DbType type) {
			static std::string sTEXT = "TEXT";
			static std::string sNUM = "NUMBER";
			static std::string sDATE = "DATE";
			static std::string sNONE = "NONE";
			switch (type) {
				case DbType::NUMBER:
					return sNUM;
				case DbType::TEXT:
					return sTEXT;
				case DbType::DATE:
					return sDATE;
				case DbType::NONE:
				default:
					return sNONE;
			}
			return sNONE;
		}

		void SyncTableDataModel(cppdb::session &sql, const DbTable &table) {
			std::string stsql = "CREATE TABLE IF NOT EXISTS " + table.tablename + "(";
			for (size_t i = 0; i < table.columns.size(); i++) {

			}
			stsql += ")";
			cppdb::statement st = sql << stsql;
			st.exec();
		}


	} //database
} // sago