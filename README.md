# SagoDbStructSync
Program designed to synchronize database model/structure between databases

## Rationale
Traditional relational databases are a perfect fit in many cases. In cases were high integrity is needed databases are often the best choice.
Unfortunately many traditional databases were designed in a time were the concept of fast development iterations and distributed development was introduced.

Updating an SQL DB is quite hard to automate the update process. Most tools involve writing manual commands that needs to be executed in a specific order and only once.

Modern NoSQL databases have no trouble with modern development practices and are therefore very popular. The problem with most widespread NoSQL databases is that they are optimized for scalability instead of integrity.

SagoDbStructSync is my attempt to create a small program/library that for trivial cases can automate the update task for SQL databases, so that the database choice is based on having the best tool for the job and not based on the database fitting the development method.

```
{
    "databasemodel": {
        "tables": [
            {
                "tablename": "my_movies",
                "columns": [
                    { "name": "movie_id", "type": "NUMBER", "length": 20, "scale": 0, "nullable": false },
                    { "name": "title", "type": "TEXT", "length": 50, "scale": 0, "nullable": false }
                ]
            }
        ],
        "unique_constraints": [
            { "name": "PRIMARY", "tablename": "my_movies", "columns": [ "movie_id" ] }
        ],
        "foreign_keys": []
    }
}
```

## Running the integration tests

The project includes a Docker-based integration test suite that tests against real MySQL and PostgreSQL databases. You need Docker and Docker Compose installed.

```bash
docker compose -f extra/docker/docker-compose.yml up --build --abort-on-container-exit --exit-code-from test-runner
```

This starts MySQL 8.0 and PostgreSQL 16 containers, builds the project in a test-runner container, and runs the full test suite covering:

- Creating tables from a JSON model
- Appending new fields to existing tables
- Widening VARCHAR columns
- Applying a model with fewer fields (existing columns are preserved)
- Ensuring VARCHAR columns are never made narrower

To clean up the containers afterwards:

```bash
docker compose -f extra/docker/docker-compose.yml down
```

## License
The source is under the MIT license. Be aware that the final product is under the GPLv2 if compiled against the MySQL library.
