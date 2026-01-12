
#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <string>

using namespace std;

constexpr const char* DB_PATH = "task.db";
constexpr const char* TABLE_NAME = "tasks";

struct task_data {
    string task;
    bool task_status;
};

class ToDo {
private:
    sqlite3* db;
    char* errorMsg = nullptr;

    void db_connect() {
        int rc = sqlite3_open(DB_PATH, &db);
        if (rc) {
            cerr<<"[-] Unable to connect to DB: "<<sqlite3_errmsg(db)<<endl;
        } else {
            // cout<< "[*] Connected to DB..."<<endl;
        }
    }

    static int collectCallback(void* data, int argc, char** argv, char** azColName) {
        auto* tasks = static_cast<vector<task_data>*>(data);
        task_data t;
        for (int i = 0; i < argc; i++) {
            string colName = azColName[i];
            if (colName == "task" && argv[i]) t.task = argv[i];
            else if (colName == "task_status" && argv[i]) t.task_status = stoi(argv[i]);
        }
        tasks->push_back(t);
        return 0;
    }

    vector<task_data> GetTasksByStatus(bool status) {
        string sql = "SELECT * FROM " + string(TABLE_NAME) + " WHERE task_status=" + (status ? "1" : "0") + ";";
        vector<task_data> output;
        int rc = sqlite3_exec(db, sql.c_str(), collectCallback, &output, &errorMsg);
        if (rc != SQLITE_OK) {
            cerr << "[-] " << errorMsg << endl;
            sqlite3_free(errorMsg);
        }
        return output;
    }

    bool InsertTaskDB(const task_data& data) {
        string sql = "INSERT INTO " + string(TABLE_NAME) + " (task, task_status) VALUES (?, ?);";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr<<"[-] Failed to prepare insert: "<<sqlite3_errmsg(db)<<endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, data.task.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, data.task_status ? 1 : 0);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr<<"[-] Insert failed: "<<sqlite3_errmsg(db)<<endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        // cout<<"[*] Task inserted successfully!"<<endl;
        return true;
    }

    bool UpdateTaskDB(const string& task, bool new_status) {
        string sql = "UPDATE " + string(TABLE_NAME) + " SET task_status = ? WHERE task = ?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr<<"[-] Failed to prepare update: "<<sqlite3_errmsg(db)<<endl;
            return false;
        }

        sqlite3_bind_int(stmt, 1, new_status ? 1 : 0);
        sqlite3_bind_text(stmt, 2, task.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr<<"[-] Update failed: "<<sqlite3_errmsg(db)<<endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        // cout<<"[*] Task updated successfully!"<<endl;
        return true;
    }

    bool DeleteTaskDB(const string& task) {
        string sql = "DELETE FROM " + string(TABLE_NAME) + " WHERE task = ?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr<<"[-] Failed to prepare delete: "<<sqlite3_errmsg(db)<<endl;
            return false;
        }

        sqlite3_bind_text(stmt, 1, task.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr<<"[-] Delete failed: "<<sqlite3_errmsg(db)<<endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        // cout<<"[*] Task deleted successfully!"<<endl;
        return true;
    }

public:
    ToDo() {
        db_connect();
    }

    void ListTask() {
        vector<task_data> done_tasks = GetTasksByStatus(true);
        vector<task_data> not_done_tasks = GetTasksByStatus(false);
        cout<<endl;
        if(done_tasks.size() !=0){
            cout<<"-------------------------------"<<endl;
            cout<<"         Done Tasks            "<<endl;
            cout<<"-------------------------------"<<endl;
            for (const auto& task : done_tasks) {
                cout <<"[-]\t  "<<task.task<<endl;
            }
            cout<<endl;
        }
        
        if(not_done_tasks.size() != 0){
            cout<<"-------------------------------"<<endl;
            cout<<"       Not Done Tasks          "<<endl;
            cout<<"-------------------------------"<<endl;
            for (const auto& task : not_done_tasks) {
                cout<<"[-]\t"<<task.task<<endl;
            }
        }
       
    }

    void AddTask(const string& task, bool status) {
        task_data new_task{task, status};
        bool output = InsertTaskDB(new_task);
        if (output) {
            cout<<"[-] Task has been Inserted..."<<endl;
        } else {
            cout<<"[-] Unable to Insert the task..."<<endl;
        }
    }

    void UpdateTask(const string& task, bool status) {
        bool output = UpdateTaskDB(task, status);
        if (output) {
            cout<<"[-] Task has been Updated..."<<endl;
        } else {
            cout<<"[-] Unable to update the task..."<<endl;
        }
    }

    void DeleteTask(const string& task) {
        bool output = DeleteTaskDB(task);
        if (output) {
            cout<<"[-] Task has been Deleted..."<<endl;
        } else {
            cout<< "[-] Unable to Delete the task..."<<endl;
        }
    }
};


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <command> [args]\n";
        cerr << "Commands:\n";
        cerr << "  list\n";
        cerr << "  add <task_name> [status]\n";
        cerr << "  update <task_name> <status>\n";
        cerr << "  delete <task_name>\n";
        return 1;
    }

    string command = argv[1];
    ToDo tasker;

    if (command == "list") {
        tasker.ListTask();
    } 
    else if (command == "add") {
        if (argc < 3) {
            cerr << "Usage: " << argv[0] << " add <task_name> [status]\n";
            return 1;
        }
        string taskName = argv[2];
        bool status = false; // default not done
        if (argc >= 4) {
            string statusArg = argv[3];
            status = (statusArg == "1" || statusArg == "true");
        }
        tasker.AddTask(taskName, status);
    } 
    else if (command == "update") {
        if (argc < 4) {
            cerr << "Usage: " << argv[0] << " update <task_name> <status>\n";
            return 1;
        }
        string taskName = argv[2];
        string statusArg = argv[3];
        bool status = (statusArg == "1" || statusArg == "true");
        tasker.UpdateTask(taskName, status);
    } 
    else if (command == "delete") {
        if (argc < 3) {
            cerr << "Usage: " << argv[0] << " delete <task_name>\n";
            return 1;
        }
        string taskName = argv[2];
        tasker.DeleteTask(taskName);
    } 
    else {
        cerr << "Unknown command: " << command << endl;
        return 1;
    }

    return 0;
}
