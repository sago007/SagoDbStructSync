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

#include "DbSyncValidator.hpp"

namespace sago {
	namespace database {

		void DbSyncValidator::ValidateModel(const DbDatabaseModel& model) {
			for (const DbTable& t : model.tables) {
				ValidateTable(t);
			}
			for (const DbForeignKeyConstraint& f : model.foreign_keys) {
				ValidateForeignKey(f);
			}
			for (const DbUniqueConstraint& u : model.unique_constraints) {
				ValidateUniqueConstraint(u);
			}
		}

		void DbSyncValidator::ValidateTable(const DbTable& table) {
			for (const DbColumn& c : table.columns) {
				ValidateColumn(c, table);
			}
		}

		void DbSyncValidator::ValidateColumn(const DbColumn& column, const DbTable& table) {
			ValidateName(column.name, table.tablename);
		}

		void DbSyncValidator::ValidateForeignKey(const DbForeignKeyConstraint& constraint) {
			ValidateName(constraint.name, constraint.tablename);
		}

		void DbSyncValidator::ValidateUniqueConstraint(const DbUniqueConstraint& constraint) {
			ValidateName(constraint.name, constraint.tablename);
		}

		void DbSyncValidator::ValidateName(const std::string& name, const std::string& path) {
			if (name.length() < 1) {
				std::string header = "Name validation error";
				std::string errmsg = std::string("Empty name detected");
				throw DbValidationException(header, errmsg, path, schemaNameBeingValidated);
			}
			if (checkNamesLength && name.length() > static_cast<size_t> (nameMaxLength)) {
				std::string header = "Name validation error";
				std::string errmsg = std::string("Length of name \"") + name + "\" is too long. Length: " + std::to_string(name.length()) + ", Max length: " + std::to_string(nameMaxLength);
				throw DbValidationException(header, errmsg, path, schemaNameBeingValidated);
			}
			if (name[0] >= '0' && name[0] <= '9') {
				std::string header = "Name validation error";
				std::string errmsg = std::string("A name may not start with a number. But \""+name+"\" start with a '"+name[0]+"'");
				throw DbValidationException(header, errmsg, path, schemaNameBeingValidated);
			}
			if (checkNamesChars) {
				for (char c : name) {
					if (c >= 'A' && c <= 'Z' ) {
						continue;
					}
					if (c >= 'a' && c <= 'z' ) {
						continue;
					}
					if (c >= '0' && c <= '9' ) {
						continue;
					}
					if (c == '_') {
						continue;
					}
					{
						std::string header = "Name validation error";
						std::string errmsg = std::string("A name may not start with a number. But \""+name+"\" start with a '"+name[0]+"'");
						throw DbValidationException(header, errmsg, path, schemaNameBeingValidated);
					}
				}
			}
		}

	} //database
} //sago