#include "DbSyncValidator.hpp"

namespace sago {
namespace database {
	
	void DbSyncValidator::ValidateModel(const DbDatabaseModel& model) {
		for (const DbTable& t : model.tables ) {
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
			ValidateColumn(c);
		}
	}
	
	void DbSyncValidator::ValidateColumn(const DbColumn& column) {
		
	}
	
	void DbSyncValidator::ValidateForeignKey(const DbForeignKeyConstraint& constraint) {
		
	}
	
	void DbSyncValidator::ValidateUniqueConstraint(const DbUniqueConstraint& constraint) {
		
	}
	
	void DbSyncValidator::ValidateName(const std::string& name, const std::string& path) {
	}
	
}  //database
}  //sago