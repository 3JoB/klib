#include "klib/sql.h"

#include <fmt/compile.h>
#include <fmt/format.h>
#include <sqlcipher/sqlite3.h>

#include "klib/exception.h"

namespace klib {

namespace {

void check_sqlite(std::int32_t rc) {
  if (rc != SQLITE_OK) {
    throw klib::RuntimeError(sqlite3_errstr(rc));
  }
}

}  // namespace

class Column::ColumnImpl {
 public:
  ColumnImpl(SqlQuery &sql_query, std::int32_t index);

  [[nodiscard]] bool is_null() const;

  [[nodiscard]] std::int32_t as_int32() const;
  [[nodiscard]] std::int64_t as_int64() const;
  [[nodiscard]] double as_double() const;
  [[nodiscard]] std::string as_string() const;
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
  explicit SqlQueryImpl(const SqlDatabase &database);
  ~SqlQueryImpl();

  std::string get_column_name(std::int32_t index);

  void prepare(std::string_view sql);

  void bind(std::int32_t index, std::int32_t value);
  void bind(std::int32_t index, std::int64_t value);
  void bind(std::int32_t index, double value);
  void bind(std::int32_t index, const std::string &value);
  void bind(std::int32_t index, const char *value, std::size_t size);

  void step();

  [[nodiscard]] bool next();

  [[nodiscard]] Column get_column(SqlQuery &sql_query, std::int32_t index);

 private:
  sqlite3 *db_ = nullptr;
  sqlite3_stmt *stmt_ = nullptr;

  std::int32_t column_count_ = 0;
};

class SqlDatabase::SqlDatabaseImpl {
  friend SqlQuery::SqlQueryImpl;

 public:
  explicit SqlDatabaseImpl(const std::string &table_name, OpenType open_type,
                           const std::string &key);
  ~SqlDatabaseImpl();

  void transaction();
  void commit();
  void rollback();

  void drop_table(const std::string &table_name);
  void drop_table_if_exists(const std::string &table_name);

  [[nodiscard]] bool table_exists(SqlDatabase &database,
                                  const std::string &table_name) const;
  std::int64_t table_line_count(SqlDatabase &database,
                                const std::string &table_name);

  std::int32_t exec(std::string_view sql);

 private:
  sqlite3 *db_ = nullptr;
};

Column::ColumnImpl::ColumnImpl(SqlQuery &sql_query, std::int32_t index)
    : stmt_(sql_query.impl_->stmt_), index_(index) {}

std::int32_t Column::ColumnImpl::as_int32() const {
  if (!is_integer()) {
    throw klib::InvalidArgument("not a integer");
  }

  return sqlite3_column_int(stmt_, index_);
}

std::int64_t Column::ColumnImpl::as_int64() const {
  if (!is_integer()) {
    throw klib::InvalidArgument("not a integer");
  }

  return sqlite3_column_int64(stmt_, index_);
}

double Column::ColumnImpl::as_double() const {
  if (!is_float()) {
    throw klib::InvalidArgument("not a float");
  }

  return sqlite3_column_double(stmt_, index_);
}

std::string Column::ColumnImpl::as_string() const {
  if (!is_text()) {
    throw klib::InvalidArgument("not a string");
  }

  return std::string(
      reinterpret_cast<const char *>(sqlite3_column_text(stmt_, index_)));
}

std::string Column::ColumnImpl::as_blob() const {
  if (!is_blob()) {
    throw klib::InvalidArgument("not a blob");
  }

  return std::string(
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

bool Column::ColumnImpl::is_text() const {
  auto fuck = get_type();
  (void)fuck;
  return get_type() == SQLITE_TEXT;
}

bool Column::ColumnImpl::is_blob() const { return get_type() == SQLITE_BLOB; }

bool Column::ColumnImpl::is_null() const { return get_type() == SQLITE_NULL; }

SqlQuery::SqlQueryImpl::SqlQueryImpl(const SqlDatabase &database)
    : db_(database.impl_->db_) {}

SqlQuery::SqlQueryImpl::~SqlQueryImpl() { sqlite3_finalize(stmt_); }

bool SqlQuery::SqlQueryImpl::next() {
  if (!stmt_) {
    throw RuntimeError("call prepare first");
  }

  if (auto rc = sqlite3_step(stmt_); rc != SQLITE_ROW) {
    sqlite3_reset(stmt_);
    return false;
  }

  return true;
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index, std::int32_t value) {
  auto rc = sqlite3_bind_int(stmt_, index, value);
  check_sqlite(rc);
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index, std::int64_t value) {
  auto rc = sqlite3_bind_int64(stmt_, index, value);
  check_sqlite(rc);
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index, double value) {
  auto rc = sqlite3_bind_double(stmt_, index, value);
  check_sqlite(rc);
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index,
                                  const std::string &value) {
  auto rc = sqlite3_bind_text(stmt_, index, value.c_str(), std::size(value),
                              SQLITE_TRANSIENT);
  check_sqlite(rc);
}

void SqlQuery::SqlQueryImpl::bind(std::int32_t index, const char *value,
                                  std::size_t size) {
  auto rc = sqlite3_bind_blob(stmt_, index, value, size, SQLITE_TRANSIENT);
  check_sqlite(rc);
}

std::string SqlQuery::SqlQueryImpl::get_column_name(std::int32_t index) {
  if (index >= column_count_) {
    throw OutOfRange("Column index out of range");
  }

  return sqlite3_column_name(stmt_, index);
}

void SqlQuery::SqlQueryImpl::prepare(std::string_view sql) {
  auto rc = sqlite3_finalize(stmt_);
  check_sqlite(rc);
  column_count_ = 0;

  rc = sqlite3_prepare_v2(db_, std::data(sql),
                          static_cast<std::int32_t>(std::size(sql)), &stmt_,
                          nullptr);

  if (rc != SQLITE_OK) {
    std::string msg = sqlite3_errmsg(db_);
    rc = sqlite3_finalize(stmt_);
    check_sqlite(rc);

    throw RuntimeError(msg);
  }

  column_count_ = sqlite3_column_count(stmt_);
}

void SqlQuery::SqlQueryImpl::step() {
  if (auto rc = sqlite3_step(stmt_); rc != SQLITE_DONE) {
    throw klib::RuntimeError(sqlite3_errstr(rc));
  }
  sqlite3_reset(stmt_);
}

Column SqlQuery::SqlQueryImpl::get_column(SqlQuery &sql_query,
                                          std::int32_t index) {
  if (index >= column_count_) {
    throw OutOfRange("Column index out of range");
  }

  return Column(sql_query, index);
}

SqlDatabase::SqlDatabaseImpl::SqlDatabaseImpl(const std::string &table_name,
                                              SqlDatabase::OpenType open_type,
                                              const std::string &key) {
  if (std::size(key) != 32) {
    throw InvalidArgument("key must be 256 bit");
  }

  std::int32_t flag = 0;
  if (open_type == OpenType::ReadOnly) {
    flag = SQLITE_OPEN_READONLY;
  } else if (open_type == OpenType::ReadWrite) {
    flag = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  }

  if (sqlite3_open_v2(table_name.c_str(), &db_, flag, nullptr) != SQLITE_OK) {
    auto msg = fmt::format(FMT_COMPILE("Can't open database '{}': {}"),
                           table_name, sqlite3_errmsg(db_));
    auto rc = sqlite3_close_v2(db_);
    check_sqlite(rc);
    throw klib::RuntimeError(msg);
  }

  sqlite3_key(db_, std::data(key), 32);
}

SqlDatabase::SqlDatabaseImpl::~SqlDatabaseImpl() { sqlite3_close_v2(db_); }

void SqlDatabase::SqlDatabaseImpl::transaction() { exec("BEGIN"); }

void SqlDatabase::SqlDatabaseImpl::commit() { exec("COMMIT"); }

void SqlDatabase::SqlDatabaseImpl::rollback() { exec("ROLLBACK"); }

void SqlDatabase::SqlDatabaseImpl::drop_table(const std::string &table_name) {
  exec(fmt::format(FMT_COMPILE("DROP TABLE {};"), table_name));
}

void SqlDatabase::SqlDatabaseImpl::drop_table_if_exists(
    const std::string &table_name) {
  exec(fmt::format(FMT_COMPILE("DROP TABLE IF EXISTS {};"), table_name));
}

bool SqlDatabase::SqlDatabaseImpl::table_exists(
    SqlDatabase &database, const std::string &table_name) const {
  SqlQuery query(database);
  query.prepare(
      "SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?");
  query.bind(1, table_name);
  (void)query.next();

  return query.get_column(0).as_int32() == 1;
}

std::int64_t SqlDatabase::SqlDatabaseImpl::table_line_count(
    SqlDatabase &database, const std::string &table_name) {
  SqlQuery query(database);
  query.prepare("SELECT count(*) FROM " + table_name);
  (void)query.next();

  return query.get_column(0).as_int64();
}

std::int32_t SqlDatabase::SqlDatabaseImpl::exec(std::string_view sql) {
  char *err_msg = nullptr;
  if (sqlite3_exec(db_, sql.data(), nullptr, nullptr, &err_msg) != SQLITE_OK) {
    std::string msg = err_msg;
    sqlite3_free(err_msg);
    throw klib::RuntimeError(msg);
  }
  sqlite3_free(err_msg);

  // INSERT, UPDATE or DELETE only
  return sqlite3_changes(db_);
}

Column::~Column() = default;

bool Column::is_null() const { return impl_->is_null(); }

std::int32_t Column::as_int32() const { return impl_->as_int32(); }

std::int64_t Column::as_int64() const { return impl_->as_int64(); }

double Column::as_double() const { return impl_->as_double(); }

std::string Column::as_string() const { return impl_->as_string(); }

std::string Column::as_blob() const { return impl_->as_blob(); }

Column::Column(SqlQuery &sql_query, std::int32_t index)
    : impl_(std::make_unique<ColumnImpl>(sql_query, index)) {}

SqlQuery::SqlQuery(const SqlDatabase &database)
    : impl_(std::make_unique<SqlQueryImpl>(database)) {}

SqlQuery::~SqlQuery() = default;

std::string SqlQuery::get_column_name(std::int32_t index) {
  return impl_->get_column_name(index);
}

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

void SqlQuery::step() { impl_->step(); }

bool SqlQuery::next() { return impl_->next(); }

Column SqlQuery::get_column(std::int32_t index) {
  return impl_->get_column(*this, index);
}

SqlDatabase::SqlDatabase(const std::string &table_name,
                         SqlDatabase::OpenType open_type,
                         const std::string &key)
    : impl_(std::make_unique<SqlDatabaseImpl>(table_name, open_type, key)) {}

SqlDatabase::~SqlDatabase() = default;

void SqlDatabase::transaction() { impl_->transaction(); }

void SqlDatabase::commit() { impl_->commit(); }

void SqlDatabase::rollback() { impl_->rollback(); }

void SqlDatabase::drop_table(const std::string &table_name) {
  impl_->drop_table(table_name);
}

void SqlDatabase::drop_table_if_exists(const std::string &table_name) {
  impl_->drop_table_if_exists(table_name);
}

bool SqlDatabase::table_exists(const std::string &name) {
  return impl_->table_exists(*this, name);
}

std::int64_t SqlDatabase::table_line_count(const std::string &table_name) {
  return impl_->table_line_count(*this, table_name);
}

std::int32_t SqlDatabase::exec(std::string_view sql) {
  return impl_->exec(sql);
}

}  // namespace klib
