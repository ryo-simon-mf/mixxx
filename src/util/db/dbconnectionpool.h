#ifndef MIXXX_DBCONNECTIONPOOL_H
#define MIXXX_DBCONNECTIONPOOL_H


#include <QAtomicInt>
#include <QThreadStorage>

#include "util/db/dbconnection.h"
#include "util/memory.h"
#include "util/assert.h"


namespace mixxx {

class DbConnectionPool;
typedef std::shared_ptr<DbConnectionPool> DbConnectionPoolPtr;

class DbConnectionPool final {
  public:
    // Creates a new pool of database connections (one per thread) that
    // all use the same connection parameters. Unique connection names
    // will be  generated based on the given connection name that serves
    // as the base name (= common prefix).
    static DbConnectionPoolPtr create(
            const DbConnection::Params& params,
            const QString& connectionName) {
        return std::make_shared<DbConnectionPool>(params, connectionName);
    }

    // NOTE(uklotzde): Should be private, but must be public for invocation
    // from std::make_shared()!
    DbConnectionPool(
            const DbConnection::Params& params,
            const QString& connectionName); // reserver for

    // Returns a database connection for the current thread, that has
    // previously been created by instantiating DbConnectionPooled. The
    // returned connection is only valid within the current thread! It
    // will be closed and removed from the pool upon the destruction of
    // the owning DbConnectionPooled (see below) or as a very last resort
    // implicitly when the current thread terminates. Since all connections
    // need to be created through DbConnectionPooled the latter case should
    // never happen.
    QSqlDatabase threadLocalConnection() const;

  private:
    DbConnectionPool(const DbConnectionPool&) = delete;
    DbConnectionPool(const DbConnectionPool&&) = delete;

    friend class DbConnectionPooled;
    bool createThreadLocalConnection();
    void destroyThreadLocalConnection();

    const DbConnection m_prototypeConnection;

    QAtomicInt m_connectionCounter;

    QThreadStorage<DbConnection*> m_threadLocalConnections;

};

} // namespace mixxx


#endif // MIXXX_DBCONNECTIONPOOL_H
