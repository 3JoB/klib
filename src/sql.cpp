#include "klib/sql.h"

#include <sqlcipher/sqlite3.h>
#include <boost/core/ignore_unused.hpp>
#include <scope_guard.hpp>

#include "klib/archive.h"
#include "klib/exception.h"
#include "klib/log.h"

namespace klib {

#define check_sqlite(rc)                      \
  do {                                        \
    if (rc != SQLITE_OK) {                    \
      throw RuntimeError(sqlite3_errstr(rc)); \
    }                                         \
  } while (0)

#define check_sqlite2(rc, db)                 \
  do {                                        \
    if (rc != SQLITE_OK) {                    \
      throw RuntimeError(sqlite3_errmsg(db)); \
    }                                         \
  } while (0)

class Column::ColumnImpl {
 public:
  ColumnImpl(SqlQuery &sql_query, std::int32_t index);

  [[nodiscard]] bool is_null() const;

  [[nodiscard]] std::int32_t as_int32() const;
  [[nodiscard]] std::int64_t as_int64() const;
  [[nodiscard]] double as_double() const;
  [[nodiscard]] std::string as_text() const;
  [[nodiscard]] std::string as_blob() const;

 private:
  [[nodiscard]] std::int32_t get_type() const;

  [[nodiscard]] bool is_integer() const;
  [[nodiscard]] bool is_float() const;
  [[nodiscard]] bool is_text() const;
  [[nodiscard]] bool is_blob() const;

  sqlite3_stmt *stmt_ = nullptr;
  std::int32_t index_ = 0;
};

class SqlQuery::SqlQueryImpl {
  friend class Column::ColumnImpl;

 public:
  explicit SqlQueryImpl(SqlDatabase &db);

  SqlQueryImpl(const SqlQueryImpl &) = delete;
  SqlQueryImpl(SqlQueryImpl &&) = delete;
  SqlQueryImpl &operator=(const SqlQueryImpl &) = delete;
  SqlQueryImpl &operator=(SqlQueryImpl &&) = delete;

  ~SqlQueryImpl();

  void finalize();

  void prepare(std::string_view sql);

  void bind(std::int32_t index, std::int32_t value);
  void bind(std::int32_t index, std::int64_t value);
  void bind(std::int32_t index, double value);
  void bind(std::int32_t index, const std::string &value);
  void bind(std::int32_t index, const char *value, std::size_t size);

  std::int32_t exec();

  [[nodiscard]] bool next();

  [[nodiscard]] std::string get_column_name(std::int32_t index);

  [[nodiscard]] Column get_column(SqlQuery &sql_query,
                                  std::int32_t index) const;

 private:
  sqlite3 *db_ = nullptr;
  sqlite3_stmt *stmt_ = nullptr;

  std::int32_t column_count_ = 0;
};

class SqlDatabase::SqlDatabaseImpl {
  friend SqlQuery::SqlQueryImpl;

 public:
  explicit SqlDatabaseImpl(const std::string &db_name, OpenMode open_mode,
                           const std::string &password);

  SqlDatabaseImpl(const SqlDatabaseImpl &) = delete;
  SqlDatabaseImpl(SqlDatabaseImpl &&) = delete;
  SqlDatabaseImpl &operator=(const SqlDatabaseImpl &) = delete;
  SqlDatabaseImpl &operator=(SqlDatabaseImpl &&) = delete;

  ~SqlDatabaseImpl();

  void transaction();
  void commit();
  void rollback();
  void vacuum();

  [[nodiscard]] static bool table_exists(SqlDatabase &db,
                                         const std::string &table_name);

  void drop_table(const std::string &table_name);
  bool drop_table_if_exists(const std::string &table_name);

  [[nodiscard]] static std::int64_t table_line_count(
      SqlDatabase &db, const std::string &table_name);

  std::int32_t exec(std::string_view sql);

 private:
  sqlite3 *db_ = nullptr;
};

Column::ColumnImpl::ColumnImpl(SqlQuery &sql_query, std::int32_t index)
    : stmt_(sql_query.impl_->stmt_), index_(index) {}

bool Column::ColumnImpl::is_null() const { return get_type() == SQLITE_NULL; }

std::int32_t Column::ColumnImpl::as_int32() const {
  if (!is_integer()) {
    throw InvalidArgument("Not a integer");
  }

  return sqlite3_column_int(stmt_, index_);
}

std::int64_t Column::ColumnImpl::as_int64() const {
  if (!is_integer()) {
    throw InvalidArgument("Not a integer");
  }

  return sqlite3_column_int64(stmt_, index_);
}

double Column::ColumnImpl::as_double() const {
  if (!is_float()) {
    throw InvalidArgument("Not a float");
  }

  return sqlite3_column_double(stmt_, index_);
}

std::string Column::ColumnImpl::as_text() const {
  if (!is_text()) {
    throw InvalidArgument("Not a text");
  }

  return std::string(
      reinterpret_cast<const char *>(sqlite3_column_text(stmt_, index_)));
}

std::string Column::ColumnImpl::as_blob() const {
  if (!is_blob()) {
    throw InvalidArgument("Not a blob");
  }

  return decompress_data(
      reinterpret_cast<const char *>(sqlite3_column_blob(stmt_, index_)),
      sqlite3_column_bytes(stmt_, index_));
}

std::int32_t Column::ColumnImpl::get_type() const {
  return sqlite3_column_type(stmt_, index_);
}

bool Column::ColumnImpl::is_integer() const {
  return get_type() == SQLITE_INTEGER;
}

bool Column::ColumnImpl::is_float() const { return get_type() == SQLITE_FLOAT; }

bool Column::ColumnImpl::is_text() const { return get_type() == SQLITE_TEXT; }

bool Column::ColumnImpl::is_blob() const { return get_type() == SQLITE_BLOB; }

SqlQuery::SqlQueryImpl::SqlQueryImpl(SqlDatabase &db) : db_(db.impl_->db_) {}

SqlQuery::SqlQueryImpl::~SqlQueryImpl() {
  try {
    finalize();
  } catch (...) {
    error("~SqlQueryImpl failed");
  }
}

void SqlQuery::SqlQueryImpl::finalize() {
  auto rc = sqlite3_finalize(stmt_);
  check_sqlite2(rc, db_);
  stmt_ = nullptr;
}

void SqlQuery::SqlQueryImpl::prepare(std::string_view sql) {
  finalize();

  auto rc =
      sqlite3_prepare_v2(db_, std::data(sql), std::size(sql), &stmt_, nullptr);

  if (rc != SQLITE_OK) {
    finalize();
    throw RuntimeError(sqlite3_errmsg(db_));
  }

  column_count_ = sqlite3_column_count(stmt_);
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index, std::int32_t value) {
  auto rc = sqlite3_bind_int(stmt_, index, value);
  check_sqlite2(rc, db_);
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index, std::int64_t value) {
  auto rc = sqlite3_bind_int64(stmt_, index, value);
  check_sqlite2(rc, db_);
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index, double value) {
  auto rc = sqlite3_bind_double(stmt_, index, value);
  check_sqlite2(rc, db_);
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index,
                                  const std::string &value) {
  auto rc = sqlite3_bind_text(stmt_, index, value.c_str(), std::size(value),
                              SQLITE_TRANSIENT);
  check_sqlite2(rc, db_);
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index, const char *value,
                                  std::size_t size) {
  auto compressed_data = compress_data(value, size);
  auto rc = sqlite3_bind_blob(stmt_, index, std::data(compressed_data),
                              std::size(compressed_data), SQLITE_TRANSIENT);
  check_sqlite2(rc, db_);
}

std::int32_t SqlQuery::SqlQueryImpl::exec() {
  if (!stmt_) {
    throw LogicError("Call prepare first");
  }

  if (auto rc = sqlite3_step(stmt_); rc != SQLITE_DONE) {
    throw RuntimeError(sqlite3_errmsg(db_));
  }
  check_sqlite2(sqlite3_reset(stmt_), db_);

  return sqlite3_changes(db_);
}

bool SqlQuery::SqlQueryImpl::next() {
  if (!stmt_) {
    throw LogicError("Call prepare first");
  }

  if (auto rc = sqlite3_step(stmt_); rc != SQLITE_ROW) {
    check_sqlite2(sqlite3_reset(stmt_), db_);
    return false;
  }

  return true;
}

std::string SqlQuery::SqlQueryImpl::get_column_name(std::int32_t index) {
  if (index >= column_count_) {
    throw OutOfRange("Column index out of range");
  }

  return sqlite3_column_name(stmt_, index);
}

Column SqlQuery::SqlQueryImpl::get_column(SqlQuery &sql_query,
                                          std::int32_t index) const {
  if (index >= column_count_) {
    throw OutOfRange("Column index out of range");
  }

  return Column(sql_query, index);
}

SqlDatabase::SqlDatabaseImpl::SqlDatabaseImpl(const std::string &db_name,
                                              OpenMode open_mode,
                                              const std::string &password) {
  if (std::empty(password)) {
    throw InvalidArgument("The password is empty");
  }

  std::int32_t flag = 0;
  if (open_mode == OpenMode::ReadOnly) {
    flag = SQLITE_OPEN_READONLY;
  } else if (open_mode == OpenMode::ReadWrite) {
    flag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  }

  if (sqlite3_open_v2(db_name.c_str(), &db_, flag, nullptr) != SQLITE_OK) {
    std::string msg = sqlite3_errmsg(db_);
    check_sqlite(sqlite3_close_v2(db_));
    throw RuntimeError(msg);
  }

  auto rc = sqlite3_key(db_, std::data(password), std::size(password));
  check_sqlite2(rc, db_);
}

SqlDatabase::SqlDatabaseImpl::~SqlDatabaseImpl() { sqlite3_close_v2(db_); }

void SqlDatabase::SqlDatabaseImpl::transaction() { exec("BEGIN"); }

void SqlDatabase::SqlDatabaseImpl::commit() { exec("COMMIT"); }

void SqlDatabase::SqlDatabaseImpl::rollback() { exec("ROLLBACK"); }

void SqlDatabase::SqlDatabaseImpl::vacuum() { exec("VACUUM"); }

bool SqlDatabase::SqlDatabaseImpl::table_exists(SqlDatabase &db,
                                                const std::string &table_name) {
  SqlQuery query(db);
  query.prepare(
      "SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?");
  query.bind(1, table_name);
  boost::ignore_unused(query.next());

  return query.get_column(0).as_int32() == 1;
}

void SqlDatabase::SqlDatabaseImpl::drop_table(const std::string &table_name) {
  if (exec("DROP TABLE " + table_name) != 1) {
    throw RuntimeError("Drop table failed");
  }
}

bool SqlDatabase::SqlDatabaseImpl::drop_table_if_exists(
    const std::string &table_name) {
  return exec("DROP TABLE IF EXISTS " + table_name);
}

std::int64_t SqlDatabase::SqlDatabaseImpl::table_line_count(
    SqlDatabase &db, const std::string &table_name) {
  SqlQuery query(db);
  query.prepare("SELECT count(*) FROM " + table_name);
  boost::ignore_unused(query.next());

  return query.get_column(0).as_int64();
}

std::int32_t SqlDatabase::SqlDatabaseImpl::exec(std::string_view sql) {
  char *err_msg = nullptr;
  SCOPE_EXIT { sqlite3_free(err_msg); };

  auto rc = sqlite3_exec(db_, sql.data(), nullptr, nullptr, &err_msg);
  if (rc != SQLITE_OK) {
    throw RuntimeError(err_msg);
  }

  return sqlite3_changes(db_);
}

Column::~Column() = default;

bool Column::is_null() const { return impl_->is_null(); }

std::int32_t Column::as_int32() const { return impl_->as_int32(); }

std::int64_t Column::as_int64() const { return impl_->as_int64(); }

double Column::as_double() const { return impl_->as_double(); }

std::string Column::as_text() const { return impl_->as_text(); }

std::string Column::as_blob() const { return impl_->as_blob(); }

Column::Column(SqlQuery &sql_query, std::int32_t index)
    : impl_(std::make_unique<ColumnImpl>(sql_query, index)) {}

SqlQuery::SqlQuery(SqlDatabase &db)
    : impl_(std::make_unique<SqlQueryImpl>(db)) {}

SqlQuery::~SqlQuery() = default;

void SqlQuery::finalize() { impl_->finalize(); }

void SqlQuery::prepare(std::string_view sql) { impl_->prepare(sql); }

void SqlQuery::bind(std::int32_t index, std::int32_t value) {
  impl_->bind(index, value);
}

void SqlQuery::bind(std::int32_t index, std::int64_t value) {
  impl_->bind(index, value);
}

void SqlQuery::bind(std::int32_t index, double value) {
  impl_->bind(index, value);
}

void SqlQuery::bind(std::int32_t index, const std::string &value) {
  impl_->bind(index, value);
}

void SqlQuery::bind(std::int32_t index, const char *value, std::size_t size) {
  impl_->bind(index, value, size);
}

std::int32_t SqlQuery::exec() { return impl_->exec(); }

bool SqlQuery::next() { return impl_->next(); }

std::string SqlQuery::get_column_name(std::int32_t index) {
  return impl_->get_column_name(index);
}

Column SqlQuery::get_column(std::int32_t index) {
  return impl_->get_column(*this, index);
}

SqlDatabase::SqlDatabase(const std::string &db_name,
                         SqlDatabase::OpenMode open_type,
                         const std::string &password)
    : impl_(std::make_unique<SqlDatabaseImpl>(db_name, open_type, password)) {}

SqlDatabase::~SqlDatabase() = default;

void SqlDatabase::transaction() { impl_->transaction(); }

void SqlDatabase::commit() { impl_->commit(); }

void SqlDatabase::rollback() { impl_->rollback(); }

void SqlDatabase::vacuum() { impl_->vacuum(); }

bool SqlDatabase::table_exists(const std::string &name) {
  return impl_->table_exists(*this, name);
}

void SqlDatabase::drop_table(const std::string &table_name) {
  impl_->drop_table(table_name);
}

bool SqlDatabase::drop_table_if_exists(const std::string &table_name) {
  return impl_->drop_table_if_exists(table_name);
}

std::int64_t SqlDatabase::table_line_count(const std::string &table_name) {
  return impl_->table_line_count(*this, table_name);
}

std::int32_t SqlDatabase::exec(std::string_view sql) {
  return impl_->exec(sql);
}

}  // namespace klib
