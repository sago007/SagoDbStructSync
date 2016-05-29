#include "DbSyncDbOracle.hpp"

DbSyncDbOracle::DbSyncDbOracle(std::shared_ptr<cppdb::session>& sql) : sql(sql) {
}

DbSyncDbOracle::DbSyncDbOracle(const DbSyncDbOracle& orig) {
}

DbSyncDbOracle::~DbSyncDbOracle() {
}

bool DbSyncDbOracle::TableExists(const std::string& tablename) {
	cppdb::result res = *sql << "SELECT 1 FROM all_tables WHERE OWNER = sys_context('userenv','current_schema') AND TABLE_NAME = ?" << tablename;
	if (res.next()) {
		return true;
	}
	return false;
}
