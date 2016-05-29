#include "dbsync_db.hpp"

namespace sago {
namespace database {
	

	DbDatabaseModel ExtractDataModel(DbSyncDb& db) {
		DbDatabaseModel ret;
		std::vector<std::string> tablenames = db.GetTableNames();
		ret = ExtractDataModelForTables(db, tablenames);
		return ret;
	}

	DbDatabaseModel ExtractDataModelForTables(DbSyncDb& db, const std::vector<std::string>& table_names) {
		DbDatabaseModel ret;
		for (const std::string& s : table_names) {
			ret.tables.push_back(db.GetTable(s));
			std::vector<std::string> unique_constraint_names = db.GetUniqueConstraintNamesForTable(s);
			std::vector<std::string> unique_foreign_keys_names = db.GetForeignKeyNamesForTable(s);
			for (const std::string& un : unique_constraint_names) {
				ret.unique_constraints.push_back(db.GetUniqueConstraint(s, un));
			}
			for (const std::string& fn : unique_foreign_keys_names) {
				ret.foreign_keys.push_back(db.GetForeignKeyConstraint(s, fn));
			}
		}
		return ret;
	}
	
}
}