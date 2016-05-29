/* 
 * File:   dbsync.hpp
 * Author: poul
 *
 * Created on 8. juli 2014, 14:51
 */

#ifndef DBSYNC_HPP
#define	DBSYNC_HPP

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
			: header(header), errmsg(errmsg), table_name(table_name), schema_name(schema_name)
		{
		}
		
		const char* what() const noexcept override {
			return errmsg.c_str();
		}
	};
	
		enum class DbType {TEXT,NUMBER,DATE,BLOB,CLOB,FLOAT,DOUBLE,NONE};
		struct DbColumn {
			std::string name;
			DbType type = DbType::NONE;
			int length = 0;  //< Text length or number precision
			int scale = 0;
			bool nullable = false;
			bool hasDefaultValue = false;
			std::string defaultValue = "";
			
			template <class Archive>
			void serialize( Archive & ar )
			{
				ar( CEREAL_NVP(name), CEREAL_NVP(type), CEREAL_NVP(length), CEREAL_NVP(scale), CEREAL_NVP(nullable), CEREAL_NVP(hasDefaultValue), CEREAL_NVP(defaultValue) );
			}
			
		};
		
		struct DbTable {
			std::string tablename;
			std::vector<DbColumn> columns;
			
			template <class Archive>
			void serialize( Archive & ar )
			{
				ar( CEREAL_NVP(tablename), CEREAL_NVP(columns) );
			}
		};
		
		struct DbUniqueConstraint {
			std::string name;
			std::string tablename;
			std::vector<std::string> columns;
			
			template <class Archive>
			void serialize( Archive & ar )
			{
				ar( CEREAL_NVP(name), CEREAL_NVP(tablename), CEREAL_NVP(columns) );
			}
		};
		
		struct DbForeignKeyConstraint {
			std::string name;
			std::string tablename;
			std::vector<std::string> columnnames;
			std::string foreigntablename;
			std::vector<std::string> foreigntablecolumnnames;
			
			template <class Archive>
			void serialize( Archive & ar )
			{
				ar( CEREAL_NVP(name), CEREAL_NVP(tablename), CEREAL_NVP(columnnames), CEREAL_NVP(foreigntablename), CEREAL_NVP(foreigntablecolumnnames) );
			}
		};
		
		struct DbDatabaseModel {
			std::vector<DbTable> tables;
			std::vector<DbUniqueConstraint> unique_constraints;
			std::vector<DbForeignKeyConstraint> foreign_keys;
			
			template <class Archive>
			void serialize( Archive & ar )
			{
				ar( CEREAL_NVP(tables), CEREAL_NVP(unique_constraints), CEREAL_NVP(foreign_keys) );
			}
		};
		
		
		void SyncTableDataModel(cppdb::session &sql,const DbTable &table);
		
		

	} //database
}  // sago

#endif	/* DBSYNC_HPP */

