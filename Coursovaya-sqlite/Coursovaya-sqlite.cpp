#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>

using namespace std;

class Database {
private:
    sqlite3* db;
    vector<string> fieldNames;

    static int callback(void* data, int argc, char** argv, char** azColName) {
        Database* db = static_cast<Database*>(data);

        if (db->fieldNames.empty()) {
            for (int i = 0; i < argc; i++) {
                db->fieldNames.push_back(azColName[i]);
            }
            // Выводим заголовок таблицы с названиями полей
            for (const auto& fieldName : db->fieldNames) {
                cout << fieldName << "\t";
            }
            cout << endl;
        }

        for (int i = 0; i < argc; i++) {
            cout << (argv[i] ? argv[i] : "NULL") << "\t";
        }
        cout << endl;
        return 0;
    }

public:
    Database() : db(nullptr) {}

    bool open(const string& dbName) {
        int rc = sqlite3_open(dbName.c_str(), &db);
        return rc == SQLITE_OK;
    }

    bool execute(const string& sql) {
        char* zErrMsg = nullptr;
        fieldNames.clear(); // Очищаем названия полей перед выполнением запроса
        int rc = sqlite3_exec(db, sql.c_str(), callback, this, &zErrMsg);
        if (rc != SQLITE_OK) {
            cerr << "Ошибка выполнения SQL-команды: " << zErrMsg << endl;
            sqlite3_free(zErrMsg);
            return false;
        }
        return true;
    }

    void close() {
        if (db) {
            sqlite3_close(db);
        }
    }
};

int main() {
    setlocale(LC_ALL, "ru"); // установка правильной кодировки русских символов
    string dbName;
    string sql;
    Database db;

    cout << "Введите имя базы данных: ";
    cin >> dbName;

    if (!db.open(dbName)) {
        cerr << "Ошибка открытия базы данных." << endl;
        return 1;
    }

    cin.ignore();
    while (true) {
        cout << "Введите SQL-команду ('exit' для выхода): ";
        getline(cin, sql);

        if (sql == "exit") {
            break;
        }

        if (!db.execute(sql)) {
            cerr << "Ошибка выполнения SQL-команды." << endl;
        }
    }

    db.close();
    return 0;
}
