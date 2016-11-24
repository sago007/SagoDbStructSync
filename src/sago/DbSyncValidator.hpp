#pragma once

#include "dbsync.hpp"


namespace sago {
namespace database {


class DbValidationException : public DbException {
	using DbException::DbException;
};

class DbSyncValidator {
public:
	std::vector<DbValidationException> validationErrors;
	
	void ValidateModel(const DbDatabaseModel& model);
	void ValidateTable(const DbTable& table);
	void ValidateColumn(const DbColumn& column);
	void ValidateForeignKey(const DbForeignKeyConstraint& constraint);
	void ValidateUniqueConstraint(const DbUniqueConstraint& constraint);
private:
	bool checkNamesCase = true;
	bool checkNamesChars = true;
	bool checkNamesLength =  true;
	int nameMaxLength = 30;
	void ValidateName(const std::string& name, const std::string& path);
};

}  //namespace database
}  //namespace sago 