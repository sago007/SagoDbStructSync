/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dbsync_db.hpp
 * Author: poul
 *
 * Created on 12. maj 2016, 10:30
 */

#ifndef DBSYNC_DB_HPP
#define DBSYNC_DB_HPP

#include "dbsync.hpp"

namespace sago {
namespace database {

class DbSyncDb {
public:
	virtual bool TableExists(const std::string& tablename) { return false; }
	virtual bool ColumnExists(const std::string& tablename, const std::string& columnname) { return false; }
	virtual bool UniqueConstraintExists(const std::string& tablename, const std::string& name) { return false; }
	virtual bool ForeignKeyExists(const std::string& tablename, const std::string& name) { return false; }
	
	virtual std::vector<std::string> GetTableNames() { return std::vector<std::string>(); }
	virtual std::vector<std::string> GetColoumNamesFromTable(const std::string& tablename) { return std::vector<std::string>(); }
	virtual std::vector<std::string> GetUniqueConstraintNamesForTable(const std::string& tablename) { return std::vector<std::string>(); }
	virtual std::vector<std::string> GetForeignKeyNamesForTable(const std::string& tablename) { return std::vector<std::string>(); }
	
	virtual DbColumn GetColumn(const std::string& tablename, const std::string& columnname) { return DbColumn(); }
	virtual DbTable GetTable(const std::string& tablename) { return DbTable(); }
	virtual DbUniqueConstraint GetUniqueConstraint(const std::string& tablename, const std::string& name) {return DbUniqueConstraint(); }
	virtual DbForeignKeyConstraint GetForeignKeyConstraint(const std::string& tablename, const std::string& name) { return DbForeignKeyConstraint(); }
	
	virtual void CreateTable(const DbTable& t) {}
	virtual void CreateColumn(const std::string& tablename, const DbColumn& c) {}
	virtual void CreateUniqueConstraint(const DbUniqueConstraint& c) {}
	virtual void CreateForeignKeyConstraint(const DbForeignKeyConstraint& c) {}
	virtual ~DbSyncDb() {};
};


DbDatabaseModel ExtractDataModel(DbSyncDb& db);
DbDatabaseModel ExtractDataModelForTables(DbSyncDb& db, const std::vector<std::string>& table_names);

}
}

#endif /* DBSYNC_DB_HPP */

