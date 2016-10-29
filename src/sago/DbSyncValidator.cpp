#include "DbSyncValidator.hpp"



namespace sago {
namespace database {
	
	void DbSyncValidator::ValidateModel(const DbDatabaseModel& model) {
		for (const DbTable& t : model.tables ) {
			ValidateTable(t);
		}
	}

	
	void DbSyncValidator::ValidateTable(const DbTable& table) {
		
	}
	
	void DbSyncValidator::ValidateColumn(const DbColumn& column) {
		
	}
	
	void DbSyncValidator::ValidateForeignKey(const DbForeignKeyConstraint& constraint) {
		
	}
	
	void DbSyncValidator::ValidateUniqueConstraint(const DbUniqueConstraint& constraint) {
		
	}
	
}  //database
}  //sago